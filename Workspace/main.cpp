#include <iostream>
#include <cstdio>
#include <cstring>

#include <vector>
#include "gpio.hpp"

using namespace std;

#define SERIAL "/dev/ttyS0"
#define PWR_PIN 20
#define STAT_PIN 21

BG96 modem(SERIAL, PWR_PIN, STAT_PIN);

int main (void) {
    gpio::init();
    int task;
    while(true) {
        cout << "1. Power on Modem" << endl;
        cout << "2. Get RSSI" << endl;
        cout << "3. POST" << endl;
        cout << "4. Power off Modem" << endl;
        cin >> task;
        if (task == 1)
            atcmd::powerOnModem();
        else if (task == 2)
            atcmd::getRssi();
        else if (task == 3)
            atcmd::post();
        else if (task == 4)
            atcmd::powerOffModem();
        else
            break;
    }
    return 0;
}