import requests, json
import packages.Define as Define

class WEB:
    def __init__(self, url):
        self.endPoint = url
        
    def post(time, event, rssi, battery, imagefile=None):
        data = {}
        if event == False:
            data = {"time":time, "event":0, "rssi":rssi, "battery":battery}
        else:
            data = {"time":time, "event":1, "rssi":rssi, "battery":battery, "filename":imagefile, "files":"@results/"+imagefile}
        response = requests.post(self.endPoint, json=data)
        return response.status_code, response.text