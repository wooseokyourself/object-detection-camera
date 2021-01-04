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
        if GPIO.getmode() == -1:
            GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.taskPinNum, GPIO.IN)
        GPIO.setup(self.offPinNum, GPIO.OUT)

    def isAdminMode(self, timeout):
        startTime = time.time()
        while True:
            if GPIO.input(self.taskPinNum) == True:
                return True
            elapsed = time.time() - startTime
            if elapsed < timeout:
                print(" Basic mode will starts after", int(timeout - elapsed + 1), "secs...", self.taskPinNum, "PIN waits for admin mode.")
                time.sleep(1)
            else:
                return False
        
    def pwrOffPi(self):
        GPIO.output(self.offPinNum, GPIO.HIGH)