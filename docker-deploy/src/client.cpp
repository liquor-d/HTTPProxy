#include "client.hpp"

Client::Client(int id, std::string ip, int fd) : id(id), ip(ip), fd(fd){}

int Client::get_id() const {
    return id;
}

std::string Client::get_ip() const {
    return ip;
}

int Client::get_fd() const {
    return fd;
}