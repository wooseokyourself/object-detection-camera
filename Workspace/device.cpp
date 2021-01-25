#include "device.hpp"

enum Pin {
    TASK_MODE_PIN = 20,
    RPI_OFF_PIN = 21,
    MODEM_PWR_PIN = 17,
    MODEM_STAT_PIN = 27
};

const std::string SERIAL_PORT = "/dev/ttyS0";

int
deviceInit () {
    int fd = serialOpen(SERIAL_PORT.c_str(), 115200);
    if  (fd == -1) {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
        return -1;
    }
    if (wiringPiSetupGpio() == -1) { // BCM
        fprintf(stderr, "Unable to start wiringPi: %s\n", strerror(errno));
        return -1;
    }
    pinMode(TASK_MODE_PIN, INPUT);
    pinMode(RPI_OFF_PIN, OUTPUT);    
    pinMode(MODEM_PWR_PIN, OUTPUT);
    pinMode(MODEM_STAT_PIN, INPUT);
    
    int cnt = 0, ret;
    do {
        atcmd::__sendATcmd(fd, "AT\r"); // at 커맨드에 맞게 문자열 수정 필요
        std::string ret = atcmd::__readBuffer(fd);
        std::cout << ret << std::endl;
        std::cout << "ret size = " << ret.size() << std::endl;
        if (ret == "\r\nOK\r\n")
            break;
        ++cnt;
    } while (cnt < 10);

    atcmd::__sendATcmd(fd, "ATE0\r");
    atcmd::__readBuffer(fd);
    atcmd::__sendATcmd(fd, "AT+CEREG=2\r");
    atcmd::__readBuffer(fd);
    
    return fd;
}

bool
gpio::isAdminMode() {
    return digitalRead(TASK_MODE_PIN) == 1 ? true : false;
}

void 
gpio::shutdownRpi() {
    digitalWrite(RPI_OFF_PIN, HIGH);
    delay(10000);
}

void
gpio::powerOnModem () {
    digitalWrite(MODEM_PWR_PIN, HIGH);
}

void
gpio::powerOffModem () {
    digitalWrite(MODEM_PWR_PIN, LOW);
}

int
atcmd::getRSSI (const int fd) {
    atcmd::__sendATcmd(fd, "AT+CSQ\r");
    std::string ret = atcmd::__readBuffer(fd);
    std::cout << ret << std::endl;
    return 0;
}

void
atcmd::__sendATcmd (const int fd, const char* cmd) {
    std::cout << "Pi) Send AT cmd: " << cmd << std::endl;
    serialFlush(fd);
    serialPuts(fd, cmd);
}

std::string
atcmd::__readBuffer (const int fd) {
    std::cout << "Pi) __readBuffer" << std::endl;
    char buf[1024] = {0};
    ssize_t readBytes = 0;
    while (int r = read(fd, buf + readBytes, sizeof(buf)) > 0) {
        readBytes += r;
        // printf("r=%d, readBytes=%d\n", r, readBytes);
    }
    // printf("\n");
    return std::string(buf);
}