#ifndef DEVICE
#define DEVICE

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <wiringPi.h>
#include <wiringSerial.h>

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

int
init ();

void
__sendATcmd (const int fd, const char* cmd);

void
__readBuffer (const int fd);

}

#endif