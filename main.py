import sys
sys.path.insert(0, "./GPIOEmulator")
from EmulatorGUI import GPIO # EmulatorGUI for debug
import subprocess
import requests, json
from datetime import datetime

import random() # for test

now = datetime.now()
API_ENDPOINT="http://ino-on.umilevx.com/api/devices/events"
API_METHOD="POST"
TIMESTAMP=str(now.year) + "-" + str(now.month) + "-" + str(now.day) + "_" + str(now.hour) + ":" + str(now.minute) + ":" + str(now.second)
IMAGEFILE=TIMESTAMP + ".jpg"

detector = "./build/detector model/yolov4-custom_best.weights model/yolov4-custom.cfg model/classes.names results/" + IMAGEFILE + " 0.4 0.5 416"

def getRssi ():
    return random.randrange(-40, -10)

def getBattery ():
    return random.randrange(1, 100)

def post (event):
    data = {}
    if event == False:
        data = {"time":TIMESTAMP, "event":0, "rssi":getRssi(), "battery":getBattery()}
    else:
        data = {"time":TIMESTAMP, "event":1, "rssi":getRssi(), "battery":getBattery(), "filename":IMAGEFILE, "files":"@results/"+IMAGEFILE}
    return requests.post(API_ENDPOINT, json=data)

def task ():
    process = subprocess.run(detector, capture_output=True, shell=True)
    exitCode = process.returncode
    response = ""
    if exitCode == 0:
        print("No event")
        response = post(event=False)
    elif exitCode == 1:
        print("Event detected")
        response = post(event=True)
    elif exitCode == 2:
        print("Camera connection failed")
    elif exitCode == 3:
        print("Writing output image failed")
    elif exitCode == 4:
        print("Load model failed")
    elif exitCode == 5:
        print("Inference failed")
    else:
        print("Invalid status")
    print(response.status_code, ":", response.text)

def main ():
    try:
        GPIO.setmode(GPIO.BCM)
        modePin = 15 # temp pin number
        outPin = 4 # temp pin number
        GPIO.setup(modePin, GPIO.IN, initial = GPIO.HIGH) # initial setting for debug
        GPIO.setup(outPin, GPIO.OUT)
        if GPIO.input(modePin) == GPIO.HIGH: # Basic mode 
            # LTE power on here
            print("LTE power on")
            detector.task()
            GPIO.output(4, GPIO.HIGH)
        else: # Admin mode
            # Wifi power on here
            print("Wifi power on")
    except Exception as e:
        traceback.print_exc()
    finally:
        GPIO.cleanup()
        # subprocess.call("sudo poweroff", shell=True) # shutdown raspi

main()