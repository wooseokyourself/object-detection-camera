#ifndef CONFIG
#define CONFIG

#include <string>
#include <jsoncpp/json/json.h>

class Config {
public:
    Config ();
    Config (std::string __jsonpath);
    float yolo_confThresh ();
    float yolo_nmsThresh ();
    int yolo_resize ();
    std::string http_url ();

private:
    float CONF_THRESH;
    float NMS_THRESH;
    int RESIZE;
    std::string URL;
};

#endif