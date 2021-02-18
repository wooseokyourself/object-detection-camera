#ifndef CONFIG
#define CONFIG

#include <fstream>
#include <string>
#include <jsoncpp/json/json.h>

#define DETECTING_MODE 0
#define PREVIEW_MODE 1

class Config {
public:
    void readFromJsonFile (const std::string filePath);
    void readFromJsonString (const std::string jsonString);
    std::string getID () const;
    float getConfThreshold () const;
    float getNmsThreshold () const;
    int getWidth () const;
    bool isPreviewMode () const;
    bool isDetectingMode () const;

private:
    void readJsonObject (Json::Value& root);

private:
    std::string ID;
    float CONF_THRESHOLD;
    float NMS_THRESHOLD;
    int WIDTH;
    int MODE;
};

#endif