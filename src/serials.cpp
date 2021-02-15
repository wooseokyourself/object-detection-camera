#include "../include/serials.hpp"

Serials::Serials (const std::string port, const int baudRate) {
    this->taskModePin = 20;
    this->rpiOffPin = 21;
    wiringPiSetupGpio(); // BCM
    pinMode(this->taskModePin, INPUT);
    pinMode(this->rpiOffPin, OUTPUT);
}

Serials::Serials (const int __taskModePin, const int __rpiOffPin, const std::string port, const int baudRate) {
    this->taskModePin = __taskModePin;
    this->rpiOffPin = __rpiOffPin;
    wiringPiSetupGpio(); // BCM
    pinMode(this->taskModePin, INPUT);
    pinMode(this->rpiOffPin, OUTPUT);
    
    this->serialFd = serialOpen(port, baudRate);
    if (serialFd) // 여기서부터 작성하셈!
}


bool
Serials::isAdminMode() {
    return digitalRead(this->taskModePin) == 1 ? true : false;
}

void 
Serials::shutdownRpi() {
    digitalWrite(this->rpiOffPin, HIGH);
    delay(10000);
}

/// @brief 라즈베리파이의 부팅간격 시간(초) 설정
/// @param secs 초
void
Serials::writeBootInterval (const int secs) {

}

/// @brief NRF에 부착된 배터리를 읽은 뒤 리턴
void
Serials::readBattery () {
    
}