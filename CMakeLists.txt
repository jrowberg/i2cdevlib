cmake_minimum_required(VERSION 3.3)
project(vibe_test)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Communicator.cpp ${PROJECT_SOURCE_DIR}/src/jsoncpp/dist/jsoncpp.cpp Sensor.cpp)
link_directories(/usr/local/lib)
include_directories(/usr/local/include ${PROJECT_SOURCE_DIR}/src/mosquitto-1.4.5/lib/cpp  
 ${PROJECT_SOURCE_DIR}/src/jsoncpp/dist/ 
${PROJECT_SOURCE_DIR} )
add_executable(main ${SOURCE_FILES} )
target_link_libraries(main mosquittopp jsoncpp)
