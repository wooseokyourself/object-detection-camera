import time
import serial
import re
import threading
import json
import packages.Define as Define
if Define.GPIO_EMULATOR == True:
    from packages.GPIOEmulator.EmulatorGUI import GPIO
else:
    import RPi.GPIO as GPIO
"""
AT 커맨드는 아래 사이트 참조하였음
https://m2msupport.net/m2msupport/atcsq-signal-quality/
"""
ATCmdList = {
    # REV=ANY 일 경우 어떤 값을 읽더라도 에러로 처리하지 않음.
    'RSSI': {'CMD': "AT+CSQ", 'REV': "\r\nOK\r\n"}, # custom
    'IMEI': {'CMD': "AT+CGSN", 'REV': "\r\nOK\r\n"},
    'FWInfo': {'CMD': "AT+CGMR", 'REV': "\r\nOK\r\n"},
    'HWInfo': {'CMD': "AT+CGMM", 'REV': "\r\nOK\r\n"},
    'NumberInfo': {'CMD': "AT+CIMI", 'REV': "\r\nOK\r\n"},
    'AttachNet' : {'CMD': "AT+QIACT=1", 'REV': "\r\nOK\r\n"},
    'myIP' : {'CMD': "AT+QIACT?", 'REV': "\r\nOK\r\n"},
    'DetachNet' : {'CMD': "AT+QIDEACT=1", 'REV': "\r\nOK\r\n"},
    'IsAttachNet' : {'CMD': "AT+CEREG?", 'REV': "\r\nOK\r\n"},
    'OpenSCK' : {'CMD': "AT+QIOPEN=1,", 'REV': "\r\n+QIOPEN"},
    'CloseSCK' : {'CMD': "AT+QICLOSE=", 'REV': "\r\nOK\r\n"}, 
    'SendSCK' : {'CMD': "AT+QISENDEX=", 'REV': "\r\nSEND OK"},
    'ReceiveSCK' : {'CMD': "AT+QIRD=", 'REV': "\r\nOK\r\n"},

    'ICSGP' : {'CMD': "AT+QICSGP=", 'REV': "\r\nOK\r\n"}, # <APN>
    'IACT' : {'CMD': "AT+QIACT=", 'REV': "\r\nOK\r\n"}, 
    'IDEACT' : {'CMD': "AT+QIDEACT=", 'REV': "\r\nOK\r\n"}, 
    'HTTPCFG' : {'CMD': "AT+QHTTPCFG=", 'REV': "\r\nOK\r\n"},
        # "requestheader",<request_header> ; Disable(0) or enable(1) to customize HTTP request header.
    # POSTFILE은 데이터의 내용을 파일로부터 읽어서 보내고, 응답을 파일로 저장하는 명령.
    'SSLCFG' : {'CMD': "AT+QSSLCFG=", 'REV': "ANY"}, 
    'HTTPURL' : {'CMD': "AT+QHTTPURL=", 'REV': "\r\nCONNECT\r\n"},  # <URL_length> ; byte size
    'HTTPPOST' : {'CMD': "AT+QHTTPPOST=", 'REV': "\r\nCONNECT\r\n"}, # <data_length> ; byte size
    'HTTPPOSTFILE' : {'CMD': "AT+QHTTPPOSTFILE=", 'REV': "\r\nOK\r\n"}, # <filepath>,<timeout> 
    'HTTPREAD' : {'CMD': "AT+QHTTPREAD=", 'REV': "\r\nCONNECT\r\n"}, 
    'HTTPREADFILE' : {'CMD': "AT+QHTTPREADFILE=", 'REV': "\r\nOK\r\n"}, # <filepath>,<timeout> 
    
}

IsRevModemData = False

def isError(recv, why):
    if recv == "Error":
        print(why)
        return True
    else:
        print(recv)
        return False

class CATM1:
    ser = None
    isConectSerial = False
    __TimeOut = 5     # seconds

    '''
    Raspberry Pi 4B default serial port -> /dev/ttyS0
        when serial login shell disabled, serial interface enabled in raspi-config
    CAT.M1 default power pin -> 17
    CAT.M1 default status pin -> 27
    '''
    def __init__(self, serialPort='/dev/ttyS0', baudrate=115200, pwrPinNum=17, statPinNum=27):

        # serial port setup
        if(CATM1.isConectSerial == False):
            CATM1.ser = serial.Serial()
            CATM1.ser.port = serialPort
            CATM1.ser.baudrate = baudrate
            CATM1.ser.parity = serial.PARITY_NONE
            CATM1.ser.stopbits = serial.STOPBITS_ONE
            CATM1.ser.bytesize = serial.EIGHTBITS
            CATM1.isConectSerial = True

        # Modem Power setup
        self.pwrPinNum = pwrPinNum
        self.statPinNum = statPinNum
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.pwrPinNum, GPIO.OUT)
        GPIO.setup(self.statPinNum, GPIO.IN)

        if(GPIO.input(self.statPinNum)==1):
            print("Reset Modem..")
            self.pwrOffModem()

        self.compose = ""
        self.response = ""
        self.timeout = CATM1.__TimeOut

        self.ipAddress = ""
        self.portNum = ""

    def getPwrPinNum(self):
        ''' get modem Power pin number '''
        return self.pwrPinNum
    
    def getStatusPinNum(self):
        ''' get modem Status pin number '''
        return self.statPinNum

    def pwrOnModem(self):
        print ("Start Modem..")
        GPIO.output(self.pwrPinNum, GPIO.HIGH)
        self.sendATCmd("AT", "OK\r\n") # Wait for turning on
        if(GPIO.input(self.statPinNum) == 1):
            print("Modem Ready..")
        else:
            print("Modem Not Ready..")

    def pwrOffModem(self):
        GPIO.output(self.pwrPinNum, GPIO.LOW)

    def resetModem(self):
        self.pwrOffModem()
        self.pwrOnModem()

    def setIPAddress(self, ip):
        ''' set ip address'''
        self.ipAddress = ip
    
    def setDNSAddress(self, dns):
        ''' set ip address'''
        self.ipAddress = dns

    def setPortNum(self, port):
        ''' set port number '''
        self.portNum = str(port)

    def __getMillSec(self):
        ''' get miliseconds '''
        return int(time.time())
    
    def __delay(self, ms):
        ''' delay as millseconds '''
        time.sleep(float(ms/1000.0))

    def __readATResponse(self, cmd_response):
        ''' getting respnse of AT command from modem '''
        timer = self.__getMillSec()
        timeout = self.timeout
        response = self.response

        while True:
            self.response = ""
            while(CATM1.ser.inWaiting()):
                try:
                    self.response += CATM1.ser.read(CATM1.ser.inWaiting()).decode('utf-8', errors='ignore')
                    #print("read response: " + self.response)
                    response = self.response
                    self.__delay(50)
                except Exception as e:
                    print(e)
                    return False
            if(self.response.find(cmd_response) != -1):
                #print("read response: " + self.response)
                return True
            if((self.__getMillSec() - timer) > timeout):
                # error rasie
                print("Recv failed: " + response)
                return False

    def readAdditionalATResponse(self):
        ''' 일정 시간 뒤에 들어오는 응답을 위해 대기 '''
        timer = self.__getMillSec()
        timeout = self.timeout
        response = self.response

        while True:
            self.response = ""
            while(CATM1.ser.inWaiting()):
                try:
                    self.response += CATM1.ser.read(CATM1.ser.inWaiting()).decode('utf-8', errors='ignore')
                    #print("read response: " + self.response)
                    response = self.response
                    self.__delay(50)
                except Exception as e:
                    print(e)
                    return False
            if((self.__getMillSec() - timer) > timeout):
                # error rasie
                print("Recv failed: " + response)
                return False
            return True
        

    def __sendATCmd(self, command):
        ''' Sending at AT command to module '''
        self.compose = ""
        self.compose = str(command) + "\r"
        CATM1.ser.reset_input_buffer()
        CATM1.ser.write(self.compose.encode())
        print(self.compose)
    
    def sendATCmd(self, command, cmd_response, timeout=None):
        ''' Send AT command & Read command response ''' 
        print("send AT command: " + command)
        if(CATM1.ser.isOpen() == False):
            CATM1.ser.open()

        if timeout is None:
            timeout = self.timeout
        
        self.__sendATCmd(command)

        timer = self.__getMillSec()

        while True: 
            if (self.__getMillSec() - timer) > timeout:
                # error rasie
                print(command + " / Send failed ")
                return "Error"
            
            if self.__readATResponse(cmd_response):
                return self.response

    # AT command methods
    def getRSSI(self, timeout=None): # custom
        ''' get RSSI number'''
        recv = self.sendATCmd(ATCmdList['RSSI']['CMD'], ATCmdList['RSSI']['REV'], timeout)
        if recv == "Error":
            return "Error", "Error"
        csq = recv[:recv.index(ATCmdList['RSSI']['REV'])]
        rssi, ber = "", ""
        sep = csq.find(",")
        if sep != -1:
            rssiIdx = sep - 2
            berIdx = sep + 1
            while rssiIdx != sep:
                if csq[rssiIdx].isdigit():
                    rssi += csq[rssiIdx]
                rssiIdx += 1
            while berIdx != sep + 3:
                if csq[berIdx].isdigit():
                    ber += csq[berIdx]
                berIdx += 1
        return rssi, ber

    def getIMEI(self, timeout=None):
        ''' get IMEI number'''
        data = self.sendATCmd(ATCmdList['IMEI']['CMD'], ATCmdList['IMEI']['REV'], timeout)
        return data[:data.index(ATCmdList['IMEI']['REV'])]

    def getFirmwareInfo(self, timeout=None):
        ''' get FW version '''
        data =  self.sendATCmd(ATCmdList['FWInfo']['CMD'], ATCmdList['FWInfo']['REV'], timeout)
        return data[:data.index(ATCmdList['FWInfo']['REV'])]

    def getHardwareInfo(self, timeout=None):
        ''' get modem model info '''
        data = self.sendATCmd(ATCmdList['HWInfo']['CMD'], ATCmdList['HWInfo']['REV'], timeout)
        return data[:data.index(ATCmdList['HWInfo']['REV'])]

    def getPhoneNumberInfo(self, timeout=None):
        ''' get modem phone number '''
        data = self.sendATCmd(ATCmdList['NumberInfo']['CMD'], ATCmdList['NumberInfo']['REV'], timeout)
        return data[:data.index(ATCmdList['NumberInfo']['REV'])]
 
    def attachNetwork(self, connect=True, timeout=10):
        ''' Activate/Deactivate a PDP Context '''
        if(connect):
            return self.sendATCmd(ATCmdList['AttachNet']['CMD'], ATCmdList['AttachNet']['REV'], timeout)
        else:
            return self.sendATCmd(ATCmdList['DetachNet']['CMD'], ATCmdList['DetachNet']['REV'], timeout)
    
    def isAttachNetwork(self, timeout=None):
        ''' True : LTE CAT.M1 Network attached, False : LTE CAT.M1 Network detached ''' 
        data = self.sendATCmd(ATCmdList['IsAttachNet']['CMD'], ATCmdList['IsAttachNet']['REV'], timeout)
        _str = ',1'
        if(data.find(_str)==-1):
            return False
        else:
            return True

    def myIP(self, timeout=10):
        ''' get modem IP Address '''
        data = self.sendATCmd(ATCmdList['myIP']['CMD'], ATCmdList['myIP']['REV'], timeout)
        _str = ',"'
        _len = len(data)-7

        idx = data.find(_str)
        if(idx == -1):
            return False
        else:
            return data[idx+1:_len]
 
    # Socket methods 
    def openSocket(self, mySocket, isTCP, ip_address=None, ip_port=None):
        if (isTCP == True):
            command = ATCmdList['OpenSCK']['CMD'] + str(mySocket) + ',"TCP",'
        else:
            command = ATCmdList['OpenSCK']['CMD'] + str(mySocket) + ',"UDP",'

        if ip_address is None:
            command += self.ipAddress
        else:
            command += str(ip_address)

        command += ','
        if ip_port is None:
            command += self.portNum
        else:
            command += str(ip_port)
        command += ',0,0'	#Buffer access mode 

        #print(command)

        data = self.sendATCmd(command, ATCmdList['OpenSCK']['REV'], 10)

        if( data  == "Error" ):
            return False
        else:
            return True
    
    def closeSocket(self, mySocket):
        command = ATCmdList['CloseSCK']['CMD'] + str(mySocket)
        command += ',3'	#TimeOut Default 10 to 3 seconds
        data = self.sendATCmd(command, ATCmdList['CloseSCK']['REV'], 10)
        if( data  == "Error" ):
            return False
        else:
            return True
        
    # HTTP request methods
    def post(self, url, data):
        '''
        http://www.dragino.com/downloads/downloads/NB-IoT/BG96/Quectel_BG96_HTTP%28S%29_AT_Commands_Manual_V1.0.pdf
        위 사이트 보고 잘 해봐라,,
        '''
        
        ''' 
        BG96의 시리얼포트에는 AT command mode와 Data mode가 존재하는데, 
        전자는 데이터를 AT command로 인식하고 후자는 AT command를 데이터로 인식함. 
        QHTTPURL, QHTTPPOST, QHTTPREAD 가 실행되면 BG96의 시리얼포트는 데이터모드로 진입함.
        "+++" 혹은 DTR을 pulling (AT&D1 이 먼저 셋업되어야함) 함으로써 데이터모드를 종료함.
        데이터모드에서 "+++" 자체를 데이터로 인식하는 일이 없게하기 위해 다음 사항을 지켜야 함.
            1) Do not input any character within 1s or longer before inputting "+++".
            2) Input "+++" within 1s, and no other characters can be inputted during the time.
            3) Do not input any character within 1s after "+++" has been inputted.
        '''
        
        # Step 1. Configure <APN>, <username>, <password> in PDP context by AT+QICSGP.
        # Step 2. Activate the PDP context by AT+QIACT.
        # Step 3. Configure the PDP context ID and SSL context ID by AT+QHTTPCFG.
        # Step 4. Configure SSL context parameters by AT+QSSLCFG.
        # Step 5. Set HTTP URL by AT+QHTTPURL.
        # Step 6. Send HTTP request.
        # Step 7. Read HTTP response.
        # Step 8. Deactivate the PDP context by AT+QIDEACT.

        command, expected = ATCmdList['HTTPCFG']['CMD'] + '"contextid",1', ATCmdList['HTTPCFG']['REV']
        recv = self.sendATCmd(command, expected)
        if isError(recv, "Failed to configure PDP context ID as 1."):
            return
        
        command, expected = ATCmdList['HTTPCFG']['CMD'] + '"contenttype",3', ATCmdList['HTTPCFG']['REV']
        recv = self.sendATCmd(command, expected)
        if isError(recv, "Failed to configure content type as 'multipart/form-data'."):
            return

        command, expected = ATCmdList['ICSGP']['CMD'] + '1,1,"internet.lte.cxn","","",1', ATCmdList['ICSGP']['REV']
        recv = self.sendATCmd(command, expected)
        if isError(recv, "Failed to configure APN."):
            return

        command, expected = ATCmdList['IACT']['CMD'] + '1', ATCmdList['IACT']['REV']
        recv = self.sendATCmd(command, expected)
        if isError(recv, "Failed to activate PDP context 1."):
            return
    
        urlBytesLen = len(url.encode('utf-8'))
        command, expected = ATCmdList['HTTPURL']['CMD'] + str(urlBytesLen) + ",80", ATCmdList['HTTPURL']['REV']
        recv = self.sendATCmd(command, expected)
        if isError(recv, "Failed to prepare for getting URL"):
            return
        
        recv = self.sendATCmd(url, "\r\nOK\r\n")
        if isError(recv, "Failed to send URL"):
            return

        strData = json.dumps(data, indent=4)
        dataBytesLen = len(strData.encode('utf-8'))
        command, expected = ATCmdList['HTTPPOST']['CMD'] + str(dataBytesLen) + ",80,80", ATCmdList['HTTPPOST']['REV']
        recv = self.sendATCmd(command, expected)
        if isError(recv, "Failed to prepare for getting POST request"):
            return
        
        recv = self.sendATCmd(strData, "\r\nOK\r\n")
        if isError(recv, "Failed to send POST request"):
            return
        if self.readAdditionalATResponse() == True: 
            print(recv)
        
        command, expected = ATCmdList['HTTPREAD']['CMD'] + "80", ATCmdList['HTTPREAD']['REV']
        recv = self.sendATCmd(command, expected)
        print(recv)
        if isError(recv, "Failed to prepare for receiving POST response"):
            return
        if self.readAdditionalATResponse() == True:
            print(recv)
        
        return recv # 다듬어서 내보내야겠지..
        
        '''
        # Customize HTTP request header
        command = ATCmdList['HTTPCFG']['CMD'] + '"responseheader",1'
        if self.sendATCmd(command, ATCmdList['HTTPCFG']['REV']):
            print "Failed to set HTTP config"
            return
        '''     
        
        '''
        fronturl, backurl = "", ""
        idx = url.find("com/")
        if idx == -1:
            print("Failed to find '.com' in URL")
            return
        fronturl = url[: idx + 2] # http://ino-on.umilevx.com
        backurl = url[idx + 3 :] # /api/devices/events/ino-on-0000

        

        "POST " + backurl + " HTTP/1.1\r\n" # 여기에 .php가 붙어야 할까?
        "Host: " + fronturl + "\r\n" # 여기에 포트넘버가 붙어야 할까?
        "Accept: */*\r\n" # 서버응답 모두 받기
        "User-Agent: Ino-on AICamera\r\n" # 라즈베리파이
        # "Connection: Keep-Alive\r\n" # 계속 연결유지, 이기능 필요없으니까 이 라인은 없어도 됨
        "Content-Type: multipart/form-data" # multipart
        "Content-Length: " + str(dataBytesLen) + "" # 헤더를 제외한 Body 길이
        '''

    # data type
    def sendSCKData(self, mySocket, data):
        ''' send UDP data 
            max data size: 512 bytes (Hex string) '''
        command = ATCmdList['SendSCK']['CMD'] + str(mySocket) + ","
        command += '"'
        command += data.encode("utf-8").hex()
        command += '"'

        result = self.sendATCmd(command, ATCmdList['SendSCK']['REV'], 10)

        if( result == "Error" ):
            return False
        else:
            return True

    def __revModem_Thread(self):
        if self.__readATResponse('+QIURC: "recv"'):
            global IsRevModemData
            IsRevModemData  = True

    def receiveSCKData(self, mySocket, rev_length=256, rev_timeOut=10):
        ''' recevie buffer size : 1460 bytes 
        return : ['ip','port','data length', 'data', 'renainnig length']'''
        duration = 500
        count = ((rev_timeOut*1000)/duration)
        datareceve = False
        data_length = ""

        global IsRevModemData
        IsRevModemData  = False 
        t1 = threading.Thread(target=self.__revModem_Thread)
        t1.daemon = True
        t1.start()
        for i in range(0,int(count)):
            if IsRevModemData:
               datareceve = True
               break

            else:
                #print("wait index {}".format(i))
                self.__delay(duration)

        if datareceve == False:
            try:
                t1._stop()
                del t1
            except AssertionError as e:
                print(e)
            
        if(datareceve):
            command = ATCmdList['ReceiveSCK']['CMD'] + str(mySocket)
            if (self.sendATCmd(command, ATCmdList['ReceiveSCK']['REV']) != "Error"):
               data = self.response.split('\n')

               result = data[2]
               result2 = result[:-1]
               length = len(result2)
               #print( str(length) )
		       	   
               return result2

        print("Data read fail")
        return ['-1','-1','-1','-1','-1']
