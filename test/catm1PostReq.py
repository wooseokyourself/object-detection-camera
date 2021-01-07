import sys, time
sys.path.append("..")
import json
from packages.API.CATM1 import CATM1
import RPi.GPIO as GPIO

node = CATM1()
node.pwrOnModem()

URL="http://ino-on.umilevx.com/api/devices/events/ino-on-0000"
data = {"time":"0000-00-00_00:00:00", "event":"0", "rssi":"99", "battery":"0"}

try:
    time.sleep(3)
    response = node.post(URL, data)
    print(response)
except Exception as e:
    print("error:", e)
finally:
    node.pwrOffModem()
    GPIO.cleanup()