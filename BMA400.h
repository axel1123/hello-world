/* 10/16/2018 Copyright Tlera Corporation
 *
 *  Created by Kris Winer
 *
 *  The BMA400 is an inexpensive (~$2), three-axis, medium-resolution (12-bit), ultra-low power (800 nA low power mode) accelerometer
 *  in a tiny 2 mm x 2 mm LGA12 package with 1024-byte FIFO,
 *  two multifunction interrupts and widely configurable sample rate (15 - 800 Hz), full range (2 - 16 g), low power modes,
 *  and interrupt detection behaviors. This accelerometer is nice choice for motion-based wake/sleep,
 *  tap detection, step counting, and simple orientation estimation.
 *
 *  Library may be used freely and without limit with attribution.
 *
 *  Modfied by: Brayan Espinoza Garcia
 *  March 2020
 */

#ifndef BMA400_h
#define BMA400_h

#include <Arduino.h>

/* Register Map BMA400
// https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMA400-DS000-02.pdf
*/
#define BMA400_CHIPID          0x00     //Contains the chip identification code is 0x90HEX
#define BMA400_ERR_REG         0x02     //
#define BMA400_STATUS          0x03     //Sensor status bits
#define BMA400_ACCD_X_LSB      0x04     //Less significant bytes for x acceleration data
#define BMA400_ACCD_X_MSB      0x05     //Most significant bytes for x acceleration data
#define BMA400_ACCD_Y_LSB      0x06     //Less significant bytes for y acceleration data
#define BMA400_ACCD_Y_MSB      0x07     //Most significant bytes for y acceleration data
#define BMA400_ACCD_Z_LSB      0x08     //Less significant bytes for z acceleration data
#define BMA400_ACCD_Z_MSB      0x09     //Most significant bytes for z acceleration data
#define BMA400_SENSOR_TIME_0   0x0A     //Contains the first byte of sensor time
#define BMA400_SENSOR_TIME_1   0x0B     //Contains the middle byte of sensor time
#define BMA400_SENSOR_TIME_2   0x0C     //Contains the last byte of sensor time
#define BMA400_EVENT           0x0D     //Contains the Event bits status
#define BMA400_INT_STAT_0      0x0E     //Contains the interrupt status bits
#define BMA400_INT_STAT_1      0x0F     //Contains the interrupt status bits
#define BMA400_INT_STAT_2      0x10     //Contains the interrupt status bits
#define BMA400_TEMP_DATA       0x11     //Contains the 8-bits data
#define BMA400_FIFO_LENGTH0    0x12     //Contains the Less significant number of bytes stored in FIFO
#define BMA400_FIFO_LENGTH1    0x13     //Contains the Most significant number of bits stored in FIFO
#define BMA400_FIFO_DATA       0x14     //Contains the FIFO data
#define BMA400_STEP_CNT_0      0x15     //Less signifivative number of steps by step counter
#define BMA400_STEP_CNT_1      0x16     //Middle significative number of steps by step counter
#define BMA400_STEP_CNT_2      0x17     //Most significative number of steps by step counter
#define BMA400_STEP_STAT       0x18     //Contains the status STILL WALK or RUN
#define BMA400_ACC_CONFIG0     0x19     //Contains the accelerometer configuration
#define BMA400_ACC_CONFIG1     0x1A     //Contains the accelerometer configurarion
#define BMA400_ACC_CONFIG2     0x1B     //Contains the accelerometer configuration
#define BMA400_INT_CONFIG0     0x1F     //Contains interrupt control bits
#define BMA400_INT_CONFIG1     0x20     //Contains interrupt control bits
#define BMA400_INT1_MAP        0x21     //Contains the interrupt to physical INT1 pin mapping
#define BMA400_INT2_MAP        0x22     //Contains the interrupt to physical INT2 pin mapping
#define BMA400_INT12_MAP       0x23     //Contains the interrup mapping to physical pins
#define BMA400_INT12_IO_CTRL   0x24     //Contains physical behaviour of interrupt pins configuration
#define BMA400_FIFO_CONFIG0    0x26     //Contains FIFO contol and FIFO configuration settings
#define BMA400_FIFO_CONFIG1    0x27     //Contains FIFO contol and FIFO configuration settings
#define BMA400_FIFO_CONFIG2    0x28     //Contains FIFO contol and FIFO configuration settings
#define BMA400_FIFO_PWR_CONFIG 0x29     //Contains FIFO read power circuit settings
#define BMA400_AUTOLOWPOW_0    0x2A     //Contain configuration for auto-low-power condition
#define BMA400_AUTOLOWPOW_1    0x2B     //Contain configuration for auto-low-power condition
#define BMA400_AUTOWAKEUP_0    0x2C     //Contain configuration for auto-low-power condition
#define BMA400_AUTOWAKEUP_1    0x2D
#define BMA400_WKUP_INT_CONFIG0 0x2F
#define BMA400_WKUP_INT_CONFIG1 0x30
#define BMA400_WKUP_INT_CONFIG2 0x31
#define BMA400_WKUP_INT_CONFIG3 0x32
#define BMA400_WKUP_INT_CONFIG4 0x33
#define BMA400_ORIENTCH_CONFIG0 0x35
#define BMA400_ORIENTCH_CONFIG1 0x36
#define BMA400_ORIENTCH_CONFIG2 0x37
#define BMA400_ORIENTCH_CONFIG3 0x38
#define BMA400_ORIENTCH_CONFIG4 0x39
#define BMA400_ORIENTCH_CONFIG5 0x3A
#define BMA400_ORIENTCH_CONFIG6 0x3B
#define BMA400_ORIENTCH_CONFIG7 0x3C
#define BMA400_ORIENTCH_CONFIG8 0x3D
#define BMA400_ORIENTCH_CONFIG9 0x3E
#define BMA400_GEN1INT_CONFIG0  0x3F
#define BMA400_GEN1INT_CONFIG1  0x40
#define BMA400_GEN1INT_CONFIG2  0x41
#define BMA400_GEN1INT_CONFIG3  0x42
#define BMA400_GEN1INT_CONFIG31 0x43
#define BMA400_GEN1INT_CONFIG4  0x44
#define BMA400_GEN1INT_CONFIG5  0x45
#define BMA400_GEN1INT_CONFIG6  0x46
#define BMA400_GEN1INT_CONFIG7  0x47
#define BMA400_GEN1INT_CONFIG8  0x48
#define BMA400_GEN1INT_CONFIG9  0x49
#define BMA400_GEN2INT_CONFIG0  0x4A
#define BMA400_GEN2INT_CONFIG1  0x4B
#define BMA400_GEN2INT_CONFIG2  0x4C
#define BMA400_GEN2INT_CONFIG3  0x4D
#define BMA400_GEN2INT_CONFIG31 0x4E
#define BMA400_GEN2INT_CONFIG4  0x4F
#define BMA400_GEN2INT_CONFIG5  0x50
#define BMA400_GEN2INT_CONFIG6  0x51
#define BMA400_GEN2INT_CONFIG7  0x52
#define BMA400_GEN2INT_CONFIG8  0x53
#define BMA400_GEN2INT_CONFIG9  0x54
#define BMA400_ACTCH_CONFIG0    0x55
#define BMA400_ACTCH_CONFIG1    0x56
#define BMA400_TAP_CONFIG       0x57
#define BMA400_TAP_CONFIG1      0x58
#define BMA400_IF_CONF          0x7C
#define BMA400_SELF_TEST        0x7D
#define BMA400_CMD              0x7E

//I2c adress
#define BMA400_ADDRESS  0x14  // for I2C conections

//Set Auto Full Scale
#define AFS_2G           0x00
#define AFS_4G           0x01
#define AFS_8G           0x02
#define AFS_16G          0x03

//SR Sensor Rate
#define SR_12_5Hz        0x05  // 12.5 Hz sample rate, etc
#define SR_25Hz          0x06
#define SR_50Hz          0x07
#define SR_100Hz         0x08
#define SR_200Hz         0x09  // 200 Hz sample rate
#define SR_400Hz         0x0A
#define SR_800Hz         0x0B  // 800 Hz sample rate

//Power modes
#define sleep_Mode       0x00
#define lowpower_Mode    0x01 ///////////////////////////////////////////////////////////
#define normal_Mode      0x02

// define oversampling
#define osr0             0x00  // lowest power, lowest oversampling, lowest accuracy
#define osr1             0x01
#define osr2             0x02
#define osr3             0x03 // highest oversampling, highest power, highest accuracy

//Define accel filter
#define acc_filt1        0x00 // variable ODR filter
#define acc_filt2        0x01 // fixed 100 Hz ODR filter
#define acc_filt_lp      0x02 // fixed 100 Hz ODR filter, 1 Hz BW

class BMA400
{
  public:
  BMA400();

  void begin (int16_t chipSelectPin = 10);
  float getAres(uint8_t Ascale);
  uint8_t getChipID();
  void initBMA400(uint8_t Ascale, uint8_t SR, uint8_t power_Mode, uint8_t OSR, uint8_t acc_filter);
  void CompensationBMA400(uint8_t Ascale, uint8_t SR, uint8_t power_Mode, uint8_t OSR, uint8_t acc_filter, float * offset);
  void resetBMA400();
  void selfTestBMA400();
  void readBMA400AccelData(int16_t &XData16, int16_t &YData16, int16_t &ZData16);
  void SetWakeupInterruption();
  int16_t readBMA400TempData();
  void activateNoMotionInterrupt();
  void deactivateNoMotionInterrupt();
  uint8_t getStatus();

  //Autolowpowerfunctions
  void SetAutolowpowertimeout(uint16_t time_code);

  //Basic SPI functions
  byte SPIreadOneRegister(byte regAddress);
  int16_t SPIreadTwoRegisters(byte regAddress);
  void SPIwriteOneRegister(byte regAddress, byte regValue);
  void SPIwriteTwoRegisters(byte regAddress, int16_t twoRegValue);
  void SPIwriteBits(byte regAddress, byte bitStart, byte length1, byte data);
  void SPIwriteBit(byte regAddress, byte bitNum, byte data);





  private:
  uint8_t _intPin1;
  uint8_t _intPin2;
  float _aRes;
};

#endif
