import pathlib
import os
os.chdir(pathlib.Path(__file__).parent.absolute())
import argparse
import subprocess
import ifcfg
import requests
import json
import time
from datetime import datetime
import random
import RPi.GPIO as GPIO
from packages.CATM1 import CATM1
from packages.NRF import NRF

### GPIO BCM ###
TASK_MODE_PIN = 20    # NRF - Task Mode Signal Pin (input)
RPI_OFF_PIN = 21      # NRF - Work Completion Signal Pin (output)
MODEM_PWR_PIN = 17    # CAT.M1 Power Pin (output)
MODEM_STAT_PIN = 27   # CAT.M1 Status Pin (input)
MODEM_SER_PORT = "/dev/ttyS0"

def adminMode (nrf):
    subprocess.run("sudo systemctl start raspapd.service", shell=True)
    webProcess = subprocess.Popen(["python3", "webapp/webapp.py", "--ip", "0.0.0.0", "--port", "4000"])
    while nrf.isAdminMode():
        time.sleep(5)
    time.sleep(2)
    webProcess.terminate() # 웹 종료

def normalMode (lte, isPPP):
    ''' Read configuration '''
    f = open("config/config.json", "r")
    config = json.load(f)
    f.close()
    URL = config["SERVER"]["URL"] + config["SERVER"]["ENDPOINT"] + config["DEVICE"]["ID"]
    confidence, nms = config["YOLO"]["CONFIDENCE_THRESHOLD"], config["YOLO"]["NMS_THRESHOLD"]
    resize = config["YOLO"]["RESIZE"]
    
    ''' Capture '''
    TIMESTAMP = datetime.isoformat(datetime.now())[:-3] + "Z"
    IMAGEFILE = TIMESTAMP + ".jpg"
    resWidth = int(resize)
    resHeight = int(float(resWidth / 4) * 3)
    subprocess.run("raspistill -w " + str(resWidth) + " -h " + str(resHeight) + " -t 10 -o results/" + IMAGEFILE, shell=True)

    ''' Detect '''
    detector = "./build/detector "\
            + "ai-cam/model/yolov4-custom_best.weights "\
            + "ai-cam/model/yolov4-custom.cfg "\
            + "ai-cam/model/classes.names "\
            + "results/" + IMAGEFILE\
            + " " + str(confidence) + " " + str(nms) + " " + str(resize)
    process = subprocess.run(detector, capture_output=True, shell=True)
    exitCode = process.returncode
    if exitCode == 0:
        print("No event")
    elif exitCode == 1:
        print("Event detected")
    elif exitCode == 2:
        print("Reading input image failed")
    elif exitCode == 3:
        print("Writing output image failed")
    elif exitCode == 4:
        print("Load model failed")
    elif exitCode == 5:
        print("Inference failed") # C++에 아직 에러처리 구현 안 되어 있음
    else:
        print("Invalid status")

    ''' Get RSSI and BER by AT Command '''
    rssi, ber = lte.rssi, lte.ber
    
    ''' Get battery '''
    battery = random.randrange(1, 100) # 배터리 부분 구현해야함   
    
    ''' Wait until PPP activated '''
    if isPPP:
        while ifcfg.interfaces().get('ppp0') == None:
            print("Wait for ppp activation...")
            time.sleep(1)

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
            time.sleep(2.5)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--p", type=int, help="(optional) '1' when using PPP")
    parser.add_argument("--m", type=str, help="(optioanl) Do only 'normal' or 'admin'")
    args = parser.parse_args()
    isPPP = False
    if args.p is not None and args.p == 1:
        isPPP = True

    nrf = NRF(modePinNum=TASK_MODE_PIN, offPinNum=RPI_OFF_PIN)
    lte = CATM1(serialPort=MODEM_SER_PORT, baudrate=115200, pwrPinNum=MODEM_PWR_PIN, statPinNum=MODEM_STAT_PIN)
    lte.pwrOnModem()

    try:
        if args.m is None:
            # Wait 5 seconds for get admin signal
            if nrf.isAdminMode():
                adminMode(nrf)
            else:
                normalMode(lte, isPPP)
        else:
            if args.m == "normal":
                normalMode(lte, isPPP)
            elif args.m == "admin":
                adminMode(nrf)
            else:
                print("Invalid argument")
                raise ValueError
    except Exception as e:
        print("exception occured:", e)
    finally:
        print("End process")
        lte.disablePpp()
        lte.pwrOffModem()
        nrf.pwrOffPi()
