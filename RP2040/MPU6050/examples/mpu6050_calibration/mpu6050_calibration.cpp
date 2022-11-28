#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#ifndef PICO_DEFAULT_LED_PIN // PICO w with WiFi
#include "pico/cyw43_arch.h"
#endif
#include "MPU6050.h"

MPU6050 accelgyro;

const int iAx = 0;
const int iAy = 1;
const int iAz = 2;
const int iGx = 3;
const int iGy = 4;
const int iGz = 5;

const int usDelay = 3150;   // empirical, to hold sampling to 200 Hz
const int NFast =  1000;    // the bigger, the better (but slower)
const int NSlow = 10000;    // ..
      int LowValue[6];
      int HighValue[6];
      int Smoothed[6];
      int LowOffset[6];
      int HighOffset[6];
      int Target[6];
      int N;

void GetSmoothed(){ int16_t RawValue[6];
    int i;
    long Sums[6];
    for (i = iAx; i <= iGz; i++)
      { Sums[i] = 0; }

    for (i = 1; i <= N; i++)
      { // get sums
        accelgyro.getMotion6(&RawValue[iAx], &RawValue[iAy], &RawValue[iAz], 
                             &RawValue[iGx], &RawValue[iGy], &RawValue[iGz]);
        if ((i % 500) == 0)
          printf(".");
        sleep_us(usDelay);
        for (int j = iAx; j <= iGz; j++)
          Sums[j] = Sums[j] + RawValue[j];
      } // get sums
    for (i = iAx; i <= iGz; i++)
      { Smoothed[i] = (Sums[i] + N/2) / N ; }
} // GetSmoothed

void Initialize(){
    printf("Initializing I2C devices...\n");
    accelgyro.initialize();

    // verify connection
    printf("Testing device connections...\n");
    if(accelgyro.testConnection()) printf("MPU6050 connection successful\n");
    else {printf("MPU6050 connection failed\n");}
    printf("\nPID tuning Each Dot = 100 readings");
  /*A tidbit on how PID (PI actually) tuning works. 
    When we change the offset in the MPU6050 we can get instant results. This allows us to use Proportional and 
    integral of the PID to discover the ideal offsets. Integral is the key to discovering these offsets, Integral 
    uses the error from set-point (set-point is zero), it takes a fraction of this error (error * ki) and adds it 
    to the integral value. Each reading narrows the error down to the desired offset. The greater the error from 
    set-point, the more we adjust the integral value. The proportional does its part by hiding the noise from the 
    integral math. The Derivative is not used because of the noise and because the sensor is stationary. With the 
    noise removed the integral value lands on a solid offset after just 600 readings. At the end of each set of 100 
    readings, the integral value is used for the actual offsets and the last proportional reading is ignored due to 
    the fact it reacts to any noise.
  */
        accelgyro.CalibrateAccel(6);
        accelgyro.CalibrateGyro(6);
        printf("\nat 600 Readings");
        accelgyro.PrintActiveOffsets();
        printf("\n");
        accelgyro.CalibrateAccel(1);
        accelgyro.CalibrateGyro(1);
        printf("700 Total Readings");
        accelgyro.PrintActiveOffsets();
        printf("\n");
        accelgyro.CalibrateAccel(1);
        accelgyro.CalibrateGyro(1);
        printf("800 Total Readings");
        accelgyro.PrintActiveOffsets();
        printf("\n");
        accelgyro.CalibrateAccel(1);
        accelgyro.CalibrateGyro(1);
        printf("900 Total Readings");
        accelgyro.PrintActiveOffsets();
        printf("\n");    
        accelgyro.CalibrateAccel(1);
        accelgyro.CalibrateGyro(1);
        printf("1000 Total Readings");
        accelgyro.PrintActiveOffsets();
     printf("\n\nAny of the above offsets will work nice\nLets proof the PID tuning using another method:"); 
} // Initialize

void SetOffsets(int TheOffsets[6]){
    accelgyro.setXAccelOffset(TheOffsets[iAx]);
    accelgyro.setYAccelOffset(TheOffsets[iAy]);
    accelgyro.setZAccelOffset(TheOffsets[iAz]);
    accelgyro.setXGyroOffset(TheOffsets[iGx]);
    accelgyro.setYGyroOffset(TheOffsets[iGy]);
    accelgyro.setZGyroOffset(TheOffsets[iGz]);
} // SetOffsets

// The following may show up as a mess in the terminal unless it is on fullscreen
void ShowProgress(){
    printf("\nXAccel\t\t\tYAccel\t\t\tZAccel\t\t\tXGyro\t\t\tYGyro\t\t\tZGyro\n");
    for (int i = iAx; i <= iGz; i++){
        printf("[%d,%d] --> [%d,%d]\t", LowOffset[i], HighOffset[i], LowValue[i], HighValue[i]);
    }
    printf("\n");
} // ShowProgress

void SetAveraging(int NewN){
    N = NewN;
    printf("\nAveraging %d readings each time\n", N);
} // SetAveraging

void PullBracketsIn(){
    bool AllBracketsNarrow;
    bool StillWorking;
    int NewOffset[6];
  
    printf("\nClosing in:\n");
    AllBracketsNarrow = false;
    StillWorking = true;
    while (StillWorking) 
      { StillWorking = false;
        if (AllBracketsNarrow && (N == NFast))
          { SetAveraging(NSlow); }
        else
          { AllBracketsNarrow = true; }// tentative
        for (int i = iAx; i <= iGz; i++)
          { if (HighOffset[i] <= (LowOffset[i]+1))
              { NewOffset[i] = LowOffset[i]; }
            else
              { // binary search
                StillWorking = true;
                NewOffset[i] = (LowOffset[i] + HighOffset[i]) / 2;
                if (HighOffset[i] > (LowOffset[i] + 10))
                  { AllBracketsNarrow = false; }
              } // binary search
          }
        SetOffsets(NewOffset);
        GetSmoothed();
        for (int i = iAx; i <= iGz; i++)
          { // closing in
            if (Smoothed[i] > Target[i])
              { // use lower half
                HighOffset[i] = NewOffset[i];
                HighValue[i] = Smoothed[i];
              } // use lower half
            else
              { // use upper half
                LowOffset[i] = NewOffset[i];
                LowValue[i] = Smoothed[i];
              } // use upper half
          } // closing in
        ShowProgress();
      } // still working
   
} // PullBracketsIn

void PullBracketsOut(){
    bool Done = false;
    int NextLowOffset[6];
    int NextHighOffset[6];

    printf("\nExpanding:\n");
 
    while (!Done)
      { Done = true;
        SetOffsets(LowOffset);
        GetSmoothed();
        for (int i = iAx; i <= iGz; i++)
          { // got low values
            LowValue[i] = Smoothed[i];
            if (LowValue[i] >= Target[i])
              { Done = false;
                NextLowOffset[i] = LowOffset[i] - 1000;
              }
            else
              { NextLowOffset[i] = LowOffset[i]; }
          } // got low values
      
        SetOffsets(HighOffset);
        GetSmoothed();
        for (int i = iAx; i <= iGz; i++)
          { // got high values
            HighValue[i] = Smoothed[i];
            if (HighValue[i] <= Target[i])
              { Done = false;
                NextHighOffset[i] = HighOffset[i] + 1000;
              }
            else
              { NextHighOffset[i] = HighOffset[i]; }
          } // got high values
        ShowProgress();
        for (int i = iAx; i <= iGz; i++)
          { LowOffset[i] = NextLowOffset[i];   // had to wait until ShowProgress done
            HighOffset[i] = NextHighOffset[i]; // ..
          }
     } // keep going
} // PullBracketsOut

void initLED() {
#ifndef PICO_DEFAULT_LED_PIN // PICO w with WiFi
    printf ("we have board with wifi (pico w) \n");
    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        exit(3);
    }
#else
    printf ("we have board without wifi\n");
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif // PICO_DEFAULT_LED_PIN

} // initLED()

void  waitForUsbConnect() {
#ifdef _PICO_STDIO_USB_H // We are using PICO_STDIO_USB. Have to wait for connection.
    
#ifndef PICO_DEFAULT_LED_PIN
    while (!stdio_usb_connected()) { // blink the pico's led until usb connection is established
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(250);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(250);
    }
#else
    while (!stdio_usb_connected()) { // blink the pico's led until usb connection is established
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(250);
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(250);
    }
#endif // PICO_DEFAULT_LED_PIN
#endif // _PICO_STDIO_USB_H
} //  waitForUsbConnect

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(i2c_default, 400*1000);
    
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // setup blink led
    initLED();
    waitForUsbConnect();

    Initialize();
    for (int i = iAx; i <= iGz; i++)
      { // set targets and initial guesses
        Target[i] = 0; // must fix for ZAccel 
        HighOffset[i] = 0;
        LowOffset[i] = 0;
      } // set targets and initial guesses
    Target[iAz] = 16384;
    SetAveraging(NFast);
    
    PullBracketsOut();
    PullBracketsIn();
    
    printf("\n\n-------------- done --------------");

    return 0;
}
