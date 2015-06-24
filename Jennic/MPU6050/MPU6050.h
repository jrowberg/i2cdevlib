// I2Cdev library collection - MPU6050 I2C device class
// Based on InvenSense MPU-6050 register map document rev. 2.0, 5/19/2011 (RM-MPU-6000A-00)
// 10/3/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
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

#ifndef _MPU6050_H_
#define _MPU6050_H_

#include "I2Cdev.h"

#define MPU6050_ADDRESS_AD0_LOW     0x68 // address pin low (GND), default for InvenSense evaluation board
#define MPU6050_ADDRESS_AD0_HIGH    0x69 // address pin high (VCC)
#define MPU6050_DEFAULT_ADDRESS     MPU6050_ADDRESS_AD0_LOW

#define MPU6050_RA_XG_OFFS_TC       0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_YG_OFFS_TC       0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_ZG_OFFS_TC       0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_X_FINE_GAIN      0x03 //[7:0] X_FINE_GAIN
#define MPU6050_RA_Y_FINE_GAIN      0x04 //[7:0] Y_FINE_GAIN
#define MPU6050_RA_Z_FINE_GAIN      0x05 //[7:0] Z_FINE_GAIN
#define MPU6050_RA_XA_OFFS_H        0x06 //[15:0] XA_OFFS
#define MPU6050_RA_XA_OFFS_L_TC     0x07
#define MPU6050_RA_YA_OFFS_H        0x08 //[15:0] YA_OFFS
#define MPU6050_RA_YA_OFFS_L_TC     0x09
#define MPU6050_RA_ZA_OFFS_H        0x0A //[15:0] ZA_OFFS
#define MPU6050_RA_ZA_OFFS_L_TC     0x0B
#define MPU6050_RA_XG_OFFS_USRH     0x13 //[15:0] XG_OFFS_USR
#define MPU6050_RA_XG_OFFS_USRL     0x14
#define MPU6050_RA_YG_OFFS_USRH     0x15 //[15:0] YG_OFFS_USR
#define MPU6050_RA_YG_OFFS_USRL     0x16
#define MPU6050_RA_ZG_OFFS_USRH     0x17 //[15:0] ZG_OFFS_USR
#define MPU6050_RA_ZG_OFFS_USRL     0x18
#define MPU6050_RA_SMPLRT_DIV       0x19
#define MPU6050_RA_CONFIG           0x1A
#define MPU6050_RA_GYRO_CONFIG      0x1B
#define MPU6050_RA_ACCEL_CONFIG     0x1C
#define MPU6050_RA_FF_THR           0x1D
#define MPU6050_RA_FF_DUR           0x1E
#define MPU6050_RA_MOT_THR          0x1F
#define MPU6050_RA_MOT_DUR          0x20
#define MPU6050_RA_ZRMOT_THR        0x21
#define MPU6050_RA_ZRMOT_DUR        0x22
#define MPU6050_RA_FIFO_EN          0x23
#define MPU6050_RA_I2C_MST_CTRL     0x24
#define MPU6050_RA_I2C_SLV0_ADDR    0x25
#define MPU6050_RA_I2C_SLV0_REG     0x26
#define MPU6050_RA_I2C_SLV0_CTRL    0x27
#define MPU6050_RA_I2C_SLV1_ADDR    0x28
#define MPU6050_RA_I2C_SLV1_REG     0x29
#define MPU6050_RA_I2C_SLV1_CTRL    0x2A
#define MPU6050_RA_I2C_SLV2_ADDR    0x2B
#define MPU6050_RA_I2C_SLV2_REG     0x2C
#define MPU6050_RA_I2C_SLV2_CTRL    0x2D
#define MPU6050_RA_I2C_SLV3_ADDR    0x2E
#define MPU6050_RA_I2C_SLV3_REG     0x2F
#define MPU6050_RA_I2C_SLV3_CTRL    0x30
#define MPU6050_RA_I2C_SLV4_ADDR    0x31
#define MPU6050_RA_I2C_SLV4_REG     0x32
#define MPU6050_RA_I2C_SLV4_DO      0x33
#define MPU6050_RA_I2C_SLV4_CTRL    0x34
#define MPU6050_RA_I2C_SLV4_DI      0x35
#define MPU6050_RA_I2C_MST_STATUS   0x36
#define MPU6050_RA_INT_PIN_CFG      0x37
#define MPU6050_RA_INT_ENABLE       0x38
#define MPU6050_RA_DMP_INT_STATUS   0x39
#define MPU6050_RA_INT_STATUS       0x3A
#define MPU6050_RA_ACCEL_XOUT_H     0x3B
#define MPU6050_RA_ACCEL_XOUT_L     0x3C
#define MPU6050_RA_ACCEL_YOUT_H     0x3D
#define MPU6050_RA_ACCEL_YOUT_L     0x3E
#define MPU6050_RA_ACCEL_ZOUT_H     0x3F
#define MPU6050_RA_ACCEL_ZOUT_L     0x40
#define MPU6050_RA_TEMP_OUT_H       0x41
#define MPU6050_RA_TEMP_OUT_L       0x42
#define MPU6050_RA_GYRO_XOUT_H      0x43
#define MPU6050_RA_GYRO_XOUT_L      0x44
#define MPU6050_RA_GYRO_YOUT_H      0x45
#define MPU6050_RA_GYRO_YOUT_L      0x46
#define MPU6050_RA_GYRO_ZOUT_H      0x47
#define MPU6050_RA_GYRO_ZOUT_L      0x48
#define MPU6050_RA_EXT_SENS_DATA_00 0x49
#define MPU6050_RA_EXT_SENS_DATA_01 0x4A
#define MPU6050_RA_EXT_SENS_DATA_02 0x4B
#define MPU6050_RA_EXT_SENS_DATA_03 0x4C
#define MPU6050_RA_EXT_SENS_DATA_04 0x4D
#define MPU6050_RA_EXT_SENS_DATA_05 0x4E
#define MPU6050_RA_EXT_SENS_DATA_06 0x4F
#define MPU6050_RA_EXT_SENS_DATA_07 0x50
#define MPU6050_RA_EXT_SENS_DATA_08 0x51
#define MPU6050_RA_EXT_SENS_DATA_09 0x52
#define MPU6050_RA_EXT_SENS_DATA_10 0x53
#define MPU6050_RA_EXT_SENS_DATA_11 0x54
#define MPU6050_RA_EXT_SENS_DATA_12 0x55
#define MPU6050_RA_EXT_SENS_DATA_13 0x56
#define MPU6050_RA_EXT_SENS_DATA_14 0x57
#define MPU6050_RA_EXT_SENS_DATA_15 0x58
#define MPU6050_RA_EXT_SENS_DATA_16 0x59
#define MPU6050_RA_EXT_SENS_DATA_17 0x5A
#define MPU6050_RA_EXT_SENS_DATA_18 0x5B
#define MPU6050_RA_EXT_SENS_DATA_19 0x5C
#define MPU6050_RA_EXT_SENS_DATA_20 0x5D
#define MPU6050_RA_EXT_SENS_DATA_21 0x5E
#define MPU6050_RA_EXT_SENS_DATA_22 0x5F
#define MPU6050_RA_EXT_SENS_DATA_23 0x60
#define MPU6050_RA_MOT_DETECT_STATUS    0x61
#define MPU6050_RA_I2C_SLV0_DO      0x63
#define MPU6050_RA_I2C_SLV1_DO      0x64
#define MPU6050_RA_I2C_SLV2_DO      0x65
#define MPU6050_RA_I2C_SLV3_DO      0x66
#define MPU6050_RA_I2C_MST_DELAY_CTRL   0x67
#define MPU6050_RA_SIGNAL_PATH_RESET    0x68
#define MPU6050_RA_MOT_DETECT_CTRL      0x69
#define MPU6050_RA_USER_CTRL        0x6A
#define MPU6050_RA_PWR_MGMT_1       0x6B
#define MPU6050_RA_PWR_MGMT_2       0x6C
#define MPU6050_RA_BANK_SEL         0x6D
#define MPU6050_RA_MEM_START_ADDR   0x6E
#define MPU6050_RA_MEM_R_W          0x6F
#define MPU6050_RA_DMP_CFG_1        0x70
#define MPU6050_RA_DMP_CFG_2        0x71
#define MPU6050_RA_FIFO_COUNTH      0x72
#define MPU6050_RA_FIFO_COUNTL      0x73
#define MPU6050_RA_FIFO_R_W         0x74
#define MPU6050_RA_WHO_AM_I         0x75

#define MPU6050_TC_PWR_MODE_BIT     7
#define MPU6050_TC_OFFSET_BIT       6
#define MPU6050_TC_OFFSET_LENGTH    6
#define MPU6050_TC_OTP_BNK_VLD_BIT  0

#define MPU6050_VDDIO_LEVEL_VLOGIC  0
#define MPU6050_VDDIO_LEVEL_VDD     1

#define MPU6050_CFG_EXT_SYNC_SET_BIT    5
#define MPU6050_CFG_EXT_SYNC_SET_LENGTH 3
#define MPU6050_CFG_DLPF_CFG_BIT    2
#define MPU6050_CFG_DLPF_CFG_LENGTH 3

#define MPU6050_EXT_SYNC_DISABLED       0x0
#define MPU6050_EXT_SYNC_TEMP_OUT_L     0x1
#define MPU6050_EXT_SYNC_GYRO_XOUT_L    0x2
#define MPU6050_EXT_SYNC_GYRO_YOUT_L    0x3
#define MPU6050_EXT_SYNC_GYRO_ZOUT_L    0x4
#define MPU6050_EXT_SYNC_ACCEL_XOUT_L   0x5
#define MPU6050_EXT_SYNC_ACCEL_YOUT_L   0x6
#define MPU6050_EXT_SYNC_ACCEL_ZOUT_L   0x7

#define MPU6050_DLPF_BW_256         0x00
#define MPU6050_DLPF_BW_188         0x01
#define MPU6050_DLPF_BW_98          0x02
#define MPU6050_DLPF_BW_42          0x03
#define MPU6050_DLPF_BW_20          0x04
#define MPU6050_DLPF_BW_10          0x05
#define MPU6050_DLPF_BW_5           0x06

#define MPU6050_GCONFIG_FS_SEL_BIT      4
#define MPU6050_GCONFIG_FS_SEL_LENGTH   2

#define MPU6050_GYRO_FS_250         0x00
#define MPU6050_GYRO_FS_500         0x01
#define MPU6050_GYRO_FS_1000        0x02
#define MPU6050_GYRO_FS_2000        0x03

#define MPU6050_ACONFIG_XA_ST_BIT           7
#define MPU6050_ACONFIG_YA_ST_BIT           6
#define MPU6050_ACONFIG_ZA_ST_BIT           5
#define MPU6050_ACONFIG_AFS_SEL_BIT         4
#define MPU6050_ACONFIG_AFS_SEL_LENGTH      2
#define MPU6050_ACONFIG_ACCEL_HPF_BIT       2
#define MPU6050_ACONFIG_ACCEL_HPF_LENGTH    3

#define MPU6050_ACCEL_FS_2          0x00
#define MPU6050_ACCEL_FS_4          0x01
#define MPU6050_ACCEL_FS_8          0x02
#define MPU6050_ACCEL_FS_16         0x03

#define MPU6050_DHPF_RESET          0x00
#define MPU6050_DHPF_5              0x01
#define MPU6050_DHPF_2P5            0x02
#define MPU6050_DHPF_1P25           0x03
#define MPU6050_DHPF_0P63           0x04
#define MPU6050_DHPF_HOLD           0x07

#define MPU6050_TEMP_FIFO_EN_BIT    7
#define MPU6050_XG_FIFO_EN_BIT      6
#define MPU6050_YG_FIFO_EN_BIT      5
#define MPU6050_ZG_FIFO_EN_BIT      4
#define MPU6050_ACCEL_FIFO_EN_BIT   3
#define MPU6050_SLV2_FIFO_EN_BIT    2
#define MPU6050_SLV1_FIFO_EN_BIT    1
#define MPU6050_SLV0_FIFO_EN_BIT    0

#define MPU6050_MULT_MST_EN_BIT     7
#define MPU6050_WAIT_FOR_ES_BIT     6
#define MPU6050_SLV_3_FIFO_EN_BIT   5
#define MPU6050_I2C_MST_P_NSR_BIT   4
#define MPU6050_I2C_MST_CLK_BIT     3
#define MPU6050_I2C_MST_CLK_LENGTH  4

#define MPU6050_CLOCK_DIV_348       0x0
#define MPU6050_CLOCK_DIV_333       0x1
#define MPU6050_CLOCK_DIV_320       0x2
#define MPU6050_CLOCK_DIV_308       0x3
#define MPU6050_CLOCK_DIV_296       0x4
#define MPU6050_CLOCK_DIV_286       0x5
#define MPU6050_CLOCK_DIV_276       0x6
#define MPU6050_CLOCK_DIV_267       0x7
#define MPU6050_CLOCK_DIV_258       0x8
#define MPU6050_CLOCK_DIV_500       0x9
#define MPU6050_CLOCK_DIV_471       0xA
#define MPU6050_CLOCK_DIV_444       0xB
#define MPU6050_CLOCK_DIV_421       0xC
#define MPU6050_CLOCK_DIV_400       0xD
#define MPU6050_CLOCK_DIV_381       0xE
#define MPU6050_CLOCK_DIV_364       0xF

#define MPU6050_I2C_SLV_RW_BIT      7
#define MPU6050_I2C_SLV_ADDR_BIT    6
#define MPU6050_I2C_SLV_ADDR_LENGTH 7
#define MPU6050_I2C_SLV_EN_BIT      7
#define MPU6050_I2C_SLV_BYTE_SW_BIT 6
#define MPU6050_I2C_SLV_REG_DIS_BIT 5
#define MPU6050_I2C_SLV_GRP_BIT     4
#define MPU6050_I2C_SLV_LEN_BIT     3
#define MPU6050_I2C_SLV_LEN_LENGTH  4

#define MPU6050_I2C_SLV4_RW_BIT         7
#define MPU6050_I2C_SLV4_ADDR_BIT       6
#define MPU6050_I2C_SLV4_ADDR_LENGTH    7
#define MPU6050_I2C_SLV4_EN_BIT         7
#define MPU6050_I2C_SLV4_INT_EN_BIT     6
#define MPU6050_I2C_SLV4_REG_DIS_BIT    5
#define MPU6050_I2C_SLV4_MST_DLY_BIT    4
#define MPU6050_I2C_SLV4_MST_DLY_LENGTH 5

#define MPU6050_MST_PASS_THROUGH_BIT    7
#define MPU6050_MST_I2C_SLV4_DONE_BIT   6
#define MPU6050_MST_I2C_LOST_ARB_BIT    5
#define MPU6050_MST_I2C_SLV4_NACK_BIT   4
#define MPU6050_MST_I2C_SLV3_NACK_BIT   3
#define MPU6050_MST_I2C_SLV2_NACK_BIT   2
#define MPU6050_MST_I2C_SLV1_NACK_BIT   1
#define MPU6050_MST_I2C_SLV0_NACK_BIT   0

#define MPU6050_INTCFG_INT_LEVEL_BIT        7
#define MPU6050_INTCFG_INT_OPEN_BIT         6
#define MPU6050_INTCFG_LATCH_INT_EN_BIT     5
#define MPU6050_INTCFG_INT_RD_CLEAR_BIT     4
#define MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT  3
#define MPU6050_INTCFG_FSYNC_INT_EN_BIT     2
#define MPU6050_INTCFG_I2C_BYPASS_EN_BIT    1
#define MPU6050_INTCFG_CLKOUT_EN_BIT        0

#define MPU6050_INTMODE_ACTIVEHIGH  0x00
#define MPU6050_INTMODE_ACTIVELOW   0x01

#define MPU6050_INTDRV_PUSHPULL     0x00
#define MPU6050_INTDRV_OPENDRAIN    0x01

#define MPU6050_INTLATCH_50USPULSE  0x00
#define MPU6050_INTLATCH_WAITCLEAR  0x01

#define MPU6050_INTCLEAR_STATUSREAD 0x00
#define MPU6050_INTCLEAR_ANYREAD    0x01

#define MPU6050_INTERRUPT_FF_BIT            7
#define MPU6050_INTERRUPT_MOT_BIT           6
#define MPU6050_INTERRUPT_ZMOT_BIT          5
#define MPU6050_INTERRUPT_FIFO_OFLOW_BIT    4
#define MPU6050_INTERRUPT_I2C_MST_INT_BIT   3
#define MPU6050_INTERRUPT_PLL_RDY_INT_BIT   2
#define MPU6050_INTERRUPT_DMP_INT_BIT       1
#define MPU6050_INTERRUPT_DATA_RDY_BIT      0

// TODO: figure out what these actually do
// UMPL source code is not very obivous
#define MPU6050_DMPINT_5_BIT            5
#define MPU6050_DMPINT_4_BIT            4
#define MPU6050_DMPINT_3_BIT            3
#define MPU6050_DMPINT_2_BIT            2
#define MPU6050_DMPINT_1_BIT            1
#define MPU6050_DMPINT_0_BIT            0

#define MPU6050_MOTION_MOT_XNEG_BIT     7
#define MPU6050_MOTION_MOT_XPOS_BIT     6
#define MPU6050_MOTION_MOT_YNEG_BIT     5
#define MPU6050_MOTION_MOT_YPOS_BIT     4
#define MPU6050_MOTION_MOT_ZNEG_BIT     3
#define MPU6050_MOTION_MOT_ZPOS_BIT     2
#define MPU6050_MOTION_MOT_ZRMOT_BIT    0

#define MPU6050_DELAYCTRL_DELAY_ES_SHADOW_BIT   7
#define MPU6050_DELAYCTRL_I2C_SLV4_DLY_EN_BIT   4
#define MPU6050_DELAYCTRL_I2C_SLV3_DLY_EN_BIT   3
#define MPU6050_DELAYCTRL_I2C_SLV2_DLY_EN_BIT   2
#define MPU6050_DELAYCTRL_I2C_SLV1_DLY_EN_BIT   1
#define MPU6050_DELAYCTRL_I2C_SLV0_DLY_EN_BIT   0

#define MPU6050_PATHRESET_GYRO_RESET_BIT    2
#define MPU6050_PATHRESET_ACCEL_RESET_BIT   1
#define MPU6050_PATHRESET_TEMP_RESET_BIT    0

#define MPU6050_DETECT_ACCEL_ON_DELAY_BIT       5
#define MPU6050_DETECT_ACCEL_ON_DELAY_LENGTH    2
#define MPU6050_DETECT_FF_COUNT_BIT             3
#define MPU6050_DETECT_FF_COUNT_LENGTH          2
#define MPU6050_DETECT_MOT_COUNT_BIT            1
#define MPU6050_DETECT_MOT_COUNT_LENGTH         2

#define MPU6050_DETECT_DECREMENT_RESET  0x0
#define MPU6050_DETECT_DECREMENT_1      0x1
#define MPU6050_DETECT_DECREMENT_2      0x2
#define MPU6050_DETECT_DECREMENT_4      0x3

#define MPU6050_USERCTRL_DMP_EN_BIT             7
#define MPU6050_USERCTRL_FIFO_EN_BIT            6
#define MPU6050_USERCTRL_I2C_MST_EN_BIT         5
#define MPU6050_USERCTRL_I2C_IF_DIS_BIT         4
#define MPU6050_USERCTRL_DMP_RESET_BIT          3
#define MPU6050_USERCTRL_FIFO_RESET_BIT         2
#define MPU6050_USERCTRL_I2C_MST_RESET_BIT      1
#define MPU6050_USERCTRL_SIG_COND_RESET_BIT     0

#define MPU6050_PWR1_DEVICE_RESET_BIT   7
#define MPU6050_PWR1_SLEEP_BIT          6
#define MPU6050_PWR1_CYCLE_BIT          5
#define MPU6050_PWR1_TEMP_DIS_BIT       3
#define MPU6050_PWR1_CLKSEL_BIT         2
#define MPU6050_PWR1_CLKSEL_LENGTH      3

#define MPU6050_CLOCK_INTERNAL          0x00
#define MPU6050_CLOCK_PLL_XGYRO         0x01
#define MPU6050_CLOCK_PLL_YGYRO         0x02
#define MPU6050_CLOCK_PLL_ZGYRO         0x03
#define MPU6050_CLOCK_PLL_EXT32K        0x04
#define MPU6050_CLOCK_PLL_EXT19M        0x05
#define MPU6050_CLOCK_KEEP_RESET        0x07

#define MPU6050_PWR2_LP_WAKE_CTRL_BIT       7
#define MPU6050_PWR2_LP_WAKE_CTRL_LENGTH    2
#define MPU6050_PWR2_STBY_XA_BIT            5
#define MPU6050_PWR2_STBY_YA_BIT            4
#define MPU6050_PWR2_STBY_ZA_BIT            3
#define MPU6050_PWR2_STBY_XG_BIT            2
#define MPU6050_PWR2_STBY_YG_BIT            1
#define MPU6050_PWR2_STBY_ZG_BIT            0

#define MPU6050_WAKE_FREQ_1P25      0x0
#define MPU6050_WAKE_FREQ_2P5       0x1
#define MPU6050_WAKE_FREQ_5         0x2
#define MPU6050_WAKE_FREQ_10        0x3

#define MPU6050_BANKSEL_PRFTCH_EN_BIT       6
#define MPU6050_BANKSEL_CFG_USER_BANK_BIT   5
#define MPU6050_BANKSEL_MEM_SEL_BIT         4
#define MPU6050_BANKSEL_MEM_SEL_LENGTH      5

#define MPU6050_WHO_AM_I_BIT        6
#define MPU6050_WHO_AM_I_LENGTH     6

#define MPU6050_DMP_MEMORY_BANKS        8
#define MPU6050_DMP_MEMORY_BANK_SIZE    256
#define MPU6050_DMP_MEMORY_CHUNK_SIZE   16

// Helper functions
int8 memcmpOwn(const void *Ptr1, const void *Ptr2, size_t Count);

// note: DMP code memory blocks defined at end of header file
void MPU6050_initialize(uint8 address);
uint8 MPU6050_testConnection();

// AUX_VDDIO register
uint8 MPU6050_getAuxVDDIOLevel();
void MPU6050_setAuxVDDIOLevel(uint8 level);

// SMPLRT_DIV register
uint8 MPU6050_getRate();
void MPU6050_setRate(uint8 rate);

// CONFIG register
uint8 MPU6050_getExternalFrameSync();
void MPU6050_setExternalFrameSync(uint8 sync);
uint8 MPU6050_getDLPFMode();
void MPU6050_setDLPFMode(uint8 bandwidth);

// GYRO_CONFIG register
uint8 MPU6050_getFullScaleGyroRange();
void MPU6050_setFullScaleGyroRange(uint8 range);

// ACCEL_CONFIG register
uint8 MPU6050_getAccelXSelfTest();
void MPU6050_setAccelXSelfTest(bool enabled);
uint8 MPU6050_getAccelYSelfTest();
void MPU6050_setAccelYSelfTest(bool enabled);
uint8 MPU6050_getAccelZSelfTest();
void MPU6050_setAccelZSelfTest(bool enabled);
uint8 MPU6050_getFullScaleAccelRange();
void MPU6050_setFullScaleAccelRange(uint8 range);
uint8 MPU6050_getDHPFMode();
void MPU6050_setDHPFMode(uint8 mode);

// FF_THR register
uint8 MPU6050_getFreefallDetectionThreshold();
void MPU6050_setFreefallDetectionThreshold(uint8 threshold);

// FF_DUR register
uint8 MPU6050_getFreefallDetectionDuration();
void MPU6050_setFreefallDetectionDuration(uint8 duration);

// MOT_THR register
uint8 MPU6050_getMotionDetectionThreshold();
void MPU6050_setMotionDetectionThreshold(uint8 threshold);

// MOT_DUR register
uint8 MPU6050_getMotionDetectionDuration();
void MPU6050_setMotionDetectionDuration(uint8 duration);

// ZRMOT_THR register
uint8 MPU6050_getZeroMotionDetectionThreshold();
void MPU6050_setZeroMotionDetectionThreshold(uint8 threshold);

// ZRMOT_DUR register
uint8 MPU6050_getZeroMotionDetectionDuration();
void MPU6050_setZeroMotionDetectionDuration(uint8 duration);

// FIFO_EN register
uint8 MPU6050_getTempFIFOEnabled();
void MPU6050_setTempFIFOEnabled(bool enabled);
uint8 MPU6050_getXGyroFIFOEnabled();
void MPU6050_setXGyroFIFOEnabled(bool enabled);
uint8 MPU6050_getYGyroFIFOEnabled();
void MPU6050_setYGyroFIFOEnabled(bool enabled);
uint8 MPU6050_getZGyroFIFOEnabled();
void MPU6050_setZGyroFIFOEnabled(bool enabled);
uint8 MPU6050_getAccelFIFOEnabled();
void MPU6050_setAccelFIFOEnabled(bool enabled);
uint8 MPU6050_getSlave2FIFOEnabled();
void MPU6050_setSlave2FIFOEnabled(bool enabled);
uint8 MPU6050_getSlave1FIFOEnabled();
void MPU6050_setSlave1FIFOEnabled(bool enabled);
uint8 MPU6050_getSlave0FIFOEnabled();
void MPU6050_setSlave0FIFOEnabled(bool enabled);

// I2C_MST_CTRL register
uint8 MPU6050_getMultiMasterEnabled();
void MPU6050_setMultiMasterEnabled(bool enabled);
uint8 MPU6050_getWaitForExternalSensorEnabled();
void MPU6050_setWaitForExternalSensorEnabled(bool enabled);
uint8 MPU6050_getSlave3FIFOEnabled();
void MPU6050_setSlave3FIFOEnabled(bool enabled);
uint8 MPU6050_getSlaveReadWriteTransitionEnabled();
void MPU6050_setSlaveReadWriteTransitionEnabled(bool enabled);
uint8 MPU6050_getMasterClockSpeed();
void MPU6050_setMasterClockSpeed(uint8 speed);

// I2C_SLV* registers (Slave 0-3)
uint8 MPU6050_getSlaveAddress(uint8 num);
void MPU6050_setSlaveAddress(uint8 num, uint8 address);
uint8 MPU6050_getSlaveRegister(uint8 num);
void MPU6050_setSlaveRegister(uint8 num, uint8 reg);
uint8 MPU6050_getSlaveEnabled(uint8 num);
void MPU6050_setSlaveEnabled(uint8 num, bool enabled);
uint8 MPU6050_getSlaveWordByteSwap(uint8 num);
void MPU6050_setSlaveWordByteSwap(uint8 num, bool enabled);
uint8 MPU6050_getSlaveWriteMode(uint8 num);
void MPU6050_setSlaveWriteMode(uint8 num, bool mode);
uint8 MPU6050_getSlaveWordGroupOffset(uint8 num);
void MPU6050_setSlaveWordGroupOffset(uint8 num, bool enabled);
uint8 MPU6050_getSlaveDataLength(uint8 num);
void MPU6050_setSlaveDataLength(uint8 num, uint8 length);

// I2C_SLV* registers (Slave 4)
uint8 MPU6050_getSlave4Address();
void MPU6050_setSlave4Address(uint8 address);
uint8 MPU6050_getSlave4Register();
void MPU6050_setSlave4Register(uint8 reg);
void MPU6050_setSlave4OutputByte(uint8 data);
uint8 MPU6050_getSlave4Enabled();
void MPU6050_setSlave4Enabled(bool enabled);
uint8 MPU6050_getSlave4InterruptEnabled();
void MPU6050_setSlave4InterruptEnabled(bool enabled);
uint8 MPU6050_getSlave4WriteMode();
void MPU6050_setSlave4WriteMode(bool mode);
uint8 MPU6050_getSlave4MasterDelay();
void MPU6050_setSlave4MasterDelay(uint8 delay);
uint8 MPU6050_getSlate4InputByte();

// I2C_MST_STATUS register
uint8 MPU6050_getPassthroughStatus();
uint8 MPU6050_getSlave4IsDone();
uint8 MPU6050_getLostArbitration();
uint8 MPU6050_getSlave4Nack();
uint8 MPU6050_getSlave3Nack();
uint8 MPU6050_getSlave2Nack();
uint8 MPU6050_getSlave1Nack();
uint8 MPU6050_getSlave0Nack();

// INT_PIN_CFG register
uint8 MPU6050_getInterruptMode();
void MPU6050_setInterruptMode(bool mode);
uint8 MPU6050_getInterruptDrive();
void MPU6050_setInterruptDrive(bool drive);
uint8 MPU6050_getInterruptLatch();
void MPU6050_setInterruptLatch(bool latch);
uint8 MPU6050_getInterruptLatchClear();
void MPU6050_setInterruptLatchClear(bool clear);
uint8 MPU6050_getFSyncInterruptLevel();
void MPU6050_setFSyncInterruptLevel(bool level);
uint8 MPU6050_getFSyncInterruptEnabled();
void MPU6050_setFSyncInterruptEnabled(bool enabled);
uint8 MPU6050_getI2CBypassEnabled();
void MPU6050_setI2CBypassEnabled(bool enabled);
uint8 MPU6050_getClockOutputEnabled();
void MPU6050_setClockOutputEnabled(bool enabled);

// INT_ENABLE register
uint8 MPU6050_getIntEnabled();
void MPU6050_setIntEnabled(uint8 enabled);
uint8 MPU6050_getIntFreefallEnabled();
void MPU6050_setIntFreefallEnabled(bool enabled);
uint8 MPU6050_getIntMotionEnabled();
void MPU6050_setIntMotionEnabled(bool enabled);
uint8 MPU6050_getIntZeroMotionEnabled();
void MPU6050_setIntZeroMotionEnabled(bool enabled);
uint8 MPU6050_getIntFIFOBufferOverflowEnabled();
void MPU6050_setIntFIFOBufferOverflowEnabled(bool enabled);
uint8 MPU6050_getIntI2CMasterEnabled();
void MPU6050_setIntI2CMasterEnabled(bool enabled);
uint8 MPU6050_getIntDataReadyEnabled();
void MPU6050_setIntDataReadyEnabled(bool enabled);

// INT_STATUS register
uint8 MPU6050_getIntStatus();
uint8 MPU6050_getIntFreefallStatus();
uint8 MPU6050_getIntMotionStatus();
uint8 MPU6050_getIntZeroMotionStatus();
uint8 MPU6050_getIntFIFOBufferOverflowStatus();
uint8 MPU6050_getIntI2CMasterStatus();
uint8 MPU6050_getIntDataReadyStatus();

// ACCEL_*OUT_* registers
void MPU6050_getMotion9(int16* ax, int16* ay, int16* az, int16* gx, int16* gy, int16* gz, int16* mx, int16* my, int16* mz);
void MPU6050_getMotion6(int16* ax, int16* ay, int16* az, int16* gx, int16* gy, int16* gz);
void MPU6050_getAcceleration(int16* x, int16* y, int16* z);
int16 MPU6050_getAccelerationX();
int16 MPU6050_getAccelerationY();
int16 MPU6050_getAccelerationZ();

// TEMP_OUT_* registers
int16 MPU6050_getTemperature();

// GYRO_*OUT_* registers
void MPU6050_getRotation(int16* x, int16* y, int16* z);
int16 MPU6050_getRotationX();
int16 MPU6050_getRotationY();
int16 MPU6050_getRotationZ();

// EXT_SENS_DATA_* registers
uint8 MPU6050_getExternalSensorByte(int position);
uint16 MPU6050_getExternalSensorWord(int position);
uint32 MPU6050_getExternalSensorDWord(int position);

// MOT_DETECT_STATUS register
uint8 MPU6050_getMotionStatus();
uint8 MPU6050_getXNegMotionDetected();
uint8 MPU6050_getXPosMotionDetected();
uint8 MPU6050_getYNegMotionDetected();
uint8 MPU6050_getYPosMotionDetected();
uint8 MPU6050_getZNegMotionDetected();
uint8 MPU6050_getZPosMotionDetected();
uint8 MPU6050_getZeroMotionDetected();

// I2C_SLV*_DO register
void MPU6050_setSlaveOutputByte(uint8 num, uint8 data);

// I2C_MST_DELAY_CTRL register
uint8 MPU6050_getExternalShadowDelayEnabled();
void MPU6050_setExternalShadowDelayEnabled(bool enabled);
uint8 MPU6050_getSlaveDelayEnabled(uint8 num);
void MPU6050_setSlaveDelayEnabled(uint8 num, bool enabled);

// SIGNAL_PATH_RESET register
void MPU6050_resetGyroscopePath();
void MPU6050_resetAccelerometerPath();
void MPU6050_resetTemperaturePath();

// MOT_DETECT_CTRL register
uint8 MPU6050_getAccelerometerPowerOnDelay();
void MPU6050_setAccelerometerPowerOnDelay(uint8 delay);
uint8 MPU6050_getFreefallDetectionCounterDecrement();
void MPU6050_setFreefallDetectionCounterDecrement(uint8 decrement);
uint8 MPU6050_getMotionDetectionCounterDecrement();
void MPU6050_setMotionDetectionCounterDecrement(uint8 decrement);

// USER_CTRL register
uint8 MPU6050_getFIFOEnabled();
void MPU6050_setFIFOEnabled(bool enabled);
uint8 MPU6050_getI2CMasterModeEnabled();
void MPU6050_setI2CMasterModeEnabled(bool enabled);
void MPU6050_switchSPIEnabled(bool enabled);
void MPU6050_resetFIFO();
void MPU6050_resetI2CMaster();
void MPU6050_resetSensors();

// PWR_MGMT_1 register
void MPU6050_reset();
uint8 MPU6050_getSleepEnabled();
void MPU6050_setSleepEnabled(bool enabled);
uint8 MPU6050_getWakeCycleEnabled();
void MPU6050_setWakeCycleEnabled(bool enabled);
uint8 MPU6050_getTempSensorEnabled();
void MPU6050_setTempSensorEnabled(bool enabled);
uint8 MPU6050_getClockSource();
void MPU6050_setClockSource(uint8 source);

// PWR_MGMT_2 register
uint8 MPU6050_getWakeFrequency();
void MPU6050_setWakeFrequency(uint8 frequency);
uint8 MPU6050_getStandbyXAccelEnabled();
void MPU6050_setStandbyXAccelEnabled(bool enabled);
uint8 MPU6050_getStandbyYAccelEnabled();
void MPU6050_setStandbyYAccelEnabled(bool enabled);
uint8 MPU6050_getStandbyZAccelEnabled();
void MPU6050_setStandbyZAccelEnabled(bool enabled);
uint8 MPU6050_getStandbyXGyroEnabled();
void MPU6050_setStandbyXGyroEnabled(bool enabled);
uint8 MPU6050_getStandbyYGyroEnabled();
void MPU6050_setStandbyYGyroEnabled(bool enabled);
uint8 MPU6050_getStandbyZGyroEnabled();
void MPU6050_setStandbyZGyroEnabled(bool enabled);

// FIFO_COUNT_* registers
uint16 MPU6050_getFIFOCount();

// FIFO_R_W register
uint8 MPU6050_getFIFOByte();
void MPU6050_setFIFOByte(uint8 data);
void MPU6050_getFIFOBytes(uint8 *data, uint8 length);

// WHO_AM_I register
uint8 MPU6050_getDeviceID();
void MPU6050_setDeviceID(uint8 id);

// ======== UNDOCUMENTED/DMP REGISTERS/METHODS ========

// XG_OFFS_TC register
uint8 MPU6050_getOTPBankValid();
void MPU6050_setOTPBankValid(bool enabled);
int8 MPU6050_getXGyroOffsetTC();
void MPU6050_setXGyroOffsetTC(int8 offset);

// YG_OFFS_TC register
int8 MPU6050_getYGyroOffsetTC();
void MPU6050_setYGyroOffsetTC(int8 offset);

// ZG_OFFS_TC register
int8 MPU6050_getZGyroOffsetTC();
void MPU6050_setZGyroOffsetTC(int8 offset);

// X_FINE_GAIN register
int8 MPU6050_getXFineGain();
void MPU6050_setXFineGain(int8 gain);

// Y_FINE_GAIN register
int8 MPU6050_getYFineGain();
void MPU6050_setYFineGain(int8 gain);

// Z_FINE_GAIN register
int8 MPU6050_getZFineGain();
void MPU6050_setZFineGain(int8 gain);

// XA_OFFS_* registers
int16 MPU6050_getXAccelOffset();
void MPU6050_setXAccelOffset(int16 offset);

// YA_OFFS_* register
int16 MPU6050_getYAccelOffset();
void MPU6050_setYAccelOffset(int16 offset);

// ZA_OFFS_* register
int16 MPU6050_getZAccelOffset();
void MPU6050_setZAccelOffset(int16 offset);

// XG_OFFS_USR* registers
int16 MPU6050_getXGyroOffset();
void MPU6050_setXGyroOffset(int16 offset);

// YG_OFFS_USR* register
int16 MPU6050_getYGyroOffset();
void MPU6050_setYGyroOffset(int16 offset);

// ZG_OFFS_USR* register
int16 MPU6050_getZGyroOffset();
void MPU6050_setZGyroOffset(int16 offset);

// INT_ENABLE register (DMP functions)
uint8 MPU6050_getIntPLLReadyEnabled();
void MPU6050_setIntPLLReadyEnabled(bool enabled);
uint8 MPU6050_getIntDMPEnabled();
void MPU6050_setIntDMPEnabled(bool enabled);

// DMP_INT_STATUS
uint8 MPU6050_getDMPInt5Status();
uint8 MPU6050_getDMPInt4Status();
uint8 MPU6050_getDMPInt3Status();
uint8 MPU6050_getDMPInt2Status();
uint8 MPU6050_getDMPInt1Status();
uint8 MPU6050_getDMPInt0Status();

// INT_STATUS register (DMP functions)
uint8 MPU6050_getIntPLLReadyStatus();
uint8 MPU6050_getIntDMPStatus();

// USER_CTRL register (DMP functions)
uint8 MPU6050_getDMPEnabled();
void MPU6050_setDMPEnabled(bool enabled);
void MPU6050_resetDMP();

// BANK_SEL register
void MPU6050_setMemoryBank(uint8 bank, bool prefetchEnabled, bool userBank); // mandatory, false, false

// MEM_START_ADDR register
void MPU6050_setMemoryStartAddress(uint8 address);

// MEM_R_W register
uint8 MPU6050_readMemoryByte();
void MPU6050_writeMemoryByte(uint8 data);
void MPU6050_readMemoryBlock(uint8 *data, uint16 dataSize, uint8 bank, uint8 address); // mandatory, mandatory, 0, 0
uint8 MPU6050_writeMemoryBlock(const uint8 *data, uint16 dataSize, uint8 bank, uint8 address, bool verify, bool useProgMem); // Mandatory, mandatory, 0, 0, true, false
uint8 writeProgMemoryBlock(const uint8 *data, uint16 dataSize, uint8 bank, uint8 address, bool verify); // Mandatory, mandatory, 0, 0, true

uint8 MPU6050_writeDMPConfigurationSet(const uint8 *data, uint16 dataSize, bool useProgMem); // Mandatory, mandatory, false
uint8 MPU6050_writeProgDMPConfigurationSet(const uint8 *data, uint16 dataSize);

// DMP_CFG_1 register
uint8 MPU6050_getDMPConfig1();
void MPU6050_setDMPConfig1(uint8 config);

// DMP_CFG_2 register
uint8 MPU6050_getDMPConfig2();
void MPU6050_setDMPConfig2(uint8 config);

// special methods for MotionApps 2.0 implementation
#ifdef MPU6050_INCLUDE_DMP_MOTIONAPPS20
#include "helper_3dmath.h"

uint8 *MPU6050_dmpPacketBuffer;
uint16 MPU6050_dmpPacketSize;

uint8 MPU6050_dmpInitialize();
bool MPU6050_dmpPacketAvailable();

uint8 MPU6050_dmpSetFIFORate(uint8 fifoRate);
uint8 MPU6050_dmpGetFIFORate();
uint8 MPU6050_dmpGetSampleStepSizeMS();
uint8 MPU6050_dmpGetSampleFrequency();
int32 MPU6050_dmpDecodeTemperature(int8 tempReg);

// Register callbacks after a packet of FIFO data is processed
//uint8 dmpRegisterFIFORateProcess(inv_obj_func func, int16 priority);
//uint8 dmpUnregisterFIFORateProcess(inv_obj_func func);
uint8 MPU6050_dmpRunFIFORateProcesses();

// Setup FIFO for various output
uint8 MPU6050_dmpSendQuaternion(uint16 accuracy);
uint8 MPU6050_dmpSendGyro(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendAccel(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendLinearAccel(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendLinearAccelInWorld(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendControlData(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendSensorData(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendExternalSensorData(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendGravity(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendPacketNumber(uint16 accuracy);
uint8 MPU6050_dmpSendQuantizedAccel(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendEIS(uint16 elements, uint16 accuracy);

// Get Fixed Point data from FIFO
uint8 MPU6050_dmpGetAccel32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetAccel16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetAccelVec(VectorInt16 *v, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetQuaternion32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetQuaternion16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetQuaternionQuat(Quaternion *q, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGet6AxisQuaternion32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGet6AxisQuaternion16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGet6AxisQuaternionQuat(Quaternion *q, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetRelativeQuaternion32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetRelativeQuaternion16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetRelativeQuaternionQuat(Quaternion *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGyro32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGyro16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGyroVec(VectorInt16 *v, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpSetLinearAccelFilterCoefficient(float coef);
uint8 MPU6050_dmpGetLinearAccel32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetLinearAccel16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetLinearAccelVec(VectorInt16 *v, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetLinearAccelVecVec(VectorInt16 *v, VectorInt16 *vRaw, VectorFloat *gravity);
uint8 MPU6050_dmpGetLinearAccelInWorld32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetLinearAccelInWorld16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetLinearAccelInWorldVec(VectorInt16 *v, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetLinearAccelInWorldVecQuat(VectorInt16 *v, VectorInt16 *vReal, Quaternion *q);
uint8 MPU6050_dmpGetGyroAndAccelSensor32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGyroAndAccelSensor16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGyroAndAccelSensorVec(VectorInt16 *g, VectorInt16 *a, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGyroSensor32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGyroSensor16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGyroSensorVec(VectorInt16 *v, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetControlData(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetTemperature(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGravity32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGravity16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGravityVec(VectorInt16 *v, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetGravityVecQuat(VectorFloat *v, Quaternion *q);
uint8 MPU6050_dmpGetUnquantizedAccel32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetUnquantizedAccel16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetUnquantizedAccelVec(VectorInt16 *v, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetQuantizedAccel32(int32 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetQuantizedAccel16(int16 *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetQuantizedAccelVec(VectorInt16 *v, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetExternalSensorData(int32 *data, uint16 size, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetEIS(int32 *data, const uint8* packet); // Default packet=0

uint8 MPU6050_dmpGetEuler(float *data, Quaternion *q);
uint8 MPU6050_dmpGetYawPitchRoll(float *data, Quaternion *q, VectorFloat *gravity);

// Get Floating Point data from FIFO
uint8 MPU6050_dmpGetAccelFloat(float *data, const uint8* packet); // Default packet=0
uint8 MPU6050_dmpGetQuaternionFloat(float *data, const uint8* packet); // Default packet=0

uint8 MPU6050_dmpProcessFIFOPacket(const unsigned char *dmpData);
uint8 MPU6050_dmpReadAndProcessFIFOPacket(uint8 numPackets, uint8 *processed); // Default processed = NULL

uint8 MPU6050_dmpSetFIFOProcessedCallback(void (*func) (void));

uint8 MPU6050_dmpInitFIFOParam();
uint8 MPU6050_dmpCloseFIFO();
uint8 MPU6050_dmpSetGyroDataSource(uint8 source);
uint8 MPU6050_dmpDecodeQuantizedAccel();
uint32 MPU6050_dmpGetGyroSumOfSquare();
uint32 MPU6050_dmpGetAccelSumOfSquare();
void MPU6050_dmpOverrideQuaternion(long *q);
uint16 MPU6050_dmpGetFIFOPacketSize();

uint8 MPU6050_writeProgMemoryBlock(const uint8 *data, uint16 dataSize,
		uint8 bank, uint8 address, bool verify);
#endif

// special methods for MotionApps 4.1 implementation
#ifdef MPU6050_INCLUDE_DMP_MOTIONAPPS41
uint8 *MPU6050_dmpPacketBuffer;
uint16 MPU6050_dmpPacketSize;

uint8 MPU6050_dmpInitialize();
bool MPU6050_dmpPacketAvailable();

uint8 MPU6050_dmpSetFIFORate(uint8 fifoRate);
uint8 MPU6050_dmpGetFIFORate();
uint8 MPU6050_dmpGetSampleStepSizeMS();
uint8 MPU6050_dmpGetSampleFrequency();
int32 MPU6050_dmpDecodeTemperature(int8 tempReg);

// Register callbacks after a packet of FIFO data is processed
//uint8 dmpRegisterFIFORateProcess(inv_obj_func func, int16 priority);
//uint8 dmpUnregisterFIFORateProcess(inv_obj_func func);
uint8 MPU6050_dmpRunFIFORateProcesses();

// Setup FIFO for various output
uint8 MPU6050_dmpSendQuaternion(uint16 accuracy);
uint8 MPU6050_dmpSendGyro(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendAccel(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendLinearAccel(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendLinearAccelInWorld(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendControlData(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendSensorData(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendExternalSensorData(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendGravity(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendPacketNumber(uint16 accuracy);
uint8 MPU6050_dmpSendQuantizedAccel(uint16 elements, uint16 accuracy);
uint8 MPU6050_dmpSendEIS(uint16 elements, uint16 accuracy);

// Get Fixed Point data from FIFO
uint8 MPU6050_dmpGetAccel(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetAccel(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetAccel(VectorInt16 *v, const uint8* packet=0);
uint8 MPU6050_dmpGetQuaternion(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetQuaternion(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetQuaternion(Quaternion *q, const uint8* packet=0);
uint8 MPU6050_dmpGet6AxisQuaternion(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGet6AxisQuaternion(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGet6AxisQuaternion(Quaternion *q, const uint8* packet=0);
uint8 MPU6050_dmpGetRelativeQuaternion(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetRelativeQuaternion(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetRelativeQuaternion(Quaternion *data, const uint8* packet=0);
uint8 MPU6050_dmpGetGyro(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetGyro(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetGyro(VectorInt16 *v, const uint8* packet=0);
uint8 MPU6050_dmpGetMag(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpSetLinearAccelFilterCoefficient(float coef);
uint8 MPU6050_dmpGetLinearAccel(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetLinearAccel(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetLinearAccel(VectorInt16 *v, const uint8* packet=0);
uint8 MPU6050_dmpGetLinearAccel(VectorInt16 *v, VectorInt16 *vRaw, VectorFloat *gravity);
uint8 MPU6050_dmpGetLinearAccelInWorld(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetLinearAccelInWorld(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetLinearAccelInWorld(VectorInt16 *v, const uint8* packet=0);
uint8 MPU6050_dmpGetLinearAccelInWorld(VectorInt16 *v, VectorInt16 *vReal, Quaternion *q);
uint8 MPU6050_dmpGetGyroAndAccelSensor(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetGyroAndAccelSensor(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetGyroAndAccelSensor(VectorInt16 *g, VectorInt16 *a, const uint8* packet=0);
uint8 MPU6050_dmpGetGyroSensor(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetGyroSensor(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetGyroSensor(VectorInt16 *v, const uint8* packet=0);
uint8 MPU6050_dmpGetControlData(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetTemperature(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetGravity(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetGravity(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetGravity(VectorInt16 *v, const uint8* packet=0);
uint8 MPU6050_dmpGetGravity(VectorFloat *v, Quaternion *q);
uint8 MPU6050_dmpGetUnquantizedAccel(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetUnquantizedAccel(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetUnquantizedAccel(VectorInt16 *v, const uint8* packet=0);
uint8 MPU6050_dmpGetQuantizedAccel(int32 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetQuantizedAccel(int16 *data, const uint8* packet=0);
uint8 MPU6050_dmpGetQuantizedAccel(VectorInt16 *v, const uint8* packet=0);
uint8 MPU6050_dmpGetExternalSensorData(int32 *data, uint16 size, const uint8* packet=0);
uint8 MPU6050_dmpGetEIS(int32 *data, const uint8* packet=0);

uint8 MPU6050_dmpGetEuler(float *data, Quaternion *q);
uint8 MPU6050_dmpGetYawPitchRoll(float *data, Quaternion *q, VectorFloat *gravity);

// Get Floating Point data from FIFO
uint8 MPU6050_dmpGetAccelFloat(float *data, const uint8* packet=0);
uint8 MPU6050_dmpGetQuaternionFloat(float *data, const uint8* packet=0);

uint8 MPU6050_dmpProcessFIFOPacket(const unsigned char *dmpData);
uint8 MPU6050_dmpReadAndProcessFIFOPacket(uint8 numPackets, uint8 *processed=NULL);

uint8 MPU6050_dmpSetFIFOProcessedCallback(void (*func) (void));

uint8 MPU6050_dmpInitFIFOParam();
uint8 MPU6050_dmpCloseFIFO();
uint8 MPU6050_dmpSetGyroDataSource(uint8 source);
uint8 MPU6050_dmpDecodeQuantizedAccel();
uint32 MPU6050_dmpGetGyroSumOfSquare();
uint32 MPU6050_dmpGetAccelSumOfSquare();
void MPU6050_dmpOverrideQuaternion(long *q);
uint16 MPU6050_dmpGetFIFOPacketSize();
#endif

#endif /* _MPU6050_H_ */
