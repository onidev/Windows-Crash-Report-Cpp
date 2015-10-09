#include <iostream>
#include "evlog.h"

int main()
{
    using namespace evlog;
    using namespace std;
    
    CrashReportWindows report("crash_logs.bin", "PlanetCentauri.exe");
    report.parseLogs(20);
    
    cout << "Captured logs: " << report.logs().size() << '\n';
    
    for(int i=0; i<report.logs().size() && i<10; ++i)
    {
        auto& log = report.logs()[i];
        cout << log.program << ' ' << log.version.asString() << ' ' << hex << log.offset << '\n';
    }
    
    cout << "\nNew filtered logs: \n";
    for(auto& e: report.newErrors())
    {
        cout << e.version.asString() << ':' << e.offset << '\n';
    }
    
    return 0;
}
