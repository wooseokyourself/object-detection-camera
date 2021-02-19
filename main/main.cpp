#include "../include/Common.hpp"
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

const int RPI_OFF_PIN = 21; // BCM
const std::string MODEM_PORT = "/dev/ttyS0";
const std::string NRF_PORT = " ";

int main (void) {
    Config config;
    BG96 modem(MODEM_PORT, 115200);
    // NRF nrf;
    GPIO gpio(RPI_OFF_PIN);
    HttpPostFormData fields;
    YoloObjectDetector vision;
    
    config.readFromJsonFile(JSON_PATH);

    std::string imageBytes;
    if (config.isDetectingMode()) {
        const std::string TIMESTAMP = getISOCurrentTimestamp();
        vision.setModel("bin/model/yolov4.weights", // .weights path
                        "bin/model/yolov4.cfg",     // .cfg path
                        "bin/model/classes.names"  // .names path
                        );
        vision.capture(config.getWidth());           // picture width (retio is 4:3)
        vision.detect(0,                             // detecting target index (in .names file)
                      config.getConfThreshold(),     // yolo confidence threshold
                      config.getNmsThreshold(),      // yolo nms threshold
                      config.getWidth()              // resize
                     );
        vision.getFrameBytes(imageBytes);

        fields.addField("text/plain", "time", TIMESTAMP);
        fields.addField("text/plain", "event", "1");
        fields.addField("text/plain", "rssi", std::to_string(modem.getRssi()));
        // fields.addField("text/plain", "battery", std::to_string(nrf.getBattery()));
        fields.addField("text/plain", "battery", "99");
        fields.addField("text/plain", "time", TIMESTAMP.substr(0, 10));
        fields.addField("image/jpeg", "files", imageBytes);
        std::string response = modem.postMultipart(HOST, DETECTING_URI + config.getID(), fields, 20);
        config.readFromJsonString(response);
        // nrf.setPowerInterval(config.getIntervalSecs());
    }
    else {
        while (config.isPreviewMode()) {
            vision.capture(256);
            vision.getFrameBytes(imageBytes);
            fields.addField("image/jpeg", "files", imageBytes);
            std::string response = modem.postMultipart(HOST, PREVIEW_URI + config.getID(), fields, 20);
            config.readFromJsonString(response);
            // nrf.setPowerInterval(config.getIntervalSecs());
            fields.clear();
        }
    }

    config.write(JSON_PATH);

    gpio.shutdownRpi();
    return 0;
}