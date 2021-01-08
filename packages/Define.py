GPIO_EMULATOR = False

### GPIO BCM ###
TASK_MODE_PIN = 20    # NRF - Task Mode Signal Pin (input)
RPI_OFF_PIN = 21      # NRF - Work Completion Signal Pin (output)
MODEM_PWR_PIN = 17       # CAT.M1 Power Pin (output)
MODEM_STAT_PIN = 27     # CAT.M1 Status Pin (input)

MODEM_SER_PORT = "/dev/ttyS0"

URL="http://ino-on.umilevx.com/api/devices/events/ino-on-0000"