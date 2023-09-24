#include "time.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <ctime>

TimeMap::TimeMap()
{
    std::vector<std::string> months_str = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    std::vector<int> months_num = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    for (int i : months_num)
    {
        map.insert(std::pair<std::string, int>(months_str[i], i));
    }
    std::vector<std::string> days_str = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    std::vector<int> days_num = {0, 1, 2, 3, 4, 5, 6};
    for (int i : days_num)
    {
        map.insert(std::pair<std::string, int>(days_str[i], i));
    }
}

int TimeMap::get(std::string str)
{
    return map.find(str)->second;
}

void Time::set_time(std::string str)
{
    TimeMap map;
    time.tm_year = atoi(str.substr(12).c_str()) - 1900;
    time.tm_hour = atoi(str.substr(17).c_str());
    time.tm_min = atoi(str.substr(20).c_str());
    time.tm_sec = atoi(str.substr(23).c_str());
    time.tm_wday = map.get(str.substr(0, 3).c_str());
    time.tm_mon = map.get(str.substr(8, 3).c_str()) - 1;
    time.tm_mday = atoi(str.substr(5).c_str());
    time.tm_isdst = 0;
    std::cout << "Time is set to be: " << asctime(&time) << std::endl;
}

struct tm *Time::get_time_struct()
{
    return &time;
}