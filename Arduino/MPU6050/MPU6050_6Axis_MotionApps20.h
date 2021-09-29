// I2Cdev library collection - MPU6050 I2C device class
// Based on InvenSense MPU-6050 register map document rev. 2.0, 5/19/2011 (RM-MPU-6000A-00)
// 10/3/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//  2021/09/27 - split implementations out of header files, finally
//     ... - ongoing debug release

// NOTE: THIS IS ONLY A PARIAL RELEASE. THIS DEVICE CLASS IS CURRENTLY UNDERGOING ACTIVE
// DEVELOPMENT AND IS STILL MISSING SOME IMPORTANT FEATURES. PLEASE KEEP THIS IN MIND IF
// YOU DECIDE TO USE THIS PARTICULAR CODE FOR ANYTHING.

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

#ifndef _MPU6050_6AXIS_MOTIONAPPS20_H_
#define _MPU6050_6AXIS_MOTIONAPPS20_H_

// take ownership of the "MPU6050" typedef
#define I2CDEVLIB_MPU6050_TYPEDEF

#include "MPU6050.h"

class MPU6050_6Axis_MotionApps20 : public MPU6050_Base {
    public:
        MPU6050_6Axis_MotionApps20(uint8_t address=MPU6050_DEFAULT_ADDRESS, void *wireObj=0) : MPU6050_Base(address, wireObj) { }

        uint8_t dmpInitialize();
        bool dmpPacketAvailable();

        uint8_t dmpSetFIFORate(uint8_t fifoRate);
        uint8_t dmpGetFIFORate();
        uint8_t dmpGetSampleStepSizeMS();
        uint8_t dmpGetSampleFrequency();
        int32_t dmpDecodeTemperature(int8_t tempReg);
        
        // Register callbacks after a packet of FIFO data is processed
        //uint8_t dmpRegisterFIFORateProcess(inv_obj_func func, int16_t priority);
        //uint8_t dmpUnregisterFIFORateProcess(inv_obj_func func);
        uint8_t dmpRunFIFORateProcesses();
        
        // Setup FIFO for various output
        uint8_t dmpSendQuaternion(uint_fast16_t accuracy);
        uint8_t dmpSendGyro(uint_fast16_t elements, uint_fast16_t accuracy);
        uint8_t dmpSendAccel(uint_fast16_t elements, uint_fast16_t accuracy);
        uint8_t dmpSendLinearAccel(uint_fast16_t elements, uint_fast16_t accuracy);
        uint8_t dmpSendLinearAccelInWorld(uint_fast16_t elements, uint_fast16_t accuracy);
        uint8_t dmpSendControlData(uint_fast16_t elements, uint_fast16_t accuracy);
        uint8_t dmpSendSensorData(uint_fast16_t elements, uint_fast16_t accuracy);
        uint8_t dmpSendExternalSensorData(uint_fast16_t elements, uint_fast16_t accuracy);
        uint8_t dmpSendGravity(uint_fast16_t elements, uint_fast16_t accuracy);
        uint8_t dmpSendPacketNumber(uint_fast16_t accuracy);
        uint8_t dmpSendQuantizedAccel(uint_fast16_t elements, uint_fast16_t accuracy);
        uint8_t dmpSendEIS(uint_fast16_t elements, uint_fast16_t accuracy);

        // Get Fixed Point data from FIFO
        uint8_t dmpGetAccel(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetAccel(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetAccel(VectorInt16 *v, const uint8_t* packet=0);
        uint8_t dmpGetQuaternion(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetQuaternion(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetQuaternion(Quaternion *q, const uint8_t* packet=0);
        uint8_t dmpGet6AxisQuaternion(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGet6AxisQuaternion(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGet6AxisQuaternion(Quaternion *q, const uint8_t* packet=0);
        uint8_t dmpGetRelativeQuaternion(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetRelativeQuaternion(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetRelativeQuaternion(Quaternion *data, const uint8_t* packet=0);
        uint8_t dmpGetGyro(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetGyro(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetGyro(VectorInt16 *v, const uint8_t* packet=0);
        uint8_t dmpSetLinearAccelFilterCoefficient(float coef);
        uint8_t dmpGetLinearAccel(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetLinearAccel(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetLinearAccel(VectorInt16 *v, const uint8_t* packet=0);
        uint8_t dmpGetLinearAccel(VectorInt16 *v, VectorInt16 *vRaw, VectorFloat *gravity);
        uint8_t dmpGetLinearAccelInWorld(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetLinearAccelInWorld(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetLinearAccelInWorld(VectorInt16 *v, const uint8_t* packet=0);
        uint8_t dmpGetLinearAccelInWorld(VectorInt16 *v, VectorInt16 *vReal, Quaternion *q);
        uint8_t dmpGetGyroAndAccelSensor(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetGyroAndAccelSensor(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetGyroAndAccelSensor(VectorInt16 *g, VectorInt16 *a, const uint8_t* packet=0);
        uint8_t dmpGetGyroSensor(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetGyroSensor(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetGyroSensor(VectorInt16 *v, const uint8_t* packet=0);
        uint8_t dmpGetControlData(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetTemperature(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetGravity(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetGravity(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetGravity(VectorInt16 *v, const uint8_t* packet=0);
        uint8_t dmpGetGravity(VectorFloat *v, Quaternion *q);
        uint8_t dmpGetUnquantizedAccel(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetUnquantizedAccel(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetUnquantizedAccel(VectorInt16 *v, const uint8_t* packet=0);
        uint8_t dmpGetQuantizedAccel(int32_t *data, const uint8_t* packet=0);
        uint8_t dmpGetQuantizedAccel(int16_t *data, const uint8_t* packet=0);
        uint8_t dmpGetQuantizedAccel(VectorInt16 *v, const uint8_t* packet=0);
        uint8_t dmpGetExternalSensorData(int32_t *data, uint16_t size, const uint8_t* packet=0);
        uint8_t dmpGetEIS(int32_t *data, const uint8_t* packet=0);
        
        uint8_t dmpGetEuler(float *data, Quaternion *q);
        uint8_t dmpGetYawPitchRoll(float *data, Quaternion *q, VectorFloat *gravity);

        // Get Floating Point data from FIFO
        uint8_t dmpGetAccelFloat(float *data, const uint8_t* packet=0);
        uint8_t dmpGetQuaternionFloat(float *data, const uint8_t* packet=0);

        uint8_t dmpProcessFIFOPacket(const unsigned char *dmpData);
        uint8_t dmpReadAndProcessFIFOPacket(uint8_t numPackets, uint8_t *processed=NULL);

        uint8_t dmpSetFIFOProcessedCallback(void (*func) (void));

        uint8_t dmpInitFIFOParam();
        uint8_t dmpCloseFIFO();
        uint8_t dmpSetGyroDataSource(uint8_t source);
        uint8_t dmpDecodeQuantizedAccel();
        uint32_t dmpGetGyroSumOfSquare();
        uint32_t dmpGetAccelSumOfSquare();
        void dmpOverrideQuaternion(long *q);
        uint16_t dmpGetFIFOPacketSize();
        uint8_t dmpGetCurrentFIFOPacket(uint8_t *data); // overflow proof

    private:
        uint8_t *dmpPacketBuffer;
        uint16_t dmpPacketSize;
};

typedef MPU6050_6Axis_MotionApps20 MPU6050;

#endif /* _MPU6050_6AXIS_MOTIONAPPS20_H_ */
