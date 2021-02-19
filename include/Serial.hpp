#ifndef SERIAL
#define SERIAL

/**
 * reference: https://github.com/WiringPi/WiringPi/blob/master/wiringPi/wiringSerial.c
 */

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstdarg>
#include <cstring>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

class Serial {
public:
    Serial (const char* _port, const int baudRate);
    ~Serial ();

protected:
    void flush ();
    int remaining ();
    void puts (const char* s);
    void puts (const char* s, const size_t len);
    int getchar ();

private:
    int fd;
};

#endif