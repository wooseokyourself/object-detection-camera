### Contents
+ [Description](https://github.com/UmileVX/ino-on_AiCam#description)   
+ [Requirements](https://github.com/UmileVX/ino-on_AiCam#requirements)
    + [Software Requirements](https://github.com/UmileVX/ino-on_AiCam#software-requirements)
    + [PIP3 Packages](https://github.com/UmileVX/ino-on_AiCam#pip3-packages)
    + [Setup Guide for Plain Raspberry Pi 4B](https://github.com/UmileVX/ino-on_AiCam#setup-guide-for-plain-raspberry-pi-4b)
+ [Configuration](https://github.com/UmileVX/ino-on_AiCam#configuration)
    + [Raspberry Pi GPIO Pin Number (BCM)](https://github.com/UmileVX/ino-on_AiCam#raspberry-pi-gpio-pin-number-bcm)
    + [Yolov4 Configuration (Json)](https://github.com/UmileVX/ino-on_AiCam#yolov4-configuration-json)
    + [HTTP Request to Server](https://github.com/UmileVX/ino-on_AiCam#http-request-to-server)
+ [Usage](https://github.com/UmileVX/ino-on_AiCam#usage)
    + [Build](https://github.com/UmileVX/ino-on_AiCam#build)
    + [Run](https://github.com/UmileVX/ino-on_AiCam#run)
    + [Test](https://github.com/UmileVX/ino-on_AiCam#test)
+ [Packages](https://github.com/UmileVX/ino-on_AiCam#packages)
    + [GPIOEmulator](https://github.com/UmileVX/ino-on_AiCam#gpioemulator)
    + [Define](https://github.com/UmileVX/ino-on_AiCam#define)
    + [API: NRF](https://github.com/UmileVX/ino-on_AiCam#api-nrf)
    + [API: CATM1](https://github.com/UmileVX/ino-on_AiCam#api-catm1)
+ [Resources](https://github.com/UmileVX/ino-on_AiCam#resources)

****

# Description
## Terms
#### Normal mode (Normal task)
사진을 촬영하고 포크레인을 검출한 뒤 결과값을 서버에 보내는 작업을 의미한다.

#### Admin mode (Admin task)
관리자 페이지를 위한 웹서버를 실행하는 작업을 의미한다.

## Network
Normal mode 에서의 결과를 서버에 보내기 위해(http post) 네트워크가 필요하다. 이 때 PPP 인터페이스를 통해 네트워크에 접속하여 현재 앱에서 요청을 보내는 방법과, 모뎀의 AT Command 를 이용하여 보내는 방법이 존재하는데, AT Command 를 이용한 방법은 아직 제대로 구현이 되지 않았다. 

## Normal Mode
1. 모뎀 전원 연결
2. 사진 촬영 (raspistill 사용)
3. 사진 분석 (ai-cam 디렉토리 소스코드)
4. ~~RSSI와 배터리 값 얻기~~
5. PPP 연결 대기
6. 네트워크 연결 대기 후 POST
7. 모뎀 전원 종료
8. NRF에게 종료신호 송신

## Admin Mode
1. 모뎀 전원 연결
2. 웹서버 실행 (webapp 디렉토리 소스코드)
3. NRF에게 종료신호 송신

****

# Requirements
## Software Requirements
+ Raspbian with Python 3.5 or later with PIP3
+ CMake 2.8 or later 
+ OpenCV 4.4 or later  - [Instruction](https://qengineering.eu/install-opencv-4.4-on-raspberry-64-os.html)(=`install-opencv.sh`)
    > `sudo apt-get install libopencv-dev python3-opencv` 를 이용한 설치는 테스트 해봐야 함 (arm 최적화 등)

Raspberry Pi OS Lite 일 경우 다음도 추가로 설치해야 함
```console
pi@raspberrypi:~ % sudo apt-get install libilmbase-dev libopenexr-dev libgstreamer1.0-dev
```

## PIP3 Packages
```console
pi@raspberrypi:~ % pip3 install RPi.GPIO    # for OS Lite
pi@raspberrypi:~ % pip3 install picamera    # for OS Lite
pi@raspberrypi:~ % pip3 install pyserial    # for OS Lite
pi@raspberrypi:~ % pip3 install flask       # for OS Lite
pi@raspberrypi:~ $ pip3 install ifcfg
pi@raspberrypi:~ $ pip3 install numpy
pi@raspberrypi:~ $ pip3 install opencv-python
pi@raspberrypi:~ $ pip3 install imutils
```

## Setup Guide for Plain Raspberry Pi 4B
### 1. raspi-config
```console
pi@raspberrypi:~ $ sudo raspi-config
```
+ 시리얼 포트 설정 ([reference](https://github.com/codezoo-ltd/CodeZoo_CATM1_Arduino/blob/master/Hands-ON/Cat.M1_RaspberryPi(with%20Python)_HandsON.pdf))
**본 프로젝트의 시리얼 포트 경로: /dev/ttyS0**
1. Interface Options 선택   
2. P6 Serial Port 선택
3. `"Would you like a login shell to be accessible over serial?"` --> **"No"**
4. `"Would you like the serial port hardware to be enabled?"` --> **"Yes"**

+ 부팅시 자동 로그인 설정
1. System Options 선택
2. S5 Boot / Auto Login 선택
3. B2 Console Autologin 혹은 B4 Desktop Autologin 선택

+ 부팅시 프로그램 자동 실행 설정 (모든 Requirements 들을 설치 후 제일 마지막에 설정)
1. `~/.profile` 파일에 다음을 입력
```python3 ino-on_AiCamera/app.py --p 1```


### 2. PPP
**PPP 인터페이스를 이용할 경우 다음을 진행**   
**※ 주의: PPP 인터페이스가 활성화되어있으면 AT Command 를 사용할 수 없다.**
#### 2.1. PPP Install
+ PPP 설치 방법 ([reference](https://github.com/sixfab/Sixfab_PPP_Installer))
1. PPP 설치 파일 다운로드
```console
pi@raspberrypi:~ $ wget https://raw.githubusercontent.com/sixfab/Sixfab_PPP_Installer/master/ppp_installer/install.sh
```
2. install.sh 권한 변경 및 실행
```console
pi@raspberrypi:~ $ chmod 755 install.sh
pi@raspberrypi:~ $ sudo ./install.sh
```
3. 3G/4G Base HAT 선택
4. APN 입력
+ 텔레노어 유심: internet.lte.cxn
+ 타타 유심: move.dataxs.mobi
5. 기타 설정을 다음과 같이 입력
```
Does your carrier need username and password? [Y/n]
>> n
What is your device communication PORT? (ttyS0/ttyUSB3/etc.)
>> ttyS0
Do you want to activate auto connect/reconnect service at R.Pi boot up? [Y/n]
>> Y
```
6. Enter 를 입력하여 라즈베리파이 재실행
**만약 wlan이 인터페이스에 없다면 다음을 입력하여 wlan의 block 풀기**
```console
pi@raspberrypi:~ $ sudo rfkill unblock wlan
```

#### 2.2. PPP Enable/Disable
+ 모뎀 전원이 켜져있을 때, PPP를 `sudo pon`으로 활성화하고, `sudo poff` 로 비활성화 할 수 있다.
+ 임의로 모뎀에 전원을 넣기 위해서는 test 디렉토리에서 `test/catm1PwrOf.py` 를 실행시키면 된다.
+ 임의로 모뎀에 전원을 차단하기 위해서는 test 디렉토리에서 `test/catm1PwrOff.py` 를 실행시키면 된다.
+ 무선랜이 연결되어있는 경우, `sudo ifconfig wlan0 down` 으로 무선랜을 비활성화 한 뒤 PPP 네트워크 연결을 확인한다.


### 3. RaspAP
#### 3.1. RaspAP Install
+ RaspAP 설치 방법 ([reference](https://github.com/billz/raspap-webgui))
```console
pi@raspberrypi:~ $ curl -sL https://install.raspap.com | bash
```
+ 라즈베리파이에서 localhost 웹에 접속하면 라즈베리파이의 시스템 정보 등과 함께 RaspAP 설정 가능
    + ID: admin / Password: secret
+ 외부에서 와이파이로 접속할 경우, SSID: raspi-webgui / Password: ChangeMe
    + 게이트웨이: 10.3.141.1
    + 웹서버에 접속할 경우: 10.3.141.1:4000

### 4. Boot Up Setting
~/.profile 파일의 제일 하단에 본 프로그램의 app.py 를 실행시키는 스크립트를 넣는다.

****

# Configuration
## Raspberry Pi GPIO Pin Number (BCM)
+ 작동상태 모드 확인: 20 input   
    > HIGH: Admin mode   
    > LOW: Normal mode
+ 작업 완료 알림: 21 output   
    > HIGH: pi 작업 완료   
    > LOW: pi 작업 진행중
+ CAT.M1 모뎀 전원 관리: 17 output
    > HIGH: 모뎀 전원 연결   
    > LOW: 모뎀 전원 차단
+ CAT.M1 모뎀 상태: 27 input
    > HIGH: 모뎀 인식   
    > LOW: 모뎀 미인식

※ 모든 핀 번호는 `packages/Define.py` 에 정의되어있다.   

## Yolov4 Configuration (Json)
+ `config/config.json`
+ 각 필드의 의미는 다음과 같다.
    + Object: `YOLO`
        + Data: `CONFIDENCE_THRESHOLD`
            > 0 - 1 사이의 부동소수점. 낮을수록 False Positive 검출 증가, 높을수록 True Positive 검출 감소
        + Data: `NMS_THRESHOLD`
            > 0 - 1 사이의 부동소수점. 높을수록 중복 검출 증가
        + Data: `RESIZE`
            > 32 배수의 정수. 낮을수록 검출속도는 빠르나 작은 객체 검출 불리, 높을수록 검출속도는 느리나 작은 객체 검출 유리

## HTTP Request to Server
+ URL
    + HOST: `http://ino-on.umilevx.com/api/devices/events/<device-id>`
    > `device-id` 는 아직 정해진 바가 없기에 임의로 `ino-on-0000`을 사용중이다.
    + 서버 로그기록 확인: http://ino-on.umilevx.com/api/logs/YYYY-MM-DD.log
    > `YYYY-MM-DD` 는 현재날짜이다.
+ Format
    + 이벤트가 있을 경우
    ```
    {
        "time"="2021-01-08T13:41:21.046Z"
        "event"="1"
        "rssi"="-31"
        "battery"="97"
        "filename"="image001.jpg"
        "files"="@test_event.jpg"
    }
    ```
    + 이벤트가 없을 경우 
    ```
    {
        "time"="2021-01-08T13:41:21.046Z"
        "event"="0"
        "rssi"="-31"
        "battery"="97"
    }
    ```
    > `time`: 사진을 촬영한 시간이다.   
    > `event`:  사진 내에서 객체가 검출된 여부이다.   
    > `rssi`: 모뎀의 RSSI값이다.   
    > `battery`: NRF에 연결되어 공급받는 배터리의 상태이다.   
    > `filename`: 서버에 저장될 이미지 파일의 서버 경로이다.   
    > `files`: 전송할 이미지 파일의 로컬 경로이다.
+ Client
    + 라즈베리파이에 `ppp0` 인터페이스가 활성화되어있으면 `ppp0`을 통하여 POST를 보내고, 아니라면 모뎀의 AT command로 보낸다.
    > AT command 구현중..

****

# Usage

## Build
```console
pi@raspberrypi:~/ino-on_AiCam $ chmod 755 build.sh
pi@raspberrypi:~/ino-on_AiCam $ ./build.sh
```

## Run
```console
pi@raspberrypi:~/ino-on_AiCam $ python3 app.py --p <PPP> --m <MODE> 
```
+ Arguments
    + `--p PPP` (optional)
        > 1 일 경우 ppp0 인터페이스를 활용, 1이 아닐 경우 AT Command 를 활용.
    + `--m MODE` (optional)
        > normal(일반모드만 실행), admin(관리자모드만 실행)

Examples
```console
pi@raspberrypi:~/ino-on_AiCam $ python3 app.py
# PPP 인터페이스 비활성화 및 완전한 기능 수행

pi@raspberrypi:~/ino-on_AiCam $ python3 app.py --p 1 --m normal
# PPP 인터페이스 활성화 및 Normal task 만 수행
```

## Test

### 촬영 및 검출
```console
pi@raspberrypi:~/ino-on_AiCam/test $ python3 cam-yolo.py --out <string> --thr <float> <float> --resize <int>
```
* out
    > 출력된 이미지를 저장할 경로
* thr(optional)
    > Yolov4 confidence threshold (0-1) / NMS threshold (0-1) (default = 0.4 0.5)
* resize(optional)
    > Yolov4 입력이미지 사이즈 (default = 416)

### 라즈베리파이 GPIO 작동여부 확인 
```console
pi@raspberrypi:~/ino-on_AiCam/test $ python3 gpio-emulator.py
```
GPIOEmulator 를 사용하기 때문에 실제 GPIO 신호를 송수신하지 않는다.

### CAT.M1 정보 확인 (AT Command)
```console
pi@raspberrypi:~/ino-on_AiCam/test $ python3 catm1Infos.py
```
+ 출력되는 정보
    + Power Pin Number
    + Status Pin Number
    + AT   
    + ATE0   
    + IMEI   
    + RSSI   
    + FW Ver   
    + HW model   

### CAT.M1 임의 전원 연결
```console
pi@raspberrypi:~/ino-on_AiCam/test $ python3 catm1PwrOn.py
```

### CAT.M1 임의 전원 차단
```console
pi@raspberrypi:~/ino-on_AiCam/test $ python3 catm1PwrOff.py
```

### CAT.M1 네트워크 연결 확인 (AT Command)
```console
pi@raspberrypi:~/ino-on_AiCam/test $ python3 socketTest.py
```

****

# Packages
## Define
**상수들이 정의된 곳이다.**
+ `GPIO_EMULATOR`: Bool
    > API.CATM1, API.NRF 를 import 하기 전에 `Define.GPIO_EMULAOTR = True` 를 먼저 진행하면 CATM1과 NRF는 RPi.GPIO 대신 [GPIOEmulaotr](https://github.com/UmileVX/ino-on_AiCam#gpioemulator) 를 import 한다.
+ `TASK_MODE_PIN`: int
    > 작동상태 모드 확인 핀
+ `RPI_OFF_PIN`: int
    > 작업 완료 알림 핀
+ `MODEM_PWR_PIN`: int
    > 모뎀 전원 관리 핀
+ `MODEM_STAT_PIN`: int
    > 모뎀 상태 핀
+ `MODEM_SER_PORT`: str
    > 모뎀 시리얼포트 경로

## API: NRF
NRF와의 GPIO 통신을 담당하는 클래스이다.
+ `NRF(modePinNum, offPinNum)`
    + `modePinNum`: 작동상태 모드 확인 GPIO Pin Number
    + `offPinNum`: 작업 완료 알림 GPIO Pin Number
+ `isAdminMode(): Bool`
    + NRF의 작동상태 모드 핀을 확인하여 현재 앱이 실행해야 할 모드가 admin이면 True를, 아니면 False를 리턴한다.
+ `pwrOffPi(): Void`
    + 앱을 종료해도 된다는 신호를 NRF에게 보낸다.

## API: CATM1
CAT.M1을 제어하는 클래스이다. 아래 외에도 많은 AT Command 메소드가 존재한다.   
+ `CATM1(serialPort='/dev/ttyS0', baudrate=115200, pwePinNum=17, statPinNum=27)`
    + `serialPort`: 라즈베리파이의 시리얼 포트 경로. 위 [Raspberry Pi Serial Port](https://github.com/UmileVX/ino-on_AiCam#serial-port) 을 진행할 경우 '/dev/ttyS0'이 생성된다.
+ `pwrOnModem(isPPP=False): Void`
    + CAT.M1의 전원을 켠다. `isPPP=True` 이면 시스템에 ppp0 인터페이스가 활성화될때까지 대기한 뒤 메소드가 종료된다.
+ `pwrOffModem(): Void`
    + CAT.M1의 전원을 끈다.
+ `getRSSI(timeout=None): str, str`
    + (AT Command) CAT.M1의 RSSI와 BER 값을 리턴한다.
+ `post(url, data): str, str` -- 구현중
    + (AT Command) CAT.M1을 통해 POST 요청을 보낸 뒤 응답 코드, 응답 텍스트를 리턴한다.

## GPIOEmulator
+ RPi.GPIO를 wrapping 하는 GUI 에뮬레이터. GPIO들이 시퀀스에 따라 잘 동작하는지 확인할때 사용한다.
+ `app.py` 의 7번째 라인인 `Define.GPIO_EMULATOR = True` 의 주석을 해제하면 `app.py` 실행시 라즈베리파이의 GPIO가 에뮬레이터와 연결된다. 이 때 GPIO는 물리적인 기능을 하지 않고 GUI 상에서 입력/출력 여부만을 보여준다.

****

# Resources
* [OpenCV](https://opencv.org/)
* [GPIOEmulator](https://sourceforge.net/projects/pi-gpio-emulator/)
* [CATM1](https://github.com/codezoo-ltd/CAT.M1_RaspberryPi/)
