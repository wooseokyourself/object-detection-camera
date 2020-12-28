from EmulatorGUI import GPIO

def Main():
    try:
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
        GPIO.setup(4, GPIO.OUT)
        GPIO.setup(15, GPIO.IN, pull_up_down = GPIO.PUD_UP)
        GPIO.setup(23, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
        while(True):
    except Exception as ex:
        traceback.print_exc()
    finally:
        GPIO.cleanup()

Main()