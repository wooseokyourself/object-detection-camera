import sys
from packages.EmulatorGUI import GPIO # Block with release
# import RPi.GPIO as GPIO # Block with dev
from packages.CZCATM1.CATM1 import CATM1
import subprocess
import requests, json
import time
from datetime import datetime
import random # for test

API_ENDPOINT = "http://ino-on.umilevx.com/api/devices/events"
TIMESTAMP = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
IMAGEFILE = TIMESTAMP + ".jpg"

detector = "./build/detector model/yolov4-custom_best.weights model/yolov4-custom.cfg model/classes.names results/" + IMAGEFILE + " 0.4 0.5 416"

waitTime = 5 # Wait 5 seconds for get admin signal, otherwise run basic mode
GPIO.setmode(GPIO.BCM)
modePin = 15 # temp pin number
outPin = 4 # temp pin number
GPIO.setup(modePin, GPIO.IN, pull_up_down = GPIO.PUD_DOWN) # pud is temporary
GPIO.setup(outPin, GPIO.OUT)

def post (event, rssi, battery):
    data = {}
    if event == False:
        data = {"time":TIMESTAMP, "event":0, "rssi":rssi, "battery":battery}
    else:
        data = {"time":TIMESTAMP, "event":1, "rssi":rssi, "battery":battery, "filename":IMAGEFILE, "files":"@results/"+IMAGEFILE}
    return requests.post(API_ENDPOINT, json=data)

# Snapshot, inference, request
def mainTask (rssi, battery):
    process = subprocess.run(detector, capture_output=True, shell=True)
    exitCode = process.returncode
    response = ""
    if exitCode == 0:
        print("No event")
        response = post(event=False, rssi, battery)
    elif exitCode == 1:
        print("Event detected")
        response = post(event=True, rssi, battery)
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

def adminMode ():
    """
    #
    # Wifi power on here
    #
    """
    print("Wifi power on")
    """
    #
    # Web server on here
    #
    """
    print("Web server on")

def basicMode ():
    lte = CATM1(serialPort='/dev/ttyS0', baudrate=115200, pwrPinNum=17, statPinNum=27)
    lte.pwrOnModem() # LTE power on
    rssi, battery = lte.getRSSI(), random.randrange(1, 100) # 배터리 부분 구현해야함
    mainTask(rssi, battery) # Task
    lte.pwrOffModem() # LTE power off
    GPIO.output(4, GPIO.HIGH)

def main ():
    try:
        startTime = time.time()
        while (True):
            if GPIO.input(modePin) == True:
                adminMode()
                break
            elapsed = time.time() - startTime
            if elapsed < waitTime:
                print(" Basic mode will starts after", int(waitTime - elapsed + 1), "secs... GPIO-BCM", modePin, "PIN waits for admin mode.")
                time.sleep(1)
            else:
                basicMode()
                break
    except Exception as e:
        traceback.print_exc()
    finally:
        GPIO.cleanup()
        print("End process")
        # subprocess.call("sudo poweroff", shell=True) # shutdown raspi

main()