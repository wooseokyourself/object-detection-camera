#ifndef DEVICE
#define DEVICE

#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <error.h>
#include <iostream>
#include <fstream>
#include <iterator>
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

std::string
post (const int fd, const std::string url, const int tryout);

void
customPost (const int fd, const std::string host, const std::string url, const int tryout);

void
__sendATcmd (const int fd, const char* cmd);

std::string
__readBuffer (const int fd);

bool
__readBufferUntil (const int fd, const std::string expected, const int count);

}

#endif