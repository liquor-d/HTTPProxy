#include "client_utils.hpp"

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

int ClientUtils::init_client(const char *hostname, const char *port)
{
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0)
    {
        // TODO!!!
        // throw runtime_error(std::string("Error: cannot get address info for host (") +
        //                     hostname + "," + port + ")");
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
    }

    socket_fd = socket(host_info_list->ai_family,
                       host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1)
    {
        // TODO!!!
        // throw runtime_error(std::string("Error: cannot create socket  (") + hostname + ", " +
        //                     port + ")");
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
    }

    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)
    {
        // TODO!!!
        // throw runtime_error(std::string("Error: cannot connect to socket  (") + hostname +
        //                     ", " + port + ")");
        std::cerr << "Error: cannot connect to socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
    }
    std::cout << "Client is connected to the proxy server." << std::endl;
    freeaddrinfo(host_info_list);
    return socket_fd;
}
