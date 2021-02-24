#ifndef __CONFIG__
#define __CONFIG__

#include <iostream>
#include <fstream>
#include <string>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/writer.h>

#define DETECTING_MODE 0
#define PREVIEW_MODE 1

class Config {
public:
    void readFromJsonFile (const std::string filePath);
    void readFromJsonString (const std::string jsonString);
    void write (const std::string filePath) const;
    std::string getID () const;
    float getConfThreshold () const;
    float getNmsThreshold () const;
    int getCaptureWidth () const;
    int getIntervalSecs () const;
    bool sendPictureAlways () const;

private:
    void readJsonObject (Json::Value& root);

private:
    std::string deviceId;
    int sendInterval;
    bool sendOnDetectedOnly;
    float confidenceThreshold;
    float nmsThreshold;
    int resizeResolution;
};

#endif
