
#include "I2Cdev.h"
#include "MAX6956.h"

int led = 13;
int globalCurrent = 15; // 15-0, 15 the max

MAX6956 ledDriver;

void setup()
{ 
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  
  // initialize serial communication
  Serial.begin(9600);

  // initialize device
  Serial.println("Initializing I2C devices...");
  ledDriver.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  if (ledDriver.testConnection()) {
    Serial.println("Connection successful");
  } else { 
    Serial.println("Connection failed");
    while (true) {
        delay(1000);
    }
  }
  
  ledDriver.setGlobalCurrent(globalCurrent);
  ledDriver.configAllPorts(MAX6956_OUTPUT_LED); // Set all ports as led drivers
  ledDriver.enableAllPorts(); // Enable all ports
  ledDriver.setTestMode(true);
  delay(1000); // wait
  ledDriver.setTestMode(false);
  delay(1000); // wait
  ledDriver.setPower(true);
}


void loop()
{
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  // Ramp brightness down
  for (int x = 15; x >= 0; x--) { 
      ledDriver.setGlobalCurrent(globalCurrent); // Set global current
      globalCurrent = x;
      delay(80); 
  }
  
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  // Ramp brightness up
  for (int x = 0; x <= 15; x++) { 
      ledDriver.setGlobalCurrent(globalCurrent); // Set global current
      globalCurrent = x;
      delay(80);
  }
}
