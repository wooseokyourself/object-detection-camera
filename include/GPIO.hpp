#ifndef GPIO
#define GPIO

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