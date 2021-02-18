#include "../include/GPIO.hpp"

GPIO::GPIO (const int _rpiOffPin) : rpiOffPin(_rpiOffPin) {
    if (wiringPiSetupGpio() == -1) { // BCM
        fprintf(stderr, "Unable to start wiringPi: %s\n", strerror(errno));
        return -1;
    }
    pinMode(rpiOffPin, OUTPUT);
}

GPIO::~GPIO () {
    digitalWrite(rpiOffPin, HIGH);
    delay(10000);
}