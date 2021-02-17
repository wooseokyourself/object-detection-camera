#ifndef BG96
#define BG96

#include <cstdlib>
#include "Serial.hpp"
#include "HttpPostFormData.hpp"

class BG96 : public Serial {
public:
    BG96 (const char* _port, const int baudRate);
    ~BG96 ();

    int getRssi();
    std::string postMultipart (const std::string host,
                               const std::string uri,
                               const HttpPostFormData& fields, 
                               const int timeoutSecs);

protected:
    void putATcmd (const char* cmd);
    void putATcmd (std::string cmd);
    void putATcmd (const char* cmd, const size_t len);
    void putATcmd (std::string cmd, const size_t len);
    std::string getResponse ();
    bool waitResponseUntil (const std::string expected, const int count);

};

#endif