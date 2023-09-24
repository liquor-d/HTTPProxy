#ifndef __CLIENT_UTILS_HPP__
#define __CLIENT_UTILS_HPP__

/**
 * This class contains the utility functions used when the proxy is the client
 * that sends requests to the original server.
 */
class ClientUtils
{
public:
    int init_client(const char *hostname, const char *port);
};

#endif