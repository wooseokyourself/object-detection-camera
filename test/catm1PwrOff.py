import sys 
sys.path.append("../")
from packages.CATM1 import CATM1
import RPi.GPIO as GPIO

node = CATM1()
node.pwrOffModem()
GPIO.cleanup()