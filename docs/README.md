## Dependencies
- Raspbian with Python 3.3 or later
- OpenCV 4.4 or later
- CMake 2.8 or later

## Information
+ GPIO Pin number (BCM)
    + NRF, 작동상태 모드 확인: 15 input   
    + NRF, 작업 완료 알림: 29 output   
    + CAT.M1, 모뎀 파워: 21 output   
    + CAT.M1, 모뎀 상태: 23 input   

※ 모든 핀 번호는 하드코딩 되어있음   

## Build
~~~
$ chmod 755 build.sh
$ ./build.sh
~~~

## Run
~~~
$ python3 app.py
~~~

## Test
#### 촬영 및 검출
~~~ 
$ python3 test/cam-yolo.py --out <string> --thr <float> <float> --resize <int>
~~~
* out: 출력된 이미지를 저장할 경로
* thr(optional): Yolov4 confidence threshold (0-1) / NMS threshold (0-1) (default = 0.4 0.5)
* resize(optional): Yolov4 입력이미지 사이즈 (default = 416)

#### CAT.M1 정보 확인
~~~
$ python3 test/catm1.py
~~~
+ 출력되는 정보
    + AT   
    + ATE0   
    + IMEI   
    + RSSI   
    + FW Ver   
    + HW model   
    + Phone Number   

#### 라즈베리파이 GPIO 작동여부 
~~~
$ python3 test/gpio-emulator.py
~~~

## Resources
* [OpenCV](https://opencv.org/)
* [GPIOEmulator](https://sourceforge.net/projects/pi-gpio-emulator/)
* [CATM1](https://github.com/codezoo-ltd/CAT.M1_RaspberryPi/)