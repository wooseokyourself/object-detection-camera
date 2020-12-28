import sys
sys.path.insert(0, "./GPIOEmulator")
from EmulatorGUI import GPIO # EmulatorGUI for debug
import subprocess
import requests, json
from datetime import datetime

import random # for test

now = datetime.now()
API_ENDPOINT="http://ino-on.umilevx.com/api/devices/events"
API_METHOD="POST"
TIMESTAMP=str(now.year) + "-" + str(now.month) + "-" + str(now.day) + "_" + str(now.hour) + ":" + str(now.minute) + ":" + str(now.second)
IMAGEFILE=TIMESTAMP + ".jpg"

detector = "./build/detector model/yolov4-custom_best.weights model/yolov4-custom.cfg model/classes.names results/" + IMAGEFILE + " 0.4 0.5 416"

waitTime = 5 # Wait 5 seconds for get admin signal, otherwise run basic mode
GPIO.setmode(GPIO.BCM)
modePin = 15 # temp pin number
outPin = 4 # temp pin number
GPIO.setup(modePin, GPIO.IN, pull_up_down = GPIO.PUD_DOWN) # pud is temporary
GPIO.setup(outPin, GPIO.OUT)

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

def adminMode (channel): # call when got admin mode signal
    # Wifi power on here
    print("Wifi power on")
    # Web server on here
    print("Web server on")

def basicMode ():
    # LTE power on here
    print("LTE power on")
    task()
    GPIO.output(4, GPIO.HIGH)

def main ():
    try:
        GPIO.add_event_detect(modePin, GPIO.RISING, callback=pinAdminCallback)
        startTime = time.time()
        while GPIO.input(modePin) == False and time.time() - startTime < waitTime:
            time.sleep(0.1)
        basicMode()
                        
    except Exception as e:
        traceback.print_exc()
    finally:
        GPIO.cleanup()
        print("End process")
        # subprocess.call("sudo poweroff", shell=True) # shutdown raspi

main()