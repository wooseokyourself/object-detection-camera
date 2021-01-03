import subprocess
import requests, json
import time
from datetime import datetime
import random # for test
from packages.API.CATM1 import CATM1
from packages.API.NRF import NRF

API_ENDPOINT = "http://ino-on.umilevx.com/api/devices/events"
TIMESTAMP = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
IMAGEFILE = TIMESTAMP + ".jpg"

detector = "./build/detector model/yolov4-custom_best.weights model/yolov4-custom.cfg model/classes.names results/" + IMAGEFILE + " 0.4 0.5 416"

taskModePin = 15    # Task Mode Signal Pin (input)
rpiOffPin = 29      # Work Completion Signal Pin (output)
ltePwrPin = 21      # CAT.M1 Power Pin (output) --> pin initialized in CZCATM1 package
lteStatPin = 23     # CAT.M1 Status Pin (input) --> pin initialized in CZCATM1 package

lte = CATM1(serialPort='/dev/ttyS0', baudrate=115200, pwrPinNum=ltePwrPin, statPinNum=lteStatPin)
nrf = NRF(taskPinNum=taskModePin, offPinNum=rpiOffPin)

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
    lte.pwrOnModem() # LTE power on
    rssi, battery = lte.getRSSI(), random.randrange(1, 100) # 배터리 부분 구현해야함
    mainTask(rssi, battery) # Task
    lte.pwrOffModem() # LTE power off
    nrf.pwrOffPi()

def main ():
    try:
        # Wait 5 seconds for get admin signal
        if nrf.isAdminMode(timeout=5):
            adminMode()
        else:
            basicMode()
    except Exception as e:
        traceback.print_exc()
    finally:
        print("End process")
        # subprocess.call("sudo poweroff", shell=True) # shutdown raspi

main()