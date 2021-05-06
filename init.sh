#!/bin/sh

mkdir -p ~/object-detection-camera/logs
mkdir -p ~/object-detection-camera/results
mkdir ~/object-detection-camera/build
wget -O ~/object-detection-camera/bin/model/yolov4.weights https://your-yolo.weights && \
wget -O ~/object-detection-camera/bin/model/yolov4.cfg https://your-yolo.cfg

echo 'cd object-detection-camera' >> ~/.profile
echo 'find logs/*.txt -type f -mtime +7 -exec rm {} +' >> ~/.profile
echo 'mkdir -p logs' >> ~/.profile
echo 'mkdir -p results' >> ~/.profile
echo './build/app 2>&1 | tee -a logs/$(date +%Y-%m-%d).txt' >> ~/.profile
