import argparse
import subprocess
import json
from datetime import datetime
import random
import packages.Define as Define
import RPi.GPIO
# Define.GPIO_EMULATOR = True
from packages.API.CATM1 import CATM1
from packages.API.NRF import NRF
from packages.API.WEB import WEB

### GPIO BCM ###
taskModePin = 20    # NRF - Task Mode Signal Pin (input)
rpiOffPin = 21      # NRF - Work Completion Signal Pin (output)
ltePwrPin = 17       # CAT.M1 Power Pin (output)
lteStatPin = 27     # CAT.M1 Status Pin (input)

nrf = NRF(taskPinNum=taskModePin, offPinNum=rpiOffPin)
web = WEB(url="http://ino-on.umilevx.com/api/devices/events")

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
    TIMESTAMP = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
    IMAGEFILE = TIMESTAMP + ".jpg"
    detector = "./build/detector model/yolov4-custom_best.weights model/yolov4-custom.cfg model/classes.names results/" + IMAGEFILE + " "
    with open("config/config.json", "r") as f:
        config = json.load(f)
        confidence, nms = config["YOLO"]["CONFIDENCE_THRESHOLD"], config["YOLO"]["NMS_THRESHOLD"]
        resize = config["YOLO"]["RESIZE"]
        detector += confidence + " " + nms + " " + resize
    process = subprocess.run(detector, capture_output=True, shell=True)
    exitCode = process.returncode

    lte = CATM1(serialPort='/dev/ttyS0', baudrate=115200, pwrPinNum=ltePwrPin, statPinNum=lteStatPin)
    lte.pwrOnModem()
    rssi, battery = lte.getRSSI(), random.randrange(1, 100) # 배터리 부분 구현해야함    
    
    if exitCode == 0:
        print("No event")
        resCode, resText = web.post(time=TIMESTAMP, event=False, rssi=rssi, battery=battery)
        print(resCode, ":", resText)
    elif exitCode == 1:
        print("Event detected")
        resCode, resText = web.post(time=TIMESTAMP, event=False, rssi=rssi, battery=battery, imagefile=IMAGEFILE)
        print(resCode, ":", resText)
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
    
    lte.pwrOffModem() # LTE power off

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--mode", type=str, help="(optioanl) basic or admin")
    args = parser.parse_args()
    try:
        if args.mode is None:
            # Wait 5 seconds for get admin signal
            if nrf.isAdminMode(timeout=5):
                adminMode()
            else:
                basicMode()
        else:
            if args.mode == "basic":
                basicMode()
            elif args.mode == "admin":
                adminMode()
            else:
                print("Invalid argument")
                raise ValueError
    except Exception as e:
        print("exception occured:", e)
    finally:
        print("End process")
        # nrf.pwrOffPi()
        # RPi.GPIO.cleanup()
        # subprocess.call("sudo poweroff", shell=True) # shutdown raspi
        exit(0)