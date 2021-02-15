#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include "../include/vision.hpp"
#include "../include/http.hpp"
#include "../include/gpio.hpp"
#include "../include/config.hpp"

using namespace std;

const string WEIGHT = "bin/model/yolov4-custom_best.weights";
const string CFG = "bin/model/yolov4-custom.cfg";
const string NAMES = "bin/model/classes.names";

int main (void) {
    const string TIMESTAMP = getISOCurrentTimestamp();
    std::cout << "\nSTART PROCESS IN SYSTEM TIME: " << TIMESTAMP << std::endl;
    Gpio gpio;
    if (gpio.isAdminMode()) {
        std::cout << " <admin mode>" << std::endl;
        // Run shell script in background
        system("sudo systemctl start raspapd.service &");
        system("python3 webapp/webapp.py --ip 0.0.0.0 --port 4000 &");
        system("python3 -m browsepy 0.0.0.0 4001 --directory . &");
        while (gpio.isAdminMode())
            delay(3000);
    }
    else {
        std::cout << " <normal mode>" << std::endl;
        Config cfg;
        cfg.readFromJsonFile("config/config.json");
        const string FILENAME = convertToISOformat(TIMESTAMP) + ".jpg";
        cv::Mat frame;
        vision::capture(frame, cfg.yolo_resize());
        const bool isDetected = vision::detect(frame, 
                                               WEIGHT, 
                                               CFG, 
                                               NAMES, 
                                               cfg.yolo_confThresh(), 
                                               cfg.yolo_nmsThresh(), 
                                               cfg.yolo_resize());
        if (isDetected) {
            cv::imwrite("results/" + FILENAME, frame);
            http::post(cfg.http_url(), TIMESTAMP, 31, 99, FILENAME, "results/" + FILENAME, cfg.http_timeout_secs());
        }
        else {
            http::post(cfg.http_url(), TIMESTAMP, 31, 99, cfg.http_timeout_secs());
        }   
    }
    sync();
    std::cout << "END PROCESS.\n" << std::endl;
    delay(3000);
    gpio.shutdownRpi();
}