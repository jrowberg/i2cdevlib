// LM73 I2C class
// Based on Texas Instruments datasheet http://www.ti.com/lit/ds/symlink/lm73.pdf
// Note that this is a bare-bones driver and does not support all of the features
// of the LM73. I only added what I needed.
// 12/03/2012 Abe Erdos (abe@erdosmiller.com)

#include "LM73.h"

LM73::LM73() {
    devAddr = LM73_DEFAULT_ADDRESS;
    devConfig.all = 0x40; // reset state
    devCtrlStat.all = 0x08; // reset state
}

LM73::LM73(uint8_t address) {
    devAddr = address;
    devConfig.all = 0x40; // reset state
    devCtrlStat.all = 0x08; // reset state
}

void LM73::initialize() {
    // there's nothing to do, really
}

bool LM73::testConnection() {
    uint16_t buf;
    I2Cdev::readWord(devAddr, LM73_RA_ID, &buf);
    return buf == 0x0190;
}

LM73ConfigReg LM73::getConfig() {
    I2Cdev::readByte(devAddr, LM73_RA_CONFIG, buffer);
    devConfig.all = buffer[0];
    return devConfig;
}

LM73CtrlStatReg LM73::getCtrlStat() {
    I2Cdev::readByte(devAddr, LM73_RA_CTRL_STAT, buffer);
    devCtrlStat.all = buffer[0];
    return devCtrlStat;
}

void LM73::setCtrlStat(LM73CtrlStatReg value) {
    I2Cdev::writeByte(devAddr, LM73_RA_CTRL_STAT, value.all);
}

uint8_t LM73::getResolution() {
    LM73::getCtrlStat();
    return devCtrlStat.bit.RES + 11;
}

float LM73::getTemp() {
    uint16_t buf;
    int16_t temp;
    I2Cdev::readWord(devAddr, LM73_RA_TEMP, &buf);
    temp = buf;
    temp = (temp>>2);
    return 0.03125f * (float)temp;
}

void LM73::setResolution(uint8_t resolution) {
    if(10 < resolution && resolution < 15) {
        LM73::getCtrlStat();
        devCtrlStat.bit.RES = resolution - 11;
        LM73::setCtrlStat(devCtrlStat);
    }
}

