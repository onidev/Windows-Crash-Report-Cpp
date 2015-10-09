#include "evlog.h"
#include <set>
#include <cstdio>
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

bool CrashReportWindows::parseLogs(unsigned buffer_size)
{
    HANDLE handle = OpenEventLog(NULL, "Application");
    if(handle)
    {
        std::vector<char> buffer(buffer_size);
        DWORD read;
        DWORD needed;
        DWORD status = ERROR_SUCCESS;
        
        while(status == ERROR_SUCCESS)
        {
            if(!ReadEventLog(handle, EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ, 0, &buffer[0], buffer_size, &read, &needed))
            {
                status = GetLastError();
                if(status == ERROR_INSUFFICIENT_BUFFER)
                {
                    return false; // Not enough memory
                }
                else 
                {
                    if(status != ERROR_HANDLE_EOF)
                    {
                        // @todo
                    }
                }
            }
            else
            {
                dumpRecords(buffer);
                break;
            }
        }
        CloseEventLog(handle);
    }
    return true;
}

void CrashReportWindows::dumpRecords(const std::vector<char>& buffer)
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
    
    while(ptr < end)
    {
        if(strncmp("Application Error", ptr, 17) == 0)
        {
            // always: 41 70 70 6c 69 63 61 74 69 6f 6e 20 45 72 72 6f 72 0 (?)
            ptr += 18;
            
            Crash crash;
            crash.computer    = readString();
            crash.program     = readString();
            crash.version     = readString();
            crash.error_from  = readString();
            crash.version_dep = readString();
            crash.offset      = strtoul(readString().c_str(), nullptr, 16);
            crash.log         = readString();
            
            while(crash.log.back() == '\n')
                crash.log.pop_back();
            
            _logs.push_back(crash);
            
            Error err{crash.version, crash.offset};
            if(crash.program == _app_name && set.find(err) == set.end())
            {
                _new_errors.push_back(err);
                set.insert(err);
            }
        }
        ptr++;
    }
}

}
