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
const int RPI_MODE_PIN = 20; 
const std::string MODEM_PORT = "/dev/ttyS0";
const std::string NRF_PORT = " ";

enum Names { // bin/model/classes.names
    EXCAVATOR = 0
};

int main (void) {
    Config config;
    BG96 modem(MODEM_PORT, 115200);
    NRF nrf;
    GPIO gpio(RPI_OFF_PIN, RPI_MODE_PIN);
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
        while (response.find("result") == -1) { 
            ++postFailed;
            std::cerr << "POST failed " << postFailed << " times." << std::endl;
            std::cerr << " response:\n" << response << std::endl;
            response = modem.postMultipart(HOST, DETECTING_URI + config.getID(), fields, 20);
        }
        std::cout << "HTTP response is below" << std::endl;
        std::cout << response << std::endl;
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
         * Preview Mode ????????? NRF????????? ????????????. ?????? GPIO::isDetectingMode() ??? ?????? ????????????.
         * Preview Mode ??? ????????????, Preview Mode ??? ??????????????? ????????? http response ????????? ????????????.
         * Config::readFromJsonString() ??? ????????? http response ??? ????????????, ??????
         * Config::isPreviewMode() ??? ?????? ??????????????? ????????? ??? ??????.
         * Preview Mode ??? ???????????? GPIO::setDetectingMode() ??? ?????? ?????? NRF?????? ????????? ????????? ??????.
         */
        std::cout << " Preview Mode" << std::endl;
        while (true) {
            vision.capture(256);
            vision.writeFrame("results/preview.jpg");
            vision.extractImagefileBytes(imageBytes, "results/preview.jpg");
            fields.addField("image/jpeg", "files", imageBytes);
            std::string response = modem.postMultipart(HOST, PREVIEW_URI + config.getID(), fields, 20);
            int postFailed = 0;
            while (response.find("result") == -1) { 
                ++postFailed;
                std::cerr << "POST failed " << postFailed << " times." << std::endl;
                std::cerr << " response:\n" << response << std::endl;
                response = modem.postMultipart(HOST, PREVIEW_URI + config.getID(), fields, 20);
            }
            std::cout << "HTTP response is below" << std::endl;
            std::cout << response << std::endl;
            config.readFromJsonString(response);
            fields.clear();
            if (!config.isPreviewMode()) {
                /**
                 * ???????????? NRF?????? UART??? Preview Mode??? ?????????????????? ????????????.
                 * ?????? ????????? ????????????????????? ?????? NRF ??????????????? ?????? 1??? 0?????? ????????? Preview Mode ????????????.
                 */
                break;
            }
        }
    }

    std::cout << "----End process----" << std::endl;
    usleep(1000000); // wait 10s to write log in background. 
    gpio.shutdownRpi();
    return 0;
}