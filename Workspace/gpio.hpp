#ifndef GPIO
#define GPIO

#include <string>
#include <wiringPi.h>

const int TASK_MODE_PIN = 20;
const int RPI_OFF_PIN = 21;
const int MODEM_PWR_PIN = 17;
const int MODEM_STAT_PIN = 27;

namespace gpio {

void
init ();

bool
isAdminMode ();

void
shutdownRpi ();

}

namespace atcmd {

void
powerOnModem ();

void
powerOffModem ();

std::string
getRssi ();

void
post ();

}

#endif