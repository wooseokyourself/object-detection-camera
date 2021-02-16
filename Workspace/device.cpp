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
    // serial
    int fd = serialOpen(SERIAL_PORT.c_str(), 115200);
    if  (fd == -1) {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
        return -1;
    }
    if (wiringPiSetupGpio() == -1) { // BCM
        fprintf(stderr, "Unable to start wiringPi: %s\n", strerror(errno));
        return -1;
    }

    // gpio
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

    std::string response;
    atcmd::__sendATcmd(fd, "ATE0\r");
    response = atcmd::__readBuffer(fd);
    std::cout << response << std::endl;
    atcmd::__sendATcmd(fd, "AT+CEREG=2\r");
    response = atcmd::__readBuffer(fd);
    std::cout << response << std::endl;
    
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
    std::string response = atcmd::__readBuffer(fd);
    std::cout << response << std::endl;
    return 0;
}

std::string
post (const int fd, std::string url, std::string imagePath) {
    std::cout << "[Configure the PDP context ID as 1]" << std::endl;
    atcmd::__sendATcmd(fd, "AT+QHTTPCFG=\"contextid\",1");
    std::cout << atcmd::__readBuffer(fd) << std::endl;

    std::cout << "[Query the state of context]" << std::endl;
    atcmd::__sendATcmd(fd, "AT+QIACT?");
    std::cout << atcmd::__readBuffer(fd) << std::endl;

    std::cout << "[Configure PDP context 1. APN is 'move.dataxs.mobi' for TATA]" << std::endl;
    atcmd::__sendATcmd(fd, "AT+QICSGP=1,1,\"move.dataxs.mobi\",\"\",\"\",1");
    std::cout << atcmd::__readBuffer(fd) << std::endl;
    
    std::cout << "[Active context 1]" << std::endl;
    atcmd::__sendATcmd(fd, "AT+QIACT=1");
    std::cout << atcmd::__readBuffer(fd) << std::endl;

    std::cout << "[Query the state of context]" << std::endl;
    atcmd::__sendATcmd(fd, "AT+QIACT?");
    std::cout << atcmd::__readBuffer(fd) << std::endl;

    std::cout << "[Set the URL which will be accessed]" << std::endl;
    const int urlLen = url.length();
    atcmd::__sendATcmd(fd, ("AT+QHTTPURL=" + std::to_string(urlLen)).c_str());
    if (atcmd::__readBuffer(fd) == "\r\nCONNECT\r\n")
        atcmd::__sendATcmd(fd, url.c_str());
    std::cout << atcmd::__readBuffer(fd) << std::endl;

    std::cout << "[Send HTTP POST request]" << std::endl;
    std::string bodyLength = "20";
    std::string maxInputBodyTime = "80";
    std::string maxResponseTime = "80";
    atcmd::__sendATcmd(fd, ("AT+QHTTPPOST="
                            + bodyLength + ","
                            + maxInputBodyTime + "," 
                            + maxResponseTime).c_str());
    if (atcmd::__readBuffer(fd) == "\r\nCONNECT\r\n")
        atcmd::__sendATcmd(fd, "Message=HelloQuectel");
    std::cout << atcmd::__readBuffer(fd) << std::endl;

    std::cout << "[Read HTTP response body and output it via UART]" << std::endl;
    atcmd::__sendATcmd(fd, "AT+QHTTPREAD=80");
    std::cout << atcmd::__readBuffer(fd) << std::endl;

/*
    std::string data = "POST / HTTP/1.1\n
                    Host: " + url + "\n
                    Content-Type: multipart/form-data;boundary=\"boundary\"
                    \n\n
                    --boundary\n
                    Content-Disposition: form-data; name=\"time\"
                    \n
                    --boundary\n
                    Content-Disposition: form-data; name=\"\"; filename=\"example.jpg\"
                    "

    std::string cmd;
    // Header
    cmd = "AT+QHTTPCFG=\"contenttype\",multipart/form-data"
    atcmd::__sendATcmd(fd, "AT+QHTTPCFG=")
*/
    return "end"; 
}

void
atcmd::__sendATcmd (const int fd, const char* cmd) {
    std::cout << "Pi) Send AT cmd: " << cmd << std::endl;
    // serialFlush(fd);
    serialPuts(fd, cmd);
    delay(3);
}

std::string
atcmd::__readBuffer (const int fd) {
    std::cout << "Pi) __readBuffer" << std::endl;
    int len = serialDataAvail(fd);
    if (len == -1)
        return "No data read";
    std::string buf(len, ' ');
    int i = 0;
    while (len = serialDataAvail(fd) > 0)
        buf[i++] = serialGetchar(fd);
    return buf;
}