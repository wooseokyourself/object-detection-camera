#include "../include/Config.hpp"

Config::Config () : result(1) { }

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

void Config::readFromJsonString (const std::string jsonString) {
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(jsonString.c_str(), root)) {
        std::cerr << "Config: " << "readFromJsonString - failed to parse" << reader.getFormattedErrorMessages() << std::endl;
        return;
    }
    this->readJsonObject(root);
}

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
    std::ofstream jsonFile(filePath, std::ifstream::binary);
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

void Config::readJsonObject (Json::Value& root) {
    if (root["result"].asString() != "1") {
        std::cerr << "Config: " << "readJsonObject - failed to parse because of {\"result\" : 0}" << std::endl;
        return;
    }
    this->deviceId = root["deviceId"].asString();
    this->sendInterval = std::stoi(root["sendInterval"].asString());
    this->sendOnDetectedOnly = root["sendOnDetectedOnly"].asString() == "true" ? true : false;
    this->confidenceThreshold = std::stof(root["confidenceThreshold"].asString());
    this->nmsThreshold = std::stof(root["nmsThreshold"].asString());
    this->resizeResolution = std::stoi(root["resizeResolution"].asString());
}
