# Requirements
### Software Requirements
- Raspbian with Python 3.3 or later
- OpenCV 4.4 or later
- CMake 2.8 or later

### Raspberry Pi Serial Port
+ 시리얼 포트 설정 ([reference](https://github.com/codezoo-ltd/CodeZoo_CATM1_Arduino/blob/master/Hands-ON/Cat.M1_RaspberryPi(with%20Python)_HandsON.pdf))
```console
pi@raspberrypi:~ $ sudo raspi-config
```
1. Interface Options 선택   
2. P6 Serial Port 선택
3. `"Would you like a login shell to be accessible over serial?"` --> **"No"**
4. `"Would you like the serial port hardware to be enabled?"` --> **"Yes"**

# Configuration
### GPIO Pin number (BCM)
+ NRF, 작동상태 모드 확인: 20 input   
+ NRF, 작업 완료 알림: 21 output   
+ CAT.M1, 모뎀 파워: 17 output   
+ CAT.M1, 모뎀 상태: 27 input   

※ 모든 핀 번호는 하드코딩 되어있음   

### Yolov4 Configuration (Json)
+ `config/config.json`
+ 각 필드의 의미는 다음과 같다.
    + Object: `YOLO`
        + Data: `CONFIDENCE_THRESHOLD` (default=0.4)
        > 0 - 1 사이의 부동소수점. 낮을수록 False Positive 검출 증가, 높을수록 True Positive 검출 감소
        + Data: `NMS_THRESHOLD` (default=0.5)
        > 0 - 1 사이의 부동소수점. 낮을수록 중복 검출 증가
        + Data: `RESIZE` (default=416)
        > 32 배수의 정수. 낮을수록 검출속도는 빠르나 작은 객체 검출 불리, 높을수록 검출속도는 느리나 작은 객체 검출 유리

# Packages
### GPIOEmulator
+ RPi.GPIO를 wrapping 하는 GUI 에뮬레이터. GPIO들이 시퀀스에 따라 잘 동작하는지 확인할때 사용한다.
+ `app.py` 의 7번째 라인인 `Define.GPIO_EMULATOR = True` 의 주석을 해제하면 `app.py` 실행시 라즈베리파이의 GPIO가 에뮬레이터와 연결된다. 이 때 GPIO는 물리적인 기능을 하지 않고 GUI 상에서 입력/출력 여부만을 보여준다.

### Define
+ CATM1, NRF 를 import 하기 전에 `Define.GPIO_EMULAOTR = True` 를 먼저 진행하면 CATM1과 NRF는 RPi.GPIO 대신 GPIOEmulaotr 를 import 한다.

### API: NRF
NRF와의 GPIO 통신을 담당하는 클래스이다.
+ `NRF(taskPinNum, offPinNum)`
    + `taskPinNum`: 작동상태 모드 확인 GPIO Pin Number
    + `offPinNum`: 작업 완료 알림 GPIO Pin Number
+ `isAdminMode(timeout): Bool`
    + NRF의 작동상태 모드 핀을 확인하여 현재 앱이 실행해야 할 모드가 admin이면 True를, 아니면 False를 리턴한다.
+ `pwrOffPi(): Void`
    + 앱을 종료해도 된다는 신호를 NRF에게 보낸다.

### API: CATM1
CAT.M1을 제어하는 클래스이다. 
+ `CATM1(serialPort='/dev/ttyS0', baudrate=115200, pwePinNum=17, statPinNum=27)`
    + `serialPort`: 라즈베리파이의 시리얼 포트 경로. 위 [Raspberry Pi Serial Port](https://github.com/UmileVX/ino-on_AiCam#raspberry-pi-serial-port) 을 진행할 경우 '/dev/ttyS0'이 생성된다.
+ `pwrOnModem(): Void`
    + CAT.M1의 전원을 켠다. 전원을 켠 뒤 AT Command가 입력될때까지 앱의 프로세스를 대기한다.
+ `pwrOffModem(): Void`
    + CAT.M1의 전원을 끈다.
+ `getRSSI(timeout=None): str`
    + CAT.M1의 RSSI를 얻기 위해 AT+CSQ 요청을 보낸뒤 응답을 리턴한다.

### API: WEB
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

# Build
```console
pi@raspberrypi:~/ino-on_AiCam $ chmod 755 build.sh
pi@raspberrypi:~/ino-on_AiCam $ ./build.sh
```

# Run
```console
pi@raspberrypi:~/ino-on_AiCam $ python3 app.py
```

# Test
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

### CAT.M1 정보 확인
```console
pi@raspberrypi:~/ino-on_AiCam/test $ python3 catm1.py
```
+ 출력되는 정보
    + AT   
    + ATE0   
    + IMEI   
    + RSSI   
    + FW Ver   
    + HW model   

### 라즈베리파이 GPIO 작동여부 
```console
pi@raspberrypi:~/ino-on_AiCam/test $ python3 gpio-emulator.py
```

# Resources
* [OpenCV](https://opencv.org/)
* [GPIOEmulator](https://sourceforge.net/projects/pi-gpio-emulator/)
* [CATM1](https://github.com/codezoo-ltd/CAT.M1_RaspberryPi/)
