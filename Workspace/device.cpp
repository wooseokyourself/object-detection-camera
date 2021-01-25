#include "device.hpp"

void
gpio::init () {
    wiringPiSetupGpio(); // BCM
    pinMode(TASK_MODE_PIN, INPUT);
    pinMode(RPI_OFF_PIN, OUTPUT);    
    pinMode(MODEM_PWR_PIN, OUTPUT);
    pinMode(MODEM_STAT_PIN, INPUT);
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
atcmd::init () {
    int fd = serialOpen(SERIAL_PORT, 115200);
    if (fd != -1)
        serialFlush(fd);
    int cnt = 0, ret;
    do {
        __sendATcmd(fd, "AT\r"); // at 커맨드에 맞게 문자열 수정 필요
        if (ret == 0)
            break;
    } while (cnt < 10);

    atcmd::__sendATcmd(fd, "ATE0\r");
    atcmd::__sendATcmd(fd, "AT+CEREG=2");

    return fd;
}

void
atcmd::__sendATcmd (const int fd, const char* cmd) {
    serialFlush(fd);
    serialPuts(fd, cmd);
}

void
atcmd::__readBuffer (const int fd) {
    while (true) {
        char buffer[100];
        ssize_t length = read(fd, &buffer, sizeof(buffer));
        if (length == -1) {
            std::cerr << "Error reading from serial port" << std::endl;
            break;
        }
        else if (length == 0) {
            std::cerr << "No more data" << endl;
            break;
        }
        else {
            buffer[length] = '\0';
            std::cout << buffer; // read serial data
        }
    }
    std::cout << std::endl;
}