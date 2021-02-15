#include "../include/config.hpp"

std::string
getISOCurrentTimestamp () {
    return date::format("%FT%TZ", date::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()));
}

std::string
convertToISOformat (std::string __timestamp) {
    return __timestamp.substr(0, 10);
}

Config::Config () {
    Json::Value root;
    Json::Reader reader;
    std::ifstream json("config/config.json", std::ifstream::binary);
    reader.parse(json, root);
    Json::Value DEVICE = root["DEVICE"];
    Json::Value SERVER = root["SERVER"];
    Json::Value YOLO = root["YOLO"];
    this->CONF_THRESH = std::stof(YOLO["CONFIDENCE_THRESHOLD"].asString());
    this->NMS_THRESH = std::stof(YOLO["NMS_THRESHOLD"].asString());
    this->RESIZE = std::stoi(YOLO["RESIZE"].asString());
    this->URL = SERVER["URL"].asString() + SERVER["ENDPOINT"].asString() + DEVICE["ID"].asString();
    this->HTTP_TIMEOUT_SECS = std::stof(SERVER["HTTP_TIMEOUT"].asString());
}

Config::Config (std::string __jsonpath) {
    Json::Value root;
    Json::Reader reader;
    std::ifstream json(__jsonpath, std::ifstream::binary);
    reader.parse(json, root);
    Json::Value DEVICE = root["DEVICE"];
    Json::Value SERVER = root["SERVER"];
    Json::Value YOLO = root["YOLO"];
    this->CONF_THRESH = std::stof(YOLO["CONFIDENCE_THRESHOLD"].asString());
    this->NMS_THRESH = std::stof(YOLO["NMS_THRESHOLD"].asString());
    this->RESIZE = std::stoi(YOLO["RESIZE"].asString());
    this->URL = SERVER["URL"].asString() + SERVER["ENDPOINT"].asString() + DEVICE["ID"].asString();
    this->HTTP_TIMEOUT_SECS = std::stof(SERVER["HTTP_TIMEOUT"].asString());
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

long
Config::http_timeout_secs () {
    return this->HTTP_TIMEOUT_SECS;
}