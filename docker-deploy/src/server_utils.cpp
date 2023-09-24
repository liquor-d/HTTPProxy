#include "server_utils.hpp"

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

int ServerUtils::init_server(const char *port)
{
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname = NULL;

    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0)
    {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << " ," << port << ")" << std::endl;
        return -1;
    }

    // TODO!!!
    // if (strcmp(port, "") == 0)
    // {
    //     struct sockaddr_in *addr_in = (struct sockaddr_in *)(host_info_list->ai_addr);
    //     addr_in->sin_port = 0;
    // }

    socket_fd = socket(host_info_list->ai_family,
                       host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1)
    {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
    }

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)
    {
        std::cerr << "Error: cannot bind socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
    }

    status = listen(socket_fd, 100);
    if (status == -1)
    {
        std::cerr << "Error: cannot listen on socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
    }

    freeaddrinfo(host_info_list);
    return socket_fd;
}

int ServerUtils::accept_connection(int socket_fd, std::string &ip)
{
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connect_fd;

    client_connect_fd =
        accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connect_fd == -1)
    {
        // TODO!!!
        // throw runtime_error("Error: cannot accept connection on socket");
        std::cerr << "Error: cannot accept connection on socket" << std::endl;
        return -1;
    }
    struct sockaddr_in *addr = (struct sockaddr_in *)&socket_addr;
    ip = inet_ntoa(addr->sin_addr);

    return client_connect_fd;
}