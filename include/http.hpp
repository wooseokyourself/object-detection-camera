#ifndef HTTP
#define HTTP

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <curl/curl.h>

using namespace std;

namespace http {

bool
post (const string& url, 
      const string& time,
      const int& rssi,
      const int& battery, 
      const string& filename, 
      const string& filepath, 
      const long& timeoutsecs);

bool
post (const string& url, 
      const string& time,
      const int& rssi,
      const int& battery, 
      const long& timeoutsecs);

}

#endif