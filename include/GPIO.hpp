#ifndef __GPIO__
#define __GPIO__

#include <iostream>
#include <wiringPi.h>

class GPIO {
public:
    GPIO (const int _rpiOffPin);
    ~GPIO ();

    void shutdownRpi ();

private:
    int rpiOffPin;
};

#endif