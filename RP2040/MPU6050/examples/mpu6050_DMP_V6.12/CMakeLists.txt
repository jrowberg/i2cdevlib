# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)
# Pull in SDK (must be before project)
# set(PICO_BOARD pico_w)  # Needed only for PICO W
# set(CYW43_LWIP 1)       # Needed only for PICO W
include(pico_sdk_import.cmake)

project(mpu6050_DMP_port C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(mpu6050_DMP_port 
         mpu6050_DMP_port.cpp 
         MPU6050.cpp
         I2Cdev.cpp
        )

# Add any user requested libraries
target_link_libraries(mpu6050_DMP_port
        pico_stdlib 
        #        pico_cyw43_arch_none     # we need Wifi to access the GPIO. Needed only for PICO W
        hardware_i2c
        pico_double
        )

pico_set_program_name(mpu6050_DMP_port "mpu6050_DMP_port")
pico_set_program_version(mpu6050_DMP_port "0.1")

pico_enable_stdio_uart(mpu6050_DMP_port 0)
pico_enable_stdio_usb(mpu6050_DMP_port 1)


# create map/bin/hex file etc.
pico_add_extra_outputs(mpu6050_DMP_port)

