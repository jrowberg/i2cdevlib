#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#ifndef PICO_DEFAULT_LED_PIN // PICO w with WiFi
#include "pico/cyw43_arch.h"
#endif
#include "MPU6050_6Axis_MotionApps_V6_12.h"

MPU6050 mpu;

//#define OUTPUT_READABLE_YAWPITCHROLL
//#define OUTPUT_READABLE_REALACCEL
//#define OUTPUT_READABLE_WORLDACCEL
#define OUTPUT_READABLE_CUSTOM


bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 gy;         // [x, y, z]            gyro sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
float yaw, pitch, roll;

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

void initLED() {
#ifndef PICO_DEFAULT_LED_PIN // PICO w with WiFi
    printf ("we have board with wifi (pico w) \n");
    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        exit(3);
    }
#else
    printf ("we have board without wifi\n");
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif // PICO_DEFAULT_LED_PIN

} // initLED()

void  waitForUsbConnect() {
#ifdef _PICO_STDIO_USB_H // We are using PICO_STDIO_USB. Have to wait for connection.
    
#ifndef PICO_DEFAULT_LED_PIN
    while (!stdio_usb_connected()) { // blink the pico's led until usb connection is established
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(250);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(250);
    }
#else
    while (!stdio_usb_connected()) { // blink the pico's led until usb connection is established
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(250);
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(250);
    }
#endif // PICO_DEFAULT_LED_PIN
#endif // _PICO_STDIO_USB_H
} //  waitForUsbConnect

int main() {
    stdio_init_all();
    // This example will use I2C0 on the default SDA and SCL (pins 6, 7 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    
    // setup blink led
    // setup blink led
    initLED();
    waitForUsbConnect();

    // ================================================================
    // ===                      INITIAL SETUP                       ===
    // ================================================================

    mpu.initialize();
    devStatus = mpu.dmpInitialize();

    /* --- if you have calibration data then set the sensor offsets here --- */
    // mpu.setXAccelOffset();
    // mpu.setYAccelOffset();
    // mpu.setZAccelOffset();
    // mpu.setXGyroOffset();
    // mpu.setYGyroOffset();
    // mpu.setZGyroOffset();

    /* --- alternatively you can try this (6 loops should be enough) --- */
    // mpu.CalibrateAccel(6);
    // mpu.CalibrateGyro(6);	
	
    if (devStatus == 0) 
    {
        mpu.setDMPEnabled(true);                // turn on the DMP, now that it's ready
        mpuIntStatus = mpu.getIntStatus();
        dmpReady = true;                        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        packetSize = mpu.dmpGetFIFOPacketSize();      // get expected DMP packet size for later comparison
    } 
    else 
    {                                          // ERROR!        1 = initial memory load failed         2 = DMP configuration updates failed        (if it's going to break, usually the code will be 1)
        printf("DMP Initialization failed (code %d)", devStatus);
        sleep_ms(2000);
    }
    yaw = 0.0;
    pitch = 0.0;
    roll = 0.0;

    // ================================================================
    // ===                    MAIN PROGRAM LOOP                     ===
    // ================================================================

    while(1){

        if (!dmpReady);                                                    // if programming failed, don't try to do anything
        mpuInterrupt = true;
        fifoCount = mpu.getFIFOCount();                                           // get current FIFO count
        if ((mpuIntStatus & 0x10) || fifoCount == 1024)                           // check for overflow (this should never happen unless our code is too inefficient)
        {
            mpu.resetFIFO();                                                      // reset so we can continue cleanly
            printf("FIFO overflow!");
        } 
        else if (mpuIntStatus & 0x01)                                             // otherwise, check for DMP data ready interrupt (this should happen frequently)
        {    
            while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();        // wait for correct available data length, should be a VERY short wait
            mpu.getFIFOBytes(fifoBuffer, packetSize);                             // read a packet from FIFO
            fifoCount -= packetSize;                                              // track FIFO count here in case there is > 1 packet available
            #ifdef OUTPUT_READABLE_YAWPITCHROLL                                               // display Euler angles in degrees
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                mpu.dmpGetGravity(&gravity, &q);
                mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
                yaw = ypr[0] * 180 / PI;
                pitch = ypr[1] * 180 / PI;
                roll = ypr[2] * 180 / PI;
                printf("ypr: %f,\t %f,\t %f\n", yaw, pitch, roll);
            #endif
            #ifdef OUTPUT_READABLE_REALACCEL
                // display real acceleration, adjusted to remove gravity
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                mpu.dmpGetAccel(&aa, fifoBuffer);
                mpu.dmpGetGravity(&gravity, &q);
                mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
                printf("areal: %d,\t %d,\t %d\n", aaReal.x, aaReal.y, aaReal.z);
            #endif
            #ifdef OUTPUT_READABLE_WORLDACCEL
                // display initial world-frame acceleration, adjusted to remove gravity
                // and rotated based on known orientation from quaternion
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                mpu.dmpGetAccel(&aa, fifoBuffer);
                mpu.dmpGetGravity(&gravity, &q);
                mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
                mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
                printf("aworld: %d,\t %d,\t %d\n", aaWorld.x, aaWorld.y, aaWorld.z);
            #endif
            #ifdef OUTPUT_READABLE_CUSTOM
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                printf("W: %f\t X: %f\t Y: %f\t Z: %f\n", q.w, q.x, q.y, q.z);
            #endif
        }
    }

    return 0;
}
