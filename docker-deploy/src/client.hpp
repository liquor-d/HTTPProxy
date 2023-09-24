#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <string>

class Client
{
private:
    int id;
    std::string ip;
    int fd;

public:
    Client(int id, std::string ip, int fd);
    int get_id() const;
    std::string get_ip() const;
    int get_fd() const;
};

#endif