cmake_minimum_required(VERSION 3.3)
project(vibe_test)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Communicator.cpp 
${PROJECT_SOURCE_DIR}/RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp
${PROJECT_SOURCE_DIR}/src/jsoncpp/dist/jsoncpp.cpp 
${PROJECT_SOURCE_DIR/Arduino/ADXL345/ADXL345.cpp})
link_directories(/usr/local/lib)
include_directories(/usr/local/include ${PROJECT_SOURCE_DIR}/src/mosquitto-1.4.5/lib/cpp  
 ${PROJECT_SOURCE_DIR}/src/jsoncpp/dist/ 
${PROJECT_SOURCE_DIR}/RaspberryPi_bcm2835/I2Cdev/
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/Arduino/ADXL345/ )
add_executable(main ${SOURCE_FILES} )
target_link_libraries(main mosquittopp jsoncpp bcm2835 pthread)
