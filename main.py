import RPI.GPIO as GPIO
import subprocess

GPIO.setmode(GPIO.BCM)

modePin = # Input: Basic or Admin
outPin = # Output: Task off

GPIO.setup(modePin, GPIO.IN)
GPIO.setup(outPin, GPIO.OUT)
# 저항 연결시 마지막인자에 pull_up_down=GPUO.PUD_UP / DOWN 

if GPIO.input(modePin) == GPIO.LOW:
    # Basic task
    # LTE 모뎀 전원 인가
    subprocess.call("./detect-send.sh", shell=True)
else:
    # Admin task
    

def work(id, start, end, result):
    # work


GPIO.output(outPin, GPIO.HIGH) # Task off

GPIO.cleanup()