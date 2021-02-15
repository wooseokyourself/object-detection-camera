#ifndef CONFIG
#define CONFIG

#include <fstream>
#include <string>
#include <jsoncpp/json/json.h>
#include <wiringPi.h>
#include "../include/date.hpp"

std::string
getISOCurrentTimestamp ();

std::string
convertToISOformat (std::string __timestamp);

class Config {
public:
    void readFromJsonFile (const std::string filePath);
    void readFromUart (const char* serialPath, const int baudRate);
    float yolo_confThresh ();
    float yolo_nmsThresh ();
    int yolo_resize ();
    std::string http_url ();
    long http_timeout_secs ();

private:
    float CONF_THRESH;
    float NMS_THRESH;
    int RESIZE;
    std::string URL;
    long HTTP_TIMEOUT_SECS;
};

#endif