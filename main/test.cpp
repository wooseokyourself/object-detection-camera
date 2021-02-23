#include "../include/Common.hpp"
#include "../include/BG96.hpp"
#include "../include/Config.hpp"
#include "../include/GPIO.hpp"
#include "../include/HttpPostFormData.hpp"
#include "../include/NRF.hpp"
#include "../include/Serial.hpp"
#include "../include/YoloObjectDetector.hpp"

using namespace std;

const std::string JSON_PATH = "config/config.json";
const std::string HOST = "ino-on.umilevx.com";
const std::string DETECTING_URI = "/api/devices/events/";
const std::string PREVIEW_URI = "/api/manager/device/";

const int RPI_OFF_PIN = 21; // BCM
const std::string MODEM_PORT = "/dev/ttyS0";
const std::string NRF_PORT = " ";

int main (void) {
    Config config;
    cout << "Config ready" << endl;
    BG96 modem(MODEM_PORT, 115200);
    cout << "BG96 ready" << endl;
    // NRF nrf;
    GPIO gpio(RPI_OFF_PIN);
    cout << "GPIO ready" << endl;
    HttpPostFormData fields;
    cout << "HttpPostFormData ready" << endl;
    YoloObjectDetector vision;
    cout << "YoloObjectDetector ready" << endl;

    int select;
    while (true) {
        cout << endl;
        cout << "=========================================================" << endl;
        cout << "1. Config  : Read json from file." << endl;
        cout << "2. Config  : Read json from string." << endl;
        cout << "3. Config  : Write." << endl;
        cout << "4. BG96    : Get RSSI." << endl;
        cout << "5. BG96    : POST request without image." << endl;
        cout << "6. BG96    : POST request with image (Image bytes from memory frame)." << endl;
        cout << "7. BG96    : POST request with image (Image bytes from image file)." << endl;
        cout << "8. BG96    : Put custom AT command." << endl;
        cout << "9. BG96    : Read response." << endl;
        cout << "10. VISION  : Capture." << endl;
        cout << "11. VISION  : Detect previous frame and save it to file." << endl;
        cout << "=========================================================" << endl;
        cout << "select: ";
        cin >> select;
        
        switch (select) {
            case 1: {
                cout << "Reading config/config.json" << endl;
                config.readFromJsonFile("config/config.json");
                cout << "ID: " << config.getID() << endl;
                cout << "Conf: " << config.getConfThreshold() << endl;
                cout << "Nms: " << config.getNmsThreshold() << endl;
                cout << "Width: " << config.getWidth() << endl;
                break;
            }
            case 2: {
                cout << "Below is input string." << endl;
                string json = "{\"ID\": \"ino-on-0005\", \"CONF_THRESHOLD\": \"0.99\", \"NMS_THRESHOLD\": \"0.99\", \"WIDTH\": \"999\", \"MODE\": \"1\", \"INTERVAL_SECS\": \"300\"}";
                cout << json << endl;
                config.readFromJsonString(json);
                cout << "ID: " << config.getID() << endl;
                cout << "Conf: " << config.getConfThreshold() << endl;
                cout << "Nms: " << config.getNmsThreshold() << endl;
                cout << "Width: " << config.getWidth() << endl;
                break;
            }
            case 3: {
                cout << "Write json in test.json" << endl;
                config.write("test.json");
                break;
            }
            case 4: {
                cout << modem.getRssi() << endl;
                break;
            }
            case 5: {
                fields.addField("text/plain", "time", "1996-03-05");
                fields.addField("text/plain", "event", "0");
                fields.addField("text/plain", "rssi", std::to_string(modem.getRssi()));
                // fields.addField("text/plain", "battery", std::to_string(nrf.getBattery()));
                fields.addField("text/plain", "battery", "44");
                string response = modem.postMultipart(HOST, DETECTING_URI + config.getID(), fields, 20);
                cout << response << endl;
                fields.clear();
                break;
            }
            case 6: {
                string imageBytes = vision.extractFrameBytes();
                fields.addField("text/plain", "time", "1996-03-05");
                fields.addField("text/plain", "event", "1");
                fields.addField("text/plain", "rssi", std::to_string(modem.getRssi()));
                // fields.addField("text/plain", "battery", std::to_string(nrf.getBattery()));
                fields.addField("text/plain", "battery", "44");
                fields.addField("text/plain", "filename", "1996-03-05.jpg");
                fields.addField("image/jpeg", "1996-03-05.jpg", imageBytes);
                string response = modem.postMultipart(HOST, DETECTING_URI + config.getID(), fields, 20);
                cout << response << endl;
                fields.clear();
                break;
            }
            case 7: {
                string path;
                cout << "image file: ";
                cin >> path;
                string imageBytes;
                vision.extractImagefileBytes(imageBytes, path);
                fields.addField("text/plain", "time", "1996-03-05");
                fields.addField("text/plain", "event", "1");
                fields.addField("text/plain", "rssi", std::to_string(modem.getRssi()));
                // fields.addField("text/plain", "battery", std::to_string(nrf.getBattery()));
                fields.addField("text/plain", "battery", "44");
                fields.addField("text/plain", "filename", "1996-03-05.jpg");
                fields.addField("image/jpeg", "1996-03-05.jpg", imageBytes);
                string response = modem.postMultipart(HOST, DETECTING_URI + config.getID(), fields, 20);
                cout << response << endl;
                fields.clear();
                break;
            }
            case 8: {
                string cmd;
                cout << "cmd: ";
                cin >> cmd;
                cmd += "\r";
                modem.putATcmd(cmd);
                break;
            }
            case 9: {
                cout << modem.getResponse() << endl;
                break;
            }
            case 10: {
                vision.capture(416);
                break;
            }
            case 11: {
                vision.setModel("bin/model/yolov4.weights", // .weights path
                                "bin/model/yolov4.cfg",     // .cfg path
                                "bin/model/classes.names"  // .names path
                                );
                vision.detect(0,                             // detecting target index (in .names file)
                              0.4,     // yolo confidence threshold
                              0.5,      // yolo nms threshold
                              416       // resize
                              );
                vision.resizeFrame(256);
                vision.writeFrame("test.jpg");
                cout << "Result frame was written to test.jpg" << endl;
                break;
            }
            default: {
                cout << "Invalid" << endl;
                break;
            }
        }
    }

    return 0;
}