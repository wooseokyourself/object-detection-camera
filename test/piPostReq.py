import requests, json
from pprint import pprint

URL="http://ino-on.umilevx.com/api/devices/events/ino-on-0000"
data = {"time":"0000-00-00_00:00:00", "event":"0", "rssi":"99", "battery":"0"}

response = requests.post(URL, files=data)
pprint(response.headers)