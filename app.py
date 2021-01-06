import argparse
import subprocess
import ifcfg, json
from datetime import datetime
import random
import packages.Define as Define
import RPi.GPIO
# Define.GPIO_EMULATOR = True
from packages.API.CATM1 import CATM1
from packages.API.NRF import NRF
from packages.API import web

### GPIO BCM ###
taskModePin = 20    # NRF - Task Mode Signal Pin (input)
rpiOffPin = 21      # NRF - Work Completion Signal Pin (output)
ltePwrPin = 17       # CAT.M1 Power Pin (output)
lteStatPin = 27     # CAT.M1 Status Pin (input)

nrf = NRF(taskPinNum=taskModePin, offPinNum=rpiOffPin)
URL="http://ino-on.umilevx.com/api/devices/events/ino-on-0000"

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
    ''' Capture, Detect '''
    TIMESTAMP = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
    IMAGEFILE = TIMESTAMP + ".jpg"
    detector = "./build/detector model/yolov4-custom_best.weights model/yolov4-custom.cfg model/classes.names results/" + IMAGEFILE + " "
    with open("config/config.json", "r") as f:
        config = json.load(f)
        confidence, nms = config["YOLO"]["CONFIDENCE_THRESHOLD"], config["YOLO"]["NMS_THRESHOLD"]
        resize = config["YOLO"]["RESIZE"]
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
    lte = CATM1(serialPort='/dev/ttyS0', baudrate=115200, pwrPinNum=ltePwrPin, statPinNum=lteStatPin)
    lte.pwrOnModem()

    ''' Get RSSI and BER by AT Command '''
    csq = lte.getRSSI()
    rssi, ber = "", ""
    sep = csq.find(",")
    if sep != -1:
        rssiIdx = sep - 2
        berIdx = sep + 1
        while rssiIdx != sep:
            if csq[rssiIdx].isdigit():
                rssi += csq[rssiIdx]
            rssiIdx += 1
        while berIdx != sep + 3:
            if csq[berIdx].isdigit():
                ber += csq[berIdx]
            berIdx += 1
    
    ''' Get battery '''
    battery = random.randrange(1, 100) # 배터리 부분 구현해야함   
    
    ''' POST '''
    data = {}
    if exitCode == 0:
        data = {"time":TIMESTAMP, "event":str(0), "rssi":rssi, "battery":str(battery)}
    else:
        data = {"time":TIMESTAMP, "event":str(1), "rssi":rssi, "battery":str(battery), "filename":IMAGEFILE, "files":"@results/"+IMAGEFILE}

    isPPP = 'ppp0' in ifcfg.interfaces()
    if isPPP:
        resCode, resText = web.post(URL, data)
        print(resCode, ":", resText)
    else:
        # AT Command for http request
    
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