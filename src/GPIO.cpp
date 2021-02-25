#include "../include/GPIO.hpp"

GPIO::GPIO (const int _rpiOffPin, const int _rpiModePin) : rpiOffPin(_rpiOffPin), rpiModePin(_rpiModePin) {
    if (wiringPiSetupGpio() == -1) // BCM
        std::cerr << "GPIO: " << "unable to start wiringPi" << std::endl;
    pinMode(this->rpiOffPin, OUTPUT);
    pinMode(this->rpiModePin, INPUT);
}

/**
 * @brief Write HIGH into GPIO::rpiOffPin pin for shutting down Raspberry Pi.
 */
void GPIO::shutdownRpi () {
    digitalWrite(this->rpiOffPin, HIGH);
    delay(10000);
}

/**
 * @brief Read GPIO::rpiModePin.
 * @return true if GPIO::rpiModePin pin is set 0.
 */
bool GPIO::isDetectingMode () {
    return !digitalRead(this->rpiModePin);
}

/**
 * @brief Write LOW into GPIO::rpiModePin pin.
 */
void GPIO::setDetectingMode () {
    pinMode(this->rpiModePin, OUTPUT);
    digitalWrite(this->rpiModePin, LOW);
    delay(1000);
    pinMode(this->rpiModePin, INPUT);
    while (!this->isDetectingMode())
        digitalWrite(this->rpiOffPin, LOW);
}