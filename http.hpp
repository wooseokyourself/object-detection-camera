#ifndef HTTP
#define HTTP

#include <cstdio>
#include <cstring>
#include <vector>
#include <curl/curl.h>

using namespace std;

namespace http {

bool
postImage (const string& time,
           const int& rssi,
           const int& battery, 
           const char* imgBuffPtr, 
           const long& imgBuffSize);

bool
postWithoutImage (const string& time,
                  const int& rssi,
                  const int& battery);

}

#endif