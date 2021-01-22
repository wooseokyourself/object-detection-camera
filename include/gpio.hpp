#ifndef GPIO
#define GPIO

#include <wiringPi.h>
#include <chrono>
#include <thread>

const int TASK_MODE_PIN = 20;
const int RPI_OFF_PIN = 21;

class Gpio {
public:
    Gpio ();
    Gpio (int __taskModePin, int __rpiOffPin);
    bool isAdminMode ();
    void shutdownRpi ();

private:
    int taskModePin;
    int rpiOffPin;
};

#endif