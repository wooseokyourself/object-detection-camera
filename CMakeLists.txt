cmake_minimum_required(VERSION 3.0)
project( app C CXX )

set( HEADER include/ )
set( SOURCE src/ )
set( MAIN main/ )

include_directories( ${HEADER} )
add_executable( app ${MAIN} ${SOURCE} )

find_package( OpenCV REQUIRED )
find_package( CURL REQUIRED )
find_package( PkgConfig REQUIRED )
pkg_check_modules( JSONCPP jsoncpp )
link_libraries( ${JSONCPP_LIBRARIES} )

target_compile_options( app -std=c++11 )
target_link_libraries( app ${OpenCV_LIBS} ${CURL_LIBS} ${JSONCPP_LIBS} ${JSONCPP_LIBRARIES} )