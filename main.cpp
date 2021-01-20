#include <wiringPi.h>
#include <string>
#include <ctime>

#include "vision.hpp"
#include "http.hpp"

using namespace std;

const int TASK_MODE_PIN = 20;
const int RPI_OFF_PIN = 21;

const string WEIGHT = "bin/model/yolov4-custom_best.weights";
const string CFG = "bin/model/yolov4-custom.cfg";
const string NAMES = "bin/model/classes.names";
const float CONF_THRESH = 0.4;
const float NMS_THRESH = 0.5;
const float RESIZE = 416;
const string URL = "http://ino-on.umilevx.com/api/devices/events/ino-on-0000"

template <class Precision> string
getISOCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    return date::format("%FT%TZ", date::floor<Precision>(now));
}

int
main (void) {
    wiringPiSetupGpio(); // BCM
    pinMode(taskModePin, INPUT);
    pinMode(rpiOffPin, OUTPUT);

    if (digitalRead(taskModePin) == 1) { // admin
        // 1. raspAP 활성화
        // 2. subprocess("webapp") 로드
        // 3. 여전히 admin인지 계속 체크
    }
    else { // normal
        const string TIMESTAMP = getISOCurrentTimestamp<chrono::milliseconds>();
        cv::Mat frame;
        vision::capture(frame, RESIZE);
        if (vision::detect(frame, WEIGHT, CFG, NAMES, CONF_THRESH, NMS_THRESH, RESIZE)) { // if detect
            // cv::imwrite("results/" + TIMESTAMP + ".jpg", frame);
            // SEND MULTIPART/FORM-DATA WITH IMAGE HERE
            vector<uchar> imgBuffer;
            cv::imencode(".jpg", frame, imgBuffer);
            char* imgBuffPtr = &imgBuffer[0];
            long imgBuffSize = imgBuffer.size();

            http::postImage(URL, TIMESTAMP, 31, 99, imgBuffPtr, imgBuffSize);
        }
        else {
            // SEND MULTIPART/FORM-DATA HERE
            http::postWithoutImage(URL, TIMESTAMP, 31, 99);
        }
    }

    digitalWrite(rpiOffPin, HIGH);
    delay(500);
}