/*
  Barometer library V1.0
  2010 Copyright (c) Seeed Technology Inc.  All right reserved.
 
  Original Author: LG
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "BMP180.h"
#include <Wire.h>
#include <Arduino.h>

void BMP180::init(void)
{
    Wire.begin();
    ac1 = bmp180ReadInt(0xAA);
    ac2 = bmp180ReadInt(0xAC);
    ac3 = bmp180ReadInt(0xAE);
    ac4 = bmp180ReadInt(0xB0);
    ac5 = bmp180ReadInt(0xB2);
    ac6 = bmp180ReadInt(0xB4);
    b1 = bmp180ReadInt(0xB6);
    b2 = bmp180ReadInt(0xB8);
    mb = bmp180ReadInt(0xBA);
    mc = bmp180ReadInt(0xBC);
    md = bmp180ReadInt(0xBE);
}
// Read 1 byte from the BMP085 at 'address'
// Return: the read byte;
char BMP180::bmp180Read(unsigned char address)
{
    //Wire.begin();
    unsigned char data;
    Wire.beginTransmission(BMP180_ADDRESS);
    Wire.write(address);
    Wire.endTransmission();

    Wire.requestFrom(BMP180_ADDRESS, 1);
    while(!Wire.available());
    return Wire.read();
}
// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int BMP180::bmp180ReadInt(unsigned char address)
{
    unsigned char msb, lsb;
    Wire.beginTransmission(BMP180_ADDRESS);
    Wire.write(address);
    Wire.endTransmission();
    Wire.requestFrom(BMP180_ADDRESS, 2);
    while(Wire.available()<2);
    msb = Wire.read();
    lsb = Wire.read();
    return (int) msb<<8 | lsb;
}
// Read the uncompensated temperature value
unsigned int BMP180::bmp180ReadUT()
{
  unsigned int ut;
  Wire.beginTransmission(BMP180_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  delay(5);
  ut = bmp180ReadInt(0xF6);
  return ut;
}
// Read the uncompensated pressure value
unsigned long BMP180::bmp180ReadUP()
{
    unsigned char msb, lsb, xlsb;
    unsigned long up = 0;
    Wire.beginTransmission(BMP180_ADDRESS);
    Wire.write(0xF4);
    Wire.write(0x34 + (OSS<<6));
    Wire.endTransmission();
    delay(2 + (3<<OSS));

    // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
    msb = bmp180Read(0xF6);
    lsb = bmp180Read(0xF7);
    xlsb = bmp180Read(0xF8);
    up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
    return up;
}
void BMP180::writeRegister(int deviceAddress, byte address, byte val)
{
    Wire.beginTransmission(deviceAddress); // start transmission to device 
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}
int BMP180::readRegister(int deviceAddress, byte address)
{
    int v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1); // read a byte

    while(!Wire.available()) {
    // waiting
    }

    v = Wire.read();
    return v;
}
float BMP180::calcAltitude(float pressure)
{
    float A = pressure/101325;
    float B = 1/5.25588;
    float C = pow(A,B);
    C = 1 - C;
    C = C /0.0000225577;
    return C;
}
float BMP180::bmp180GetTemperature(unsigned int ut)
{
    long x1, x2;

    x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
    x2 = ((long)mc << 11)/(x1 + md);
    PressureCompensate = x1 + x2;

    float temp = ((PressureCompensate + 8)>>4);
    temp = temp /10;

    return temp;
}
long BMP180::bmp180GetPressure(unsigned long up)
{
    long x1, x2, x3, b3, b6, p;
    unsigned long b4, b7;
    b6 = PressureCompensate - 4000;
    x1 = (b2 * (b6 * b6)>>12)>>11;
    x2 = (ac2 * b6)>>11;
    x3 = x1 + x2;
    b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

    // Calculate B4
    x1 = (ac3 * b6)>>13;
    x2 = (b1 * ((b6 * b6)>>12))>>16;
    x3 = ((x1 + x2) + 2)>>2;
    b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

    b7 = ((unsigned long)(up - b3) * (50000>>OSS));
    if (b7 < 0x80000000)
    p = (b7<<1)/b4;
    else
    p = (b7/b4)<<1;

    x1 = (p>>8) * (p>>8);
    x1 = (x1 * 3038)>>16;
    x2 = (-7357 * p)>>16;
    p += (x1 + x2 + 3791)>>4;

    long temp = p;
    return temp;
}

