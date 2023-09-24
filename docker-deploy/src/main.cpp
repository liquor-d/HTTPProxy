#include <cstdlib>
#include <pthread.h>

#include "proxy.hpp"

int main(int argc, char ** argv) {
  const char * port = "12345";
  Proxy * http_proxy_server = new Proxy(port);
  http_proxy_server->start();
  return EXIT_SUCCESS;
}
