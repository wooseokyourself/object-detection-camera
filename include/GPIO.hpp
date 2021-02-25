#ifndef __GPIO__
#define __GPIO__

#include <iostream>
#include <wiringPi.h>

class GPIO {
public:
    GPIO (const int _rpiOffPin, const int _rpiModePin);
    void shutdownRpi ();
    bool isDetectingMode ();
    void setDetectingMode ();

private:
    int rpiOffPin;
    int rpiModePin;
};

#endif