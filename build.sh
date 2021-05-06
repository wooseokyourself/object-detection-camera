#!/bin/sh

mkdir -p ~/object-detection-camera/build
cd ~/object-detection-camera/build
cmake .. && make && cd ..
