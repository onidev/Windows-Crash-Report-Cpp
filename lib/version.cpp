#include "evlog.h"
#include <cassert>
#include <cstdio>
namespace evlog
{

Version::Version():
    n{0, 0, 0, 0}
{
}
Version::Version(unsigned major, unsigned minor, unsigned build, unsigned revision)
{
    set(major, minor, build, revision);
}
Version::Version(unsigned value)
{
    set(value);
}
Version::Version(const std::string& str)
{
    set(str);
}

unsigned Version::value() const
{
    return n[0] | n[1]<<8 | n[2]<<16 | n[3]<<24;
}

void Version::set(unsigned major, unsigned minor, unsigned build, unsigned revision)
{
    n[0] = major>255? 255:major;
    n[1] = minor>255? 255:minor;
    n[2] = build>255? 255:build;
    n[3] = revision>255? 255:revision;
}
void Version::set(unsigned value)
{
    n[0] = value & 0xFF;
    n[1] = (value >> 8) & 0xFF;
    n[2] = (value >> 16) & 0xFF;
    n[3] = (value >> 24) & 0xFF;
}
void Version::set(const std::string& str)
{
    unsigned i=0;
    int x=0;
    for(unsigned j=0; j<str.length(); ++j)
    {
        char c = str[j];
        if(c == '.') {
            n[i] = x;
            x = 0;
            i++;
        }
        else if(c >= '0' && c <= '9')
        {
            x = x*10 + c - '0';
        }
    }
    n[i] = x;
    for(++i; i<4; ++i)
        n[i] = 0;
}

unsigned char Version::major() const
{
    return n[0];
}
unsigned char Version::minor() const
{
    return n[1];
}
unsigned char Version::build() const
{
    return n[2];
}
unsigned char Version::revision() const
{
    return n[3];
}

unsigned char Version::operator()(unsigned i) const
{
    assert(i < 4);
    return n[i];
}

unsigned char& Version::operator()(unsigned i)
{
    assert(i < 4);
    return n[i];
}

bool Version::operator< (const Version& o) const
{
    for(int i=0; i<4; ++i)
    {
        if(n[i] < o.n[i])
            return true;
        if(n[i] > o.n[i])
            return false;
    }
    return false;
}
bool Version::operator<=(const Version& o) const
{
    return *this<o || *this==o;
}
bool Version::operator> (const Version& o) const
{
    return !(*this<=o);
}
bool Version::operator>=(const Version& o) const
{
    return !(*this<o);
}
bool Version::operator==(const Version& o) const
{
    return n[0]==o.n[0] && n[1]==o.n[1] && n[2]==o.n[2] && n[3]==o.n[3];
}
bool Version::operator!=(const Version& o) const
{
    return !(*this == o);
}

std::string Version::asString() const
{
    char buff[16];
    sprintf(buff, "%d.%d.%d.%d", n[0], n[1], n[2], n[3]);
    return buff;
}

}
