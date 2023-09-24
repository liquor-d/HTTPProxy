all: main

main: main.cpp proxy.cpp proxy.hpp server_utils.cpp client_utils.cpp client.cpp http_request.cpp http_response.cpp cache.cpp log.cpp time.cpp
	g++ --std=c++11 -g -o main main.cpp proxy.cpp server_utils.cpp client_utils.cpp client.cpp http_request.cpp http_response.cpp cache.cpp log.cpp time.cpp -lpthread

test_boost: test_boost.cpp
	g++ -o test_boost test_boost.cpp

.PHONY:
	clean
clean:
	rm -rf *.o *~ *# main test_boost