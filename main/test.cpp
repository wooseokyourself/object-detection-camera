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

    int select;
    while (true) {
        cout << endl;
        cout << "=========================================================" << endl;
        cout << "1. Config  : Read json from file." << endl;
        cout << "2. Config  : Read json from string." << endl;
        cout << "3. Config  : Write." << endl;
        cout << "4. BG96    : Get RSSI." << endl;
        cout << "5. BG96    : POST request with image." << endl;
        cout << "6. BG96    : Put custom AT command." << endl;
        cout << "7. BG96    : Read response." << endl;
        cout << "8. VISION  : Capture." << endl;
        cout << "9. VISION  : Detect previous frame and save it to file." << endl;
        cout << "=========================================================" << endl;
        
        switch (select) {
            case 1:
                cout << "Reading config/config.json" << endl;
                config.readFromJsonFile();
                cout << "ID: " << config.getID() << endl;
                cout << "Conf: " << config.getConfThreshold() << endl;
                cout << "Nms: " << config.getNmsThreshold() << endl;
                cout << "Width: " << config.getWidth() << endl;
                break;
            case 2:
                cout << "Below is input string." << endl;
                string json = "{\"ID\": \"ino-on-xxxx\", \"CONF_THRESHOLD\": \"0.99\", \"NMS_THRESHOLD\": \"0.99\", \"WIDTH\": \"999\", \"MODE\": \"1\", \"INTERVAL_SECS\": \"300\"}";
                cout << json << endl;
                config.readFromJsonString(json);
                cout << "ID: " << config.getID() << endl;
                cout << "Conf: " << config.getConfThreshold() << endl;
                cout << "Nms: " << config.getNmsThreshold() << endl;
                cout << "Width: " << config.getWidth() << endl;
                break;
            case 3:
                cout << "Write json in test.json" << endl;
                config.write("test.json");
                break;
            case 4:
                modem.getRssi();
                break;
            case 5:
                string imageBytes;
                vision.getFrameBytes(imageBytes);
                fields.addField("text/plain", "time", "1996-03-05");
                fields.addField("text/plain", "event", "1");
                fields.addField("text/plain", "rssi", "44");
                // fields.addField("text/plain", "battery", std::to_string(nrf.getBattery()));
                fields.addField("text/plain", "battery", "44");
                fields.addField("text/plain", "time", "1996-03-05");
                fields.addField("image/jpeg", "files", imageBytes);
                string response = modem.postMultipart(HOST, DETECTING_URI + config.getID(), fields, 20);
                cout << response << endl;
                break;
            case 6:
                string cmd;
                cout << "cmd: ";
                cin >> cmd;
                cmd += "\r";
                modem.putATcmd(cmd);
                break;
            case 7: 
                cout << modem.getResponse() << endl;
                break;
            case 8: 
                vision.capture(800);
                break;
            case 9:
                vision.setModel("bin/model/yolov4.weights", // .weights path
                                "bin/model/yolov4.cfg",     // .cfg path
                                "bin/model/classes.names"  // .names path
                                );
                vision.detect(0,                             // detecting target index (in .names file)
                              0.4,     // yolo confidence threshold
                              0.5,      // yolo nms threshold
                              800              // resize
                              );
                vision.writeFrame("test.jpg");
                cout << "Result frame was written to test.jpg" << endl;
                break;
            default: 
                cout << "Invalid" << endl;
                break;
        }
    }

    return 0;
}