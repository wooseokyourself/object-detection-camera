#ifndef CONFIG
#define CONFIG

#include <fstream>
#include <string>
#include <jsoncpp/json/json.h>
#include "../include/date.hpp"

std::string
getISOCurrentTimestamp();

std::string
getISOCurrentDate();

class Config {
public:
    Config ();
    Config (std::string __jsonpath);
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