#ifndef ACCEL_GYRO_HAL_H
#define ACCEL_GYRO_HAL_H

//the sensor values might have a constant DC offset

#include "Arduino.h"
#include <Wire.h>

//Registers_adr must be included first then settings
#include "ICM20789_Registers_adr.h"
#include "ICM20789_RegisterSettings.h"


#define ICM_AG_ADR 0x68 //Accel, Gyro Sensor I2C Adrress
//#define ACCEL_FULL_SCALE 2 //full scale accel = +-2,4,8,16g
//#define GYRO_FULL_SCALE 250 //full scale gyro = +- 250,500,1000,2000 dps

//#ifndef ACCEL_FULL_SCALE
//  #define ACCEL_FULL_SCALE 16 //default value
//#endif

//config Settings of ICM20789 Accel-Gyro Sensor
  //argument to config function
struct TYP_ICM20789_AccelGyro_config{
    //Structure of the elements in this struct
    //  RegisterAdress (ICM29789_Registers_adr.h)
    //  optional: note
    //  setting;   //= example setting that works //explanation of example setting
  
    //ICM20789_SMPLRT_DIV
    uint8_t smplrt_div; //= 0;

    //ICM20789_CONFIG
    uint8_t icm_config; // = 0 | FIFO_FULL_REPL_OLD | DLPF_CFG_4; //gyro lowpass 20 Hz

    //ICM20789_GYRO_CONFIG
    uint8_t gyro_config; // = 0 | set_GYRO_FS_BITS | FCHOICE_B_00; //gyro lp filter enabeled

    //ICM20789_ACCEL_CONFIG
    //full scale accel = +-2,4,8,16g
    uint8_t accel_config; // = 0 | set_ACCEL_FS_BITS;

    //ICM20789_ACCEL_CONFIG2
    uint8_t accel_config2; // = 0 | DEC2_CFG_8 | ACCEL_FCHOICE_B_0 | ACCEL_DLPF_1; //average 8 samples, lowpass enabeled, 21 Hz lowpass

    //ICM20789_LP_MODE_CTRL
    uint8_t lp_mode_ctrl; // = 0; 

    //ICM20789_ACCEL_WOM_X_THR
    uint8_t accel_wom_x_thr; // = 0;

    //ICM20789_ACCEL_WOM_Y_THR
    uint8_t accel_wom_y_thr; // = 0; 

    //ICM20789_ACCEL_WOM_Z_THR
    uint8_t accel_wom_z_thr; // = 0; 

    //ICM20789_FIFO_EN
    uint8_t fifo_en; // = 0; //send nothing to fifo 

    //ICM20789_INT_PIN_CFG
    uint8_t int_pin_cfg; // = 0; //no int config 

    //ICM20789_INT_ENABLE
    uint8_t int_enable; // = 0; //all interrupts disabled 

    //ICM20789_ACCEL_INTEL_CTRL
    uint8_t accel_intel_ctrl; // = 0; //no Wake on motion logic

    //ICM20789_USER_CTRL
    uint8_t user_ctrl; // = 0; //fifo dmp disabeled

    //ICM20789_PWR_MGMT_1
    uint8_t pwr_mgmt_1; // = 0; //never sleep 

    //ICM20789_PWR_MGMT_2 
    uint8_t pwr_mgmt_2; // = 0 | LP_DISABLE; //disable lowpower

  }; //struct TYP_ICM20789_AccelGyro_config{



//#define ACCEL_IN_MS2 //accel value in m/s^2

class ICM20789_AccelGyro_HAL{
  private:
  
  float accel_fullScale_Faktor;
  float gyro_fullScale_Faktor;

  //uint8_t accel_FullScale_bits;
  //uint8_t gyro_FullScale_bits;
  

  

  
  public:

  //used for Accel, Gyro xyz raw data. 
  //each value is a 16 bit integer
  //old. use seperate get functions for x,y,z
  //struct TYP_XYZ_rawData_16Bit{
  //  int16_t x;

  //  int16_t y;

  //  int16_t z;
  //};


  

  ICM20789_AccelGyro_HAL(TYP_ICM20789_AccelGyro_config settings_p);

  //extern const float accel_fullScale_Faktor; //init in Accel_gyro.cpp
  //extern const float gyro_fullScale_Faktor; //init in Accel_gyro.cpp

  

  

//---------------------------- I2C ICM20789 Sensor Register Read Write Functions----------------------------
  //returns true (1) if successfull
  //parameter
  //uint8_t register_address - I2C register address (ICM20789_Registers_adr.h)
  //uint8_t *dataByte_Address - pointer to the byte where the data from the register should be read into
  //returns 1 if one byte was received. 0 if nothing was receivet. that can mean that there is problem or the sensor is busy
  uint8_t ICM_AG_readOneRegister(uint8_t register_address, uint8_t *dataByte_Address);

  //ICM_AG_writeOneRegister
  //Parameter: 
  //uint8_t register_address - Sensor I2C Register Address
  //uint8_t registerData - data byte to write into the register
  //
  //return error code 
  //0 .. success
  //1 .. length to long for buffer
  //2 .. address send, NACK received
  //3 .. data send, NACK received
  //4 .. other twi error (lost bus arbitration, bus error, ..)
  //5 .. timeout
  uint8_t ICM_AG_writeOneRegister(uint8_t register_address, uint8_t registerData);

//--------------------- Debug Functions ----------------------------------
  //https://forum.arduino.cc/t/print-hex-with-leading-zeroes/38203
  //print Hex value in the format 0xXX (the arduino ide just prints one or two letters, so in the ide 0x00 would be 0, 0x0A A, ...)
  //parameter
  //int num - number to print as hex
  //int precision - number of half-bytes (1 byte of data -> precision = 2)
  void printHex(uint32_t num, uint32_t precision);

  //Wait for Register
  //do not use in final code, the controller could also do sth else while waiting or sleep
  //parameter
  //uint8_t register_Address - I2C Register address
  //uint8_t *data_Address - pointer to the data byte that the data should be written inot
  //long timeout_ms - timeout in ms. until then this function will try to read from the sensor register
  uint8_t ICM_AG_WaitForRegister(uint8_t register_Address,uint8_t *data_Address, long timeout_ms);

//------------------------ ICM20789 Configuration -------------------------------------------------------

 //config the ICM20789 accel, gyro sensor. See datasheet and ICM20789_RegisterSettings.h
  //returns the sum of all error codes. if everything was fine this function returns 0. 
  //If the return is > 0 then try to find the responsible "ICM_AG_writeOneRegister" call and interpret the error (interpretation ICM_AG_writeOneRegister doku)
  uint8_t ConfigSensor(TYP_ICM20789_AccelGyro_config settings_p);

  //inline tells the compiler to return the variable directly instead of creating a new function
  //but the modern compilers ignore it and do the optimal solution instead (so it actually doesnt matter if i write inline or not)
  //the compiler can also optimize the code better if small (one liner) functions are in the header instead of the cpp file
  //in this case that is not really nescessary because the size and effiency of the code doesnt really matter while prototyping
  //https://stackoverflow.com/questions/1759300/when-should-i-write-the-keyword-inline-for-a-function-method
  
  //inline uint8_t Get_accel_FullScale_bits(void) {
  //  return accel_FullScale_bits;
  //}

  //inline uint8_t Get_gyro_FullScale_bits(void) {
  //  return gyro_FullScale_bits;
  //}

  inline float Get_accel_fullScale_Faktor(void){
    return accel_fullScale_Faktor;
  }
  
  inline float Get_gyro_fullScale_Faktor(void){
    return gyro_fullScale_Faktor;
  }
  

 

//----------------------Read Data------------------------------------------------
  //read accel data from the sensor by reading its registers containig the data
  //returns a struct with the x,y,z data where each value is a 16 bit integer. 
  //this 16 bit number is the raw sensor data. Not the value in g or m/s^2
  //old
  //TYP_XYZ_rawData_16Bit GetAccelData(void);

  int16_t GetAccel_x_raw(void);

  int16_t GetAccel_y_raw(void);

  int16_t GetAccel_z_raw(void);

  //read gyro data from the sensor by reading its registers containig the data
  //returns a struct with the x,y,z data where each value is a 16 bit integer. 
  //this 16 bit number is the raw sensor data. Not the value in dps
  //old
  //TYP_XYZ_rawData_16Bit GetGyroData(void);

  int16_t GetGyro_x_raw(void);

  int16_t GetGyro_y_raw(void);

  int16_t GetGyro_z_raw(void);

  
  //read temperature data from the sensor by reading its registers containig the data
  //returns a struct with the x,y,z data where each value is a 16 bit integer. 
  //this 16 bit number is the raw sensor data. Not the value °C
  //The values are not very stable. For this project the temperature of the accel, gyro senor is not needed
  //the temperature of the pressure sensor (on the same IC) can be used instead
  uint16_t GetDIE_Temperature_val_accelGyro(void);


}; //class ICM20789_AccelGyro_HAL{

#endif //#ifndef ACCEL_GYRO_HAL_H
