#ifndef __HTTP_REQUEST_HPP__
#define __HTTP_REQUEST_HPP__

#include <string>

class HttpRequest
{

public:
    std::string raw_content;
    std::string start_line;
    std::string host;
    std::string port;
    std::string method;

public:
    HttpRequest(std::string request) : raw_content(request)
    {
        parse_start_line();
        parse_method();
        parse_host_and_port();
    }
    void parse_start_line();
    void parse_method();
    void parse_host_and_port();
};

#endif