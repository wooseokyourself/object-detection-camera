cmake_minimum_required(VERSION 3.0)
project(app)

set( ROOT .. )
set( HEADER ${ROOT}/include/*.hpp )
set( SOURCE ${ROOT}/src/*.cpp )
set( OUTPUT ${ROOT}/app.out )
set( MAIN ${ROOT}/main/main.cpp )

include_directories( ${HEADER} )
add_executable( ${OUTPUT} ${MAIN} ${SOURCE} )

find_package( OpenCV REQUIRED )
find_package( CURL REQUIRED )
find_package( JSONCPP REQUIRED )

target_link_libraries( ${OUTPUT} ${OpenCV_LIBS} ${CURL_LIBS} ${JSONCPP_LIBS} )