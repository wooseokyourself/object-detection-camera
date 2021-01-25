#include <iostream>
#include "device.hpp"

using namespace std;

int main (void) {
    int task;
    int fd = deviceInit();
    while(true) {
        cout << "1. Power on Modem" << endl;
        cout << "2. ATcmd Init" << endl;
        cout << "3. Power off Modem" << endl;
        cin >> task;
        if (task == 1)
            gpio::powerOnModem();
        else if (task == 2)
            cout << fd << endl;
        else if (task == 4)
            gpio::powerOffModem();
        else
            break;
    }
    return 0;
}