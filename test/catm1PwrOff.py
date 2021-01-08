import sys 
sys.path.append("../")
from packages.Define import *
from packages.API.CATM1 import CATM1
import RPi.GPIO as GPIO

node = CATM1(serialPort=MODEM_SER_PORT, baudrate=115200, pwrPinNum=MODEM_PWR_PIN, statPinNum=MODEM_STAT_PIN)
node.pwrOffModem()
GPIO.cleanup()