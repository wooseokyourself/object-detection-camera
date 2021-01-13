from flask import Flask
from flask import Response
from flask import render_template
from flask import redirect
from flask import url_for
from flask import request
from flask import session
from flask import abort
from flask import flash
import threading
import argparse
import datetime
import imutils
import time
import json
from imutils.video import VideoStream
import imutils.encodings as encodings
import cv2

outputFrame = None
lock = threading.Lock()
app = Flask(__name__)
vs = VideoStream(usePiCamera=1, framerate=10).start()
time.sleep(2.0)

@app.route('/submit', methods=['POST'])
def submit(confidence_threshold=None, nms_threshold=None, resize=None):
    if request.method == 'POST':
        ''' 아래 값들을 config.json 에 넣는 코드 구현해야함 '''
        print(request.form['confidence_threshold'])
        print(request.form['nms_threshold'])
        print(request.form['resize'])
        
    return render_template('index.html')

@app.route('/')
def index():
    f = open("config/config.json", "r")
    config = json.load(f)
    confidence, nms = config["YOLO"]["CONFIDENCE_THRESHOLD"], config["YOLO"]["NMS_THRESHOLD"]
    resize = config["YOLO"]["RESIZE"]
    f.close()
    ''' 여기에서 conf, nms, resize 를 웹 상의 UI에 집어넣어야함. '''
    return render_template('index.html')

def readFrame(frameCount):
    global vs, outputFrame, lock
    while True:
        frame = vs.read()
        frame = imutils.resize(frame, width=400)
        timestamp = datetime.datetime.now()
        cv2.putText(frame, timestamp.strftime(
			"%A %d %B %Y %I:%M:%S%p"), (10, frame.shape[0] - 10),
			cv2.FONT_HERSHEY_SIMPLEX, 0.35, (0, 0, 255), 1)
        with lock:
            outputFrame = frame.copy()
    
def generate():
    global outputFrame, lock
    while True:
        with lock:
            if outputFrame is None:
                continue
            (flag, encodedImage) = cv2.imencode(".jpg", outputFrame)
            if not flag:
                continue
        yield(b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' + 
			bytearray(encodedImage) + b'\r\n')

@app.route("/videoFeed")
def videoFeed():
	# return the response generated along with the specific media
	# type (mime type)
	return Response(generate(),
		mimetype = "multipart/x-mixed-replace; boundary=frame")

if __name__ == '__main__':
	# construct the argument parser and parse command line arguments
	ap = argparse.ArgumentParser()
	ap.add_argument("-i", "--ip", type=str, required=True,
		help="ip address of the device")
	ap.add_argument("-o", "--port", type=int, required=True,
		help="ephemeral port number of the server (1024 to 65535)")
	ap.add_argument("-f", "--frame-count", type=int, default=32,
		help="# of frames used to construct the background model")
	args = vars(ap.parse_args())
	# start a thread that will perform motion detection
	t = threading.Thread(target=readFrame, args=(
		args["frame_count"],))
	t.daemon = True
	t.start()
	# start the flask app
	app.run(host=args["ip"], port=args["port"], debug=True,
		threaded=True, use_reloader=False)
# release the video stream pointer
vs.stop()