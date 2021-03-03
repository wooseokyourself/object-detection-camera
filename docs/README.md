# [Code Documentation](https://wooseokyourself.github.io/my-github-docs/Ino-on_AICamera/html/annotated.html)

# Instructions to Start
**본 장의 1, 2 가 진행된 라즈베리파이 이미지는 다음 링크에서 다운받을 수 있다. (최소 4GB의 SD카드 필요)**   
> http://solution.technonia.com/download/inoon_raspbian_image/PiLite4GB-no-ppp.zip   
*라즈베리파이 계정: pi / raspberry*   
*자동로그인 설정은 안 되어 있음!*   

### 1. Software Requirements
```console
# Requirements 설치
pi@raspberrypi:~ % sudo apt-get update
pi@raspberrypi:~ % sudo apt-get upgrade
pi@raspberrypi:~ % sudo apt-get install git
pi@raspberrypi:~ % git clone https://github.com/WiringPi/WiringPi
pi@raspberrypi:~ % ./WiringPi/build
pi@raspberrypi:~ % sudo apt-get install libjsoncpp-dev libilmbase-dev libopenexr-dev libgstreamer1.0-dev

# OpenCV 4.5 (https://qengineering.eu/install-opencv-4.5-on-raspberry-64-os.html)
pi@raspberrypi:~ % git clone https://github.com/wooseokyourself/raspi-opencv-sh
pi@raspberrypi:~ % cd raspi-opencv-sh
pi@raspberrypi:~/raspi-opencv-sh % chmod 755 opencv.sh
pi@raspberrypi:~ % ./opencv.sh 
```

### 2. Setup raspi-config
```console
pi@raspberrypi:~ $ sudo raspi-config
```
+ 시리얼 포트 설정 ([reference](https://github.com/codezoo-ltd/CodeZoo_CATM1_Arduino/blob/master/Hands-ON/Cat.M1_RaspberryPi(with%20Python)_HandsON.pdf))
**모뎀의 시리얼 경로: /dev/ttyS0**
1. Interface Options 선택   
2. Serial Port 선택
3. `"Would you like a login shell to be accessible over serial?"` --> **"No"**
4. `"Would you like the serial port hardware to be enabled?"` --> **"Yes"**
   

+ 카메라 사용 설정
1. Interface Options 선택
2. Camera 선택 및 Enable 설정
   

+ 부팅시 자동 로그인 설정
1. System Options 선택
2. Boot / Auto Login 선택
3. Console Autologin 선택

### 3. Install Project
```console
pi@raspberrypi:~ $ git clone https://github.com/UmileVX/ino-on_AICamera
pi@raspberrypi:~ $ cd ino-on_AICamera

# 처음 clone 시 한 번만 실행.
# ~/.profile 에 부팅시 자동실행 등을 설정하고 모델을 다운받는다.
pi@raspberrypi:~/ino-on_AICamera $ ./init.sh

# 매번 컴파일이 필요할때마다 실행.
pi@raspberrypi:~/ino-on_AICamera $ ./build.sh
```

### 4. Run & Test
라즈베리파이 전원이 켜지면 자동으로 시작된다. 실행을 멈추고싶다면 본 프로그램 실행 중 ```Ctrl + C``` 로 인터럽트를 줘야 한다.   
테스트는 다음과 같이 실행가능하다.
```console
pi@raspberrypi:~/ino-on_AICamera $ ./build/test
```

### 5. Tuning
+ 라즈베리파이의 부팅속도를 줄이기 위해 dhcp 서버 비활성화
```console
pi@raspberrypi:~ $ sudo systemctl disable dhcpcd.service
```
추후 외부인터넷을 사용할 때 `sudo systemctl start dhcpcd.service` 가 필요함

****

# Informations
## Raspberry Pi GPIO Pin Number (BCM)
+ 작업 완료 알림: 21 output   
    > HIGH: pi 작업 완료   
    > LOW: pi 작업 진행중

## Yolov4 Configuration (Json)
+ `config/config.json`
+ 각 필드의 의미는 다음과 같다.
    + **`deviceId`**   
        > AICamera 에 부여되는 고유한 ID. ino-on-xxxx 형식을 따름
    + **`sendInterval`**   
        > AICamera 의 작동 주기 (초 단위)
    + **`sendOnDetectedOnly`**   
        > true 일 경우 이벤트 검출 시에만 사진을 POST, false 일 경우 항상 사진을 POST 하도록 작동
    + **`confidenceThreshold`**
        > 0 - 1 사이의 부동소수점. 낮을수록 False Positive 검출 증가, 높을수록 True Positive 검출 감소
    + **`nmsThreshold`**
        > 0 - 1 사이의 부동소수점. 높을수록 중복 검출 증가
    + **`resizeResolution`**
        > 32 배수의 정수. 낮을수록 검출속도는 빠르나 작은 객체 검출 불리, 높을수록 검출속도는 느리나 작은 객체 검출 유리
    + **`result`**   
        > 서버 측의 HTTP request 정상 수신 여부. 1일 경우 정상, 0일 경우 비정상

## HTTP Request to Server
+ URL
    + HOST: `http://ino-on.umilevx.com/api/devices/events/ino-on-xxxx`
        > `ino-on-xxxx` 는 본 카메라 디바이스의 고유한 아이디를 의미.
    + 서버 로그기록 확인: http://ino-on.umilevx.com/api/logs/YYYY-MM-DD.log
        > `YYYY-MM-DD` 는 현재날짜이다.
+ Format
    + `sendOnDetectedOnly = false` 일 경우   
    ```
    {
        "time"="2021-01-08T13:41:21.046Z"
        "event"="1"
        "rssi"="31"
        "battery"="97"
        "filename"="2021-01-08.jpg"
        "files"="@results/2021-01-08.jpg"
    }
    ```
    + `sendOnDetectedOnly = true` 이며 이벤트 미검출일 경우    
    ```
    {
        "time"="2021-01-08T13:41:21.046Z"
        "event"="0"
        "rssi"="31"
        "battery"="97"
    }
    ```
    > `time`: 사진 촬영 시간   
    > `event`:  이벤트 검출 여부   
    > `rssi`: 모뎀의 RSSI   
    > `battery`: NRF에 연결된 배터리 잔여용량   
    > `filename`: 서버에 저장될 이미지 파일의 서버 경로이다.   
    > `files`: 전송할 이미지 파일의 로컬 경로이다.

****

# Resources
**Imported**
+ [WiringPi](http://wiringpi.com/)
+ [OpenCV](https://opencv.org/)
+ [JsonCpp](http://open-source-parsers.github.io/jsoncpp-docs/doxygen/index.html/)
+ [Date](https://github.com/HowardHinnant/date/)

**Code reference**
+ [Opencv-dnn-yolo](https://github.com/opencv/opencv/blob/master/samples/dnn/object_detection.cpp/)

**Others**
+ [ACID-dateset](https://www.acidb.ca/)
+ [Darknet](https://github.com/AlexeyAB/darknet/)
+ [CATM1](https://github.com/codezoo-ltd/CodeZoo_CATM1_Arduino/)
