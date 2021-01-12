#!/bin/sh

rm -rf build
mkdir build && \
cd build && \
cmake .. \
&& make && \
cd ../ai-cam/model && \
wget http://solution.technonia.com/download/inoon_models/yolov4-custom_best.weights && \
wget http://solution.technonia.com/download/inoon_models/yolov4-custom.cfg && \
cd ../../
mkdir results