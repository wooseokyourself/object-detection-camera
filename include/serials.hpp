#ifndef SERIALS
#define SERIALS

#include <string>
#include <wiringPi.h>

const int TASK_MODE_PIN = 20;
const int RPI_OFF_PIN = 21;

class Serials {
public:
    Serials (const std::string port, const int baudRate);
    Serials (const int __taskModePin, const int __rpiOffPin, const std::string port, const int baudRate);
    bool isAdminMode ();
    void shutdownRpi ();
    int writeBootInterval (const int secs);
    float readBattery ();

private:
    int serialFd;
    int taskModePin;
    int rpiOffPin;
};

#endif