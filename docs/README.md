### Contents
+ [Description](https://github.com/UmileVX/ino-on_AICamera#description)   
+ [Requirements](https://github.com/UmileVX/ino-on_AICamera#requirements)
    + [Software Requirements](https://github.com/UmileVX/ino-on_AICamera#software-requirements)
    + [PIP3 Packages](https://github.com/UmileVX/ino-on_AICamera#pip3-packages)
    + [Setup Guide for Plain Raspberry Pi 4B](https://github.com/UmileVX/ino-on_AICamera#setup-guide-for-plain-raspberry-pi-4b)
+ [Configuration](https://github.com/UmileVX/ino-on_AICamera#configuration)
    + [Raspberry Pi GPIO Pin Number (BCM)](https://github.com/UmileVX/ino-on_AICamera#raspberry-pi-gpio-pin-number-bcm)
    + [Yolov4 Configuration (Json)](https://github.com/UmileVX/ino-on_AICamera#yolov4-configuration-json)
    + [HTTP Request to Server](https://github.com/UmileVX/ino-on_AICamera#http-request-to-server)
+ [Usage](https://github.com/UmileVX/ino-on_AICamera#usage)
    + [Build](https://github.com/UmileVX/ino-on_AICamera#build)
    + [Run](https://github.com/UmileVX/ino-on_AICamera#run)
    + [Test](https://github.com/UmileVX/ino-on_AICamera#test)
+ [Others](https://github.com/UmileVX/ino-on_AICamera#others)
+ [Resources](https://github.com/UmileVX/ino-on_AICamera#resources)

****

# Description
## Terms
#### Normal mode (Normal task)
사진을 촬영하고 포크레인을 검출한 뒤 결과값을 서버에 보내는 작업을 의미한다.

#### Admin mode (Admin task)
관리자 페이지를 위한 웹서버를 실행하는 작업을 의미한다.   

## Network
Normal mode 에서의 결과를 서버에 보내기 위해(http post) 네트워크가 필요하다. 이 때 PPP 인터페이스를 통해 네트워크에 접속하여 현재 앱에서 요청을 보내는 방법과, 모뎀의 AT Command 를 이용하여 보내는 방법이 존재하는데, AT Command 를 이용한 방법은 Workspace 에서 구현 및 테스트중이고 (issue #2, #6) 실제 구현은 PPP 인터페이스를 사용중이다.

## Normal Mode
1. 모뎀 전원 연결
2. 사진 촬영 (OpenCV 사용)
3. 사진 분석
4. ~~RSSI와 배터리 값 얻기~~
5. 서버에 결과 POST
6. NRF에게 종료신호 송신

## Admin Mode
1. 모뎀 전원 연결
2. AP 및 웹서버 실행
+ SSID: ino-on-camera / Password: 00000000
    + RaspAP 기본 웹페이지 (라즈베리파이 네트워크 관련 설정): 10.3.141.1
        + ID: admin / Password: secret
    + Configuration 설정 및 카메라 프리뷰 페이지: 10.3.141.1:4000
    + 라즈베리파이 내부 디렉토리 브라우저 탐색 페이지: 10.3.141.1:4001
3. NRF에게 종료신호 송신

****

# Requirements
## Software Requirements
+ Raspbian with Python 3.5 or later with PIP3
+ CMake 2.8 or later 
+ OpenCV 4.4 or later  - [Instruction](https://qengineering.eu/install-opencv-4.4-on-raspberry-64-os.html)
    > `opencv-contrib` 는 필요없음   
    > `sudo apt-get install libopencv-dev python3-opencv` 를 이용한 설치는 테스트 해봐야 함 (arm 최적화 등)
```console
pi@raspberrypi:~ % sudo apt-get install libjsoncpp-dev
pi@raspberrypi:~ % sudo apt-get install libcurl4-openssl-dev
```
Raspberry Pi OS Lite 일 경우 다음도 추가로 설치해야 함
```console
pi@raspberrypi:~ % sudo apt-get install libilmbase-dev libopenexr-dev libgstreamer1.0-dev
```

## PIP3 Packages
```console
pi@raspberrypi:~ % sudo apt-get install python3-pip
pi@raspberrypi:~ % pip3 install flask       # for OS Lite
pi@raspberrypi:~ $ pip3 install numpy
pi@raspberrypi:~ $ pip3 install opencv-python
pi@raspberrypi:~ $ pip3 install imutils
pi@raspberrypi:~ $ pip3 install browsepy
pi@raspberrypi:~ $ pip3 install --upgrade --force-reinstall unicategories
pi@raspberrypi:~ $ pip3 install -I unicategories
pi@raspberrypi:~ $ pip3 install --ignore-installed unicategories
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
`~/.profile` 파일 제일 하단에 다음을 입력   
```
cd ino-on_AICamera
# 7일 지난 로그파일 삭제
find logs/*.txt -type f -mtime +7 -exec rm {} +
mkdir -p logs
mkdir -p results
./build/app 2>&1 | tee -a logs/$(date +%Y-%m-%d).txt 
```


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

6. Enter 를 입력하여 라즈베리파이 재실행   
**만약 wlan이 인터페이스에 없다면 다음을 입력하여 wlan의 block 풀기**
```console
pi@raspberrypi:~ $ sudo rfkill unblock wlan
```

### 3. RaspAP
#### 3.1. RaspAP Install
+ RaspAP 설치 방법 ([reference](https://github.com/billz/raspap-webgui))
```console
pi@raspberrypi:~ $ curl -sL https://install.raspap.com | bash
```
+ ```raspapd.service``` 가 활성화된 상태에서, 외부에서 SSID=raspi-webgui / Password=ChangeMe 로 와이파이 연결이 가능하다.
+ 외부 단말에서 라즈베리파이의 와이파이에 연결 후 10.3.141.1 웹에 접속하면 라즈베리파이의 시스템 정보 등과 함께 RaspAP 설정 가능
    + 사이트 ID: admin / Password: secret
+ 본 프로젝트에서 라즈베리파이 와이파이를 다음과 같이 변경한다. (10.3.141.1 에서 변경가능)
    + SSID: ino-on-camera(default=raspi-webgui) / Password: 00000000(default=ChangeMe)
+ 나는 라즈베리파이의 부팅시간을 줄이기 위해 ```dhcpcd.service```와 ```raspapd.service``` 를 비활성화 한 뒤, 관리자모드에 진입하였을 때에만 ```raspapd.service```를 활성화하도록 코드를 짰다.
```console
pi@raspberrypi:~ $ sudo systemctl disable dhcpcd.service
pi@raspberrypi:~ $ sudo systemctl disable raspapd.service
# 이후 개발단계에서 외부인터넷을 사용할 일이 있으면 sudo systemctl start dhcpcd.service 를 해야 이더넷을 사용할 수 있다.
```

****

# Configuration
**GPIO 를 제외한 모든 값들은 관리자모드를 통해 외부에서 변경할 수 있다.**
## Raspberry Pi GPIO Pin Number (BCM)
+ 작동상태 모드 확인: 20 input   
    > HIGH: Admin mode   
    > LOW: Normal mode
+ 작업 완료 알림: 21 output   
    > HIGH: pi 작업 완료   
    > LOW: pi 작업 진행중
+ ~~CAT.M1 모뎀 전원 관리: 17 output~~
    > HIGH: 모뎀 전원 연결   
    > LOW: 모뎀 전원 차단
+ ~~CAT.M1 모뎀 상태: 27 input~~
    > HIGH: 모뎀 인식   
    > LOW: 모뎀 미인식   

**모뎀전원이 라즈베리파이 전원과 동일하게 온/오프 되도록 변경되어서 더이상 라즈베리파이에서 모뎀을 제어하지 않는다.**

※ 모든 핀 번호는 `Gpio` 클래스의 생성자에 정의되어있다.   

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
    > `device-id` 는 아직 정해진 바가 없음. `ino-on-xxxx` 에서 뒤의 숫자는 관리자페이지에서 변경할 수 있다.
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
        "filename"="2021-01-08.jpg"
        "files"="@results/2021-01-08.jpg"
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
**[Requirements](https://github.com/UmileVX/ino-on_AICamera#requirements) 를 먼저 만족해야 정상적으로 동작한다.**
## Build
```console
pi@raspberrypi:~/ino-on_AICamera $ mkdir build && cd build && cmake .. && make && cd ..
pi@raspberrypi:~/ino-on_AICamera $ cd bin/model && ./getModel.sh # 모델 다운로드
```

## Run
```console
pi@raspberrypi:~/ino-on_AICamera $ ./build/app # 본 프로젝트 디렉토리에서 실행해야한다.
```
****

# Others

****

# Resources
**Imported**
+ [WiringPi](http://wiringpi.com/)
+ [OpenCV](https://opencv.org/)
+ [libcurl](https://curl.se/libcurl/)
+ [JsonCpp](http://open-source-parsers.github.io/jsoncpp-docs/doxygen/index.html/)
+ [Date](https://github.com/HowardHinnant/date/)
+ [RaspAP](https://raspap.com/)
+ [PPP](https://sixfab.com/ppp-installer-for-sixfab-shield-hat/)
+ [Browsepy](https://pypi.org/project/browsepy/0.4.0/)

**Code reference**
+ [Camera-preview](https://www.pyimagesearch.com/2019/09/02/opencv-stream-video-to-web-browser-html-page/)
+ [Responsive-web-design](https://github.com/sdhutchins/flask-demo/)
+ [Opencv-dnn-yolo](https://github.com/opencv/opencv/blob/master/samples/dnn/object_detection.cpp/)

**Others**
+ [ACID-dateset](https://www.acidb.ca/)
+ [Darknet](https://github.com/AlexeyAB/darknet/)
+ [CATM1](https://github.com/codezoo-ltd/CodeZoo_CATM1_Arduino/)
