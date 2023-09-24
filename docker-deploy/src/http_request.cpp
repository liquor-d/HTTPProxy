#include "http_request.hpp"

#include <cstring>
#include <exception>
#include <iostream>

void HttpRequest::parse_start_line()
{
  // start line includes an http method, the request target, and http version
  size_t start_line_pos = raw_content.find_first_of("\r\n");
  start_line = raw_content.substr(0, start_line_pos);
}

void HttpRequest::parse_method()
{
  size_t method_pos = start_line.find_first_of(" ");
  method = raw_content.substr(0, method_pos);
}

void HttpRequest::parse_host_and_port()
{
  size_t host_pos = raw_content.find("Host: ");
  if (host_pos != std::string::npos)
  {
    std::string host_raw = raw_content.substr(host_pos + 6);
    size_t host_line_end = host_raw.find_first_of("\r\n");
    std::string host_line = host_raw.substr(0, host_line_end);
    size_t port_start = host_line.find_first_of(":\r");
    if ((port_start != std::string::npos))
    {
      host = host_raw.substr(0, port_start);
      port = host_line.substr(port_start + 1);
    }
    else
    {
      host = host_line;
      port = "80";
    }
  }
}