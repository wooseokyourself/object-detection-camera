#ifndef DEVICE
#define DEVICE

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <wiringPi.h>
#include <wiringSerial.h>

enum Pin {
    TASK_MODE_PIN = 20,
    RPI_OFF_PIN = 21,
    MODEM_PWR_PIN = 17,
    MODEM_STAT_PIN = 27
}

std::string SERIAL_PORT = "/dev/ttyS0";

namespace gpio {

void
init ();

bool
isAdminMode ();

void
shutdownRpi ();

void
powerOnModem ();

void
powerOffModem ();

}

namespace atcmd {

std::string
getRssi ();

void
post ();

void
__sendATcmd (const int fd, const char* cmd);

void
__readBuffer (const int fd);

}

#endif