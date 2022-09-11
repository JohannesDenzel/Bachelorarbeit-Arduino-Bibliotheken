#include "Accel_Gyro.h"

//if other Sensor is used:
//Write HAL library for other sensor
//change corresponding functions in this library 
//that way only the contents of this libraries functions must be changed but not the main code

//constructor
//uint16_t accelFullscale = 2; //compiler doesnt likes it if i init these in the header 
//uint16_t gyroFullscale = 250;
AccelGyro::AccelGyro(TYP_ICM20789_AccelGyro_config settings_p): //const uint16_t fullScale_accel, const uint16_t fullScale_gyro) : 
  accelGyro_HAL(settings_p) {}; //fullScale_accel, fullScale_gyro){}; //set fullscale config bits, calculate fullscale faktor


//---------------------------- Sensor Register Read Write Functions----------------------------

//returns true (1) if successfull
  //parameter
  //uint8_t register_address - I2C register address (ICM20789_Registers_adr.h)
  //uint8_t *dataByte_Address - pointer to the byte where the data from the register should be read into
  //returns 1 if one byte was received. 0 if nothing was receivet. that can mean that there is problem or the sensor is busy
uint8_t AccelGyro::ReadOneRegister(uint8_t register_address, uint8_t *dataByte_Pointer){
  uint8_t err = accelGyro_HAL.ICM_AG_readOneRegister(register_address, dataByte_Pointer);
  return err;
}

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
uint8_t AccelGyro::WriteOneRegister(uint8_t register_address_p, uint8_t registerData_p){
  uint8_t err = accelGyro_HAL.ICM_AG_writeOneRegister(register_address_p, registerData_p);
  return err;
}


//--------------------- Debug Functions ----------------------------------

void AccelGyro::printHex(uint32_t num_p, uint32_t precision_p){
  //https://forum.arduino.cc/t/print-hex-with-leading-zeroes/38203
  //print Hex value in the format 0xXX (the arduino ide just prints one or two letters, so in the ide 0x00 would be 0, 0x0A A, ...)
  //parameter
  //uint32_t num - number to print as hex
  //uint32_t precision - number of half-bytes (1 byte of data -> precision = 2)
  accelGyro_HAL.printHex(num_p, precision_p);
  
}


uint8_t AccelGyro::WaitForRegister(uint8_t register_Address_p,uint8_t *data_Address_p, long timeout_ms_p){
  uint8_t err = accelGyro_HAL.ICM_AG_WaitForRegister(register_Address_p, data_Address_p, timeout_ms_p);
  return err;
}

//------------------------ Sensor Configuration -------------------------------------------------------


uint8_t AccelGyro::ConfigAccelGyro(TYP_ICM20789_AccelGyro_config config_ICM20789){

  //uint8_t gyro_FullScale_bits = accelGyro_HAL.Get_accel_FullScale_bits();
  //uint8_t accel_FullScale_bits = accelGyro_HAL.Get_gyro_FullScale_bits();

  //Settings
  //ICM20789_AccelGyro_HAL::TYP_ICM20789_AccelGyro_config config_ICM20789;
  
  //ICM20789_GYRO_CONFIG
  //config_ICM20789.gyro_config = config_ICM20789.gyro_config | gyro_FullScale_bits; //set fullscale

  //ICM20789_ACCEL_CONFIG
  //full scale accel = +-2,4,8,16g
  //config_ICM20789.accel_config = config_ICM20789.gyro_config | accel_FullScale_bits; //set full scale
  //Send Settings
  uint8_t err = accelGyro_HAL.ConfigSensor(config_ICM20789);
  
  return err;
  
} //uint8_t AccelGyro::ConfigAccelGyro(void)


//---------------------Get Data-------------------------------------------

//get accel data in g. 1 g = 9.81 m/s^2
//get data as struct (old)
/*
AccelGyro::TYP_XYZ_AccelGyro AccelGyro::GetAccel_xyz_g(void){
  AccelGyro::TYP_XYZ_AccelGyro accel_float;
  
  //accelGyro_HAL.
  
  //read accel data from the sensor by reading its registers containig the data
  //returns a struct with the x,y,z data where each value is a 16 bit integer. 
  //this 16 bit number is the raw sensor data. Not the value in g or m/s^2
  ICM20789_AccelGyro_HAL::TYP_XYZ_rawData_16Bit accel_xyz_16bit = accelGyro_HAL.GetAccelData();

  accel_float.x = (float) (accel_xyz_16bit.x * accelFullscale * 1.0/65536);
  accel_float.y = (float) (accel_xyz_16bit.y * accelFullscale * 1.0/65536);
  accel_float.z = (float) (accel_xyz_16bit.z * accelFullscale * 1.0/65536);
  
  return accel_float;
}
*/

float AccelGyro::GetAccel_x_g(void){
  int16_t raw16 = accelGyro_HAL.GetAccel_x_raw();
  float accelFullScale_Faktor = accelGyro_HAL.Get_accel_fullScale_Faktor();
  return (float) (raw16 * accelFullScale_Faktor);
}

float AccelGyro::GetAccel_y_g(void){
  int16_t raw16 = accelGyro_HAL.GetAccel_y_raw();
  float accelFullScale_Faktor = accelGyro_HAL.Get_accel_fullScale_Faktor();
  return (float) (raw16 * accelFullScale_Faktor);
}

float AccelGyro::GetAccel_z_g(void){
  int16_t raw16 = accelGyro_HAL.GetAccel_z_raw();
  float accelFullScale_Faktor = accelGyro_HAL.Get_accel_fullScale_Faktor();
  
  return (float) (raw16 * accelFullScale_Faktor);
}

//get gyro data in dps
//get xyz as struct (old)
/*
AccelGyro::TYP_XYZ_AccelGyro AccelGyro::GetGyro_xyz_dps(void){
  AccelGyro::TYP_XYZ_AccelGyro gyro_float;

  //read accel data from the sensor by reading its registers containig the data
  //returns a struct with the x,y,z data where each value is a 16 bit integer. 
  //this 16 bit number is the raw sensor data. Not the value in g or m/s^2
  ICM20789_AccelGyro_HAL::TYP_XYZ_rawData_16Bit gyro_xyz_16bit = accelGyro_HAL.GetGyroData();

  gyro_float.x = (float) (gyro_xyz_16bit.x * gyroFullscale * 1.0/65536);
  gyro_float.y = (float) (gyro_xyz_16bit.y * gyroFullscale * 1.0/65536);
  gyro_float.z = (float) (gyro_xyz_16bit.z * gyroFullscale * 1.0/65536);
  
  return gyro_float;
  
}
*/

float AccelGyro::GetGyro_x_dps(void){
  int16_t raw16 = accelGyro_HAL.GetGyro_x_raw();
  float gyroFullScale_Faktor = accelGyro_HAL.Get_gyro_fullScale_Faktor();
  return (float) (raw16 * gyroFullScale_Faktor);
}

float AccelGyro::GetGyro_y_dps(void){
  int16_t raw16 = accelGyro_HAL.GetGyro_y_raw();
  float gyroFullScale_Faktor = accelGyro_HAL.Get_gyro_fullScale_Faktor();
  return (float) (raw16 * gyroFullScale_Faktor);
}

float AccelGyro::GetGyro_z_dps(void){
  int16_t raw16 = accelGyro_HAL.GetGyro_y_raw();
  float gyroFullScale_Faktor = accelGyro_HAL.Get_gyro_fullScale_Faktor();
  return (float) (raw16 * gyroFullScale_Faktor);
}

