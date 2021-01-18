import sys 
sys.path.append("../")
from packages.Define import *
from packages.CATM1 import CATM1

node = CATM1(serialPort=MODEM_SER_PORT, baudrate=115200, pwrPinNum=MODEM_PWR_PIN, statPinNum=MODEM_STAT_PIN)
node.pwrOnModem()