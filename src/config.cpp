
#include "../include/config.hpp"

std::string
getISOCurrentTimestamp () {
    return date::format("%FT%TZ", date::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()));
}

/// @brief .json 파일 읽기
/// @param filePath json 파일의 경로
void
Config::readFromJsonFile (const std::string filePath) {
    Json::Value root;
    Json::Reader reader;
    std::ifstream json(filePath, std::ifstream::binary);
    reader.parse(json, root);
    Json::Value DEVICE = root["DEVICE"];
    Json::Value SERVER = root["SERVER"];
    Json::Value YOLO = root["YOLO"];
    this->CONF_THRESH = std::stof(YOLO["CONFIDENCE_THRESHOLD"].asString());
    this->NMS_THRESH = std::stof(YOLO["NMS_THRESHOLD"].asString());
    this->RESIZE = std::stoi(YOLO["RESIZE"].asString());
    this->NORMAL_URL = SERVER["URL"].asString() + SERVER["NORMAL_ENDPOINT"].asString() + DEVICE["ID"].asString();
    this->ADMIN_URL = SERVER["URL"].asString() + SERVER["ADMIN_ENDPOINT"].asString() + DEVICE["ID"].asString();
    this->HTTP_TIMEOUT_SECS = std::stof(SERVER["HTTP_TIMEOUT"].asString());
}

/// @brief UART 통신으로 설정변수 읽기
/// @param serialPort 시리얼포트 경로 (ex. "/dev/AMA0")
/// @param baudRate 데이터 전송 속도 (ex. 115200)
void
Config::readFromUart (const char* serialPort, const int baudRate) {
    
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
Config::http_normal_url () {
    return this->NORMAL_URL;
}

std::string
Config::http_admin_url () {
    return this->ADMIN_URL;
}

long
Config::http_timeout_secs () {
    return this->HTTP_TIMEOUT_SECS;
}