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
    subprocess.run("python3 webapp/webapp.py --ip 0.0.0.0 --port 4000", shell=True)

def basicMode (isPPP):
    ''' Read configuration '''
    f = open("config/config.json", "r")
    config = json.load(f)
    URL = config["SERVER"]["URL"] + config["SERVER"]["ENDPOINT"] + config["DEVICE"]["ID"]
    confidence, nms = config["YOLO"]["CONFIDENCE_THRESHOLD"], config["YOLO"]["NMS_THRESHOLD"]
    resize = config["YOLO"]["RESIZE"]
    f.close()
    
    ''' Capture, Detect '''
    TIMESTAMP = datetime.isoformat(datetime.now())[:-3] + "Z"
    IMAGEFILE = TIMESTAMP + ".jpg"
    detector = "./build/detector "\
            + "model/yolov4-custom_best.weights "\
            + "model/yolov4-custom.cfg "\
            + "model/classes.names "\
            + "results/" + IMAGEFILE\
            + " " + str(confidence) + " " + str(nms) + " " + str(resize)
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
    lte.pwrOnModem()

    ''' Get RSSI and BER by AT Command '''
    rssi, ber = lte.rssi, lte.ber
    
    ''' Get battery '''
    battery = random.randrange(1, 100) # 배터리 부분 구현해야함   
    
    ''' Wait until PPP activated '''
    if isPPP:
        while 'ppp0' in ifcfg.interfaces() == False:
            print("Wait for ppp activation...")
            pass

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
    while True:
        try:
            if isPPP: # POST from this process
                response = requests.post(URL, files=data)
                resCode, resText = response.status_code, response.text
                print(resCode, ":", resText)
                break
            else: # POST from CAT.M1 process
                response = lte.post(URL, data) # 아직 많이 봐야함
                break
        except requests.exceptions.RequestException as e:
            print(e)
            
        
    ''' Power Off Modem '''
    lte.disablePpp()
    lte.pwrOffModem()

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