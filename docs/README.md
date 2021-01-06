### Contents
+ [Description](https://github.com/UmileVX/ino-on_AiCam#description)   
+ [Requirements](https://github.com/UmileVX/ino-on_AiCam#requirements)
    + [Software Requirements](https://github.com/UmileVX/ino-on_AiCam#software-requirements)
    + [PIP3 Packages](https://github.com/UmileVX/ino-on_AiCam#pip3-packages)
    + [Setup Guide for Plain Raspberry Pi 4B](https://github.com/UmileVX/ino-on_AiCam#setup-guide-for-plain-raspberry-pi-4b)
+ [Configuration](https://github.com/UmileVX/ino-on_AiCam#configuration)
    + [Raspberry Pi GPIO Pin Number (BCM)](https://github.com/UmileVX/ino-on_AiCam#raspberry-pi-gpio-pin-number-bcm)
    + [Yolov4 Configuration (Json)](https://github.com/UmileVX/ino-on_AiCam#yolov4-configuration-json)
    + [Server URL for HTTP Request](https://github.com/UmileVX/ino-on_AiCam#server-url-for-http-request)
+ [Packages](https://github.com/UmileVX/ino-on_AiCam#packages)
    + [GPIOEmulator](https://github.com/UmileVX/ino-on_AiCam#gpioemulator)
    + [Define](https://github.com/UmileVX/ino-on_AiCam#define)
    + [API: NRF](https://github.com/UmileVX/ino-on_AiCam#api-nrf)
    + [API: CATM1](https://github.com/UmileVX/ino-on_AiCam#api-catm1)
    + [API: WEB](https://github.com/UmileVX/ino-on_AiCam#api-web)
+ [Usage](https://github.com/UmileVX/ino-on_AiCam#usage)
    + [Build](https://github.com/UmileVX/ino-on_AiCam#build)
    + [Run](https://github.com/UmileVX/ino-on_AiCam#run)
    + [Test](https://github.com/UmileVX/ino-on_AiCam#test)
+ [Resources](https://github.com/UmileVX/ino-on_AiCam#resources)

****

# Description


****

# Requirements
## Software Requirements
+ Raspbian with Python 3.3 or later
+ CMake 2.8 or later 
+ OpenCV 4.4 or later  - [Instruction](https://qengineering.eu/install-opencv-4.4-on-raspberry-64-os.html)
    > `sudo apt-get install libopencv-dev python3-opencv` 를 이용한 설치는 테스트 해봐야 함 (arm 최적화 등)

## PIP3 Packages
```console
pi@raspberrypi:~ $ pip3 install ifcfg
```

## Setup Guide for Plain Raspberry Pi 4B
### 1. Serial Port
** 시리얼 포트 경로는 다음을 사용: /dev/ttyS0 **
+ 시리얼 포트 설정 방법 ([reference](https://github.com/codezoo-ltd/CodeZoo_CATM1_Arduino/blob/master/Hands-ON/Cat.M1_RaspberryPi(with%20Python)_HandsON.pdf))
1. `raspi-config` 실행
```console
pi@raspberrypi:~ $ sudo raspi-config
```
2. Interface Options 선택   
3. P6 Serial Port 선택
4. `"Would you like a login shell to be accessible over serial?"` --> **"No"**
5. `"Would you like the serial port hardware to be enabled?"` --> **"Yes"**

### 2. PPP
*** PPP 인터페이스를 이용할 경우 다음을 진행 ***
#### 2.1. PPP Install
+ PPP 설치 방법 ([reference](https://github.com/codezoo-ltd/CodeZoo_CATM1_Arduino/blob/master/Hands-ON/Cat.M1_RaspberryPi(with%20PPP)_HandsON.pdf), device communication PORT 를 제외한 나머지 내용 동일)
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
n
What is your device communication PORT? (ttyS0/ttyUSB3/etc.)
ttyS0
Do you want to activate auto connect/reconnect service at R.Pi boot up? [Y/n]
Y
```
6. Enter 를 입력하여 라즈베리파이 재실행

#### 2.2. PPP Enable
+ 모뎀에 전원이 들어와있어야 하며, ppp0 인터페이스도 활성화되어있어야 한다.
+ ppp0 인터페이스의 활성화 및 비활성화는 `sudo ifconfig ppp0 up`, `sudo ifconfig ppp0 down` 
+ 임의로 모뎀에 전원을 넣기 위해서는 `test/catm1PwrOf.py` 를 실행시키면 된다.
+ 무선랜이 연결되어있는 경우, `sudo ifconfig wlan0 down` 으로 무선랜을 비활성화 한 뒤 PPP 네트워크 연결을 확인한다.

****

# Configuration
## Raspberry Pi GPIO Pin Number (BCM)
+ 작동상태 모드 확인: 20 input   
    > HIGH: 관리자 모드 (admin)   
    > LOW: 일반 모드 (basic)
+ 작업 완료 알림: 21 output   
    > HIGH: pi 작업 완료   
    > LOW: pi 작업 진행중
+ CAT.M1 모뎀 전원 관리: 17 output
    > HIGH: 모뎀 전원 연결   
    > LOW: 모뎀 전원 차단
+ CAT.M1 모뎀 상태: 27 input
    > HIGH: 모뎀 인식   
    > LOW: 모뎀 미인식

※ 모든 핀 번호는 하드코딩 되어있다.   

## Yolov4 Configuration (Json)
+ `config/config.json`
+ 각 필드의 의미는 다음과 같다.
    + Object: `YOLO`
        + Data: `CONFIDENCE_THRESHOLD` (default=0.4)
            > 0 - 1 사이의 부동소수점. 낮을수록 False Positive 검출 증가, 높을수록 True Positive 검출 감소
        + Data: `NMS_THRESHOLD` (default=0.5)
            > 0 - 1 사이의 부동소수점. 낮을수록 중복 검출 증가
        + Data: `RESIZE` (default=416)
            > 32 배수의 정수. 낮을수록 검출속도는 빠르나 작은 객체 검출 불리, 높을수록 검출속도는 느리나 작은 객체 검출 유리

## Server URL for HTTP Request
+ `http://ino-on.umilevx.com/api/devices/events/<device-id>`
+ `device-id` 는 아직 정해진 바가 없기에 임의로 `ino-on-0000`을 사용중이다.

****

# Packages
## GPIOEmulator
+ RPi.GPIO를 wrapping 하는 GUI 에뮬레이터. GPIO들이 시퀀스에 따라 잘 동작하는지 확인할때 사용한다.
+ `app.py` 의 7번째 라인인 `Define.GPIO_EMULATOR = True` 의 주석을 해제하면 `app.py` 실행시 라즈베리파이의 GPIO가 에뮬레이터와 연결된다. 이 때 GPIO는 물리적인 기능을 하지 않고 GUI 상에서 입력/출력 여부만을 보여준다.

## Define
+ CATM1, NRF 를 import 하기 전에 `Define.GPIO_EMULAOTR = True` 를 먼저 진행하면 CATM1과 NRF는 RPi.GPIO 대신 GPIOEmulaotr 를 import 한다.

## API: NRF
NRF와의 GPIO 통신을 담당하는 클래스이다.
+ `NRF(taskPinNum, offPinNum)`
    + `taskPinNum`: 작동상태 모드 확인 GPIO Pin Number
    + `offPinNum`: 작업 완료 알림 GPIO Pin Number
+ `isAdminMode(timeout): Bool`
    + NRF의 작동상태 모드 핀을 확인하여 현재 앱이 실행해야 할 모드가 admin이면 True를, 아니면 False를 리턴한다.
+ `pwrOffPi(): Void`
    + 앱을 종료해도 된다는 신호를 NRF에게 보낸다.

## API: CATM1
CAT.M1을 제어하는 클래스이다. 
+ `CATM1(serialPort='/dev/ttyS0', baudrate=115200, pwePinNum=17, statPinNum=27)`
    + `serialPort`: 라즈베리파이의 시리얼 포트 경로. 위 [Raspberry Pi Serial Port](https://github.com/UmileVX/ino-on_AiCam#serial-port) 을 진행할 경우 '/dev/ttyS0'이 생성된다.
+ `pwrOnModem(): Void`
    + CAT.M1의 전원을 켠다. 전원을 켠 뒤 AT Command가 입력될때까지 앱의 프로세스를 대기한다.
+ `pwrOffModem(): Void`
    + CAT.M1의 전원을 끈다.
+ `getRSSI(timeout=None): str`
    + CAT.M1의 RSSI를 얻기 위해 AT+CSQ 요청을 보낸뒤 응답을 리턴한다.

## API: WEB
서버와 http 통신을 하기 위한 클래스이다.
+ `WEB(url)`
    + `url`: 서버의 end point 경로이다.
+ `post(time, event, rssi, battery, imagefile=None): str`
    + 서버에 POST request를 진행하고 응답을 리턴한다.
    + `time`: 사진을 촬영한 시간이다.
    + `event`:  사진 내에서 객체가 검출된 여부이다.
    + `rssi`: CAT.M1의 RSSI값이다.
    + `battery`: NRF에 연결되어 공급받는 배터리의 상태이다.
    + `imagefile`: 서버에 전송할 이미지 파일의 경로이다.
+ POST의 서버 로그기록은 다음 링크에서 확인할 수 있다.
> http://ino-on.umilevx.com/api/logs/YYYY-MM-DD.log

****

# Usage

## Build
```console
pi@raspberrypi:~/ino-on_AiCam $ chmod 755 build.sh
pi@raspberrypi:~/ino-on_AiCam $ ./build.sh
```

## Run
```console
pi@raspberrypi:~/ino-on_AiCam $ python3 app.py
```
+ Optional Argument
    + `--mode MODE` (optional)
    > basic(일반모드만 실행), admin(관리자모드만 실행)

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

### CAT.M1 정보 확인
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

### CAT.M1 네트워크 연결 확인
```console
pi@raspberrypi:~/ino-on_AiCam/test $ python3 socketTest.py
```

****

# Resources
* [OpenCV](https://opencv.org/)
* [GPIOEmulator](https://sourceforge.net/projects/pi-gpio-emulator/)
* [CATM1](https://github.com/codezoo-ltd/CAT.M1_RaspberryPi/)