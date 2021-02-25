#include "../include/GPIO.hpp"

GPIO::GPIO (const int _rpiOffPin) : rpiOffPin(_rpiOffPin) {
    if (wiringPiSetupGpio() == -1) // BCM
        std::cerr << "GPIO: " << "unable to start wiringPi" << std::endl;
    pinMode(this->rpiOffPin, OUTPUT);
}

void GPIO::shutdownRpi () {
    digitalWrite(this->rpiOffPin, HIGH);
    delay(10000);
}