#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include <time.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

class HttpResponse {
 private:
  std::string msg;
  std::string start_line;
  int max_age;
  std::string exp_str;
  // bool must_revalidate;

  // two validators
  // std::string etag;
  // std::string lastModified;

 public:
  HttpResponse() : msg("") {}
  HttpResponse(char * server_buf, int len);
  void init_response(char *server_buf, int mbyte_count);
  void parse_start_line(const char * first_part, int len);  // parse and fill start line                                       // get raw content size
  const std::string & get_msg() const;
  void set_msg(const std::string & set_msg);
  void parse_field(char * first_msg, int len);
  bool is_chunk();
  bool is_no_store();
  bool is_no_cache();
  const std::string & get_start_line() const;
};

#endif
