import asyncio
import requests, json
import subprocess
from datetime import datetime

now = datetime.now()
API_ENDPOINT="http://ino-on.umilevx.com/api/devices/events"
API_METHOD="POST"
TIMESTAMP=str(now.year) + "-" + str(now.month) + "-" + str(now.day) + "_" + str(now.hour) + ":" + str(now.minute) + ":" + str(now.second)
IMAGEFILE=TIMESTAMP + ".jpg"

detector = "./build/detector model/yolov4-custom_best.weights model/yolov4-custom.cfg model/classes.names results/" + IMAGEFILE + " 0.4 0.5 416"

def post (event):
    data = {}
    if event == False:
        data = {"time":TIMESTAMP, "event":0, "rssi":0, "battery":0}
    else:
        data = {"time":TIMESTAMP, "event":1, "rssi":0, "battery":0, "filename":IMAGEFILE, "files":"@results/"+IMAGEFILE}
    return requests.post(API_ENDPOINT, json=data)

async def task ():
    process = subprocess.run(detector, capture_output=True, shell=True)
    exitCode = process.returncode
    response = ""
    if exitCode == 0:
        print("이벤트 없음")
        response = post(event=False)
    elif exitCode == 1:
        print("이벤트 감지")
        response = post(event=True)
    elif exitCode == 2:
        print("카메라 연결 실패")
    elif exitCode == 3:
        print("분석이미지 저장 실패")
    elif exitCode == 4:
        print("모델 로딩 실패")
    elif exitCode == 5:
        print("사진 분석 실패")
    else:
        print("Invalid status")
    print(response.status_code, ":", response.text)