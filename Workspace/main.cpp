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
            atcmd::__sendATcmd(fd, cmd.c_str());
        }
        else if (task == 2) {
            cout << atcmd::__readBuffer(fd) << endl;
        }
        else if (task == 3)
            atcmd::getRSSI(fd);
        else if (task == 4) {
            std::string url;
            cout << "input url: ";
            cin >> url;
            atcmd::post(fd, url);
        }
        else
            break;
    }
    return 0;
}