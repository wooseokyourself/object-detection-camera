import time
import packages.Define as Define
import RPi.GPIO as GPIO

class NRF:
    def __init__(self, modePinNum, offPinNum):
        self.modePinNum = modePinNum
        self.offPinNum = offPinNum
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.modePinNum, GPIO.IN)
        GPIO.setup(self.offPinNum, GPIO.OUT)

    def isAdminMode(self):
        if GPIO.input(self.modePinNum) == True:
            return True
        else:
            return False
        
    def pwrOffPi(self):
        GPIO.output(self.offPinNum, GPIO.HIGH)