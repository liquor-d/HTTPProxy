#include <iostream>
#include <boost/array.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <pthread.h>

using namespace std;
int main(){
  boost::array<int, 4> arr = {{1,2,3,4}};
  cout << "hi" << arr[0] << endl;
  return 0;
}