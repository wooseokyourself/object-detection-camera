#ifndef __NRF__
#define __NRF__

#include <iostream>
#include <chrono>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include "Serial.hpp"

class NRF : public Serial {
public:
    NRF (); // just leaving now
    NRF (const char* port, const int baudRate);
    NRF (const std::string port, const int baudRate);
    ~NRF ();

    int getBattery () const;
    void setPowerInterval (const int intervalSecs) const;
};

#endif