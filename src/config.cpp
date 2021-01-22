#include "../include/config.hpp"

std::string
getISOCurrentTimestamp() {
    return date::format("%FT%TZ", date::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()));
}

std::string
getISOCurrentDate() {
    return getISOCurrentTimestamp().substr(0, 9);
}

Config::Config () {
    Json::Value root;
    Json::Reader reader;
    std::ifstream json("config/config.json", std::ifstream::binary);
    reader.parse(json, root);
    Json::Value DEVICE = root["DEVICE"];
    Json::Value SERVER = root["SERVER"];
    Json::Value YOLO = root["YOLO"];
    this->CONF_THRESH = YOLO["CONFIDENCE_THRESHOLD"].asDouble();
    this->NMS_THRESH = YOLO["NMS_THRESHOLD"].asDouble();
    this->RESIZE = YOLO["RESIZE"].asInt();
    this->URL = SERVER["URL"].asString() + SERVER["ENDPOINT"].asString() + DEVICE["ID"].asString();
}

Config::Config (std::string __jsonpath) {
    Json::Value root;
    Json::Reader reader;
    std::ifstream json(__jsonpath, std::ifstream::binary);
    reader.parse(json, root);
    Json::Value DEVICE = root["DEVICE"];
    Json::Value SERVER = root["SERVER"];
    Json::Value YOLO = root["YOLO"];
    this->CONF_THRESH = YOLO["CONFIDENCE_THRESHOLD"].asDouble();
    this->NMS_THRESH = YOLO["NMS_THRESHOLD"].asDouble();
    this->RESIZE = YOLO["RESIZE"].asInt();
    this->URL = SERVER["URL"].asString() + SERVER["ENDPOINT"].asString() + DEVICE["ID"].asString();
    
}

float
Config::yolo_confThresh () {
    return this->CONF_THRESH;
}

float
Config::yolo_nmsThresh () {
    return this->NMS_THRESH;
}

int
Config::yolo_resize () {
    return this->RESIZE;
}

std::string
Config::http_url () {
    return this->URL;
}