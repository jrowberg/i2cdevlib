/*
 * Arduino library for the MMA8452Q three-axis accelerometer.
 *
 * The MMA8452Q is a tiny and low-power three-axis accelerometer. This
 * library implements a simple interface to access most of the chip
 * functionality from an Arduino.
 *
 * FEATURES:
 *  + Normal and fast raw axes values reading
 *  + Scale configuration (allowed 2g, 4g and 8g scales)
 *  + Custom offset calibration
 *  + Low noise mode
 *  + Orientation detection (portrait/landscape and back/front)
 *  + Auto-WAKE/SLEEP mode and detection
 *
 * GETTING STARTED:
 *
 *  #include <MMA8452Q.h>
 *  #include <Wire.h> // so the Arduino IDE auto-detects the dependency
 *
 *  MMA8452Q accel;
 *
 *  int axes[3];
 *
 *  void setup() {
 *    Serial.begin(9600);
 *
 *    if (accel.begin())
 *      while (1); // error
 *  }
 *
 *  void loop() {
 *    // get and print raw axes values
 *    accel.axes(axes);
 *
 *    Serial.print("x: ");
 *    Serial.print(axes[0]);
 *    Serial.print(", y: ");
 *    Serial.print(axes[1]);
 *    Serial.print(", z: ");
 *    Serial.println(axes[2]);
 *
 *    delay(1000);
 *  }
 *
 * TODO:
 *  + Motion and freefall detection
 *  + Single and double tap detection
 *  + Interrupts configuration
 *
 * See MMA8452Q.h for the API documentation.
 *
 * Copyright (c) 2013, Alessandro Ghedini
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <Arduino.h>
#include <Wire.h>

#include "MMA8452Q.h"

#define MMA8452Q_ADDRESS 0x1D

enum MMA8452Q_REGISTERS {
	STATUS       = 0x00,

	OUT_X_MSB    = 0x01,
	OUT_X_LSB    = 0x02,

	OUT_Y_MSB    = 0x03,
	OUT_Y_LSB    = 0x04,

	OUT_Z_MSB    = 0x05,
	OUT_Z_LSB    = 0x06,

	SYSMOD       = 0x0B,
	INT_SOURCE   = 0x0C,
	WHO_AM_I     = 0x0D,
	XYZ_DATA_CFG = 0x0E,

	PL_STATUS    = 0x10,
	PL_CFG       = 0x11,
	PL_COUNT     = 0x12,

	CTRL_REG1    = 0x2A,
	CTRL_REG2    = 0x2B,
	CTRL_REG3    = 0x2C,
	CTRL_REG4    = 0x2D,
	CTRL_REG5    = 0x2E,

	OFF_X        = 0x2F,
	OFF_Y        = 0x30,
	OFF_Z        = 0x31
};

enum MMA8452Q_STATUS {
	ZYX_OW = 0b10000000,
	Z_OW   = 0b01000000,
	Y_OW   = 0b00100000,
	X_OW   = 0b00010000,
	ZYX_DR = 0b00001000,
	Z_DR   = 0b00000100,
	Y_DR   = 0b00000010,
	X_DR   = 0b00000001
};

enum MMA8452Q_CTRL_REG1 {
	ACTIVE = 0,
	F_READ = 1,
	LNOISE = 2,
	DR0    = 3,
	DR1    = 4,
	DR2    = 5
};

enum MMA8452Q_CTRL_REG2 {
	MODS0  = 0,
	MODS1  = 1,
	SLPE   = 2,
	SMODS0 = 3,
	SMODS1 = 4,
	RST    = 6,
	ST     = 7
};

enum MMA8452Q_CTRL_REG3 {
	PP_OD       = 0,
	IPOL        = 1,
	WAKE_FF_MT  = 3,
	WAKE_PULSE  = 4,
	WAKE_LNDPRT = 5,
	WAKE_TRANS  = 6
};

enum MMA8452Q_CTRL_REG4 {
	INT_EN_DRDY   = 0,
	INT_EN_FF_MT  = 2,
	INT_EN_PULSE  = 3,
	INT_EN_LNDPRT = 4,
	INT_EN_TRANS  = 5,
	INT_EN_ASLP   = 7
};

enum MMA8452Q_CTRL_REG5 {
	INT_CFG_DRDY   = 0,
	INT_CFG_FF_MT  = 2,
	INT_CFG_PULSE  = 3,
	INT_CFG_LNDPRT = 4,
	INT_CFG_TRANS  = 5,
	INT_CFG_ASLP   = 7
};

enum MMA8452Q_PL_STATUS {
	BAFRO = 0,
	LAPO0 = 1,
	LAPO1 = 2,
	LO    = 6,
	NEWLP = 7
};

enum MMA8452Q_PL_CFG {
	PL_EN  = 6,
	DBCNTM = 7
};

static inline uint8_t registerRead(uint8_t addr) {
	Wire.beginTransmission(MMA8452Q_ADDRESS);
	Wire.write(addr);
	Wire.endTransmission(false);

	Wire.requestFrom(MMA8452Q_ADDRESS, 1);

	while (!Wire.available());
	return Wire.read();
}

static inline void registersRead(uint8_t addr, uint8_t data[], size_t count) {
	Wire.beginTransmission(MMA8452Q_ADDRESS);
	Wire.write(addr);
	Wire.endTransmission(false);

	Wire.requestFrom(MMA8452Q_ADDRESS, count);

	while (Wire.available() < count);

	for (size_t i = 0; i < count; i++)
		data[i] = Wire.read();
}

static inline void registerWrite(uint8_t addr, uint8_t value) {
	Wire.beginTransmission(MMA8452Q_ADDRESS);
	Wire.write(addr);
	Wire.write(value);
	Wire.endTransmission();
}

static inline void registersWrite(uint8_t addr, uint8_t data[], size_t count) {
	Wire.beginTransmission(MMA8452Q_ADDRESS);
	Wire.write(addr);

	for (int i = 0; i < count; i++)
		Wire.write(data[i]);

	Wire.endTransmission();
}

static inline void registerSetBit(uint8_t addr, uint8_t bit, bool value) {
	uint8_t val = registerRead(addr);
	bitWrite(val, bit, value);
	registerWrite(addr, val);
}

MMA8452Q::MMA8452Q() {
}

int MMA8452Q::begin(void) {
	uint8_t whoami;

	Wire.begin();

	whoami = registerRead(WHO_AM_I);

	if (whoami != 0x2A)
		return -1;

	this -> active(true);

	return 0;
}

uint8_t MMA8452Q::status(void) {
	return registerRead(STATUS);
}

uint8_t MMA8452Q::sysmod(void) {
	return registerRead(SYSMOD);
}

uint8_t MMA8452Q::intSource(void) {
	return registerRead(INT_SOURCE);
}

void MMA8452Q::scale(uint8_t scale) {
	uint8_t value = registerRead(XYZ_DATA_CFG);

	switch (scale) {
		case 2: bitWrite(value, 0, 0); bitWrite(value, 1, 0); break;
		case 4: bitWrite(value, 0, 1); bitWrite(value, 1, 0); break;
		case 8: bitWrite(value, 0, 0); bitWrite(value, 1, 1); break;
	}
}

void MMA8452Q::offset(int8_t off_x, int8_t off_y, int8_t off_z) {
	registerWrite(OFF_X, off_x);
	registerWrite(OFF_Y, off_y);
	registerWrite(OFF_Z, off_z);
}

void MMA8452Q::active(bool enable) {
	registerSetBit(CTRL_REG1, ACTIVE, enable);
}

void MMA8452Q::fastRead(bool enable) {
	registerSetBit(CTRL_REG1, F_READ, enable);
}

void MMA8452Q::lowNoise(bool enable) {
	registerSetBit(CTRL_REG1, LNOISE, enable);
}

void MMA8452Q::reset(void) {
	registerSetBit(CTRL_REG2, RST, 1);
}

void MMA8452Q::selfTest(bool enable) {
	registerSetBit(CTRL_REG2, ST, enable);
}

void MMA8452Q::autoSleep(bool enable) {
	registerSetBit(CTRL_REG2, SLPE, enable);
}

void MMA8452Q::detectOrientation(bool enable) {
	registerSetBit(PL_CFG, PL_EN, enable);
}

void MMA8452Q::wakeOn(bool enable, uint8_t events) {
	if (events & FREEFALL_MOTION)
		registerSetBit(CTRL_REG3, WAKE_FF_MT, enable);

	if (events & PULSE)
		registerSetBit(CTRL_REG3, WAKE_PULSE, enable);

	if (events & ORIENTATION)
		registerSetBit(CTRL_REG3, WAKE_LNDPRT, enable);
}

/* void MMA8452Q::intDataRdy(bool enable, uint8_t pin) { */
/* 	registerSetBit(CTRL_REG4, INT_EN_DRDY, enable); */
/* 	registerSetBit(CTRL_REG5, INT_CFG_DRDY, enable); */
/* } */

/* void MMA8452Q::intFreefallMotion(bool enable, uint8_t pin) { */
/* 	registerSetBit(CTRL_REG4, INT_EN_FF_MT, enable); */
/* 	registerSetBit(CTRL_REG5, INT_CFG_FF_MT, enable); */
/* } */

/* void MMA8452Q::intPulse(bool enable, uint8_t pin) { */
/* 	registerSetBit(CTRL_REG4, INT_EN_PULSE, enable); */
/* 	registerSetBit(CTRL_REG5, INT_CFG_PULSE, enable); */
/* } */

/* void MMA8452Q::intOrientation(bool enable, uint8_t pin) { */
/* 	registerSetBit(CTRL_REG4, INT_EN_LNDPRT, enable); */
/* 	registerSetBit(CTRL_REG5, INT_CFG_LNDPRT, enable); */
/* } */

/* void MMA8452Q::intAutoSlp(bool enable, uint8_t pin) { */
/* 	registerSetBit(CTRL_REG4, INT_EN_ASLP, enable); */
/* 	registerSetBit(CTRL_REG5, INT_CFG_ASLP, enable); */
/* } */

void MMA8452Q::axes(int axes[]) {
	uint8_t *data;
	uint8_t read_count = 0;
	uint8_t val = registerRead(CTRL_REG1);

	if (bitRead(val, F_READ) == 0)
		read_count = 6;
	else
		read_count = 3;

	data = new uint8_t[read_count];

	registersRead(OUT_X_MSB, data, read_count);

	for (int i = 0; i < 3; i++) {
		axes[i]  = data[i * (read_count / 3)] << 8;

		if (bitRead(val, F_READ) == 0)
			axes[i] |= data[(i * 2) + 1];

		axes[i] >>= 4;
	}

	delete[] data;
}

bool MMA8452Q::orientation(uint8_t *value) {
	*value = registerRead(PL_STATUS);
	return bitRead(*value, NEWLP);
}

int MMA8452Q::portrait(uint8_t orient) {
	if ((bitRead(orient, LAPO1) == 0) && (bitRead(orient, LAPO0) == 0))
		return HIGH;
	else if ((bitRead(orient, LAPO1) == 0) && (bitRead(orient, LAPO0) == 1))
		return LOW;
	else
		return -1;
}

int MMA8452Q::landscape(uint8_t orient) {
	if ((bitRead(orient, LAPO1) == 1) && (bitRead(orient, LAPO0) == 0))
		return HIGH;
	else if ((bitRead(orient, LAPO1) == 1) && (bitRead(orient, LAPO0) == 1))
		return LOW;
	else
		return -1;
}

int MMA8452Q::backFront(uint8_t orient) {
	return bitRead(orient, BAFRO);
}
