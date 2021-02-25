#include "../include/Common.hpp"
#include "../include/BG96.hpp"
#include "../include/Config.hpp"
#include "../include/GPIO.hpp"
#include "../include/HttpPostFormData.hpp"
#include "../include/NRF.hpp"
#include "../include/Serial.hpp"
#include "../include/YoloObjectDetector.hpp"

const std::string JSON_PATH = "config/config.json";
const std::string HOST = "ino-on.umilevx.com";
const std::string DETECTING_URI = "/api/devices/events/";
const std::string PREVIEW_URI = "/api/manager/device/preview/";

const int RPI_OFF_PIN = 21; // BCM
const std::string MODEM_PORT = "/dev/ttyS0";
const std::string NRF_PORT = " ";

enum Names { // bin/model/classes.names
    EXCAVATOR = 0
};

int main (void) {
    Config config;
    BG96 modem(MODEM_PORT, 115200);
    NRF nrf;
    GPIO gpio(RPI_OFF_PIN);
    HttpPostFormData fields;
    YoloObjectDetector vision;

    config.readFromJsonFile(JSON_PATH);
    std::string imageBytes;
    const std::string TIMESTAMP = getISOCurrentTimestamp();
    std::cout << "[Start Process: " << TIMESTAMP << "]" << std::endl;
    if (gpio.isDetectingMode()) {
        std::cout << " Detecting Mode" << std::endl;
        const std::string FILENAME = TIMESTAMP.substr(0, 10) + ".jpg"; 
        vision.setModel("bin/model/yolov4.weights", "bin/model/yolov4.cfg", "bin/model/classes.names");
        vision.capture(config.getCaptureWidth()); // picture width (ratio is 4:3)
        std::cout << "Capture done" << std::endl;
        const int detectedCount
            = vision.detect(EXCAVATOR, config.getConfThreshold(), config.getNmsThreshold(), config.getCaptureWidth());
        std::cout << "Detected objects: " << detectedCount << std::endl;
        fields.addField("text/plain", "time", TIMESTAMP);
        fields.addField("text/plain", "event", detectedCount > 0 ? "1" : "0");
        fields.addField("text/plain", "rssi", std::to_string(modem.getRssi()));
        fields.addField("text/plain", "battery", std::to_string(nrf.getBattery()));
        if (detectedCount > 0 || config.sendPictureAlways()) {
            vision.resizeFrame(256);
            vision.writeFrame("results/" + FILENAME);
            vision.extractImagefileBytes(imageBytes, "results/" + FILENAME);
            fields.addField("text/plain", "filename", FILENAME);
            fields.addField("image/jpeg", FILENAME, imageBytes);
        }

        std::string response = modem.postMultipart(HOST, DETECTING_URI + config.getID(), fields, 20);
        int postFailed = 0;
        while (response.find("705") != -1) { // HTTP(S) no GET/POST requests
            response = modem.postMultipart(HOST, DETECTING_URI + config.getID(), fields, 20);
            ++postFailed;
            std::cerr << "POST failed " << postFailed << " times." << std::endl;
        }
        config.readFromJsonString(response);
        config.write(JSON_PATH);
        nrf.setPowerInterval(config.getIntervalSecs());
    }
    else {
    /**
     *      ----Set Preview Mode---->        --------Captured Image------>
     * NRF                              RPi                                 Server
     *      <---Set Detecting Mode---        <---Terminate Preview Mode---
     * 
     * Preview Mode 여부는 NRF로부터 읽어온다. 이는 GPIO::isDetectingMode() 를 통해 확인한다.
     * Preview Mode 에 진입하면, Preview Mode 의 종료여부는 서버의 http response 로부터 읽어온다.
     * Config::readFromJsonString() 이 서버의 http response 를 분석하고, 이후
     * Config::isPreviewMode() 를 통해 분석결과를 확인할 수 있다.
     * Preview Mode 가 종료되면 GPIO::setDetectingMode() 를 통해 다시 NRF에게 종료를 알려야 한다.
     */
    std::cout << " Preview Mode" << std::endl;
        while (true) {
            vision.capture(256);
            vision.writeFrame("results/preview.jpg")
            vision.extractImagefileBytes(imageBytes, "results/preview.jpg");
            fields.addField("image/jpeg", "files", imageBytes);
            std::string response = modem.postMultipart(HOST, PREVIEW_URI + config.getID(), fields, 20);
            config.readFromJsonString(response);
            fields.clear();
            if (!config.isPreviewMode()) {
                gpio.setDetectingMode();
                break;
            }
        }
    }

    std::cout << "----End process----" << std::endl;
    gpio.shutdownRpi();
    return 0;
}