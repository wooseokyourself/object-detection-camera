#ifndef __BG96__
#define __BG96__

#include <chrono>
#include <string>
#include <cstdlib>
#include "Serial.hpp"
#include "HttpPostFormData.hpp"

class BG96 : public Serial {
public:
    BG96 (const char* _port, const int baudRate);
    BG96 (const std::string _port, const int baudRate);
    ~BG96 ();

    int getRssi();
    std::string postMultipart (const std::string host,
                               const std::string uri,
                               const HttpPostFormData& fields, 
                               const int timeoutSecs);

public:
    void putATcmd (const char* cmd);
    void putATcmd (std::string cmd);
    void putATcmd (const char* cmd, const size_t len);
    void putATcmd (std::string cmd, const size_t len);
    std::string getResponse ();
    bool waitResponseUntil (const std::string expected, const int timeoutSecs);

};

#endif