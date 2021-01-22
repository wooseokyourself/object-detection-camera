#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "../include/vision.hpp"
#include "../include/http.hpp"
#include "../include/gpio.hpp"
#include "../include/config.hpp"

using namespace std;

const string WEIGHT = "bin/model/yolov4-custom_best.weights";
const string CFG = "bin/model/yolov4-custom.cfg";
const string NAMES = "bin/model/classes.names";
const string URL = "http://ino-on.umilevx.com/api/devices/events/ino-on-0000";

int main (void) {
    Gpio gpio;
    if (gpio.isAdminMode()) {
        // Run shell script in background
        system("sudo systemctl start raspapd.service &");
        system("python3 webapp/webapp.py --ip 0.0.0.0 --port 4000 &");
        while (gpio.isAdminMode())
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    else { // normal
        Config cfg;
        const string TIMESTAMP = getISOCurrentTimestamp();
        const string FILENAME = getISOCurrentDate();
        cv::Mat frame;
        std::cout << "before picture" << std::endl;
        vision::capture(frame, cfg.yolo_resize());
        std::cout << "capture done" << std::endl;
        const bool isDetected = vision::detect(frame, 
                                               WEIGHT, 
                                               CFG, 
                                               NAMES, 
                                               cfg.yolo_confThresh(), 
                                               cfg.yolo_nmsThresh(), 
                                               cfg.yolo_resize());
        std::cout << "detect done" << std::endl;
        if (isDetected) {
            cv::imwrite("results/" + FILENAME + ".jpg", frame);
            http::post(cfg.http_url(), TIMESTAMP, 31, 99, FILENAME, "results/" + FILENAME + ".jpg");
        }
        else {
            http::post(cfg.http_url(), TIMESTAMP, 31, 99);
        }
        std::cout << "post done" << std::endl;
    }
    gpio.shutdownRpi();
}