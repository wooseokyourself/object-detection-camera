import sys
sys.path.insert(0, "./GPIOEmulator")
from EmulatorGUI import GPIO
import asyncio
import detector


try:
    GPIO.setmode(GPIO.BCM)
    
    modePin = 15
    outPin = 4
    
    GPIO.setup(modePin, GPIO.IN)
    GPIO.setup(outPin, GPIO.OUT)
    while True:
        # @modePin should be set initially
        if GPIO.input(modePin) == 1:
            print("get GPIO pin!")
            # LTE power on here
            asyncio.run(detector.task())
            print("break")
            break
        # else:
            # Admin mode
except Exception as e:
    traceback.print_exc()
finally:
    GPIO.output(4, GPIO.HIGH)
    GPIO.cleanup()
