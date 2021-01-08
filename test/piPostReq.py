import sys 
sys.path.append("../")
from packages.Define import *
import requests, json
from pprint import pprint

data = {
        "time": (None, "0000-00-00_00:00:00"), 
        "event": (None, "1"), 
        "rssi": (None, "-31"), 
        "battery": (None, "1"), 
        "filename": (None, "example.jpg"), 
        "files": open("example.jpg", 'rb')
        }
response = requests.post(URL, files=data)
pprint(response)