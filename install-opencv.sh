# https://qengineering.eu/install-opencv-4.4-on-raspberry-64-os.html

sudo apt-get update && upgrade
sudo apt-get install build-essential cmake git unzip pkg-config
sudo apt-get install libjpeg-dev libpng-dev
sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install libgtk2.0-dev libcanberra-gtk* libgtk-3-dev
sudo apt-get install libxvidcore-dev libx264-dev
sudo apt-get install python3-dev python3-numpy python3-pip
sudo apt-get install libtbb2 libtbb-dev libdc1394-22-dev
sudo apt-get install libv4l-dev v4l-utils
sudo apt-get install libopenblas-dev libatlas-base-dev libblas-dev
sudo apt-get install liblapack-dev gfortran libhdf5-dev
sudo apt-get install libprotobuf-dev libgoogle-glog-dev libgflags-dev
sudo apt-get install protobuf-compiler


wget -O ~/opencv.zip https://github.com/opencv/opencv/archive/4.5.0.zip
# wget -O ~/opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.5.0.zip

unzip ~/opencv.zip
# unzip ~/opencv_contrib.zip

mkdir ~/opencv-4.5.0/build
cd ~/opencv-4.5.0/build
cmake -D CMAKE_BUILD_TYPE=RELEASE \
        -D CMAKE_INSTALL_PREFIX=/usr/local \
#         -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib-4.5.0/modules \
        -D ENABLE_NEON=ON \
        -D WITH_FFMPEG=ON \
        -D WITH_TBB=ON \
        -D BUILD_TBB=ON \
        -D BUILD_TESTS=OFF \
        -D WITH_EIGEN=OFF \
        -D WITH_V4L=ON \
        -D WITH_LIBV4L=ON \
        -D WITH_VTK=OFF \
        -D WITH_QT=OFF \
        -D OPENCV_ENABLE_NONFREE=ON \
        -D INSTALL_C_EXAMPLES=OFF \
        -D INSTALL_PYTHON_EXAMPLES=OFF \
        -D BUILD_NEW_PYTHON_SUPPORT=OFF \
        -D BUILD_opencv_python3=FALSE \
        -D OPENCV_GENERATE_PKGCONFIG=ON \
        -D BUILD_EXAMPLES=OFF ..

make -j4

sudo make install
sudo ldconfig
# cleaning (frees 300 KB)
make clean
sudo apt-get update
