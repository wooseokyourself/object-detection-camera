import sys
sys.path.append("..")
from packages.Define import Define
Define.GPIO_EMULATOR = True
from packages.API.CATM1 import CATM1
from packages.API.NRF import NRF
from packages.API.WEB import WEB

taskModePin = 15    # NRF - Task Mode Signal Pin (input)
rpiOffPin = 29      # NRF - Work Completion Signal Pin (output)
ltePwrPin = 21      # CAT.M1 Power Pin (output)
lteStatPin = 23     # CAT.M1 Status Pin (input)

print("NRF: Task Mode Pin  =", taskModePin)
print("NRF: Raspi Off Pin  =", rpiOffPin)
print("CAT: LTE Power Pin  =", ltePwrPin)
print("CAT: LTE Status Pin =", lteStatPin)
print("\n")

print("CAT LTE Power Pin Test")
lte = CATM1(serialPort='/dev/ttyS0', baudrate=115200, pwrPinNum=ltePwrPin, statPinNum=lteStatPin)
print("...connected", lte.getPwrPinNum, " Pin")
print("\n")
print("CAT LTE Status Pin Test")
print("...connected", lte.getStatusPinNum, " Pin")

nrf = NRF(taskPinNum=taskModePin, offPinNum=rpiOffPin)
print("\n")
print("NRF Task Mode Pin Test")
print("Press", taskModePin, " Pin in 5 secs")
if nrf.isAdminMode(timeout=5) == True:
    print("...You pressed", taskModePin, " Pin.")
else:
    print("...You didn't press", taskModePin, " Pin.")

print("\n")
print("NRF Raspi Off Pin Test")
print(rpiOffPin, " Pin will be high after 1 sec")
nrf.pwrOffPi()

print("end")