#ifndef __TIME_HPP__
#define __TIME_HPP__

#include <string>
#include <map>

class TimeMap
{
private:
    std::map<std::string, int> map;

public:
    TimeMap();
    int get(std::string str);
};

class Time
{
private:
    struct tm time;

public:
    void set_time(std::string str);
    struct tm *get_time_struct();
};

#endif