// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/9/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     ... - ongoing debug release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#define MPU6050_INCLUDE_DMP_MOTIONAPPS20
#include "MPU6050.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;

uint8_t fifoCount;
uint8_t fifoBuffer[128];
uint8_t mpuIntStatus;

//#define DEBUG
#ifdef DEBUG
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTF(x, y) Serial.print(x, y)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTLNF(x, y) Serial.println(x, y)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTF(x, y)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTLNF(x, y)
#endif

#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    DEBUG_PRINTLN("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    DEBUG_PRINTLN("Testing device connections...");
    DEBUG_PRINTLN(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    
    // wait for ready
    DEBUG_PRINT("\nSend any character to begin DMP programming and demo: ");
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again

    // ====================== BEGIN DMP INITIALIZATION CODE ===========================

    // reset device
    DEBUG_PRINTLN("\n\nResetting MPU6050...");
    accelgyro.reset();
    delay(50); // wait after reset

    // enable sleep mode and wake cycle
    //Serial.println("Enabling sleep mode...");
    //accelgyro.setSleepEnabled(true);
    //Serial.println("Enabling wake cycle...");
    //accelgyro.setWakeCycleEnabled(true);
    //delay(10);

    // disable sleep mode
    DEBUG_PRINTLN("Disabling sleep mode...");
    accelgyro.setSleepEnabled(false);

    // get MPU hardware revision
    DEBUG_PRINTLN("Selecting user bank 16...");
    accelgyro.setMemoryBank(0x10, true, true);
    DEBUG_PRINTLN("Selecting memory byte 6...");
    accelgyro.setMemoryStartAddress(0x06);
    DEBUG_PRINTLN("Checking hardware revision...");
    uint8_t hwRevision = accelgyro.readMemoryByte();
    DEBUG_PRINT("Revision @ user[16][6] = ");
    DEBUG_PRINTLNF(hwRevision, HEX);
    DEBUG_PRINTLN("Resetting memory bank selection to 0...");
    accelgyro.setMemoryBank(0, false, false);

    // check OTP bank valid
    DEBUG_PRINTLN("Reading OTP bank valid flag...");
    uint8_t otpValid = accelgyro.getOTPBankValid();
    DEBUG_PRINT("OTP bank is ");
    DEBUG_PRINTLN(otpValid ? "valid!" : "invalid!");

    // get X/Y/Z gyro offsets
    DEBUG_PRINTLN("Reading gyro offset values...");
    int8_t xgOffset = accelgyro.getXGyroOffset();
    int8_t ygOffset = accelgyro.getYGyroOffset();
    int8_t zgOffset = accelgyro.getZGyroOffset();
    DEBUG_PRINT("X gyro offset = ");
    DEBUG_PRINTLN(xgOffset);
    DEBUG_PRINT("Y gyro offset = ");
    DEBUG_PRINTLN(ygOffset);
    DEBUG_PRINT("Z gyro offset = ");
    DEBUG_PRINTLN(zgOffset);

    // setup weird slave stuff (?)
    DEBUG_PRINTLN("Setting slave 0 address to 0x7F...");
    accelgyro.setSlaveAddress(0, 0x7F);
    DEBUG_PRINTLN("Disabling I2C Master mode...");
    accelgyro.setI2CMasterModeEnabled(false);
    DEBUG_PRINTLN("Setting slave 0 address to 0x68 (self)...");
    accelgyro.setSlaveAddress(0, 0x68);
    DEBUG_PRINTLN("Resetting I2C Master control...");
    accelgyro.resetI2CMaster();
    delay(20);

    // load DMP code into memory banks
    DEBUG_PRINT("Writing DMP code to MPU memory banks (");
    DEBUG_PRINT(MPU6050_DMP_CODE_SIZE);
    DEBUG_PRINTLN(" bytes)");
    if (accelgyro.writeProgMemoryBlock(dmpMemory, MPU6050_DMP_CODE_SIZE)) {
        DEBUG_PRINTLN("Success! DMP code written and verified.");

        // write DMP configuration
        DEBUG_PRINT("Writing DMP configuration to MPU memory banks (");
        DEBUG_PRINT(MPU6050_DMP_CONFIG_SIZE);
        DEBUG_PRINTLN(" bytes in config def)");
        if (accelgyro.writeProgDMPConfigurationSet(dmpConfig, MPU6050_DMP_CONFIG_SIZE)) {
            DEBUG_PRINTLN("Success! DMP configuration written and verified.");
            DEBUG_PRINTLN("Setting clock source to Z Gyro...");
            accelgyro.setClockSource(MPU6050_CLOCK_PLL_ZGYRO);
            DEBUG_PRINTLN("Setting sample rate to 200Hz...");
            accelgyro.setRate(4); // 1khz / (1 + 4) = 200 Hz
            DEBUG_PRINTLN("Setting external frame sync to TEMP_OUT_L[0]...");
            accelgyro.setExternalFrameSync(MPU6050_EXT_SYNC_TEMP_OUT_L);
            DEBUG_PRINTLN("Setting DLPF bandwidth to 42Hz...");
            accelgyro.setDLPFMode(MPU6050_DLPF_BW_42);
            DEBUG_PRINTLN("Setting DMP configuration bytes (function unknown)...");
            accelgyro.setDMPConfig1(0x03);
            accelgyro.setDMPConfig2(0x00);
            DEBUG_PRINTLN("Clearing OTP Bank flag...");
            accelgyro.setOTPBankValid(false);
            DEBUG_PRINTLN("Setting X/Y/Z gyro offsets to zero...");
            accelgyro.setXGyroOffset(0);
            accelgyro.setYGyroOffset(0);
            accelgyro.setZGyroOffset(0);
            DEBUG_PRINTLN("Setting X/Y/Z accel offsets to zero...");
            accelgyro.setXAccelOffset(0);
            accelgyro.setYAccelOffset(0);
            accelgyro.setZAccelOffset(0);

            uint8_t dmpMemUpdates[7][11] = {
                { 0x01, 0xB2, 0x02, 0xFF, 0xFF },
                { 0x01, 0x90, 0x04, 0x09, 0x23, 0xA1, 0x35 },
                { 0x01, 0x6A, 0x02, 0x06, 0x00 },
                { 0x01, 0x60, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                { 0x00, 0x60, 0x04, 0x40, 0x00, 0x00, 0x00 },
                { 0x01, 0x62, 0x02, 0x00, 0x00 },
                { 0x00, 0x60, 0x04, 0x00, 0x40, 0x00, 0x00 }
            };

            DEBUG_PRINTLN("Writing final memory update 1/7 (function unknown)...");
            accelgyro.writeMemoryBlock(dmpMemUpdates[0] + 3, dmpMemUpdates[0][2], dmpMemUpdates[0][0], dmpMemUpdates[0][1]);
            DEBUG_PRINTLN("Writing final memory update 2/7 (function unknown)...");
            accelgyro.writeMemoryBlock(dmpMemUpdates[1] + 3, dmpMemUpdates[1][2], dmpMemUpdates[1][0], dmpMemUpdates[1][1]);

            DEBUG_PRINTLN("Resetting FIFO...");
            accelgyro.resetFIFO();
            DEBUG_PRINTLN("Reading FIFO count...");
            fifoCount = accelgyro.getFIFOCount();
            DEBUG_PRINT("Current FIFO count=");
            DEBUG_PRINTLN(fifoCount);
            accelgyro.getFIFOBytes(fifoBuffer, fifoCount);

            DEBUG_PRINTLN("Setting motion detection threshold to 2...");
            accelgyro.setMotionDetectionThreshold(2);
            DEBUG_PRINTLN("Setting zero-motion detection threshold to 156...");
            accelgyro.setZeroMotionDetectionThreshold(2);
            DEBUG_PRINTLN("Setting motion detection duration to 80...");
            accelgyro.setMotionDetectionDuration(80);
            DEBUG_PRINTLN("Setting zero-motion detection duration to 0...");
            accelgyro.setZeroMotionDetectionDuration(0);
            DEBUG_PRINTLN("Resetting FIFO...");
            accelgyro.resetFIFO();
            
            DEBUG_PRINTLN("Enabling FIFO...");
            accelgyro.setFIFOEnabled(true);
            DEBUG_PRINTLN("Enabling DMP...");
            accelgyro.setDMPEnabled(true);
            DEBUG_PRINTLN("Resetting DMP...");
            accelgyro.resetDMP();

            DEBUG_PRINTLN("Writing final memory update 3/7 (function unknown)...");
            accelgyro.writeMemoryBlock(dmpMemUpdates[2] + 3, dmpMemUpdates[2][2], dmpMemUpdates[2][0], dmpMemUpdates[2][1]);
            DEBUG_PRINTLN("Writing final memory update 4/7 (function unknown)...");
            accelgyro.writeMemoryBlock(dmpMemUpdates[3] + 3, dmpMemUpdates[3][2], dmpMemUpdates[3][0], dmpMemUpdates[3][1]);
            DEBUG_PRINTLN("Writing final memory update 5/7 (function unknown)...");
            accelgyro.writeMemoryBlock(dmpMemUpdates[4] + 3, dmpMemUpdates[4][2], dmpMemUpdates[4][0], dmpMemUpdates[4][1]);

            DEBUG_PRINTLN("Waiting for FIFO count > 2...");
            while ((fifoCount = accelgyro.getFIFOCount()) < 3);
            DEBUG_PRINT("Current FIFO count=");
            DEBUG_PRINTLN(fifoCount);
            DEBUG_PRINTLN("Reading FIFO data...");
            accelgyro.getFIFOBytes(fifoBuffer, fifoCount);
            DEBUG_PRINTLN("Reading interrupt status...");
            mpuIntStatus = accelgyro.getIntStatus();
            DEBUG_PRINT("Current interrupt status=");
            DEBUG_PRINTLNF(mpuIntStatus, HEX);

            DEBUG_PRINTLN("Reading final memory update 6/7 (function unknown)...");
            accelgyro.readMemoryBlock(dmpMemUpdates[5] + 3, dmpMemUpdates[5][2], dmpMemUpdates[5][0], dmpMemUpdates[5][1]);

            DEBUG_PRINTLN("Waiting for FIFO count > 2...");
            while ((fifoCount = accelgyro.getFIFOCount()) < 3);
            DEBUG_PRINT("Current FIFO count=");
            DEBUG_PRINTLN(fifoCount);
            DEBUG_PRINTLN("Reading FIFO data...");
            accelgyro.getFIFOBytes(fifoBuffer, fifoCount);
            DEBUG_PRINTLN("Reading interrupt status...");
            mpuIntStatus = accelgyro.getIntStatus();
            DEBUG_PRINT("Current interrupt status=");
            DEBUG_PRINTLNF(mpuIntStatus, HEX);

            DEBUG_PRINTLN("Writing final memory update 7/7 (function unknown)...");
            accelgyro.writeMemoryBlock(dmpMemUpdates[6] + 3, dmpMemUpdates[6][2], dmpMemUpdates[6][0], dmpMemUpdates[6][1]);

            DEBUG_PRINTLN("DMP is good to go! Finally.");
            
            DEBUG_PRINTLN("Enabling interrupt detection (Arduino external interrupt 0)...");
            attachInterrupt(0, dmpDataReady, CHANGE);
        } else {
            DEBUG_PRINTLN("ERROR! DMP configuration verification failed.");
        }
    } else {
        DEBUG_PRINTLN("ERROR! DMP code verification failed.");
    }

    // ====================== END DMP INITIALIZATION CODE ===========================

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

// raw quaternion container
float q[4];

// boolean to indicate whether the MPU interrupt pin has changed
volatile bool dmpIntChange = false;

void dmpDataReady() {
    dmpIntChange = true;
}

void loop() {
    // wait for DMP interrupt
    while (!dmpIntChange);
    dmpIntChange = false;
    mpuIntStatus = accelgyro.getIntStatus();
    fifoCount = accelgyro.getFIFOCount();
    accelgyro.getFIFOBytes(fifoBuffer, fifoCount);

    // check for the correct FIFO size to indicate full data ready
    if (fifoCount >= 42) {

        // display quaternion values in easy matrix form: [w, x, y, z]
        q[0] = (float)((fifoBuffer[0] << 8) + fifoBuffer[1]) / 16384;
        q[1] = (float)((fifoBuffer[4] << 8) + fifoBuffer[5]) / 16384;
        q[2] = (float)((fifoBuffer[8] << 8) + fifoBuffer[9]) / 16384;
        q[3] = (float)((fifoBuffer[12] << 8) + fifoBuffer[13]) / 16384;
        Serial.print("[");
        Serial.print(q[0]);
        Serial.print(", ");
        Serial.print(q[1]);
        Serial.print(", ");
        Serial.print(q[2]);
        Serial.print(", ");
        Serial.print(q[3]);
        Serial.print("]\n");
        //*/

        /*
        // display quaternion values in Teapot demo format: (Teapot demo is not fully working yet)
        teapotPacket[2] = fifoBuffer[0];
        teapotPacket[3] = fifoBuffer[1];
        teapotPacket[4] = fifoBuffer[4];
        teapotPacket[5] = fifoBuffer[5];
        teapotPacket[6] = fifoBuffer[8];
        teapotPacket[7] = fifoBuffer[9];
        teapotPacket[8] = fifoBuffer[12];
        teapotPacket[9] = fifoBuffer[13];
        Serial.write(teapotPacket, 14);
        teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
        //*/

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
    }
}
