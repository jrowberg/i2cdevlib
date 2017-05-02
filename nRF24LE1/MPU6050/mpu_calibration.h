#ifndef MPU_CALIBRATION_H
#define MPU_CALIBRATION_H

#include <timer0.h>
#include <dmp.h>
#include <pacotes_inerciais.h>

#define Aquire_Freq 100
//Variables for storing raw data from accelerometers gyroscope and magnetometer
int16_t xdata ax, ay, az; //Accel
int16_t xdata gx, gy, gz; //Gyro
int16_t xdata mx, my, mz; //Mag
//Calibration variables
uint8_t xdata calibIt = 0; //Counter for the number of iterations in calibration
uint8_t xdata calibCounter = 0; //Counter for the number of samples read during calibration
uint8_t xdata calibStep = 1; //Determines whichs step of the calibration should be performed
#define accelTol  8 //Error tolerance for accelerometer readings
#define gyroTol  2 //Error tolerance for gyroscope readings
#define timeTol  10 //Maximum duration of the calibration process (seconds)
int16_t xdata accelBuffer[3] = {0,0,0}; //Buffer to store the mean values from accel
int16_t xdata gyroBuffer[3] = {0,0,0};  //Buffer to store the mean values from gyro
uint8_t xdata calibOk = 0; //Variable for checking if every sensor is calibrated

/**
 * [calibrationRoutine description]
 * Everything should happen inside the timer
 * 1st step: Read raw sensor data for 1s to measure the mean values
 * 2nd step: Estimate the first offset values
 * 3rd step: Read 1s and check if the offsets are providing good values
 * according to the specified tolerance
 * 4th step: Keep looping through step 3 until the values are achieved
 * or the function runs for a specified amount of time
 */
void calibrationRoutine();

/**
 * 1st step: Measuring data to estimate the first offsets
 */
void calibrationStepOne();

/**
 * Retorna o modulo de um numero inteiro. Necessaria para calibration step2
 * @param  test_value numero para ser avaliado (int16_t)
 * @return            modulo do numero
 */
int16_t MATH_ABS(int16_t test_value);

/**
 * 2nd step: Iteratively update the offsets for accurate readings
 */
void calibrationStepTwo();

//TODO: document
void init_calibration();

////////////
//.C File //
////////////

void init_calibration(){
	//Configures the accel offsets to zero
	setXAccelOffset(0);
	setYAccelOffset(0);
	setZAccelOffset(0);
	//Configures the gyro offsets to zero
	setXGyroOffset(0);
	setYGyroOffset(0);
	setZGyroOffset(0);
	//Variables that needs to be cleared for calibration
	calibIt = 0;
	calibCounter = 0;
	calibStep = 1;
	//Small delay
	delay_ms(50);
	//Triggers the calibration method
	start_T0();//Timer calibration
}

void calibrationRoutine() {
  //Switch according to which step of the calibration should be performed
  switch (calibStep) {
    case 1:  //1st step: Measuring data to estimate the first offsets
    calibrationStepOne();
    break;
    case 2:  //2nd step: Iteratively update the offsets for accurate readings
    calibrationStepTwo();
    break;
  }
}

void calibrationStepOne(){
  //First readings to measure the mean raw values from accel and gyro
  //Measures during 2 seconds
  if(calibCounter < Aquire_Freq*2)
  {
    //Reads the imu sensors
    getMotion6_variables(&ax, &ay, &az, &gx, &gy, &gz);
    //Iteratively updating the mean value for each sensor
    //Accel-X
    accelBuffer[0] = (calibCounter*accelBuffer[0] + ax)/(calibCounter+1);
    //Accel-Y
    accelBuffer[1] = (calibCounter*accelBuffer[1] + ay)/(calibCounter+1);
    //Accel-Z
    accelBuffer[2] = (calibCounter*accelBuffer[2] + az)/(calibCounter+1);
    //Gyro-X
    gyroBuffer[0] = (calibCounter*gyroBuffer[0] + gx)/(calibCounter+1);
    //Gyro-Y
    gyroBuffer[1] = (calibCounter*gyroBuffer[1] + gy)/(calibCounter+1);
    //Gyro-Z
    gyroBuffer[2] = (calibCounter*gyroBuffer[2] + gz)/(calibCounter+1);
    //Increments the sample counter
    calibCounter++;
  }
  else
  {
    //Setting the new offset values
    //Accelerometer offsets
    setXAccelOffset(-accelBuffer[0] / 8);
    setYAccelOffset(-accelBuffer[1] / 8);
    setZAccelOffset((16384 - accelBuffer[2]) / 8);
    //Gyroscope offsets
    setXGyroOffset(-gyroBuffer[0] / 4);
    setYGyroOffset(-gyroBuffer[1] / 4);
    setZGyroOffset(-gyroBuffer[2] / 4);
    //Goes to the next step of the calibration process
    calibStep = 2;
  }
}

int16_t MATH_ABS(int16_t test_value){
	if(test_value<0){
		return (-test_value);
	} else {
		return test_value;
	}
}

void calibrationStepTwo(){
  //Reads the sensors during 1 second
  if(calibCounter <= Aquire_Freq)
  {
    //Reads the imu sensors
    getMotion6_variables(&ax, &ay, &az, &gx, &gy, &gz);
    //Iteratively updating the mean value for each sensor
    accelBuffer[0] = (calibCounter*accelBuffer[0] + ax)/(calibCounter+1);
    accelBuffer[1] = (calibCounter*accelBuffer[1] + ay)/(calibCounter+1);
    accelBuffer[2] = (calibCounter*accelBuffer[2] + az)/(calibCounter+1);
    gyroBuffer[0] = (calibCounter*gyroBuffer[0] + gx)/(calibCounter+1);
    gyroBuffer[1] = (calibCounter*gyroBuffer[1] + gy)/(calibCounter+1);
    gyroBuffer[2] = (calibCounter*gyroBuffer[2] + gz)/(calibCounter+1);
    //Increments the sample counter
    calibCounter++;
  }
  else
  {
    //Variable for checking if every sensor is calibrated
    //A sensor is calibrated if its mean value is below the tolerance
    calibOk = 0;

    //Accel-X
    if(MATH_ABS(accelBuffer[0]) < accelTol) calibOk++;
    else setXAccelOffset(getXAccelOffset() - (accelBuffer[0]/accelTol));
    //Accel-Y
    if(MATH_ABS(accelBuffer[1]) < accelTol) calibOk++;
    else setYAccelOffset(getYAccelOffset() - (accelBuffer[1]/accelTol));
    //Accel-Z
    if(MATH_ABS(accelBuffer[2]) < accelTol) calibOk++;
    else setZAccelOffset(getZAccelOffset() + ((16384-accelBuffer[2])/accelTol));
    //Gyro-X
    if(MATH_ABS(gyroBuffer[0]) < gyroTol) calibOk++;
    else setXGyroOffset(getXGyroOffset() - (gyroBuffer[0]/(gyroTol+1)));
    //Gyro-Y
    if(MATH_ABS(gyroBuffer[1]) < gyroTol) calibOk++;
    else setYGyroOffset(getYGyroOffset() - (gyroBuffer[1]/(gyroTol+1)));
    //Gyro-Z
    if(MATH_ABS(gyroBuffer[2]) < gyroTol) calibOk++;
    else setZGyroOffset(getZGyroOffset() - (gyroBuffer[2]/(gyroTol+1)));

    //Check if the calibration process can be finished
    if(calibOk == 6 || calibIt == timeTol)
    {
      stop_T0();
      //TODO: send some signal that calibration is done and the values
			send_rf_command_with_arg(CMD_CALIBRATE,CMD_OK,MY_SUB_ADDR);
      //EN_MPU_CALIBRATED_FLAG;
    }
    else
    {
      accelBuffer[0] = 0;
      accelBuffer[1] = 0;
      accelBuffer[2] = 0;
      gyroBuffer[0] = 0;
      gyroBuffer[1] = 0;
      gyroBuffer[2] = 0;
      calibIt++;
      calibCounter=0;
    }
  }
}

#endif /*MPU_CALIBRATION_H*/
