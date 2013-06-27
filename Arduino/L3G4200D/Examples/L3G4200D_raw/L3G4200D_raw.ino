/**
* A sketch to test the L3G4200D sensor
* AUTHOR: JONATHAN ARNETT
* MODIFIED: 02/04/2013
*/

#include <I2Cdev.h>
#include <L3G4200D.h>
#include <Wire.h>

L3G4200D gyro;

#define X 0
#define Y 1
#define Z 2

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  // initialize
  Serial.println("Initializing L3G4200D...");
  gyro.initialize();

  // test connection
  Serial.println("Testing connection to device...");
  Serial.println(gyro.testConnection() ? "L3G4200D connection successful" : 
   "L3G4200D connection failed");
}

void loop() {
  bool indiv = 0;
  String names[3] = {"x", "y", "z"};
  int16_t data[3] = {0, 0, 0};
  
  if (indiv) {
    data[X] = gyro.getAngularVelocityX();
    data[Y] = gyro.getAngularVelocityY();
    data[Z] = gyro.getAngularVelocityZ();
  } else {
    gyro.getAngularVelocity(&data[X], &data[Y], &data[Z]);
  }
  
  for (int i = 0; i < 3; i++) {
    Serial.print(names[i]);
    Serial.print(" = ");
    Serial.print(data[i]);
    Serial.print(" ");
  }  

  Serial.println();
  delay(500);
}


