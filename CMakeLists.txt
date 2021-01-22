cmake_minimum_required(VERSION 3.0)
project(app)

set( HEADER include )
set( SOURCE src )
set( MAIN main )
set( OUTPUT app )

include_directories( ${HEADER} )
add_executable( ${OUTPUT} ${MAIN} ${SOURCE} )

find_package( OpenCV REQUIRED )
find_package( CURL REQUIRED )
find_package( PkgConfig REQUIRED )
pkg_check_modules( JSONCPP jsoncpp )
link_libraries( ${JSONCPP_LIBRARIES} )

target_link_libraries( ${OUTPUT} ${OpenCV_LIBS} ${CURL_LIBS} ${JSONCPP_LIBS} ${JSONCPP_LIBRARIES} )