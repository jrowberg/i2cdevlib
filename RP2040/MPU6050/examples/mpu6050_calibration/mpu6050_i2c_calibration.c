#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


/* Example code to talk to a MPU6050 MEMS accelerometer and gyroscope

   This is taking to simple approach of simply reading registers. It's perfectly
   possible to link up an interrupt line and set things up to read from the
   inbuilt FIFO to make it more useful.

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO PICO_DEFAULT_I2C_SDA_PIN (On Pico this is 4 (pin 6)) -> SDA on MPU6050 board
   GPIO PICO_DEFAULT_I2C_SCK_PIN (On Pico this is 5 (pin 7)) -> SCL on MPU6050 board
   3.3v (pin 36) -> VCC on MPU6050 board
   GND (pin 38)  -> GND on MPU6050 board
*/

#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_OUT_START_REG 0x3B
#define TEMP_OUT_REG 0x41
#define GYRO_OUT_START_REG 0x43
#define PWR_MGMT_1_REG 0x6B
/*
Offset Registers
*/
#define MPU6050_XA_OFFS_H        0x06 //[15:0] XA_OFFS
#define MPU6050_XA_OFFS_L_TC     0x07
#define MPU6050_YA_OFFS_H        0x08 //[15:0] YA_OFFS
#define MPU6050_YA_OFFS_L_TC     0x09
#define MPU6050_ZA_OFFS_H        0x0A //[15:0] ZA_OFFS
#define MPU6050_ZA_OFFS_L_TC     0x0B
#define MPU6050_XG_OFFS_USRH     0x13 //[15:0] XG_OFFS_USR
#define MPU6050_XG_OFFS_USRL     0x14
#define MPU6050_YG_OFFS_USRH     0x15 //[15:0] YG_OFFS_USR
#define MPU6050_YG_OFFS_USRL     0x16
#define MPU6050_ZG_OFFS_USRH     0x17 //[15:0] ZG_OFFS_USR
#define MPU6050_ZG_OFFS_USRL     0x18

/*
Calibration macros
*/
#define MAX_DISCARD_SAMPLES 100
#define MAX_CALIBRATION_SAMPLES (uint16_t)10000

// By default these devices  are on bus address 0x68
static int addr = 0x68;

/*
 * The calibration offsets of each sensor in each axis.
 * Default values are set to 0.
 */
int16_t accelerometerXOffset = 0;
int16_t accelerometerYOffset = 0;
int16_t accelerometerZOffset = 0;
int16_t gyroscopeXOffset = 0;
int16_t gyroscopeYOffset = 0;
int16_t gyroscopeZOffset = 0;

int16_t accel_gyro_avg[6] = {INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX};

void setXAccelOffset(int16_t offset);

void setYAccelOffset(int16_t offset);

void setZAccelOffset(int16_t offset);

void setXGyroOffset(int16_t offset);

void setYGyroOffset(int16_t offset);

void setZGyroOffset(int16_t offset);

int getDeviceOffsets(int16_t *accel_gyro_offsets);

int16_t getXAccelOffset();

int16_t getYAccelOffset();

int16_t getZAccelOffset();

int16_t getXGyroOffset();

int16_t getYGyroOffset();

int16_t getZGyroOffset();

static void mpu6050_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {PWR_MGMT_1_REG, 0x01};
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
    /* Set Gyroscope range */
    buf[0] = GYRO_CONFIG_REG;
    buf[1] = 0x00;
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
    /* Set Accelerometer range */
    buf[0] = ACCEL_CONFIG_REG;
    buf[1] = 0x00;
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

static void mpu6050_read_raw_accel(int16_t accel[3]){
    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = ACCEL_OUT_START_REG;
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }
}

static void mpu6050_read_raw_gyro(int16_t gyro[3]){
    uint8_t buffer[6];

    // Read gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    uint8_t val = GYRO_OUT_START_REG;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);  // False - finished with bus

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }
}

static void mpu6050_read_raw_temperature(int16_t *temp){
    uint8_t buffer[6];

    // Read temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    uint8_t val = TEMP_OUT_REG;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);  // False - finished with bus

    *temp = buffer[0] << 8 | buffer[1];
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    mpu6050_read_raw_accel(accel);
    mpu6050_read_raw_gyro(gyro);
    mpu6050_read_raw_temperature(temp);
}

static void mpu6050_discard_values(){
    int16_t buffer[3];

    for(int i=0; i<MAX_DISCARD_SAMPLES; i++){
        mpu6050_read_raw_accel(buffer);
        mpu6050_read_raw_gyro(buffer);
        sleep_ms(2);    /* To avoid repeated measurements. */
    }
}

static void mpu6050_average(uint16_t max_samples){
    uint8_t buffer[6];
    int16_t accel[3], gyro[3];
    int32_t accel_buffer_x = 0;
    int32_t accel_buffer_y = 0;
    int32_t accel_buffer_z = 0;
    int32_t gyro_buffer_x = 0;
    int32_t gyro_buffer_y = 0;
    int32_t gyro_buffer_z = 0;

    for(int i=0; i<max_samples; i++){
        mpu6050_read_raw_accel(accel);
        mpu6050_read_raw_gyro(gyro);

        accel_buffer_x += accel[0];
        accel_buffer_y += accel[1];
        accel_buffer_z += accel[2];
        gyro_buffer_x += gyro[0];
        gyro_buffer_y += gyro[1];
        gyro_buffer_z += gyro[2];

        sleep_ms(2);    /* To avoid repeated measurements. */
    }

    accel_gyro_avg[0] = (int16_t)(accel_buffer_x/max_samples);
    accel_gyro_avg[1] = (int16_t)(accel_buffer_y/max_samples);
    accel_gyro_avg[2] = (int16_t)(accel_buffer_z/max_samples);
    accel_gyro_avg[3] = (int16_t)(gyro_buffer_x/max_samples);
    accel_gyro_avg[4] = (int16_t)(gyro_buffer_y/max_samples);
    accel_gyro_avg[5] = (int16_t)(gyro_buffer_z/max_samples);
}

void setXAccelOffset(int16_t offset){
    uint8_t buf[] = {MPU6050_XA_OFFS_H, (uint8_t)(offset >> 8), (uint8_t)offset};
    i2c_write_blocking(i2c_default, addr, buf, 3, false);
}

void setYAccelOffset(int16_t offset){
    uint8_t buf[] = {MPU6050_YA_OFFS_H, (uint8_t)(offset >> 8), (uint8_t)offset};
    i2c_write_blocking(i2c_default, addr, buf, 3, false);
}

void setZAccelOffset(int16_t offset){
    uint8_t buf[] = {MPU6050_ZA_OFFS_H, (uint8_t)(offset >> 8), (uint8_t)offset};
    i2c_write_blocking(i2c_default, addr, buf, 3, false);
}

void setXGyroOffset(int16_t offset){
    uint8_t buf[] = {MPU6050_XG_OFFS_USRH, (uint8_t)(offset >> 8), (uint8_t)offset};
    i2c_write_blocking(i2c_default, addr, buf, 3, false);
}

void setYGyroOffset(int16_t offset){
    uint8_t buf[] = {MPU6050_YG_OFFS_USRH, (uint8_t)(offset >> 8), (uint8_t)offset};
    i2c_write_blocking(i2c_default, addr, buf, 3, false);
}

void setZGyroOffset(int16_t offset){
    uint8_t buf[] = {MPU6050_ZG_OFFS_USRH, (uint8_t)(offset >> 8), (uint8_t)offset};
    i2c_write_blocking(i2c_default, addr, buf, 3, false);
}

int16_t getXAccelOffset(){
    uint8_t buffer[6], val = MPU6050_XA_OFFS_H;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

int16_t getYAccelOffset(){
    uint8_t buffer[6], val = MPU6050_YA_OFFS_H;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

int16_t getZAccelOffset(){
    uint8_t buffer[6], val = MPU6050_ZA_OFFS_H;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

int16_t getXGyroOffset(){
    uint8_t buffer[6], val = MPU6050_XG_OFFS_USRH;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

int16_t getYGyroOffset(){
    uint8_t buffer[6], val = MPU6050_YG_OFFS_USRH;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

int16_t getZGyroOffset(){
    uint8_t buffer[6], val = MPU6050_ZG_OFFS_USRH;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

int getDeviceOffsets(int16_t *accel_gyro_offsets){
    accel_gyro_offsets[0] = getXAccelOffset();
    accel_gyro_offsets[1] = getYAccelOffset();
    accel_gyro_offsets[2] = getZAccelOffset();
    accel_gyro_offsets[3] = getXGyroOffset();
    accel_gyro_offsets[4] = getYGyroOffset();
    accel_gyro_offsets[5] = getZGyroOffset();

    return 0;
}

int main() {
    stdio_init_all();

    printf("Hello, MPU6050! Reading raw data from registers...\n");

    /* This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico) */
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    mpu6050_reset();

    int16_t acceleration[3], gyro[3], temp;
    int16_t  accel_gyro_offsets[6] = {0, 0, 0, 0, 0, 0};
    char op;
    int16_t mask_16bit = 0x0001, mask_bit[3] = {0, 0, 0};

    while(1){
        printf("\nSelect option: \n"
                "1. Read raw values\n"
                "2. Calibrate device\n"
                "3. Read offset values\n"
                "4. Manually set offset values\n"
                "5. Get average readings\n"
                "0. Terminate program\n\n");
        op = getchar();
        
        if(op == '1'){
            while (1) {
                mpu6050_read_raw(acceleration, gyro, &temp);
                printf("Acc. X = %f, Y = %f, Z = %f\n", acceleration[0]/16384.0, acceleration[1]/16384.0, acceleration[2]/16384.0);
                printf("Gyro. X = %f, Y = %f, Z = %f\n", gyro[0]/131.0, gyro[1]/131.0, gyro[2]/131.0);
                /*
                Temperature is simple so use the datasheet calculation to get deg C.
                Note this is chip temperature.
                */
                printf("Temp. = %f\n", (temp / 340.0) + 36.53);

                sleep_ms(100);
            }
        }
        else if(op == '2'){
            mpu6050_discard_values();
            printf("Computing averages, please wait...\n");
            mpu6050_average(MAX_CALIBRATION_SAMPLES);
            printf("\nAverage Values\n");
            printf("Acc. X = %d, Y = %d, Z = %d\n", accel_gyro_avg[0], accel_gyro_avg[1], accel_gyro_avg[2]);
            printf("Gyro. X = %d, Y = %d, Z = %d\n", accel_gyro_avg[3], accel_gyro_avg[4], accel_gyro_avg[5]);
            /*printf("\nAcc. X = %f, Y = %f, Z = %f\n", acceleration[0]/16384.0, acceleration[1]/16384.0, acceleration[2]/16384.0);
            printf("Gyro. X = %f, Y = %f, Z = %f\n\n", gyro[0]/131.0, gyro[1]/131.0, gyro[2]/131.0);*/

            if(getDeviceOffsets(accel_gyro_offsets))
                return -1;

            for(int i=0; i<3; i++){
                if(accel_gyro_offsets[i] & mask_16bit)
                    mask_bit[i] = mask_16bit;
            }

            accelerometerXOffset = (accel_gyro_offsets[0] - (accel_gyro_avg[0] / 8)) | mask_bit[0];
            accelerometerYOffset = (accel_gyro_offsets[1] - (accel_gyro_avg[1] / 8)) | mask_bit[1];
            /* Accelerometer Z Offset drifts and gets bigger, cause? */
            accelerometerZOffset = (accel_gyro_offsets[2] - ((16384 - accel_gyro_avg[2]) / 8)) | mask_bit[2];

            gyroscopeXOffset -= (accel_gyro_avg[3] / 4);
            gyroscopeYOffset -= (accel_gyro_avg[4] / 4);
            gyroscopeZOffset -= (accel_gyro_avg[5] / 4);
            
            printf("\nCalculated Offset Values\n");
            printf("Acc. X = %d, Y = %d, Z = %d\n", accelerometerXOffset, accelerometerYOffset, accelerometerZOffset);
            printf("Gyro. X = %d, Y = %d, Z = %d\n", gyroscopeXOffset, gyroscopeYOffset, gyroscopeZOffset);
            printf("\nWritting offset values to device...\n");
            setXAccelOffset(accelerometerXOffset);
            setYAccelOffset(accelerometerYOffset);
            setZAccelOffset(accelerometerZOffset);
            setXGyroOffset(gyroscopeXOffset);
            setYGyroOffset(gyroscopeYOffset);
            setZGyroOffset(gyroscopeZOffset);
            mpu6050_average(1000);
            printf("\nNEW Average Values\n");
            printf("Acc. X = %d, Y = %d, Z = %d\n", accel_gyro_avg[0], accel_gyro_avg[1], accel_gyro_avg[2]);
            printf("Gyro. X = %d, Y = %d, Z = %d\n", accel_gyro_avg[3], accel_gyro_avg[4], accel_gyro_avg[5]);
        }
        else if(op == '3'){
            if(getDeviceOffsets(accel_gyro_offsets))
                return -1;

            printf("\nOffset Values on device:\n");
            printf("Acc. X = %d, Y = %d, Z = %d\n", accel_gyro_offsets[0], accel_gyro_offsets[1], accel_gyro_offsets[2]);
            printf("Gyro. X = %d, Y = %d, Z = %d\n", accel_gyro_offsets[3], accel_gyro_offsets[4], accel_gyro_offsets[5]);
        }
        else if(op =='4'){
            setXAccelOffset(0);
            setYAccelOffset(0);
            setZAccelOffset(0);
            setXGyroOffset(0);
            setYGyroOffset(0);
            setZGyroOffset(0);
            /*setXAccelOffset(-457);
            setYAccelOffset(319);
            setZAccelOffset(1392);
            setXGyroOffset(7);
            setYGyroOffset(-198);
            setZGyroOffset(-57);*/
        }
        else if(op == '5'){
            /* Get average from 1000 readings */
            mpu6050_average(1000);
            printf("\nAverage Values\n");
            printf("Acc. X = %d, Y = %d, Z = %d\n", accel_gyro_avg[0], accel_gyro_avg[1], accel_gyro_avg[2]);
            printf("Gyro. X = %d, Y = %d, Z = %d\n", accel_gyro_avg[3], accel_gyro_avg[4], accel_gyro_avg[5]);
        }
        else if(op == '0'){
            return 0;
        }
        else{
            printf("\nInvalid option!\n");
        }
    }
    return 0;
}
