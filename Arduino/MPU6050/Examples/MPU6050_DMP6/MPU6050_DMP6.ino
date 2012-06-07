// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/9/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-06-05 - add gravity-compensated initial reference frame acceleration output
//                - add 3D math helper file to DMP6 example sketch
//                - add Euler output and Yaw/Pitch/Roll output formats
//     2012-06-04 - remove accel offset clearing for better results (thanks Sungon Lee)
//     2012-06-01 - fixed gyro sensitivity to be 2000 deg/sec instead of 250
//     2012-05-30 - basic DMP initialization working

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

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
#include "helper_3dmath.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;

// =============================================================
// =============================================================
//                            NOTE!
// =============================================================
// =============================================================
// In addition to connection 3.3v, GND, SDA, and SCL, this sketch
// depends on the MPU-6050's INT pin being connected to the
// Arduino's external interrupt #0 pin. On the Arduino Uno and
// Mega 2560, this is digital I/O pin 2.

uint8_t fifoCount;
uint8_t fifoBuffer[128];
uint8_t mpuIntStatus;

// uncomment "OUTPUT_READABLE_QUATERNION" if you want to see the actual
// quaternion components in a [w, x, y, z] format (not best for parsing
// on a remote host such as Processing or something though)
//#define OUTPUT_READABLE_QUATERNION

// uncomment "OUTPUT_READABLE_EULER" if you want to see Euler angles
// (in degrees) calculated from the quaternions coming from the FIFO.
// Note that Euler angles suffer from gimbal lock (for more info, see
// http://en.wikipedia.org/wiki/Gimbal_lock)
//#define OUTPUT_READABLE_EULER

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires a gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_FRAMEACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the initial "zero
// rotation" frame of reference. Could be quite handy in some cases.
//#define OUTPUT_READABLE_FRAMEACCEL

// uncomment "OUTPUT_TEAPOT" if you want output that matches the
// format used for the InvenSense teapot demo
//#define OUTPUT_TEAPOT



// NOTE! Enabling DEBUG adds about 3.3kB to the flash program size.
// Debug output is now working even on ATMega328P MCUs (e.g. Arduino Uno)
// after moving string constants to flash memory storage using the F()
// compiler macro (Arduino IDE 1.0+ required).

#define DEBUG
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
    DEBUG_PRINTLN(F("Initializing I2C devices..."));
    accelgyro.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(accelgyro.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
    
    // wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again

    // ====================== BEGIN DMP INITIALIZATION CODE ===========================

    // reset device
    DEBUG_PRINTLN(F("\n\nResetting MPU6050..."));
    accelgyro.reset();
    delay(50); // wait after reset

    // enable sleep mode and wake cycle
    /*Serial.println(F("Enabling sleep mode..."));
    accelgyro.setSleepEnabled(true);
    Serial.println(F("Enabling wake cycle..."));
    accelgyro.setWakeCycleEnabled(true);*/

    // disable sleep mode
    DEBUG_PRINTLN(F("Disabling sleep mode..."));
    accelgyro.setSleepEnabled(false);

    // get MPU hardware revision
    DEBUG_PRINTLN(F("Selecting user bank 16..."));
    accelgyro.setMemoryBank(0x10, true, true);
    DEBUG_PRINTLN(F("Selecting memory byte 6..."));
    accelgyro.setMemoryStartAddress(0x06);
    DEBUG_PRINTLN(F("Checking hardware revision..."));
    uint8_t hwRevision = accelgyro.readMemoryByte();
    DEBUG_PRINT(F("Revision @ user[16][6] = "));
    DEBUG_PRINTLNF(hwRevision, HEX);
    DEBUG_PRINTLN(F("Resetting memory bank selection to 0..."));
    accelgyro.setMemoryBank(0, false, false);

    // check OTP bank valid
    DEBUG_PRINTLN(F("Reading OTP bank valid flag..."));
    uint8_t otpValid = accelgyro.getOTPBankValid();
    DEBUG_PRINT(F("OTP bank is "));
    DEBUG_PRINTLN(otpValid ? F("valid!") : F("invalid!"));

    // get X/Y/Z gyro offsets
    DEBUG_PRINTLN(F("Reading gyro offset values..."));
    int8_t xgOffset = accelgyro.getXGyroOffset();
    int8_t ygOffset = accelgyro.getYGyroOffset();
    int8_t zgOffset = accelgyro.getZGyroOffset();
    DEBUG_PRINT(F("X gyro offset = "));
    DEBUG_PRINTLN(xgOffset);
    DEBUG_PRINT(F("Y gyro offset = "));
    DEBUG_PRINTLN(ygOffset);
    DEBUG_PRINT(F("Z gyro offset = "));
    DEBUG_PRINTLN(zgOffset);

    // setup weird slave stuff (?)
    DEBUG_PRINTLN(F("Setting slave 0 address to 0x7F..."));
    accelgyro.setSlaveAddress(0, 0x7F);
    DEBUG_PRINTLN(F("Disabling I2C Master mode..."));
    accelgyro.setI2CMasterModeEnabled(false);
    DEBUG_PRINTLN(F("Setting slave 0 address to 0x68 (self)..."));
    accelgyro.setSlaveAddress(0, 0x68);
    DEBUG_PRINTLN(F("Resetting I2C Master control..."));
    accelgyro.resetI2CMaster();
    delay(20);

    // load DMP code into memory banks
    DEBUG_PRINT(F("Writing DMP code to MPU memory banks ("));
    DEBUG_PRINT(MPU6050_DMP_CODE_SIZE);
    DEBUG_PRINTLN(F(" bytes)"));
    if (accelgyro.writeProgMemoryBlock(dmpMemory, MPU6050_DMP_CODE_SIZE)) {
        DEBUG_PRINTLN(F("Success! DMP code written and verified."));

        // write DMP configuration
        DEBUG_PRINT(F("Writing DMP configuration to MPU memory banks ("));
        DEBUG_PRINT(MPU6050_DMP_CONFIG_SIZE);
        DEBUG_PRINTLN(F(" bytes in config def)"));
        if (accelgyro.writeProgDMPConfigurationSet(dmpConfig, MPU6050_DMP_CONFIG_SIZE)) {
            DEBUG_PRINTLN(F("Success! DMP configuration written and verified."));

            DEBUG_PRINTLN(F("Setting clock source to Z Gyro..."));
            accelgyro.setClockSource(MPU6050_CLOCK_PLL_ZGYRO);

            DEBUG_PRINTLN(F("Setting DMP interrupt (only) enabled..."));
            accelgyro.setIntEnabled(0x02);

            DEBUG_PRINTLN(F("Setting sample rate to 200Hz..."));
            accelgyro.setRate(4); // 1khz / (1 + 4) = 200 Hz

            DEBUG_PRINTLN(F("Setting external frame sync to TEMP_OUT_L[0]..."));
            accelgyro.setExternalFrameSync(MPU6050_EXT_SYNC_TEMP_OUT_L);

            DEBUG_PRINTLN(F("Setting DLPF bandwidth to 42Hz..."));
            accelgyro.setDLPFMode(MPU6050_DLPF_BW_42);

            DEBUG_PRINTLN(F("Setting gyro sensitivity to +/- 2000 deg/sec..."));
            accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);

            DEBUG_PRINTLN(F("Setting DMP configuration bytes (function unknown)..."));
            accelgyro.setDMPConfig1(0x03);
            accelgyro.setDMPConfig2(0x00);

            DEBUG_PRINTLN(F("Clearing OTP Bank flag..."));
            accelgyro.setOTPBankValid(false);

            DEBUG_PRINTLN(F("Setting X/Y/Z gyro offsets to zero..."));
            accelgyro.setXGyroOffset(0);
            accelgyro.setYGyroOffset(0);
            accelgyro.setZGyroOffset(0);

            // Sungon Lee reports better results by disabling accel offset zeroing here
            //DEBUG_PRINTLN(F("Setting X/Y/Z accel offsets to zero..."));
            //accelgyro.setXAccelOffset(0);
            //accelgyro.setYAccelOffset(0);
            //accelgyro.setZAccelOffset(0);

            uint8_t dmpMemUpdates[7][11] = {
                { 0x01, 0xB2, 0x02, 0xFF, 0xFF },
                { 0x01, 0x90, 0x04, 0x09, 0x23, 0xA1, 0x35 },
                { 0x01, 0x6A, 0x02, 0x06, 0x00 },
                { 0x01, 0x60, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                { 0x00, 0x60, 0x04, 0x40, 0x00, 0x00, 0x00 },
                { 0x01, 0x62, 0x02, 0x00, 0x00 },
                { 0x00, 0x60, 0x04, 0x00, 0x40, 0x00, 0x00 }
            };

            DEBUG_PRINTLN(F("Writing final memory update 1/7 (function unknown)..."));
            accelgyro.writeMemoryBlock(dmpMemUpdates[0] + 3, dmpMemUpdates[0][2], dmpMemUpdates[0][0], dmpMemUpdates[0][1]);

            DEBUG_PRINTLN(F("Writing final memory update 2/7 (function unknown)..."));
            accelgyro.writeMemoryBlock(dmpMemUpdates[1] + 3, dmpMemUpdates[1][2], dmpMemUpdates[1][0], dmpMemUpdates[1][1]);

            DEBUG_PRINTLN(F("Resetting FIFO..."));
            accelgyro.resetFIFO();

            DEBUG_PRINTLN(F("Reading FIFO count..."));
            fifoCount = accelgyro.getFIFOCount();

            DEBUG_PRINT(F("Current FIFO count="));
            DEBUG_PRINTLN(fifoCount);
            accelgyro.getFIFOBytes(fifoBuffer, fifoCount);

            DEBUG_PRINTLN(F("Setting motion detection threshold to 2..."));
            accelgyro.setMotionDetectionThreshold(2);

            DEBUG_PRINTLN(F("Setting zero-motion detection threshold to 156..."));
            accelgyro.setZeroMotionDetectionThreshold(156);

            DEBUG_PRINTLN(F("Setting motion detection duration to 80..."));
            accelgyro.setMotionDetectionDuration(80);

            DEBUG_PRINTLN(F("Setting zero-motion detection duration to 0..."));
            accelgyro.setZeroMotionDetectionDuration(0);

            DEBUG_PRINTLN(F("Resetting FIFO..."));
            accelgyro.resetFIFO();

            DEBUG_PRINTLN(F("Enabling FIFO..."));
            accelgyro.setFIFOEnabled(true);

            DEBUG_PRINTLN(F("Enabling DMP..."));
            accelgyro.setDMPEnabled(true);

            DEBUG_PRINTLN(F("Resetting DMP..."));
            accelgyro.resetDMP();

            DEBUG_PRINTLN(F("Writing final memory update 3/7 (function unknown)..."));
            accelgyro.writeMemoryBlock(dmpMemUpdates[2] + 3, dmpMemUpdates[2][2], dmpMemUpdates[2][0], dmpMemUpdates[2][1]);

            DEBUG_PRINTLN(F("Writing final memory update 4/7 (function unknown)..."));
            accelgyro.writeMemoryBlock(dmpMemUpdates[3] + 3, dmpMemUpdates[3][2], dmpMemUpdates[3][0], dmpMemUpdates[3][1]);

            DEBUG_PRINTLN(F("Writing final memory update 5/7 (function unknown)..."));
            accelgyro.writeMemoryBlock(dmpMemUpdates[4] + 3, dmpMemUpdates[4][2], dmpMemUpdates[4][0], dmpMemUpdates[4][1]);

            DEBUG_PRINTLN(F("Waiting for FIFO count > 2..."));
            while ((fifoCount = accelgyro.getFIFOCount()) < 3);

            DEBUG_PRINT(F("Current FIFO count="));
            DEBUG_PRINTLN(fifoCount);
            DEBUG_PRINTLN(F("Reading FIFO data..."));
            accelgyro.getFIFOBytes(fifoBuffer, fifoCount);

            DEBUG_PRINTLN(F("Reading interrupt status..."));
            mpuIntStatus = accelgyro.getIntStatus();

            DEBUG_PRINT(F("Current interrupt status="));
            DEBUG_PRINTLNF(mpuIntStatus, HEX);

            DEBUG_PRINTLN(F("Reading final memory update 6/7 (function unknown)..."));
            accelgyro.readMemoryBlock(dmpMemUpdates[5] + 3, dmpMemUpdates[5][2], dmpMemUpdates[5][0], dmpMemUpdates[5][1]);

            DEBUG_PRINTLN(F("Waiting for FIFO count > 2..."));
            while ((fifoCount = accelgyro.getFIFOCount()) < 3);

            DEBUG_PRINT(F("Current FIFO count="));
            DEBUG_PRINTLN(fifoCount);

            DEBUG_PRINTLN(F("Reading FIFO data..."));
            accelgyro.getFIFOBytes(fifoBuffer, fifoCount);

            DEBUG_PRINTLN(F("Reading interrupt status..."));
            mpuIntStatus = accelgyro.getIntStatus();

            DEBUG_PRINT(F("Current interrupt status="));
            DEBUG_PRINTLNF(mpuIntStatus, HEX);

            DEBUG_PRINTLN(F("Writing final memory update 7/7 (function unknown)..."));
            accelgyro.writeMemoryBlock(dmpMemUpdates[6] + 3, dmpMemUpdates[6][2], dmpMemUpdates[6][0], dmpMemUpdates[6][1]);

            DEBUG_PRINTLN(F("DMP is good to go! Finally."));

            DEBUG_PRINTLN(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
            attachInterrupt(0, dmpDataReady, CHANGE);

            DEBUG_PRINTLN(F("Waiting for 1st interrupt..."));
        } else {
            DEBUG_PRINTLN(F("ERROR! DMP configuration verification failed."));
        }
    } else {
        DEBUG_PRINTLN(F("ERROR! DMP code verification failed."));
    }

    // ====================== END DMP INITIALIZATION CODE ===========================

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

// raw accel sensor measurements (not used in some output formats)
VectorInt16 aa;         // [x, y, z]

// gravity-free accel sensor measurements (not used in some output formats)
VectorInt16 aaReal;     // [x, y, z]

// initial-frame accel sensor measurements (not used in some output formats)
VectorInt16 aaFrame;    // [x, y, z]

// raw quaternion container
Quaternion q;           // [w, x, y, z]

// gravity vector
VectorFloat gravity;    // [x, y, z]

// Euler angle container
float euler[3];         // [psi, theta, phi]

// yaw/pitch/roll container and gravity vector
float ypr[3];           // [yaw, pitch, roll]

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

    if (fifoCount >= 42) {
        // read raw sensor data ASAP if we're later going to display initial-frame
        // acceleration values; we want to do this as soon as possible to make sure
        // the acceleration values read are as close to the ones used by the DMP as
        // possible. Ideally the DMP could also spit this information out to us,
        // but I don't know if that is possible yet.
        #ifdef OUTPUT_READABLE_FRAMEACCEL
            accelgyro.getAcceleration(&aa.x, &aa.y, &aa.z);
        #endif
    
        // convert FIFO output (16-bit unsigned integer, 0-32767 used)
        // to [-1, 1] ranged float variable
        // (not necessary for TEAPOT output, but used by others)
        q.w = (float)((fifoBuffer[0] << 8) + fifoBuffer[1]) / 16384;   // w
        q.x = (float)((fifoBuffer[4] << 8) + fifoBuffer[5]) / 16384;   // x
        q.y = (float)((fifoBuffer[8] << 8) + fifoBuffer[9]) / 16384;   // y
        q.z = (float)((fifoBuffer[12] << 8) + fifoBuffer[13]) / 16384; // z
    
        // calculate gravity vector (not necessary for TEAPOT output)
        gravity.x = 2 * (q.x*q.z - q.w*q.y);                       // x
        gravity.y = 2 * (q.w*q.x + q.y*q.z);                       // y
        gravity.z = q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z;     // z
    
        #ifdef OUTPUT_READABLE_QUATERNION
            // display quaternion values in easy matrix form: [w, x, y, z]
            Serial.print("quat\t[");
            Serial.print(q.w);
            Serial.print(", ");
            Serial.print(q.x);
            Serial.print(", ");
            Serial.print(q.y);
            Serial.print(", ");
            Serial.print(q.z);
            Serial.print("]\n");
        #endif
    
        #ifdef OUTPUT_READABLE_EULER
            // display Euler angles in degrees
    
            // psi
            euler[0] = atan2(2*q.x*q.y - 2*q.w*q.z, 2*q.w*q.w + 2*q.x*q.x - 1) * 180/M_PI;
            // theta
            euler[1] = -asin(2*q.x*q.z + 2*q.w*q.y) * 180/M_PI;
            // phi
            euler[2] = atan2(2*q.y*q.z - 2*q.w*q.x, 2*q.w*q.w + 2*q.z*q.z - 1) * 180/M_PI;
    
            Serial.print("euler\t");
            Serial.print(euler[0]);
            Serial.print("\t");
            Serial.print(euler[1]);
            Serial.print("\t");
            Serial.println(euler[2]);
        #endif
    
        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees: phi
    
            // yaw: (about Z axis)
            ypr[0] = atan2(2*q.x*q.y - 2*q.w*q.z, 2*q.w*q.w + 2*q.x*q.x - 1) * 180/M_PI;
            // pitch: (nose up/down, about Y axis)
            ypr[1] = atan(gravity.x / sqrt(gravity.y*gravity.y + gravity.z*gravity.z)) * 180/M_PI;
            // roll: (tilt left/right, about X axis)
            ypr[2] = atan(gravity.y / sqrt(gravity.x*gravity.x + gravity.z*gravity.z)) * 180/M_PI;
    
            Serial.print("ypr\t");
            Serial.print(ypr[0]);
            Serial.print("\t");
            Serial.print(ypr[1]);
            Serial.print("\t");
            Serial.println(ypr[2]);
        #endif
    
        #ifdef OUTPUT_READABLE_FRAMEACCEL
            // display initial-frame acceleration, adjusted to remove gravity
            // and rotated based on known orientation from quaternion
    
            // get rid of the gravity component (+1g = +16384 in standard DMP)
            aaReal.x = (aa.x - gravity.x*16384);
            aaReal.y = (aa.y - gravity.y*16384);
            aaReal.z = (aa.z - gravity.z*16384);
            
            // rotate measured 3D acceleration vector into original state
            // frame of reference based on orientation quaternion
            aaFrame = aaReal.getRotated(q);

            Serial.print("aframe\t");
            Serial.print(aaFrame.x);
            Serial.print("\t");
            Serial.print(aaFrame.y);
            Serial.print("\t");
            Serial.println(aaFrame.z);
        #endif
    
        #ifdef OUTPUT_TEAPOT
            // display quaternion values in InvenSense Teapot demo format:
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
        #endif

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
    }
}
