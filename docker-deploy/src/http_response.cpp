#include "http_response.hpp"

HttpResponse::HttpResponse(char *server_buf, int mbyte_count)
{
  init_response(server_buf, mbyte_count);
}

void HttpResponse::init_response(char *server_buf, int mbyte_count)
{
  parse_start_line(server_buf, mbyte_count);
  set_msg(std::string(server_buf, mbyte_count));
  parse_field(server_buf, mbyte_count);
}

void HttpResponse::parse_start_line(const char *first_part, int len)
{
  std::string first_part_str(first_part, len);
  size_t pos = first_part_str.find_first_of("\r\n");
  start_line = first_part_str.substr(0, pos);
}

const std::string &HttpResponse::get_msg() const
{
  return msg;
}

void HttpResponse::set_msg(const std::string &set_msg)
{
  msg = set_msg;
}

void HttpResponse::parse_field(char *first_msg, int len)
{
  std::string msg(first_msg, len);
  size_t date_pos;
  if ((date_pos = msg.find("Date: ")) != std::string::npos)
  {
    size_t GMT_pos = msg.find(" GMT");
    std::string date_str = msg.substr(date_pos + 6, GMT_pos - date_pos - 6);
    // response_time.init(date_str);
  }
  size_t max_age_pos;
  if ((max_age_pos = msg.find("max-age=")) != std::string::npos)
  {
    size_t max_age_end_pos = msg.find("\r\n", max_age_pos + 9);
    std::string max_age_str = msg.substr(max_age_pos + 9, max_age_end_pos - max_age_pos - 9);
    max_age = atoi(max_age_str.c_str());
    std::cout << "\n\nmax-age: " << max_age_str << ", int: " << max_age << std::endl;
  }
  size_t expire_pos;
  if ((expire_pos = msg.find("Expires: ")) != std::string::npos)
  {
    size_t GMT_pos = msg.find(" GMT", expire_pos);
    exp_str = msg.substr(expire_pos + 9, GMT_pos - expire_pos - 9);
    // expire_time.init(exp_str);
  }
  // size_t nocatch_pos;
  // if ((nocatch_pos = msg.find("no-cache")) != std::string::npos) {
  //   no_cache = true;
  // }
  // size_t mustrevalidate_pos;
  // if ((mustrevalidate_pos = msg.find("must-revalidate")) != std::string::npos) {
  //   must_revalidate = true;
  // }

  // size_t etag_pos;
  // if ((etag_pos = msg.find("ETag: ")) != std::string::npos) {
  //   size_t etag_end = msg.find("\r\n", etag_pos + 6);
  //   etag = msg.substr(etag_pos + 6, etag_end - etag_pos - 6);
  // }
  // size_t lastmodified_pos;
  // if ((lastmodified_pos = msg.find("Last-Modified: ")) != std::string::npos) {
  //   size_t lastmodified_end = msg.find("\r\n", lastmodified_pos + 15);
  //   lastModified =
  //       msg.substr(lastmodified_pos + 15, lastmodified_end - lastmodified_pos - 15);
  // }
}

bool HttpResponse::is_chunk()
{
  if (msg.find("chunked") != std::string::npos)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool HttpResponse::is_no_store()
{
  size_t nostore_pos;
  if (msg.find("no-store") != std::string::npos)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool HttpResponse::is_no_cache()
{
  size_t nocache_pos;
  if (msg.find("no-cache") != std::string::npos)
  {
    return true;
  }
  else
  {
    return false;
  }
}

const std::string & HttpResponse::get_start_line() const {
  return start_line;
}