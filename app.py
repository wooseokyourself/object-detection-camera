import pathlib
import os
os.chdir(pathlib.Path(__file__).parent.absolute())
from packages.Define import *
import argparse
import subprocess
import ifcfg
import requests
import json
from datetime import datetime
import random
import RPi.GPIO as GPIO
# Define.GPIO_EMULATOR = True
from packages.API.CATM1 import CATM1
from packages.API.NRF import NRF

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

def basicMode (isPPP):
    ''' Capture, Detect '''
    TIMESTAMP = datetime.isoformat(datetime.now())[:-3] + "Z"
    IMAGEFILE = TIMESTAMP + ".jpg"
    detector = "./build/detector model/yolov4-custom_best.weights model/yolov4-custom.cfg model/classes.names results/" + IMAGEFILE + " "
    with open("config/config.json", "r") as f:
        config = json.load(f)
        confidence, nms = config["YOLO"]["CONFIDENCE_THRESHOLD"], config["YOLO"]["NMS_THRESHOLD"]
        resize = config["YOLO"]["RESIZE"]
        print(" confidence, nms, resize=", confidence, nms, resize)
        detector += str(confidence) + " " + str(nms) + " " + str(resize)
    process = subprocess.run(detector, capture_output=True, shell=True)
    exitCode = process.returncode
    if exitCode == 0:
        print("No event")
    elif exitCode == 1:
        print("Event detected")
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

    ''' Power On Modem '''
    lte = CATM1(serialPort=MODEM_SER_PORT, baudrate=115200, pwrPinNum=MODEM_PWR_PIN, statPinNum=MODEM_STAT_PIN)
    lte.pwrOnModem(isPPP=isPPP)

    ''' Get RSSI and BER by AT Command '''
    rssi, ber = "", ""
    if isPPP:
        rssi, ber = "99", "99" # 이거 어떻게 구하지?
    else:
        lte.getRSSI()
    
    ''' Get battery '''
    battery = random.randrange(1, 100) # 배터리 부분 구현해야함   
    
    ''' POST '''
    data = {}
    if exitCode == 0:
        data = {
            "time": (None, TIMESTAMP), 
            "event": (None, "0"), 
            "rssi": (None, rssi), 
            "battery": (None, str(battery))
            }
    else:
        data = {
            "time": (None, TIMESTAMP), 
            "event": (None, "1"), 
            "rssi": (None, rssi), 
            "battery": (None, str(battery)), 
            "filename": (None, IMAGEFILE), 
            "files": open("results/" + IMAGEFILE, 'rb')
            }

    
    if isPPP: # POST from this process
        response = requests.post(URL, files=data)
        resCode, resText = response.status_code, response.text
        print(resCode, ":", resText)
    else: # POST from CAT.M1 process
        response = lte.post(URL, data) # 아직 많이 봐야함
    
    lte.pwrOffModem() # LTE power off

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--p", type=int, help="(optional) '1' when using PPP")
    parser.add_argument("--m", type=str, help="(optioanl) Do only 'basic' or 'admin'")
    args = parser.parse_args()
    isPPP = False
    if args.p is not None and args.p == 1:
        isPPP = True
    nrf = NRF(taskPinNum=TASK_MODE_PIN, offPinNum=RPI_OFF_PIN)
    try:
        if args.m is None:
            # Wait 5 seconds for get admin signal
            if nrf.isAdminMode(timeout=5):
                adminMode()
            else:
                basicMode(isPPP)
        else:
            if args.m == "basic":
                basicMode(isPPP)
            elif args.m == "admin":
                adminMode()
            else:
                print("Invalid argument")
                raise ValueError
    except Exception as e:
        print("exception occured:", e)
    finally:
        print("End process")
        # nrf.pwrOffPi()
        # GPIO.cleanup()
        exit(0)