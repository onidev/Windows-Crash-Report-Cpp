#ifndef EVLOG_H_INCLUDED
#define EVLOG_H_INCLUDED

#include <string>
#include <vector>

namespace evlog
{

struct Version
{
    unsigned char n[4];
    
public:
    Version();
    Version(unsigned major, unsigned minor, unsigned build, unsigned revision);
    Version(unsigned value);
    Version(const std::string& str);
    
    unsigned value() const;
    void set(unsigned major, unsigned minor, unsigned build, unsigned revision);
    void set(unsigned value);
    void set(const std::string& str);
    
    unsigned char major() const;
    unsigned char minor() const;
    unsigned char build() const;
    unsigned char revision() const;
    
    unsigned char operator()(unsigned i) const;
    unsigned char& operator()(unsigned i);
    
    bool operator< (const Version& o) const;
    bool operator<=(const Version& o) const;
    bool operator> (const Version& o) const;
    bool operator>=(const Version& o) const;
    bool operator==(const Version& o) const;
    bool operator!=(const Version& o) const;
    
    std::string asString() const;
};

struct Crash
{
    std::string computer;
    std::string program;
    Version     version;
    std::string error_from;
    Version     version_dep;
    unsigned    offset;
    unsigned    timestamp;
};

struct Error
{
    Version  version;
    unsigned offset;
    bool operator<(const Error& o) const;
};

class CrashReportWindows
{
    std::string _fname;
    std::string _app_name;
    
    std::vector<Crash> _logs;
    
    std::vector<Error> _processed;
    std::vector<Error> _new_errors;
    
    unsigned dumpRecords(const std::vector<char>& buffer);
    
public:
    CrashReportWindows(const std::string& fname, const std::string& app_name);
    ~CrashReportWindows();
    
    bool parseLogs(unsigned nb_records_max = 0);
    
    const std::vector<Crash>& logs() const { return _logs; }
    const std::vector<Error>& newErrors() const { return _new_errors; }
};

}

#endif // EVLOG_H_INCLUDED
