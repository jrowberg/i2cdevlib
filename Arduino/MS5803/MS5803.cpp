// I2Cdev library collection - MS5803 I2C device class
// Based on Measurement Specialties MS5803 document, 3/25/2013 (DA5803-01BA_010)
// 3/29/2016 by Ryan Neve <Ryan@PlanktosInstruments.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     ... - ongoing debug release

// NOTE: THIS IS ONLY A PARIAL RELEASE. THIS DEVICE CLASS IS CURRENTLY UNDERGOING ACTIVE
// DEVELOPMENT AND IS STILL MISSING SOME IMPORTANT FEATURES. PLEASE KEEP THIS IN MIND IF
// YOU DECIDE TO USE THIS PARTICULAR CODE FOR ANYTHING.

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2014 Ryan Neve

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
#include "MS5803_I2C.h"

const static uint8_t INIT_TRIES = 3;
const static uint16_t PRESS_ATM_MBAR_DEFAULT = 1015;

	
const char* CALIBRATION_CONSTANTS[] = {
	"_c1_SENSt1",
	"_c2_OFFt1",
	"_c3_TCS",
	"_c4_TCO",
	"_c5_Tref",
	"_c6_TEMPSENS"
};

/** Default constructor, uses default I2C address.
* @see MPU6050_DEFAULT_ADDRESS
*/
MS5803::MS5803() { MS5803(MS5803_DEFAULT_ADDRESS);}

/** Specific address constructor.
* @param address I2C address
* @see MS5803_DEFAULT_ADDRESS
* @see MS5803_ADDRESS_AD0_LOW
* @see MS5803_ADDRESS_AD0_HIGH
*/
MS5803::MS5803(uint8_t address) {
	setAddress(address);
	_initialized = false;
	_c1_SENSt1		= 0;
	_c2_OFFt1		= 0;
	_c3_TCS			= 0;
	_c4_TCO			= 0;
	_c5_Tref		= 0;
	_c6_TEMPSENS	= 0;
	_press_atm_mBar = (float)PRESS_ATM_MBAR_DEFAULT/1000.0; //default, can be changed with setAtmospheric() 
}
// Because sometimes you want to set the address later.
void MS5803::setAddress(uint8_t address) {
	_dev_address = address;
}

/** Power on and prepare for general usage.
* This will reset the device to make sure that the calibration PROM gets loaded into
* the internal register. It will then read the calibration constants from the PROM
*/
bool MS5803::initialize(uint8_t model) {
	_initialized = false;
	// Make sure model is valid.
	switch (model) {
		case ( 1): _model = BA01; break;
		case ( 2): _model = BA02; break;
		case ( 5): _model = BA05; break;
		case (14): _model = BA14; break;
		case (30): _model = BA30; break;
		default:
			Serial.print(F("MS5803 Model entered (")); Serial.print(model); Serial.println(F(") is not valid/supported."));
			_model = INVALID; 
			return 0;
	}
	if (_debug) Serial.println(reset());
	uint8_t tries = 0;
	do {
		_getCalConstants(); // Seems to partially fail the first try sometimes.
		if (_c1_SENSt1 && _c2_OFFt1 && _c3_TCS && _c4_TCO && _c5_Tref && _c6_TEMPSENS ) _initialized = true; // They must all be non-0
		tries++;
	} while (!_initialized && ( tries < INIT_TRIES) );
	if (_debug) {
		for ( uint8_t i = 1 ; i <= 6; i++){
			Serial.print(i);
			Serial.print(": ");
			Serial.print(CALIBRATION_CONSTANTS[i-1]);
			Serial.print(" = ");
			Serial.println(_getCalConstant(i));
		}
	}
	return _initialized;
}

// See if we can communicate
bool MS5803::testConnection(){
	uint8_t reg_address = CMD_ADC_CONV + TEMPERATURE + ADC_256;
	return I2Cdev::writeBytes(_dev_address,reg_address,0,_buffer);
}

uint16_t MS5803::reset(){
	// Not sure how to send no buffer to an address.
	return I2Cdev::writeBytes(_dev_address, MS5803_RESET,0,_buffer);
}

void MS5803::_getCalConstants(){
	/* Query and parse calibration constants */
	I2Cdev::readBytes(_dev_address,MS5803_PROM_C1,2,_buffer);
	_c1_SENSt1 =   (((uint16_t)_buffer[0] << 8) + _buffer[1]);
	I2Cdev::readBytes(_dev_address,MS5803_PROM_C2,2,_buffer);
	_c2_OFFt1 =    (((uint16_t)_buffer[0] << 8) + _buffer[1]);
	I2Cdev::readBytes(_dev_address,MS5803_PROM_C3,2,_buffer);
	_c3_TCS =      (((uint16_t)_buffer[0] << 8) + _buffer[1]);
	I2Cdev::readBytes(_dev_address,MS5803_PROM_C4,2,_buffer);
	_c4_TCO =      (((uint16_t)_buffer[0] << 8) + _buffer[1]);
	I2Cdev::readBytes(_dev_address,MS5803_PROM_C5,2,_buffer);
	_c5_Tref =     (((uint16_t)_buffer[0] << 8) + _buffer[1]);
	I2Cdev::readBytes(_dev_address,MS5803_PROM_C6,2,_buffer);
	_c6_TEMPSENS = (((uint16_t)_buffer[0] << 8) + _buffer[1]);
}

int32_t MS5803::_getCalConstant(uint8_t constant_no){
	switch ( constant_no ) {
		case 1: return _c1_SENSt1;
		case 2: return _c2_OFFt1;
		case 3: return _c3_TCS;
		case 4: return _c4_TCO;
		case 5: return _c5_Tref;
		case 6: return _c6_TEMPSENS;
		default: return 0;
	}
}

/*	This function communicates with the sensor and does all the math to convert 
	raw values to good data. Data can be accessed with various getters.
*/
void MS5803::calcMeasurements(precision _precision){
	// Get raw temperature and pressure values
	_d2_temperature = _getADCconversion(TEMPERATURE, _precision);
	_d1_pressure = _getADCconversion(PRESSURE, _precision);
	//Now that we have a raw temperature, let's compute our actual.
	_dT = _d2_temperature - ((int32_t)_c5_Tref << 8);
	double temp_dT = _dT / (double)pow(2,23);
	_TEMP = 2000 +  (int32_t)(temp_dT * _c6_TEMPSENS);
	if ( _debug ) {
		Serial.println("Raw values:");
		Serial.print("    _d2_temperature = "); Serial.println(_d2_temperature);
		Serial.print("    _d1_pressure = "); Serial.println(_d1_pressure);
		Serial.println("First order values:");
		Serial.print("    _dT = "); Serial.println(_dT);
		Serial.print("    _TEMP = "); Serial.println(_TEMP);
	}
	// Second order variables
	int64_t T2 = 0;
	int64_t off2 = 0;
	int64_t sens2 = 0;
	// Every variant does the calculations differently, so...
	switch (_model) {
		case (BA01):  //MS5803-01-----------------------------------------------------------
			_OFF  = ((int64_t)_c2_OFFt1  << 16 ) + ((((int64_t)_c4_TCO * (int64_t)_dT)) >> 7 );
			_SENS = ((int64_t)_c1_SENSt1 << 15 ) + ((((int64_t)_c3_TCS * (int64_t)_dT)) >> 8 );
			// 2nd Order calculations
			if ( _TEMP < 2000.0) {  // Is temperature below or above 20.00 deg C ?
				T2 = 3 * pow(_dT,2)/(int64_t)pow(2,31);
				off2  = 3 * pow((_TEMP - 2000.0),2);
				sens2 = 7 * pow((_TEMP - 2000.0),2) / 8;
				if ( _TEMP < 1500.0 ) sens2 += 2 * pow((_TEMP + 1500.0),2);// below 15C
			}
			else {
				T2 = 0;
				off2 = 0;
				sens2 = 0;
				if ( _TEMP >= 4500.0 ) {
					sens2 -= ((int64_t)pow((_TEMP - 4500.0),2) >> 3);
				}
			}
			break;
		case (BA02):  //MS5803-02-----------------------------------------------------------
			_OFF  = ((int64_t)_c2_OFFt1  << 17 ) + ((((int64_t)_c4_TCO * (int64_t)_dT)) >> 6 );
			_SENS = ((int64_t)_c1_SENSt1 << 16 ) + ((((int64_t)_c3_TCS * (int64_t)_dT)) >> 7 );
			// 2nd Order calculations
			if ( _TEMP < 2000.0) {  // Is temperature below or above 20.00 deg C ?
				T2 = 3 * pow(_dT,2)/(int64_t)pow(2,31);
				off2 = 61 * pow((_TEMP - 2000.0),2) / 16;
				sens2 = 2 * pow(((int64_t)_TEMP - 2000.0),2);
				if ( _TEMP < 1500.0 ) { // below 15C
					off2  += 20 * pow((_TEMP + 1500.0),2);
					sens2 += 12 * pow((_TEMP + 1500.0),2);
				}
			}
			else {
				T2 = 0;
				off2 = 0;
				sens2 = 0;
			}
			break;
		case (BA05):  //MS5803-05-----------------------------------------------------------
			_OFF  = ((int64_t)_c2_OFFt1  << 18 ) + ((((int64_t)_c4_TCO * (int64_t)_dT)) >> 5 );
			_SENS = ((int64_t)_c1_SENSt1 << 17 ) + ((((int64_t)_c3_TCS * (int64_t)_dT)) >> 7 );
			// 2nd Order calculations
			if ( _TEMP < 2000.0) {  // Is temperature below or above 20.00 deg C ?
				T2 = 3 * pow(_dT,2)/(int64_t)pow(2,33);
				off2 =  3 * pow((_TEMP - 2000.0),2) / 8;
				sens2 = 7 * pow((_TEMP - 2000.0),2) / 8;
				if ( _TEMP < -1500.0 ) { // below -15C
					sens2 += 3 * pow((_TEMP + 1500.0),2);
				}
			}
			else {
				T2 = 0;
				off2 = 0;
				sens2 = 0;
			}
			break;
		case (BA14):  //MS5803-14-----------------------------------------------------------
			// 14 and 30 are the same calculations...
		case (BA30):  //MS5803-30-----------------------------------------------------------
			_OFF  = ((int64_t)_c2_OFFt1  << 16 ) + ((((int64_t)_c4_TCO * (int64_t)_dT)) >> 7 );
			_SENS = ((int64_t)_c1_SENSt1 << 15 ) + ((((int64_t)_c3_TCS * (int64_t)_dT)) >> 8 );
			// 2nd Order calculations
			if ( _TEMP < 2000.0) {  // Is temperature below or above 20.00 deg C ?
				T2 = 3 * pow(_dT,2)/(int64_t)pow(2,33);
				off2 =  3 * pow((_TEMP - 2000.0),2) / 2;
				sens2 = 5 * pow((_TEMP - 2000.0),2) / 8;
				if ( _TEMP < 1500.0 ) { // below 15C
					off2  += 7 * pow((_TEMP + 1500.0),2);
					sens2 += 4 * pow((_TEMP + 1500.0),2);
				}
			}
			else {
				T2 =    7 * ((int64_t)_dT * _dT) >> 37;
				off2 =  1 * (pow((_TEMP - 2000.0),2)) / 16;
				sens2 = 0;
			}
			break;
		default:
			_OFF = 0;
			_SENS = 0;
			T2 = 0;
			sens2 = 0;
			off2 = 0;
	}
	if ( _debug ) {
		Serial.print("    _OFF = "); serialPrintln64(_OFF);
		Serial.print("    _SENS = "); serialPrintln64(_SENS);
	}
	 // Second Order
	_TEMP  -= T2;
	_SENS  -= sens2;
	_OFF   -= off2;
	// Now pressure
	switch (_model) {
		case (BA05):  //MS5803-05-----------------------------------------------------------
			_P = ((((int32_t)_d1_pressure * _SENS) >> 21 ) - _OFF) >> 15;
			_P /= 10; // NO IDEA WHY THIS NEEDS TO BE DONE. PERHAPS AN ERROR IN THE DATASHEET?
			break;
		case (BA01):  //MS5803-01--passthrough----------------------------------------------
		case (BA02):  //MS5803-02--passthrough----------------------------------------------
		case (BA14):  //MS5803-14--passthrough----------------------------------------------
			_P = ((((int32_t)_d1_pressure * _SENS) >> 21 ) - _OFF) >> 15;
			break;
		case (BA30):  //MS5803-30-----------------------------------------------------------
			_P = ((((int32_t)_d1_pressure * _SENS) >> 21 ) - _OFF) >> 13;
			break;
		default:
			_P = 0;
	}
	if ( _debug ) {
		Serial.println("Second order values:");
		Serial.print("    T2 = "); serialPrintln64(T2);
		Serial.print("    sens2 = "); serialPrintln64(sens2);
		Serial.print("    off2 = "); serialPrintln64(off2);
		Serial.print("    _TEMP = "); Serial.println(_TEMP);
		Serial.print("    _SENS = "); serialPrintln64(_SENS);
		Serial.print("    _OFF = "); serialPrintln64(_OFF);
		Serial.println("Pressure:");
		Serial.print("    _P = "); Serial.println(_P);
	}
}

int32_t MS5803::_getADCconversion(measurement _measurement, precision _precision){
	// Retrieve ADC measurement from the device.
	// Select measurement type and precision
	uint32_t result;
	uint8_t reg_address = CMD_ADC_CONV + _measurement + _precision;
	uint8_t write_length = 0;
	uint8_t read_length = 3;
	uint16_t read_timeout = 2000;
	//sendCommand(CMD_ADC_CONV + _measurement + _precision);
	I2Cdev::writeBytes(_dev_address,reg_address,write_length,_buffer); // buffer is ignored when write_length is 0
		
	// Wait for conversion to complete
	delay(1); //general delay
	switch( _precision )
	{
		case ADC_256 : delay(1); break;
		case ADC_512 : delay(3 >> CLKPR); break;
		case ADC_1024: delay(4 >> CLKPR); break;
		case ADC_2048: delay(6 >> CLKPR); break;
		case ADC_4096: delay(10 >> CLKPR); break;
	}
	I2Cdev::readBytes(_dev_address,MS5803_ADC_READ,read_length,_buffer,read_timeout);
	result = ((uint32_t)_buffer[0] << 16) + ((uint32_t)_buffer[1] << 8) + _buffer[2];
	if (_debug) {
		Serial.print("Reading MS5803 ADC");
		switch (_measurement) {
			case PRESSURE:    Serial.println(" Pressure: "   ); break;
			case TEMPERATURE: Serial.println(" Temperature: "); break;
		}
		Serial.print("    buffer[0] = "); Serial.println(_buffer[0]);
		Serial.print("    buffer[1] = "); Serial.println(_buffer[1]);
		Serial.print("    buffer[2] = "); Serial.println(_buffer[2]);
	}
	return result;
}

void serialPrintln64(int64_t val64){
	serialPrint64(val64);
	Serial.println();
}
void serialPrint64(int64_t val64){
	int32_t result_high = val64 / 10000000L;
	int32_t result_low = val64 - (10000000LL * result_high);
	Serial.print(result_high);
	Serial.print(result_low);
}