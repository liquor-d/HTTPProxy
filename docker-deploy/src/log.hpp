#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <string>

class Log
{
public:
    static void writeMsg(std::string msg);
    static void writeMsg(int id, std::string msg);
    static void writeMsg(int id, std::string msg1, std::string ip, std::string msg2);
    static void writeNote(std::string note);
    static void writeNote(int id, std::string note);
    static void writeWarning(std::string warning);
    static void writeWarning(int id, std::string warning);
    static void writeError(std::string error);
    static void writeError(int id, std::string error);
};

#endif