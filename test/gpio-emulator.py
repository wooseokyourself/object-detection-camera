import packages.Define as Define
Define.GPIO_EMULATOR = True
from packages.API.CATM1 import CATM1
from packages.API.NRF import NRF

taskModePin = 15    # NRF - Task Mode Signal Pin (input)
rpiOffPin = 29      # NRF - Work Completion Signal Pin (output)
ltePwrPin = 21      # CAT.M1 Power Pin (output)
lteStatPin = 23     # CAT.M1 Status Pin (input)

print("NRF: Task Mode Pin  =", taskModePin)
print("NRF: Raspi Off Pin  =", rpiOffPin)
print("CAT: LTE Power Pin  =", ltePwrPin)
print("CAT: LTE Status Pin =", lteStatPin)