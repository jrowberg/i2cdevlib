// LM73 I2C class
// Based on Texas Instruments datasheet http://www.ti.com/lit/ds/symlink/lm73.pdf
// Note that this is a bare-bones driver and does not support all of the features
// of the LM73. I only added what I needed.
// 12/03/2012 Abe Erdos (abe@erdosmiller.com)

#ifndef _LM73_H_
#define _LM73_H_

#include "I2Cdev.h"

// LM73-0
#define LM73_0_ADDRESS_ADDR_FLT		0x48 // address pin is floating
#define LM73_0_ADDRESS_ADDR_GND		0x49 // address pin low (GND)
#define LM73_0_ADDRESS_ADDR_VDD		0x4A // address pin high (VDD)
#define LM73_1_ADDRESS_ADDR_FLT		0x4C // address pin if floating
#define LM73_1_ADDRESS_ADDR_GND		0x4D // address pin low (GND)
#define LM73_1_ADDRESS_ADDR_VDD		0x4E // address pin high (VDD)
#define LM73_DEFAULT_ADDRESS 		LM73_0_ADDRESS_ADDR_FLT

#define LM73_RA_TEMP			0x00
#define LM73_RA_CONFIG 			0x01
#define LM73_RA_HI_THRESH		0x02
#define LM73_RA_LO_THRESH		0x03
#define LM73_RA_CTRL_STAT		0x04
#define LM73_RA_ID			0x07

typedef struct {
    unsigned :2;          // reserved
    unsigned ONE_SHOT:1;  // one shot (write 1 to start a conversion when PD == 1)
    unsigned ALRT_RST:1;  // alert reset (write 1 to reset, always reads 0)
    unsigned ALRT_POL:1;  // alert polarity (1 == active high, 0 == active low0
    unsigned nALRT_EN:1;  // alert enable (0 == alert enabled)
    unsigned :1;          // reserved
    unsigned PD:1;        // power down bit (0 == enabled)
} LM73ConfigBits;

typedef union {
    LM73ConfigBits bit;
    uint8_t all;
} LM73ConfigReg;

typedef struct {
    unsigned DAV:1;       // data available flag
    unsigned TLOW:1;      // temperature low flag
    unsigned THI:1;       // temperature high flag
    unsigned ALRT_STAT:1; // alert pin status
    unsigned :1;          // reserved
    unsigned RES:2;       // resolution (00 == 11 bit res, 11 == 14 bit res)
    unsigned TO_DIS:1;    // SMB timeout disable
} LM73CtrlStatBits;

typedef union {
    LM73CtrlStatBits bit;
    uint8_t all;
} LM73CtrlStatReg;

class LM73 {
    public:
        LM73();
        LM73(uint8_t address);
		
        void initialize();
        bool testConnection();
        
        LM73ConfigReg getConfig();
        void setConfig(LM73ConfigReg value);
        LM73CtrlStatReg getCtrlStat();
        void setCtrlStat(LM73CtrlStatReg value);
        uint8_t getResolution(); // returns resolution in bits (including sign bit)
        void setResolution(uint8_t resolution); // enter resolution in bits (including sign bit)
        float getTemp(); // return temperature in C
		
    private:
        uint8_t devAddr;
        uint8_t buffer[1];
        LM73ConfigReg devConfig;
        LM73CtrlStatReg devCtrlStat;
};

#endif
