#include "../include/BG96.hpp"

BG96::BG96 (const char* _port, const int baudRate) : Serial(_port, baudRate) {
    // Wait until modem ready
    this->putATcmd("AT\r");
    this->waitResponseUntil("\r\nOK\r\n", 3);
    this->putATcmd("ATE0\r");
    this->waitResponseUntil("\r\nOK\r\n", 3);
    this->putATcmd("AT+CEREG=2\r");
    this->waitResponseUntil("\r\nOK\r\n", 3);
}

BG96::BG96 (const std::string _port, const int baudRate) : Serial(_port.c_str(), baudRate) {
    // Wait until modem ready
    this->putATcmd("AT\r");
    this->waitResponseUntil("\r\nOK\r\n", 3);
    this->putATcmd("ATE0\r");
    this->waitResponseUntil("\r\nOK\r\n", 3);
    this->putATcmd("AT+CEREG=2\r");
    this->waitResponseUntil("\r\nOK\r\n", 3);
}

BG96::~BG96 () { }

int BG96::getRssi() {
    this->putATcmd("AT+CSQ\r");
    std::string response = this->getResponse();
    int colonIdx = -1;
    for (int i = 0 ; i < response.length() ; i ++) {
        if (response[i] == ':') {
            colonIdx = i;
            break;
        }
    }
    if (colonIdx == -1)
        return -1;
    // response form = "+CSQ: 31,99"
    return std::stoi(response.substr(colonIdx + 2, colonIdx + 3));
}

std::string BG96::postMultipart (const std::string host,
                                 const std::string uri,
                                 const HttpPostFormData& fields, 
                                 const int timeoutSecs) {

    // multipart/form-data boundary
    std::string boundary = "flesruoykoesoow";

    this->putATcmd("AT+QHTTPCFG=\"contextid\",1\r");
    this->waitResponseUntil("\r\nOK\r\n", timeoutSecs);

    this->putATcmd("AT+QHTTPCFG=\"contenttype\",3\r"); // 3: multipart/form-data
    this->waitResponseUntil("\r\nOK\r\n", timeoutSecs);

    this->putATcmd("AT+QHTTPCFG=\"requestheader\",1\r");
    this->waitResponseUntil("\r\nOK\r\n", timeoutSecs);

    const std::string fullUrl = "http://" + host + uri;
    this->putATcmd("AT+QHTTPURL=" + std::to_string(fullUrl.length()) + "\r");
    this->waitResponseUntil("\r\nCONNECT\r\n", timeoutSecs);
    this->putATcmd(fullUrl);
    this->waitResponseUntil("\r\nOK\r\n", timeoutSecs);

    // Construct body of request form
    std::string body;
    for (int i = 0 ; i < fields.size() ; i ++) {
        std::string contentType = fields.getContentType(i);
        std::string contentName = fields.getContentName(i);
        std::string content = fields.getContent(i);
        if (contentType == "text/plain") {
            body += ("--" + boundary + "\r\n" + 
                    "Content-Type: " + contentType + "\r\n" + 
                    "Content-Disposition: form/data; name=\"" + contentName + "\"\r\n" + 
                    "\r\n" + 
                    content + "\r\n"
            );
        }
        else if (contentType == "image/jpeg") {
            body += ("--" + boundary + "\r\n" + 
                    "Content-Type: " + contentType + "\r\n" + 
                    "Content-Disposition: form/data; name=\"files\"; filename=\"" + contentName + "\"\r\n" + 
                    "\r\n" + 
                    content + "\r\n"
            );
        }
    }
    body += ("--" + boundary + "--\r\n");

    // Construct header of request form
    std::string header = (
        std::string("POST ") + uri + " HTTP/1.1\r\n" + 
        "Host: " + host + "\r\n" + 
        "Content-Length: " + std::to_string(body.length()) + "\r\n" +  
        "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n" + 
        "\r\n"
    );

    std::string maxInputBodyTime = std::to_string(timeoutSecs);
    std::string maxResponseTime = std::to_string(timeoutSecs);
    this->putATcmd("AT+QHTTPPOST="
                    + std::to_string(header.length() + body.length()) + ","
                    + maxInputBodyTime + "," 
                    + maxResponseTime + "\r");
    this->waitResponseUntil("\r\nCONNECT\r\n", timeoutSecs);
    this->putATcmd(header);
    this->putATcmd(body, body.length());
    this->waitResponseUntil("\r\nOK\r\n", timeoutSecs);

    this->putATcmd("AT+QHTTPREAD=80\r");
    std::string response = this->getResponse();
    while (response.find("703") != -1) // 703: HTTP BUSY
        response = this->getResponse();
    return response;
}

void BG96::putATcmd (const char* cmd) {
    Serial::puts(cmd);
}

void BG96::putATcmd (std::string cmd) {
    std::cout << "[Send AT command]" << std::endl;
    std::cout << cmd << std::endl;
    Serial::puts(cmd.c_str());
}

void BG96::putATcmd (const char* cmd, const size_t len) {
    Serial::puts(cmd, len);
}

void BG96::putATcmd (std::string cmd, const size_t len) {
    std::cout << "[Send AT command]" << std::endl;
    std::cout << cmd << std::endl;
    Serial::puts(cmd.c_str(), len);
}

std::string BG96::getResponse () {
    int len = Serial::remaining();
    if (len < 0)
        return "Error";
    else if (len == 0)
        return "No data read";
    else {
        std::string buf(len, ' ');
        int i = 0;
        while (len = Serial::remaining() > 0)
            buf[i++] = Serial::getchar();
        return buf;
    }
}

bool BG96::waitResponseUntil (const std::string expected, const int timeoutSecs) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    while (true) {
        std::string response = this->getResponse();
        if (response == expected)
            return true;
        else if (response == "\r\nERROR\r\n")
            return false;
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() > timeoutSecs)
            return false;
        usleep(500000); // 0.5 ms
    }
}