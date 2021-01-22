cmake_minimum_required(VERSION 3.0)
project(app)

set( HEADER include/config.hpp include/date.hpp include/gpio.hpp include/http.hpp include/vision.hpp )
set( SOURCE src/config.cpp src/gpio.cpp src/http.cpp src/vision.cpp )
set( MAIN main/main.cpp )
set( OUTPUT app )

include_directories( ${HEADER} )
add_executable( ${OUTPUT} ${MAIN} ${SOURCE} )

find_package( OpenCV REQUIRED )
find_package( CURL REQUIRED )
find_package( PkgConfig REQUIRED )
pkg_check_modules( JSONCPP jsoncpp )
link_libraries( ${JSONCPP_LIBRARIES} )

target_link_libraries( ${OUTPUT} ${OpenCV_LIBS} ${CURL_LIBS} ${JSONCPP_LIBS} ${JSONCPP_LIBRARIES} )