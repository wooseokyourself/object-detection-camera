cmake_minimum_required(VERSION 3.0)
project( app C CXX )

set( HEADER include/ )
set( SOURCE src/config.cpp src/gpio.cpp src/http.cpp src/vision.cpp )
set( MAIN main/main.cpp )

include_directories( ${HEADER} )
add_executable( app ${MAIN} ${SOURCE} )

find_package( PkgConfig REQUIRED )
pkg_check_modules( OPENCV opencv4 )
pkg_check_modules( CURL libcurl )
pkg_check_modules( JSONCPP jsoncpp )
link_libraries( ${OPENCV_LIBRARIES} ${CURL_LIBRARIES} ${JSONCPP_LIBRARIES} )

target_compile_options( app PUBLIC -std=c++11 )
target_link_libraries( app ${OPENCV_LIBRARIES} ${CURL_LIBRARIES} ${JSONCPP_LIBRARIES} )
