import sys 
sys.path.append("../")
import subprocess
import argparse

parser = argparse.ArgumentParser(description="cam and yolo test")
parser.add_argument("--out", type=str, help="output image path", required=True)
parser.add_argument("--thr", nargs=2, type=str, help="conf/nms threshold (default= 0.4 0.5)")
parser.add_argument("--resize", type=str, help="resize (default=416)")
args = parser.parse_args()

outp = args.io[0]
conf, nms = 0.4, 0.5
if args.thr is not None:
    conf, nms = args.thr[0], args.thr[1]
resize = 416
if args.resize is not None:
    resize = args.resize[0]

detector = "./../build/detector ../model/yolov4-custom_best.weights ../model/yolov4-custom.cfg ../model/classes.names " + outp + " " + conf + " " + nms + " " + resize

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