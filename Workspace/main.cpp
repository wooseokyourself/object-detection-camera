#include <iostream>
#include "device.hpp"

using namespace std;

int main (void) {
    int task;
    int fd = deviceInit();
    while(true) {
        cout << "1. Power on Modem" << endl;
        cout << "2. get RSSI" << endl;
        cout << "3. Post image" << endl;
        cout << "4. Power off Modem" << endl;
        cin >> task;
        if (task == 1)
            gpio::powerOnModem();
        else if (task == 2)
            atcmd::getRSSI(fd);
        else if (task == 3) {
            std::string url;
            cout << "input url: ";
            cin >> url;
            atcmd::post(fd, url);
        }
        else if (task == 4)
            gpio::powerOffModem();
        else
            break;
    }
    return 0;
}