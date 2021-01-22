#include "../include/config.hpp"

Config::Config () {
    Json::Value root;
    Json::Reader reader;
    ifstream json("config/config.json", ifstream::binary);
    reader.parser(json, root);
    Json::Value DEVICE = root["DEVICE"];
    Json::Value SERVER = root["SERVER"];
    Json::Value YOLO = root["YOLO"];
    this->CONF_THRESH = YOLO["CONFIDENCE_THRESHOLD"].asFloat();
    this->NMS_THRESH = YOLO["NMS_THRESHOLD"].asFloat();
    this->RESIZE = YOLO["RESIZE"].asInt();
    this->URL = SERVER["URL"].asString() + SERVER["ENDPOINT"].asString() + DEVICE["ID"].asString();
}

Config::Config (std::string __jsonpath) {
    Json::Value root;
    Json::Reader reader;
    ifstream json(__jsonpath, ifstream::binary);
    reader.parser(json, root);
    Json::Value DEVICE = root["DEVICE"];
    Json::Value SERVER = root["SERVER"];
    Json::Value YOLO = root["YOLO"];
    this->CONF_THRESH = YOLO["CONFIDENCE_THRESHOLD"].asFloat();
    this->NMS_THRESH = YOLO["NMS_THRESHOLD"].asFloat();
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