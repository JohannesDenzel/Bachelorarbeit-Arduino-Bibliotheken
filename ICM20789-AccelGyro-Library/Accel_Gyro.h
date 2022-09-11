#ifndef ACCEL_GYRO_H
#define ACCEL_GYRO_H

//#include "Arduino.h"

#include "ICM20789_Accel_Gyro_HAL.h"

  
  

class AccelGyro{

  private:
    
    ICM20789_AccelGyro_HAL accelGyro_HAL;

    //used for accel gyro values
    //old. get x,y,z with seperate functions
    //struct TYP_XYZ_AccelGyro{
    //  float x;

    //  float y;

    //  float z;
    //}; 
	

  
  public:

   


  //constructor
  AccelGyro(TYP_ICM20789_AccelGyro_config settings_p);

//---------------------------- Sensor Register Read Write Functions----------------------------

  //read one byte from Sensor Register
  //parameter: 
  //uint8_t register_address - address of Sensor Register
  //uint8_t *dataByte_Pointer - pointer to byte where the register data should be read into
  //
  //return:
  //
  uint8_t ReadOneRegister(uint8_t register_address, uint8_t *dataByte_Pointer);

  //write one byte to a sensor register
  //parameter:
  //uint8_t register_address - address of Sensor Register
  //uint8_t registerData - data that should be written into the Sensor register
  //
  //return:
  //
  uint8_t WriteOneRegister(uint8_t register_address, uint8_t registerData);


//--------------------- Debug Functions ----------------------------------
  //Not in UML
  
  //parameter:
  //int num - number to print as HEX
  //int precision - number of "half-bytes" 
  //example print 255 as HEX: num = 255, precision = 2
  void printHex(uint32_t num, uint32_t precision);

  //Wait for Sensor to answer
  //parameter
  //uint8_t register_Address - 
  //uint8_t *data_Address - 
  //long timeout_ms - 
  //
  //return:
  // 1 if successfull
  uint8_t WaitForRegister(uint8_t register_Address_p,uint8_t *data_Address_p, long timeout_ms_p);
  
  	//------------------------ Sensor Configuration -------------------------------------------------------
  //config Accel-Gyro Sensor
  //
  //return:
  //
  uint8_t ConfigAccelGyro(TYP_ICM20789_AccelGyro_config config_ICM20789);



//---------------------Get Data-------------------------------------------

  //get accel data in g. 1 g = 9.81 m/s^2
  //old
  //TYP_XYZ_AccelGyro GetAccel_xyz_g(void);

  float GetAccel_x_g(void);

  float GetAccel_y_g(void);

  float GetAccel_z_g(void);
  
  //get gyro data in dps
  //old
  //TYP_XYZ_AccelGyro GetGyro_xyz_dps(void);

  
  float GetGyro_x_dps(void);

  float GetGyro_y_dps(void);

  float GetGyro_z_dps(void);

   //raw Sensor reading. No formula for calculating actual temperature from accel gyro sensor, only from pressure sensor
  inline uint16_t GetDIE_Temperature_val(void){
    return accelGyro_HAL.GetDIE_Temperature_val_accelGyro();
  }


};//class AccelGyro{

#endif //#ifndef ACCEL_GYRO_H
