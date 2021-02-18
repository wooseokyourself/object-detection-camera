#include "../include/BG96.hpp"
#include "../include/Config.hpp"
#include "../include/GPIO.hpp"
#include "../include/HttpPostFormData.hpp"
#include "../include/NRF.hpp"
#include "../include/Serial.hpp"
#include "../YoloObjectDetector.hpp"

const std::string JSON_PATH = "config/config.json";
const std::string HOST = "http://ino-on.umilevx.com/";
const std::string DETECTING_URI = "api/devices/events/";
const std::string PREVIEW_URI = "api/manager/device/";

int main (void) {
    Config config;
    BG96 modem;
    NRF nrf;
    HttpPostFormData fields;
    YoloObjectDetector vision;

    config.readFromJsonFile(JSON_PATH);

    std::string imageBytes;
    if (config.isDetectingMode()) {
        vision.init ("bin/model/yolov4.weights",    // .weights path
                     "bin/model/yolov4.cfg",        // .cfg path
                     "bin/model/classes.names",     // .names path
                     0,                             // detecting target index
                     config.getConfThreshold(),     // yolo confidence threshold
                     config.getNmsThreshold(),      // yolo nms threshold
                     config.getWidth()              // picture width (ratio=4:3)
                    );
        vision.capture();
        vision.detect();
        vision.getFrameBytes(imageBytes);

        fields.addField("text/plain", "time", TIMESTAMP);
        fields.addField("text/plain", "event", "1");
        fields.addField("text/plain", "rssi", std::to_string(modem.getRssi()));
        fields.addField("text/plain", "battery", std::to_string(nrf.getBattery()));
        fields.addField("text/plain", "time", TIMESTAMP.substr(0, 10));
        fields.addField("image/jpeg", "files", imageBytes);
        std::string response = modem.postMultipart(HOST, DETECTING_URI + config.getID(), fields, 20);
        config.readFromJsonString(response);
        // nrf.setPowerInterval(config.getIntervalSecs());
    }
    else {
        while (config.isPreviewMode()) {
            vision.capture();
            vision.getFrameBytes(imageBytes);
            fields.addField("image/jpeg", "files", imageBytes);
            std::string response = modem.postMultipart(HOST, PREVIEW_URI = config.getID(), fields, 20);
            config.readFromJsonString(response);
            // nrf.setPowerInterval(config.getIntervalSecs());
            fields.clear();
        }
    }

    config.write(JSON_PATH);

    gpio.shutdownRpi();
    return 0;
}