import sys
sys.path.append("..")
from packages.API.CATM1 import CATM1

ltePwrPin = 17       # CAT.M1 Power Pin (output)
lteStatPin = 27     # CAT.M1 Status Pin (input)

node = CATM1(serialPort='/dev/ttyAMA0', baudrate=115200, pwrPinNum=ltePwrPin, statPinNum=lteStatPin)

print( "RPI3 Modem Power PIN: " + str(node.getPwrPinNum()) )
print( "RPI3 Modem Status PIN: " + str(node.getStatusPinNum()) )

print("AT: " + node.sendATCmd("AT", "OK\r\n", timeout=10))
print("ATE0: " + node.sendATCmd("ATE0", "OK\r\n", timeout=10))
print("IMEI: " + node.getIMEI(timeout=10)) 
print("RSSI: " + node.getRSSI(timeout=10))
print("FW Ver: " + node.getFirmwareInfo(timeout=10))
print("HW model: " + node.getHardwareInfo(timeout=10))
print("Phone Number: " + node.getPhoneNumberInfo(timeout=10))
time.sleep(2)

#program End
print("end")
GPIO.cleanup()