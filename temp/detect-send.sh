#
#!/bin/sh

API_ENDPOINT="http://ino-on.umilevx.com/api/devices/events"
API_METHOD="POST"
TIMESTAMP=`date +%Y-%m-%d_%H:%M:%S`
IMAGEFILE=$TIMESTAMP.jpg
echo "Engine runs at $TIMESTAMP"

function post_noevent(){ # time, event, rssi, battery
    # 이벤트 미발생
    curl -X $API_METHOD $API_ENDPOINT/ino-on-0000 \
    -F time=$1 \
    -F event=$2 \
    -F rssi=$3 \
    -F battery=$4 \
    -w %{http_code}
}

function post_event(){ # time, event, rssi, battery, filename
    # 이벤트 발생
    curl -X $API_METHOD $API_ENDPOINT/ino-on-0000 \
    -F time=$1 \
    -F event=$2 \
    -F rssi=$3 \
    -F battery=$4 \
    -F filename=$5 \
    -F files=@results/$5 \
    -w %{http_code}
}

# CPP 실행
bash -c '(./build/detector model/yolov4-custom_best.weights model/yolov4-custom.cfg model/classes.names results/wooseok.jpg 0.4 0.5 416); exit $?' 2>&1
STATUS=$?
case $STATUS in
    0)
        echo "이벤트 없음"
        post_noevent $TIMESTAMP 0 0 100
        ;;
    1)
        echo "이벤트 감지"
        post_event $TIMESTAMP 1 0 100 wooseok.jpg
        ;;
    2)
        echo "카메라 연결 실패"
        ;;
    3)
        echo "분석이미지 저장 실패"
        ;;
    4)
        echo "모델 로딩 실패"
        ;;
    5)
        echo "사진 분석 실패"
        ;;
    *)
        echo "Invalid status"
        ;;
esac
