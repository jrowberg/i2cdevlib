

#include <Wire.h>
#include <I2Cdev.h>
#include <MS5803_I2C.h>

//const uint8_t MS_MODEL = 1; // MS5803-01BA
//const uint8_t MS_MODEL = 2; // MS5803-02BA
const uint8_t MS_MODEL = 5; // MS5803-05BA
//const uint8_t MS_MODEL = 14; // MS5803-14BA
//const uint8_t MS_MODEL = 30; // MS5803-30BA

MS5803 presstemp(0x76);
const uint8_t loop_delay = 10; // Seconds between readings
uint32_t wake_time = millis();


void setup() {
  Serial.begin(57600);
  Wire.begin();
  // Start up and get Calibration constants.
  presstemp.initialize(MS_MODEL);
  if ( presstemp.testConnection() ) Serial.println("We are communicating with MS5803 via I2C.");
  else Serial.println("I2C Communications with MS5803 failed.");
}
void loop(){
    Serial.print("Getting temperature");
    presstemp.calcMeasurements(ADC_4096);
    Serial.print("The temperature is "); Serial.print(presstemp.getTemp_C()); Serial.println(" C");
    Serial.print("The pressure is "); Serial.print(presstemp.getPress_mBar()); Serial.println(" mBar");
  delay(2000);
}