#include <fstream>
#include <mutex>

#include "log.hpp"

std::ofstream proxy_log_file("proxy.log");
std::mutex log_lock;

void Log::writeMsg(std::string msg)
{
    log_lock.lock();
    proxy_log_file << msg << std::endl;
    log_lock.unlock();
}

void Log::writeMsg(int id, std::string msg)
{
    log_lock.lock();
    proxy_log_file << id << msg << std::endl;
    log_lock.unlock();
}

void Log::writeMsg(int id, std::string msg1, std::string ip, std::string msg2)
{
    log_lock.lock();
    proxy_log_file << id << msg1 << ip << msg2 << std::endl;
    log_lock.unlock();
}

void Log::writeNote(std::string note)
{
    log_lock.lock();
    proxy_log_file << "no-id: NOTE " << note << std::endl;
    log_lock.unlock();
}

void Log::writeNote(int id, std::string note)
{
    log_lock.lock();
    proxy_log_file << id << ": NOTE " << note << std::endl;
    log_lock.unlock();
}

void Log::writeWarning(std::string warning)
{
    log_lock.lock();
    proxy_log_file << "no-id: WARNING " << warning << std::endl;
    log_lock.unlock();
}

void Log::writeWarning(int id, std::string warning)
{
    log_lock.lock();
    proxy_log_file << id << ": WARNING " << warning << std::endl;
    log_lock.unlock();
}

void Log::writeError(std::string error)
{
    log_lock.lock();
    proxy_log_file << "no-id: ERROR " << error << std::endl;
    log_lock.unlock();
}

void Log::writeError(int id, std::string error)
{
    log_lock.lock();
    proxy_log_file << id << ": ERROR " << error << std::endl;
    log_lock.unlock();
}

