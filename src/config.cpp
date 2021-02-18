#include "../include/Config.hpp"

void Config::readFromJsonFile (const std::string filePath) {
    Json::Value root;
    Json::Reader reader;
    std::ifstream jsonFile(filePath, std::ifstream::binary);
    if (!reader.parse(jsonFile, root)) {
        std::cerr << "Config: " << "failed to parse" << reader.getFormattedErrorMessages() << std::endl;
        return;
    }
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
}