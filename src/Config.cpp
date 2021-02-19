#include "../include/Config.hpp"

void Config::readFromJsonFile (const std::string filePath) {
    Json::Value root;
    Json::Reader reader;
    std::ifstream jsonFile(filePath, std::ifstream::binary);
    if (!reader.parse(jsonFile, root))
        std::cerr << "Config: " << "failed to parse" << reader.getFormattedErrorMessages() << std::endl;
    jsonFile.close();
    this->readJsonObject(root);
}

void Config::readFromJsonString (const std::string jsonString) {
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(jsonString.c_str(), root)) {
        std::cerr << "Config: " << "failed to parse" << reader.getFormattedErrorMessages() << std::endl;
        return;
    }
    this->readJsonObject(root);
}

void Config::write (const std::string filePath) const {
    Json::Value root;
    root["ID"] = this->ID;
    root["CONF_THRESHOLD"] = this->CONF_THRESHOLD;
    root["NMS_THRESHOLD"] = this->NMS_THRESHOLD;
    root["WIDTH"] = this->WIDTH;
    root["MODE"] = this->MODE;
    root["INTERVAL_SECS"] = this->INTERVAL_SECS;
    
    Json::StyledStreamWriter writer;
    std::ofstream jsonFile(filePath, std::ifstream::binary);
    writer.write(jsonFile, root);
    jsonFile.close();
}

std::string Config::getID () const {
    return this->ID;
}

float Config::getConfThreshold () const {
    return this->CONF_THRESHOLD;
}

float Config::getNmsThreshold () const {
    return this->NMS_THRESHOLD;
}

int Config::getWidth () const {
    return this->WIDTH;
}

int Config::getIntervalSecs () const {
    return this->INTERVAL_SECS;
}

bool Config::isPreviewMode () const {
    return this->MODE == PREVIEW_MODE ? true : false;
}

bool Config::isDetectingMode () const {
    return this->MODE == DETECTING_MODE ? true : false;
}

void Config::readJsonObject (Json::Value& root) {
    this->ID = root["ID"].asString();
    this->CONF_THRESHOLD = std::stof(root["CONF_THRESHOLD"].asString());
    this->NMS_THRESHOLD = std::stof(root["NMS_THRESHOLD"].asString());
    this->WIDTH = std::stoi(root["WIDTH"].asString());
    this->MODE = std::stoi(root["MODE"].asString());
    this->INTERVAL_SECS = std::stoi(root["INTERVAL_SECS"].asString());
}
