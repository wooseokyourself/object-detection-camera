#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include "../include/vision.hpp"
#include "../include/http.hpp"
#include "../include/serials.hpp"
#include "../include/config.hpp"

using namespace std;

const string WEIGHT = "bin/model/yolov4-custom_best.weights";
const string CFG = "bin/model/yolov4-custom.cfg";
const string NAMES = "bin/model/classes.names";

int main (void) {
    Serials serials;
    Config cfg;
    cfg.readFromJsonFile("config/config.json");
    const string TIMESTAMP = getISOCurrentTimestamp();
    std::cout << "\nSTART PROCESS IN SYSTEM TIME: " << TIMESTAMP << std::endl;

    if (serials.isAdminMode()) {
        std::cout << " <admin mode>" << std::endl;
        const std::string FILENAME = "preview.jpg";
        while (serials.isAdminMode()) {
            cv::Mat frame;
            vision::capture(frame, 256);
            cv::imwrite("results/" + FILENAME, frame);
            http::post(cfg.http_admin_url(), TIMESTAMP, FILENAME, "results/" + FILENAME, cfg.http_timeout_secs);
            // delay(1000);
        }
        system("rm -f results/preview.jpg");
    }
    else {
        std::cout << " <normal mode>" << std::endl;
        const string FILENAME = TIMESTAMP.substr(0, 10) + ".jpg"; // yyyy-mm-dd.jpg
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
            http::post(cfg.http_normal_url(), TIMESTAMP, 31, 99, FILENAME, "results/" + FILENAME, cfg.http_timeout_secs());
        }
        else {
            http::post(cfg.http_normal_url(), TIMESTAMP, 31, 99, cfg.http_timeout_secs());
        }   
    }

    sync();
    std::cout << "END PROCESS.\n" << std::endl;
    delay(1000);
    serials.shutdownRpi();
}