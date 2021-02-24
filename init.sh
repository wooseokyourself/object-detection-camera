#!/bin/sh

mkdir -p ~/ino-on_AICamera/logs
mkdir -p ~/ino-on_AICamera/results
mkdir ~/ino-on_AICamera/build
wget -O ~/ino-on_AICamera/bin/model/yolov4.weights http://solution.technonia.com/download/inoon_models/yolov4-v02.weights && \
wget -O ~/ino-on_AICamera/bin/model/yolov4.cfg http://solution.technonia.com/download/inoon_models/yolov4-custom.cfg

echo 'find logs/*.txt -type f -mtime +7 -exec rm {} +' >> ~/.profile
echo 'mkdir -p logs' >> ~/.profile
echo 'mkdir -p results' >> ~/.profile
echo './build/app 2>&1 | tee -a logs/$(date +%Y-%m-%d).txt' >> ~/.profile