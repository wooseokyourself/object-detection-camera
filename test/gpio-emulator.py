import sys 
sys.path.append("../")
from packages.Define import *
GPIO_EMULATOR = True
from packages.API.CATM1 import CATM1
from packages.API.NRF import NRF

print("NRF: Task Mode Pin  =", TASK_MODE_PIN)
print("NRF: Raspi Off Pin  =", RPI_OFF_PIN)
print("CAT: LTE Power Pin  =", MODEM_PWR_PIN)
print("CAT: LTE Status Pin =", MODEM_STAT_PIN)
print("\n")

print("CAT LTE Power Pin Test")
lte = CATM1(serialPort=MODEM_SER_PORT, baudrate=115200, pwrPinNum=MODEM_PWR_PIN, statPinNum=MODEM_STAT_PIN)
print("...connected", lte.getPwrPinNum(), " Pin")
print("\n")
print("CAT LTE Status Pin Test")
print("...connected", lte.getStatusPinNum(), " Pin")
lte.pwrOnModem()

nrf = NRF(taskPinNum=TASK_MODE_PIN, offPinNum=RPI_OFF_PIN)
print("\n")
print("NRF Task Mode Pin Test")
print("Press", TASK_MODE_PIN, " Pin in 5 secs")
if nrf.isAdminMode(timeout=5) == True:
    print("...You pressed", TASK_MODE_PIN, " Pin.")
else:
    print("...You didn't press", TASK_MODE_PIN, " Pin.")

print("\n")
print("NRF Raspi Off Pin Test")
print(RPI_OFF_PIN, " Pin will be high after 1 sec")
nrf.pwrOffPi()

print("end")