#include "../include/gpio.hpp"

Gpio::Gpio () {
    this->taskModePin = 20;
    this->rpiOffPin = 21;
    wiringPiSetupGpio(); // BCM
    pinMode(this->taskModePin, INPUT);
    pinMode(this->rpiOffPin, OUTPUT);    
}

Gpio::Gpio (int __taskModePin, int __rpiOffPin) {
    this->taskModePin = __taskModePin;
    this->rpiOffPin = __rpiOffPin;
    wiringPiSetupGpio(); // BCM
    pinMode(this->taskModePin, INPUT);
    pinMode(this->rpiOffPin, OUTPUT);
}

bool
Gpio::isAdminMode() {
    return digitalRead(this->taskModePin) == 1 ? true : false;
}

void 
Gpio::shutdownRpi() {
    digitalWrite(this->rpiOffPin, HIGH);
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
}