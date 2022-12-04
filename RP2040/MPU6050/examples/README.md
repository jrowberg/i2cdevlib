These are examples for the Raspberry Pi Pico development board and have serial output enabled through USB.
Also, if the libraries files (I2Cdev.h, I2Cdev.cpp, MPU6050.h, MPU6050.cpp, MPU6050\_6Axis\_MotionApps\_V6\_12.h, helper\_3dmath.h) are not in a common include directory, you will have to copy them to the example folder (whichever you want to build), unless you modify the CMakeLists.txt and specify the path.

#### Instructions for building examples
1. ```cd``` to the folder of the example you want to build.
2. If you have PICO W (PICO board with Infineon CYW43439 wireless chip), you need to uncommented 3 lines in CMakeLists.txt file.
3. ```mkdir build && cd build```
4. ```cmake ..```
5. ```make```
6. Copy the uf2 file to your Pico board, using ```cp``` or the file explorer you have.
7. ```sudo minicom -D /dev/ttyACM0``` to watch the serial output. Use ```sudo```, otherwise minicom will fail to open the device and show no warnings. On Windows you can use PuTTY, choosing the COM port that was assigned (check the Device Manager) and a baudrate of 115200.

#### Sensor calibration
You will get better results if you measure the gyro and accelerometer offsets for your sensor *(e.g. with the accompanying calibration example)*. Set the initial offsets using `mpu.setXAccelOffset()` and friends in mpu6050_DMP_port.cpp after the call to `mpu.dmpInitialize()`.

Alternatively you could call `mpu.CalibrateAccel(6)` and `mpu.CalibrateGyro(6)` in the same location (6 loops should be enough).
