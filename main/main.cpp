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
// const std::string PREVIEW_URI = "/api/manager/device/";

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

    // Detecting Mode
    const std::string TIMESTAMP = getISOCurrentTimestamp();
    const std::stirng FILENAME = TIMESTAMP.substr(0, 10) + ".jpg"; 
    vision.setModel("bin/model/yolov4.weights", "bin/model/yolov4.cfg", "bin/model/classes.names");
    vision.capture(config.getCaptureWidth()); // picture width (ratio is 4:3)
    const int detectedCount
        = vision.detect(EXCAVATOR, config.getConfThreshold(), config.getNmsThreshold(), config.getCaptureWidth());
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
    config.readFromJsonString(response);
    nrf.setPowerInterval(config.getIntervalSecs());
    
    /* // Preview Mode
    while (config.isPreviewMode()) {
        vision.capture(256);
        vision.writeFrame("results/preview.jpg")
        vision.extractImagefileBytes(imageBytes, "results/preview.jpg");
        fields.addField("image/jpeg", "files", imageBytes);
        std::string response = modem.postMultipart(HOST, PREVIEW_URI + config.getID(), fields, 20);
        config.readFromJsonString(response);
        nrf.setPowerInterval(config.getIntervalSecs());
        fields.clear();
    }
    */

    config.write(JSON_PATH);

    // gpio.shutdownRpi();
    return 0;
}