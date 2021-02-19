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
                               const int timeoutSecs) const;

public:
    void putATcmd (const char* cmd) const;
    void putATcmd (std::string cmd) const;
    void putATcmd (const char* cmd, const size_t len) const;
    void putATcmd (std::string cmd, const size_t len) const;
    std::string getResponse () const;
    bool waitResponseUntil (const std::string expected, const int timeoutSecs) const;
};

#endif