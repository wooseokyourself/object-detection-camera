#include "../include/Config.hpp"

Config::Config () : result(1), previewMode(false) { }

/**
 * @brief Read json from .json file and save it in this object.
 * @param filePath The path of .json file.
 */
void Config::readFromJsonFile (const std::string filePath) {
    Json::Value root;
    Json::Reader reader;
    std::ifstream jsonFile(filePath, std::ifstream::binary);
    if (!reader.parse(jsonFile, root)) {
        std::cerr << "Config: " << "readFromJsonFile() - failed to parse" << reader.getFormattedErrorMessages() << std::endl;
        return;
    }
    jsonFile.close();
    this->readJsonObject(root);
}

/**
 * @brief Read json from string and save it in this object.
 * @param jsonString The string formed json. ex) {"time"="2021-01-08T13:41:21.046Z"}
 */
void Config::readFromJsonString (const std::string jsonString) {
    const int l = jsonString.find("{");
    const int r = jsonString.find("}");
    if (l == -1 || r == -1) {
        std::cerr << "Config: " << "readFromJsonString - failed to parse" << " because jsonString has no json form" << std:: endl;
        return;
    }
    std::string jsonStringCpy;
    for (int i = l ; i <= r ; i ++)
        jsonStringCpy += jsonString[i];
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(jsonStringCpy.c_str(), root)) {
        std::cerr << "Config: " << "readFromJsonString - failed to parse" << reader.getFormattedErrorMessages() << std::endl;
        return;
    }
    this->readJsonObject(root);
}

/**
 * @brief Write json from this object to file. Not checking for invalid values.
 * @param filePath The path of .json file to write.
 */
void Config::write (const std::string filePath) const {
    Json::Value root;
    root["deviceId"] = this->deviceId;
    root["sendInterval"] = this->sendInterval;
    root["sendOnDetectedOnly"] = this->sendOnDetectedOnly;
    root["confidenceThreshold"] = this->confidenceThreshold;
    root["nmsThreshold"] = this->nmsThreshold;
    root["resizeResolution"] = this->resizeResolution;
    root["result"] = this->result;
    
    Json::StyledStreamWriter writer;
    std::ofstream jsonFile(filePath);
    writer.write(jsonFile, root);
    jsonFile.close();
}

std::string Config::getID () const {
    return this->deviceId;
}

float Config::getConfThreshold () const {
    return this->confidenceThreshold;
}

float Config::getNmsThreshold () const {
    return this->nmsThreshold;
}

int Config::getCaptureWidth () const {
    return this->resizeResolution;
}

int Config::getIntervalSecs () const {
    return this->sendInterval;
}

bool Config::sendPictureAlways () const {
    return this->sendOnDetectedOnly;
}

bool Config::isPreviewMode () const {
    return this->previewMode;
}

/**
 * @brief Parse json object to member variables. Preview or Detecting mode is distinguished 
 *          through checking the fields of the json object.
 * @param root Json object.
 */
void Config::readJsonObject (Json::Value& root) {
    if (root["result"].asString() != "1") {
        std::cerr << "Config: " << "readJsonObject - failed to parse because of {\"result\" : 0}" << std::endl;
        return;
    }
    if (root.isMember("deviceId")) { // Detecting Mode
        this->deviceId = root["deviceId"].asString();
        this->sendInterval = std::stoi(root["sendInterval"].asString());
        this->sendOnDetectedOnly = root["sendOnDetectedOnly"].asString() == "true" ? true : false;
        this->confidenceThreshold = std::stof(root["confidenceThreshold"].asString());
        this->nmsThreshold = std::stof(root["nmsThreshold"].asString());
        this->resizeResolution = std::stoi(root["resizeResolution"].asString());
    }
    else { // Preview Mode
        this->previewMode = root["previewMode"].asString() == "true" ? true : false;
    }
}
