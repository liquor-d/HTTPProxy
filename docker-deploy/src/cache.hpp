#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include "http_response.hpp"

class Node {
  public:
    std::string start_line;
    HttpResponse response;
    Node *next;
    Node *prev;

    Node(){}
    Node(std::string start_line, HttpResponse response): start_line(start_line), response(response), next(NULL), prev(NULL) {}
};

class Cache{
  private:
    Node * head;
    Node * tail;
    int num_cache;

  public:
    Cache();
    HttpResponse find_cache(std::string start_line);
    void add_cache(std::string start_line, HttpResponse response);
    void remove_cache(std::string start_line);
    bool check_cache(HttpResponse &response, std::string req_start_line);
    void print();
};


#endif