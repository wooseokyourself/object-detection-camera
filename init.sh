#!/bin/sh

echo 'cd ino-on_AICamera' >> ~/.profile
echo 'mkdir -p logs' >> ~/.profile
echo 'mkdir -p results' >> ~/.profile
echo 'find logs/*.txt -type f -mtime +7 -exec rm {} +' >> ~/.profile
echo 'python3 ./build/app.py 2>&1 | tee -a logs/$(date +%Y-%m-%d).txt' >> ~/.profile

rm -rf build
mkdir build && \
cd build && \
cmake .. \
&& make && \
cd ../ai-cam/model && \
wget -O ~/ino-on_AICamera/bin/model/yolov4.weights http://solution.technonia.com/download/inoon_models/yolov4-v02.weights && \
wget -O ~/ino-on_AICamera/bin/model/yolov4.cfg http://solution.technonia.com/download/inoon_models/yolov4-custom.cfg
cd ../../