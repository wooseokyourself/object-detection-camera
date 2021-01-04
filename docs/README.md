# Dependencies
- Raspbian with Python 3.3 or later
- OpenCV 4.4 or later
- CMake 2.8 or later

# Information
### GPIO Pin number (BCM)
+ NRF, 작동상태 모드 확인: 20 input   
+ NRF, 작업 완료 알림: 21 output   
+ CAT.M1, 모뎀 파워: 17 output   
+ CAT.M1, 모뎀 상태: 27 input   

※ 모든 핀 번호는 하드코딩 되어있음   

### Configuration (Json)
+ config/config.json
+ 각 필드의 의미는 다음과 같다.
    + Object: YOLO
        + Data: CONFIDENCE_THRESHOLD (default=0.4)
        > 0 - 1 사이의 부동소수점. 낮을수록 False Positive 검출 증가, 높을수록 True Positive 검출 감소
        + Data: NMS_THRESHOLD (default=0.5)
        > 0 - 1 사이의 부동소수점. 낮을수록 중복 검출 증가
        + Data: RESIZE (default=416)
        > 32 배수의 정수. 낮을수록 검출속도는 빠르나 작은 객체 검출 불리, 높을수록 검출속도는 느리나 작은 객체 검출 유리
    

# Build
~~~
ino-on_AiCam $ chmod 755 build.sh
ino-on_AiCam $ ./build.sh
~~~

# Run
~~~
ino-on_AiCam $ python3 app.py
~~~

# Test
### 촬영 및 검출
~~~ 
ino-on_AiCam/test $ python3 test/cam-yolo.py --out <string> --thr <float> <float> --resize <int>
~~~
* out
> 출력된 이미지를 저장할 경로
* thr(optional)
> Yolov4 confidence threshold (0-1) / NMS threshold (0-1) (default = 0.4 0.5)
* resize(optional)
> Yolov4 입력이미지 사이즈 (default = 416)

### CAT.M1 정보 확인
~~~
ino-on_AiCam/test $ python3 test/catm1.py
~~~
+ 출력되는 정보
    + AT   
    + ATE0   
    + IMEI   
    + RSSI   
    + FW Ver   
    + HW model   
    + Phone Number   

### 라즈베리파이 GPIO 작동여부 
~~~
ino-on_AiCam/test $ python3 test/gpio-emulator.py
~~~

# Resources
* [OpenCV](https://opencv.org/)
* [GPIOEmulator](https://sourceforge.net/projects/pi-gpio-emulator/)
* [CATM1](https://github.com/codezoo-ltd/CAT.M1_RaspberryPi/)