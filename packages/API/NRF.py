import time
import packages.Define as Define
if Define.GPIO_EMULATOR == True:
    from packages.GPIOEmulator.EmulatorGUI import GPIO
else:
    import RPi.GPIO as GPIO # Block with dev

class NRF:
    def __init__(self, taskPinNum, offPinNum):
        self.taskPinNum = taskPinNum
        self.offPinNum = offPinNum
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.taskPinNum, GPIO.IN)
        GPIO.setup(self.offPinNum, GPIO.OUT)

    def isAdminMode(self):
        if GPIO.input(self.taskPinNum) == True:
            return True
        else:
            return False
        
    def pwrOffPi(self):
        GPIO.output(self.offPinNum, GPIO.HIGH)