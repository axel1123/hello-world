/* 10/16/2018 Copyright Tlera Corporation

    Created by Kris Winer

    The BMA400 is an inexpensive (~$2), three-axis, medium-resolution (12-bit), ultra-low power (800 nA low power mode) accelerometer
    in a tiny 2 mm x 2 mm LGA12 package with 1024-byte FIFO,
    two multifunction interrupts and widely configurable sample rate (15 - 800 Hz), full range (2 - 16 g), low power modes,
    and interrupt detection behaviors. This accelerometer is nice choice for motion-based wake/sleep,
    tap detection, step counting, and simple orientation estimation.

    Library may be used freely and without limit with attribution.

    Modfied by: Brayan Espinoza Garcia
    March 2020
*/

#include <Arduino.h>
#include "BMA400.h"
#include <SPI.h>

int16_t slaveSelectPin = 10;

BMA400::BMA400() {
}

void BMA400::begin(int16_t chipSelectPin) {
  //Begin SPI Comunicaction
  slaveSelectPin = chipSelectPin;
  pinMode(slaveSelectPin, OUTPUT);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0); //CPHA = CPOL = 0    MODE = 0
}

uint8_t BMA400::getChipID()
{ //Read ChipID register, It would be 0x90
  uint8_t c = SPIreadOneRegister(BMA400_CHIPID);
  return c;
}

uint8_t BMA400::getStatus()
{ //Read Status bit
  uint8_t c = SPIreadOneRegister(BMA400_INT_STAT_0);
  return c;
}

float  BMA400::getAres(uint8_t Ascale) {
  //Get the factor conversion to escale in terms of Gee´s
  switch (Ascale)
  {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs , 4 Gs , 8 Gs , and 16 Gs .
    case AFS_2G:
      _aRes = 2.0f / 2048.0f; // per data sheet
      return _aRes;
      break;
    case AFS_4G:
      _aRes = 4.0f / 2048.0f;
      break;
    case AFS_8G:
      _aRes = 8.0f / 2048.0f;
      return _aRes;
      break;
    case AFS_16G:
      _aRes = 16.0f / 2048.0f;
      return _aRes;
      break;
  }
}

void BMA400::initBMA400(uint8_t Ascale, uint8_t SR, uint8_t power_Mode, uint8_t OSR, uint8_t acc_filter)
{
  /* Normal mode configuration */
  // set bandwidth to 0.2x sample rate (bit 7), OSR in low-power mode, power mode
  SPIwriteOneRegister(BMA400_ACC_CONFIG0, 0x80 | OSR << 5 | power_Mode);
  delay(2); // wait 1.5 ms
  SPIwriteOneRegister(BMA400_ACC_CONFIG1, Ascale << 6 | OSR << 4 | SR); // set full-scale range, oversampling and sample rate
  SPIwriteOneRegister(BMA400_ACC_CONFIG2, acc_filter << 2);             // set accel filter
}


//Set Autolower registers

void BMA400::SetAutolowpowertimeout(uint16_t time_code) {
  /* Low power mode configuration */
  time_code = time_code & 0xFFF;    //Turning 16bits into 12bits
  /*
     Time=time_code*2.5ms
     Example: Time_code=4000 => Time=4000*2.5ms=10s
  */
  byte byte1 = time_code >> 4;     //First byte
  byte byte2 = time_code & 0x0F;   //Second Byte
  SPIwriteOneRegister(BMA400_AUTOLOWPOW_0, byte1);                // Set first autopow register
  SPIwriteOneRegister(BMA400_AUTOLOWPOW_1, byte2 << 4 | 0x04);    // Set second autopow register
  //SPIwriteOneRegister(BMA400_AUTOWAKEUP_1, 0x03);                 // Enable interrupt for auto wake up interrupt
}

void BMA400::SetWakeupInterruption() {
  SPIwriteOneRegister(BMA400_AUTOLOWPOW_1, 0x02);             // switch to low power when GEN1 interrupt triggered
  SPIwriteOneRegister(BMA400_AUTOWAKEUP_1, 0x02);             // enable interrupt for auto wake up interrupt
  // Enable interrupt on all three axes (bits 5 - 7)for auto wake up
  // Use 4 samples (n + 1) for wakeup condition (bits 4 - 2), 4 samples = 160 ms at 25 Hz
  // Reference updated whenever going into low power mode (bits 1, 0)
  SPIwriteOneRegister(BMA400_WKUP_INT_CONFIG0,0x07 << 5 | 0x03 << 2 | 0x01);
  SPIwriteOneRegister(BMA400_WKUP_INT_CONFIG1, 0x08); //  50 mg threshold for wake on any axis, n x 6.25 mgs at 2 g FS
   /* Interrupt configuration */
  // Map interrupts and set interrupt behavior
  SPIwriteOneRegister(BMA400_INT1_MAP, 0x01);              // map WKUP interrupt to INT1 (bit 0)
  //SPIwriteOneRegister(BMA400_INT2_MAP, 0x04);            // map GEN1 interrupt to INT2 (bit 2)
  SPIwriteOneRegister(BMA400_INT12_IO_CTRL, 0x20 | 0x02);  // set both interrupts push-pull, active HIGH
  
  // Config GEN1 on INT2
  SPIwriteOneRegister(BMA400_GEN1INT_CONFIG0, 0xFA);       // all axes, fixed 100 Hz, reference every time, hysteresis 48 mg
  SPIwriteOneRegister(BMA400_GEN1INT_CONFIG1, 0x01);       // Active motion interrupt configuration
  SPIwriteOneRegister(BMA400_GEN1INT_CONFIG2, 0x03);       // set threshold 24 mg (8 mg per count)
  SPIwriteOneRegister(BMA400_GEN1INT_CONFIG31,0x40);       // measured over 64 data samples
   /* Enable interrupts */
  SPIwriteOneRegister(BMA400_INT_CONFIG0,  0x04);          // enable GEN1 interrupt (bit 2)
}

void BMA400::activateNoMotionInterrupt()
{
  SPIwriteOneRegister(BMA400_INT_CONFIG0,  0x04);           // enable GEN1 (no_Motion) interrupt
}

void BMA400::deactivateNoMotionInterrupt()
{
  SPIwriteOneRegister(BMA400_INT_CONFIG0,  0x00);           // disable GEN1 (no_Motion) interrupt
}


void BMA400::CompensationBMA400(uint8_t Ascale, uint8_t SR, uint8_t power_Mode, uint8_t OSR, uint8_t acc_filter, float * offset)
{
  Serial.println("hold flat and motionless for bias calibration");
  delay(5000);

  SPIwriteOneRegister(BMA400_ACC_CONFIG0, 0x80 | OSR << 5 | power_Mode);// set bandwidth to 0.2x sample rate, OSR in low-power mode, power mode
  delay(2);
  SPIwriteOneRegister(BMA400_ACC_CONFIG1, Ascale << 6 | OSR << 4 | SR); // set full-scale range, oversampling and sample rate
  SPIwriteOneRegister(BMA400_ACC_CONFIG2, acc_filter << 2);             // set accel filter

  int16_t temp[3] = {0, 0, 0};
  int32_t sum[3] = {0, 0, 0};

  for (uint8_t ii = 0; ii < 128; ii++)
  {
    readBMA400AccelData(temp[0], temp[1], temp[2]);
    sum[0] += temp[0];
    sum[1] += temp[1];
    sum[2] += temp[2];
    delay(100);
  }

  offset[0] = float(sum[0]) / 128.0f;
  offset[1] = float(sum[1]) / 128.0f;
  offset[2] = float(sum[2]) / 128.0f;
  offset[0] *= _aRes;
  offset[1] *= _aRes;
  offset[2] *= _aRes;
  if (offset[2] > +0.5f) offset[2] = offset[2] - 1.0f;
  if (offset[2] < -0.5f) offset[2] = offset[2] + 1.0f;

  Serial.print("x-axis offset = "); Serial.print(offset[0] * 1000.0f, 1); Serial.println(" mg");
  Serial.print("y-axis offset = "); Serial.print(offset[1] * 1000.0f, 1); Serial.println(" mg");
  Serial.print("z-axis offset = "); Serial.print(offset[2] * 1000.0f, 1); Serial.println(" mg");
  /* end of accel calibration */
}


void BMA400::resetBMA400()
{
  SPIwriteOneRegister(BMA400_CMD, 0xB6); // software reset the BMA400
}


void BMA400::selfTestBMA400()
{
  int16_t posX, posY, posZ, negX, negY, negZ;

  // initialize sensor for self test
  SPIwriteOneRegister(BMA400_ACC_CONFIG0, 0x80 | osr3 << 5 | normal_Mode);// set bandwidth to 0.2x sample rate, OSR in low-power mode, power mode
  delay(2);
  SPIwriteOneRegister(BMA400_ACC_CONFIG1, AFS_4G << 6 | osr3 << 4 | SR_100Hz); // set full-scale range to +/- 4 g
  SPIwriteOneRegister(BMA400_ACC_CONFIG2, acc_filt1 << 2);             // set accel filter
  float STres = 4000.0f / 2048.0f; // mg/LSB for 4 g full scale
  delay(100);

  // positive axes test
  SPIwriteOneRegister(BMA400_SELF_TEST, 0x00 | 0x07); // positive axes
  delay(100);
  readBMA400AccelData(posX, posY, posZ);


  // negative axes test
  SPIwriteOneRegister(BMA400_SELF_TEST, 0x08 | 0x07); // negative axes
  delay(100);
  readBMA400AccelData(negX, negY, negZ);

  Serial.print("x-axis self test = "); Serial.print((float)(posX - negX)*STres, 1); Serial.println("mg, should be > 2000 mg");
  Serial.print("y-axis self test = "); Serial.print((float)(posY - negY)*STres, 1); Serial.println("mg, should be > 1800 mg");
  Serial.print("z-axis self test = "); Serial.print((float)(posZ - negZ)*STres, 1); Serial.println("mg, should be > 800 mg");

  SPIwriteOneRegister(BMA400_SELF_TEST, 0x00); // disable self test
  /* end of self test*/
}


//void BMA400::readBMA400AccelData(int16_t * destination)
void BMA400::readBMA400AccelData(int16_t &XData16, int16_t &YData16, int16_t &ZData16)
{
  XData16 = SPIreadOneRegister(BMA400_ACCD_X_LSB);  // Start at XData Reg
  XData16 = (SPIreadOneRegister(BMA400_ACCD_X_MSB) & 0x0F) << 8 | XData16;
  YData16 = SPIreadOneRegister(BMA400_ACCD_Y_LSB);  // Start at YData Reg
  YData16 = (SPIreadOneRegister(BMA400_ACCD_Y_MSB) & 0x0F) << 8 | YData16;
  ZData16 = SPIreadOneRegister(BMA400_ACCD_Z_LSB);  // Start at ZData Reg
  ZData16 = (SPIreadOneRegister(BMA400_ACCD_Z_MSB) & 0x0F) << 8 | ZData16;
  if (XData16 > 2047) XData16 += -4096;
  if (YData16 > 2047) YData16 += -4096;
  if (ZData16 > 2047) ZData16 += -4096;
}


int16_t BMA400::readBMA400TempData()
{
  uint8_t temp = SPIreadOneRegister(BMA400_TEMP_DATA);    // Read the raw data register
  int16_t tmp = (int16_t) ( ((int16_t)temp << 8) | 0x00) >> 8;  // Turn into signed 8-bit temperature value
  return tmp;
}

/*
  +++++++++BASIC SPI ROUTINES TO SIMPLY CODE+++++++++++
*/

// read and write one register
byte BMA400::SPIreadOneRegister(byte regAddress) {
  byte regValue = 0;
  byte dummybyte = 0;
  digitalWrite(slaveSelectPin, LOW);
  //SPI.transfer(READ);  // read instruction
  SPI.transfer(bitWrite(regAddress, 7, 1));
  dummybyte = SPI.transfer(0x00);
  regValue = SPI.transfer(0x00);
  digitalWrite(slaveSelectPin, HIGH);
  return regValue;
}
/*
  void BMA400::SPIreadRegisters(byte regAddress, uint8_t count, uint8_t * dest)
  {
  digitalWrite(slaveSelectPin, LOW); // Put Cs bit in low mode
  SPI.transfer(subAddress);            // Put slave register address in Tx buffer
  uint8_t i = 0;
  Wire.requestFrom(address, count);  // Read bytes from slave register address
  while (Wire.available()) {
        dest[i++] = Wire.read(); }   // Put read results in the Rx buffer
  }
*/

int16_t BMA400::SPIreadTwoRegisters(byte regAddress) {
  int16_t twoRegValue = 0;

  digitalWrite(slaveSelectPin, LOW);
  SPI.transfer(regAddress);
  twoRegValue = SPI.transfer(0x00);
  twoRegValue = twoRegValue + (SPI.transfer(0x00) << 8);
  digitalWrite(slaveSelectPin, HIGH);

  return twoRegValue;
}

//WRITE

void BMA400::SPIwriteOneRegister(byte regAddress, byte regValue) {
  digitalWrite(slaveSelectPin, LOW);
  //SPI.transfer(WRITE);  // write instruction
  SPI.transfer(regAddress);
  SPI.transfer(regValue);
  digitalWrite(slaveSelectPin, HIGH);
}

void BMA400::SPIwriteTwoRegisters(byte regAddress, int16_t twoRegValue) {
  byte twoRegValueH = twoRegValue >> 8;
  byte twoRegValueL = twoRegValue;

  digitalWrite(slaveSelectPin, LOW);
  //SPI.transfer(WRITE);  // write instruction
  SPI.transfer(regAddress);
  SPI.transfer(twoRegValueL);
  SPI.transfer(twoRegValueH);
  digitalWrite(slaveSelectPin, HIGH);
}


void BMA400::SPIwriteBits(byte regAddress, byte bitStart, byte length1, byte data) {
  //      010 value to write
  // 76543210 bit numbers
  //    xxx   args: bitStart=4, length=3
  // 00011100 mask byte
  // 10101111 original value (sample)
  // 10100011 original & ~mask
  // 10101011 masked | value
  byte b;
  b = SPIreadOneRegister(regAddress);
  byte mask = ((1 << length1) - 1) << (bitStart - length1 + 1);
  data <<= (bitStart - length1 + 1); // shift data into correct position
  data &= mask; // zero all non-important bits in data
  b &= ~(mask); // zero all important bits in existing byte
  b |= data; // combine data with existing byte
  SPIwriteOneRegister(regAddress, b);
}

void BMA400::SPIwriteBit(byte regAddress, byte bitNum, byte data) {
  byte b;
  b = SPIreadOneRegister(regAddress);
  b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
  SPIwriteOneRegister(regAddress, b);
}
