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
    Gpio gpio;
    if (gpio.isAdminMode()) {
        std::cout << "admin mode" << std::endl;
        // Run shell script in background
        system("sudo systemctl start raspapd.service &");
        system("python3 webapp/webapp.py --ip 0.0.0.0 --port 4000 &");
        while (gpio.isAdminMode())
            delay(3000);
    }
    else { // normal
        std::cout << "normal mode" << std::endl;
        Config cfg;
        const string TIMESTAMP = getISOCurrentTimestamp();
        const string FILENAME = getISOCurrentDate();
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
            cv::imwrite("results/" + FILENAME + ".jpg", frame);
            http::post(cfg.http_url(), TIMESTAMP, 31, 99, FILENAME, "results/" + FILENAME + ".jpg");
        }
        else {
            http::post(cfg.http_url(), TIMESTAMP, 31, 99);
        }
        std::cout << "post done" << std::endl;
    }
    sync();
    gpio.shutdownRpi();
}