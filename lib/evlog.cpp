#include "evlog.h"
#include <set>
#include <cstdio>
#include <iostream>
#include <windows.h>

namespace evlog
{

bool Error::operator<(const Error& o) const
{
    if(version == o.version)
        return offset < o.offset;
    
    return version < o.version;
}

CrashReportWindows::CrashReportWindows(const std::string& fname, const std::string& app_name):
    _fname(fname),
    _app_name(app_name)
{
    // read already processed errors
    FILE * f = fopen(fname.c_str(), "rb");
    if(f)
    {
        unsigned count, size;
        fread(&count, 1, sizeof(unsigned), f);
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 4, SEEK_SET);
        
        size = (size-sizeof(unsigned))/sizeof(unsigned)/2;
        
        if(count == size)
        {
            _processed.resize(count);
            for(unsigned i=0; i<count; ++i)
            {
                unsigned version;
                fread(&version, 1, sizeof(unsigned), f);
                fread(&_processed[i].offset, 1, sizeof(unsigned), f);
                _processed[i].version = Version(version);
            }
        }
        
        fclose(f);
    }
}

CrashReportWindows::~CrashReportWindows()
{
    FILE * f = fopen(_fname.c_str(), "wb");
    if(f)
    {
        _processed.insert(end(_processed), begin(_new_errors), end(_new_errors));
        unsigned size = _processed.size();
        fwrite(&size, 1, sizeof(unsigned), f);
        for(auto& e: _processed)
        {
            unsigned version = e.version.value();
            fwrite(&version, 1, sizeof(unsigned), f);
            fwrite(&e.offset, 1, sizeof(unsigned), f);
        }
        fclose(f);
    }
}

bool CrashReportWindows::parseLogs(unsigned nb_records_max)
{
    HANDLE handle = OpenEventLog(NULL, "Application");
    if(handle)
    {
        DWORD number_of_records, first_record;
        GetNumberOfEventLogRecords(handle, &number_of_records);
        GetOldestEventLogRecord(handle, &first_record);
        
        int nb_processed = 0;
        int i;
        if(nb_records_max == 0)
            i = first_record;
        else
            i = first_record + number_of_records - nb_records_max;
        while(i<first_record + number_of_records)
        {
            DWORD bytes_to_read = 20 * 1024;
            DWORD bytes_read = 0;
            DWORD min_bytes_to_read = 0;
            
            std::vector<char> buffer(bytes_to_read);
            
            int ret = ReadEventLog(handle, EVENTLOG_SEEK_READ | EVENTLOG_FORWARDS_READ, i, &buffer[0], bytes_to_read, &bytes_read, &min_bytes_to_read);
            if(!ret)
            {
                return false;
            }
            unsigned count = dumpRecords(buffer);
            nb_processed += count;
            
            if(count == 0)
                i++;
            else
                i += count;
            
            if(nb_records_max != 0 && nb_processed >= nb_records_max)
                break;
        }
        
        CloseEventLog(handle);
    }
    else
    {
        return false;
    }
    return true;
}

unsigned CrashReportWindows::dumpRecords(const std::vector<char>& buffer)
{
    const char* ptr = &buffer[0];
    const char* end = ptr + buffer.size();
    
    auto readString = [&]()
    {
        std::string ret(ptr);
        ptr += ret.length();
        
        while(*ptr == 0 && ptr != end)
            ptr++;
        
        return ret;
    };
    
    std::set<Error> set(_processed.begin(), _processed.end());
    
    unsigned count = 0;
    int meuh = 0;
    while(ptr < end)
    {
        if(strncmp("Application Error", ptr, 17) == 0)
        {
            const char* ptr2 = ptr - 40;
            ptr += 18; // Skip "Application Error" string
            
            Crash crash;
            crash.computer    = readString();
            crash.program     = readString();
            crash.version     = readString();
            crash.error_from  = readString();
            crash.version_dep = readString();
            crash.offset      = strtoul(readString().c_str(), nullptr, 16);
            
            std::string log   = readString(); // on windows xp this is the log - on other the timestamp
            
            if(log.size() <= 10) // windows vista(?), 7, 8, 10
            {
                crash.timestamp = strtoul(log.c_str(), nullptr, 16);
                readString(); // Exception code
                crash.offset = strtoul(readString().c_str(), nullptr, 16);
            }
            else // win xp
            {
                crash.timestamp = ptr2[0]<<24 | ptr2[1]<<16 | ptr2[2]<<8 | ptr2[3];
            }
            
            _logs.push_back(crash);
            
            Error err{crash.version, crash.offset};
            if(crash.program == _app_name && set.find(err) == set.end())
            {
                _new_errors.push_back(err);
                set.insert(err);
            }
            
            count++;
        }
        ptr++;
    }
    return count;
}

}
