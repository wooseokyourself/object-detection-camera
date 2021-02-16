#include <iostream>
#include "device.hpp"

using namespace std;

int main (void) {
    int task;
    int fd = deviceInit();
    while(true) {
        cout << "1. Send Manually" << endl;
        cout << "2. Read Manually" << endl;
        cout << "3. get RSSI" << endl;
        cout << "4. Post image" << endl;
        cin >> task;
        if (task == 1) {
            string cmd;
            cout << "input cmd: ";
            cin >> cmd;
            cmd += "\r";
            atcmd::__sendATcmd(fd, cmd.c_str());
        }
        else if (task == 2) {
            cout << atcmd::__readBuffer(fd) << endl;
        }
        else if (task == 3)
            atcmd::getRSSI(fd);
        else if (task == 4) {
            int tryout;
            cout << "tryout: ";
            cin >> tryout;
            std::string url = "http://ino-on.umilevx.com/api/devices/events/ino-on-1996";
            atcmd::post(fd, url, tryout);
        }
        else
            break;
    }
    return 0;
}