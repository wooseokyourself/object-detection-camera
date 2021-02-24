#include "../include/NRF.hpp"

NRF::NRF () { }

NRF::NRF (const char* port, const int baudRate) : Serial(port, baudRate) {
    
}

NRF::NRF (const std::string port, const int baudRate) : Serial(port.c_str(), baudRate) {

}

NRF::~NRF () { }

int NRF::getBattery () const {
    return 100;
}

void NRF::setPowerInterval (const int intervalSecs) const {
    // send to nrf
}