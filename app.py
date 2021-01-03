import subprocess
from datetime import datetime
import random
import packages.Define as Define
#Define.GPIO_EMULATOR = True
from packages.API.CATM1 import CATM1
from packages.API.NRF import NRF
from packages.API.WEB import WEB

TIMESTAMP = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
IMAGEFILE = TIMESTAMP + ".jpg"
detector = "./build/detector model/yolov4-custom_best.weights model/yolov4-custom.cfg model/classes.names results/" + IMAGEFILE + " 0.4 0.5 416"

### GPIO BCM ###
taskModePin = 15    # NRF - Task Mode Signal Pin (input)
rpiOffPin = 29      # NRF - Work Completion Signal Pin (output)
ltePwrPin = 21      # CAT.M1 Power Pin (output)
lteStatPin = 23     # CAT.M1 Status Pin (input)

lte = CATM1(serialPort='/dev/ttyS0', baudrate=115200, pwrPinNum=ltePwrPin, statPinNum=lteStatPin)
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
    lte.pwrOnModem() # LTE power on
    rssi, battery = lte.getRSSI(), random.randrange(1, 100) # 배터리 부분 구현해야함

    process = subprocess.run(detector, capture_output=True, shell=True)
    exitCode = process.returncode
    
    if exitCode == 0:
        print("No event")
        resCode, resText = web.post(TIMESTAMP, event=False, rssi, battery)
        print(resCode, ":", resText)
    elif exitCode == 1:
        print("Event detected")
        resCode, resText = web.post(TIMESTAMP, event=False, rssi, battery, IMAGEFILE)
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
        subprocess.call("sudo poweroff", shell=True) # shutdown raspi

main()