import sys 
sys.path.append("../")
import time
import RPi.GPIO as GPIO
from packages.Define import *
from packages.API.CATM1 import CATM1

node = CATM1(serialPort=MODEM_SER_PORT, baudrate=115200, pwrPinNum=MODEM_PWR_PIN, statPinNum=MODEM_STAT_PIN)
node.pwrOnModem()

print( "RPI3 Modem Power PIN: " + str(node.getPwrPinNum()) )
print( "RPI3 Modem Status PIN: " + str(node.getStatusPinNum()) )

print("AT: " + node.sendATCmd("AT", "OK\r\n"))
print("ATE0: " + node.sendATCmd("ATE0", "OK\r\n"))
print("IMEI: " + node.getIMEI()) 
print("RSSI: " + node.getRSSI())
print("FW Ver: " + node.getFirmwareInfo())
print("HW model: " + node.getHardwareInfo())
# print("Phone Number: " + node.getPhoneNumberInfo())
time.sleep(2)
node.pwrOffModem()

#program End
print("end")
GPIO.cleanup()