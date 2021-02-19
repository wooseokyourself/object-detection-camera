cmake_minimum_required(VERSION 3.0)
project( app C CXX )

set( HEADER include/ )
set( SOURCE src/BG96.cpp src/Common.cpp src/Config.cpp src/GPIO.cpp src/HttpPostFormData.cpp src/NRF.cpp src/Serial.cpp src/YoloObjectDetector.cpp )
set( MAIN main/test.cpp )
#set ( MAIN main/main.cpp )

find_package( PkgConfig REQUIRED )
find_library( WIRINGPI_LIBRARIES wiringPi )
pkg_check_modules( OPENCV opencv4 )
pkg_check_modules( CURL libcurl )
pkg_check_modules( JSONCPP jsoncpp )

include_directories( ${HEADER} ${OPENCV_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS} ${WIRINGPI_INCLUDE_DIRS} )
link_libraries( ${OPENCV_LIBRARIES} ${CURL_LIBRARIES} ${JSONCPP_LIBRARIES} ${WIRINGPI_LIBRARIES} )
add_executable( app ${MAIN} ${SOURCE} )

target_compile_options( app PUBLIC -std=c++17 )
target_link_libraries( app ${OPENCV_LIBRARIES} ${CURL_LIBRARIES} ${JSONCPP_LIBRARIES} ${WIRINGPI_LIBRARIES} )
