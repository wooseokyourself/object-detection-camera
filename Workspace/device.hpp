#ifndef DEVICE
#define DEVICE

#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <error.h>
#include <iostream>
#include <string>
#include <wiringPi.h>
#include <wiringSerial.h>

int
deviceInit ();

namespace gpio {

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
getRSSI (const int fd);

void
__sendATcmd (const int fd, const char* cmd);

std::string
__readBuffer (const int fd);

}

#endif