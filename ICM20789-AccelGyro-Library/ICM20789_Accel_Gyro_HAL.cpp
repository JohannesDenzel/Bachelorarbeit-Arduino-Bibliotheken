#include "ICM20789_Accel_Gyro_HAL.h"



  


//constructor
//p for parameter
ICM20789_AccelGyro_HAL::ICM20789_AccelGyro_HAL(TYP_ICM20789_AccelGyro_config settings){ //uint16_t accelFullscale_p, uint16_t gyroFullScale_p){
  
  
  const uint8_t accel_FS_BitsMask = 0b00011000;
  uint8_t accel_Fullscale_bits = settings.accel_config & accel_FS_BitsMask;
  
  const uint8_t gyro_FS_BitsMask = 0b00011000;
  uint8_t gyro_Fullscale_bits = settings.gyro_config & accel_FS_BitsMask;
  
  uint16_t accelFullscale = 0;
  uint16_t gyroFullscale = 0;
  
  
  if (accel_Fullscale_bits == ACCEL_FS_SEL_2G){
    accelFullscale = 2;
  }else if (accel_Fullscale_bits == ACCEL_FS_SEL_4G){
	accelFullscale = 4;
    
  }else if (accel_Fullscale_bits == ACCEL_FS_SEL_8G){
	accelFullscale = 8;
    
  }else if (accel_Fullscale_bits == ACCEL_FS_SEL_16G){
	accelFullscale = 16;
    
  }else {
    //accel_FullScale_bits = ACCEL_FS_SEL_16G;
    accelFullscale = 16; 
  }


  if ( gyro_Fullscale_bits == GYRO_FS_SEL_250_DPS){
    gyroFullscale = 250;
  }else if (gyro_Fullscale_bits == GYRO_FS_SEL_500_DPS){
    gyroFullscale = 500;  
  }else if ( gyro_Fullscale_bits == GYRO_FS_SEL_1000_DPS){
    gyroFullscale = 1000;
  }else if (gyro_Fullscale_bits == GYRO_FS_SEL_2000_DPS){
    gyroFullscale = 2000;
  }else{
    //gyro_FullScale_bits == GYRO_FS_SEL_2000_DPS;
    gyroFullscale = 2000; 
  }
	

	//Fullscale = 16 -> faktor = 2*16 = 32
  accel_fullScale_Faktor = 2 * accelFullscale * 1.0 / 32768.0; //the accel sensors have a constant offset if that doesnt go into the calculation the values might be wrong 
  gyro_fullScale_Faktor = 2 * gyroFullscale * 1.0 / 32768.0; //the gyro sensors have a constant offset if that doesnt go into the calculation the values might be wrong 

	

}

//---------------------------- I2C ICM20789 Sensor Register Read Write Functions----------------------------
//returns true (1) if successfull
//parameter
//uint8_t register_address - I2C register address (ICM20789_Registers_adr.h)
//uint8_t *dataByte_Address - pointer to the byte where the data from the register should be read into
//returns 1 if one byte was received. 0 if nothing was receivet. that can mean that there is problem or the sensor is busy
uint8_t ICM20789_AccelGyro_HAL::ICM_AG_readOneRegister(uint8_t register_address, uint8_t *dataByte_Address){
   uint8_t err;
   uint8_t r_bytes;
   uint8_t success = 0;

  //Wire Library:
  //https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/Wire.cpp
  
   
   Wire.beginTransmission(ICM_AG_ADR);
  //write register adress
  Wire.write(register_address);
  
  err = Wire.endTransmission(false); //send reister adress and start instead of stop https://www.arduino.cc/en/Reference/WireEndTransmission
  //Wire.endTransmission uses the TWI Library and returns the result of twi_writeTo doku: https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/utility/twi.c
  //Output   0 .. success
  //         1 .. length to long for buffer
  //         2 .. address send, NACK received
  //         3 .. data send, NACK received
  //         4 .. other twi error (lost bus arbitration, bus error, ..)
  //         5 .. timeout
  
  //Serial.println(err);
  
  //master transmits start signal followed by slave adress and read bit
  r_bytes = Wire.requestFrom(ICM_AG_ADR, 1);    // request 1 bytes from slave device 
  
  //ICM sends ACK signal and Data 
  //Wire.available() is > 0 when r_bytes is > 0 and stays > 0 until all bytes are read
  while (Wire.available()) { // slave may send less than requested
    *dataByte_Address = Wire.read(); // receive one byte
    success = 1;
  }

  //debug
  //Serial.print("reg data: ");
  //Serial.println(data, HEX);         // print the data
  //Serial.println("succsess: " + String(succsess));
  //Serial.println("rbytes: " + String(r_bytes)); 
  //Serial.println("adrErr: " + String(err));

  return success;
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
uint8_t ICM20789_AccelGyro_HAL::ICM_AG_writeOneRegister(uint8_t register_address, uint8_t registerData){
  uint8_t err;
  uint8_t success = 0;
   
   Wire.beginTransmission(ICM_AG_ADR);
  //write register address
  Wire.write(register_address);
  
  //write register Data
  Wire.write(registerData);
  
  err = Wire.endTransmission(true); //send stop after last transmitted byte

  return err;
} //uint8_t ICM_AG_writeOneRegister(uint8_t register_address, uint8_t registerData)


//--------------------- Debug Functions ----------------------------------
//https://forum.arduino.cc/t/print-hex-with-leading-zeroes/38203
//print Hex value in the format 0xXX (the arduino ide just prints one or two letters, so in the ide 0x00 would be 0, 0x0A A, ...)
//parameter
//int num - number to print as hex
//int precision - number of half-bytes (1 byte of data -> precision = 2)
void ICM20789_AccelGyro_HAL::printHex(uint32_t num, uint32_t precision) {
      char tmp[16];
      char format[128];

      sprintf(format, "0x%%.%dX", precision);

      sprintf(tmp, format, num);
      Serial.print(tmp);
}


//Wait for Register
//do not use in final code, the controller could also do sth else while waiting or sleep
//parameter
//uint8_t register_Address - I2C Register address
//uint8_t *data_Address - pointer to the data byte that the data should be written inot
//long timeout_ms - timeout in ms. until then this function will try to read from the sensor register
uint8_t ICM20789_AccelGyro_HAL::ICM_AG_WaitForRegister(uint8_t register_Address,uint8_t *data_Address, long timeout_ms){
  uint8_t success = 0;
  
  //wait for success or timeout
  long start = millis();
  while(millis() - start < timeout_ms){
    success = ICM_AG_readOneRegister(register_Address, data_Address);
    if(success > 0){
      break;
    }
  }
  return success;
}

//------------------------ ICM20789 Configuration -------------------------------------------------------

//config the ICM20789 accel, gyro sensor. See datasheet and ICM20789_RegisterSettings.h
//returns the sum of all error codes. if everything was fine this function returns 0. 
//If the return is > 0 then try to find the responsible "ICM_AG_writeOneRegister" call and interpret the error (interpretation ICM_AG_writeOneRegister doku)
uint8_t ICM20789_AccelGyro_HAL::ConfigSensor(TYP_ICM20789_AccelGyro_config settings_p){
  uint8_t err = 0;
  //uint8_t checkWrite; //debug

//write 0 | Settings_bit_1 | ...

//ICM20789_SMPLRT_DIV
err += ICM_AG_writeOneRegister(ICM20789_SMPLRT_DIV, settings_p.smplrt_div); 

//ICM20789_CONFIG

err += ICM_AG_writeOneRegister(ICM20789_CONFIG, settings_p.icm_config);

//ICM20789_GYRO_CONFIG
err += ICM_AG_writeOneRegister(ICM20789_GYRO_CONFIG, settings_p.gyro_config);

//ICM20789_ACCEL_CONFIG
//full scale accel = +-2,4,8,16g
err += ICM_AG_writeOneRegister(ICM20789_ACCEL_CONFIG, settings_p.accel_config);

//ICM20789_ACCEL_CONFIG2
err = ICM_AG_writeOneRegister(ICM20789_ACCEL_CONFIG2, settings_p.accel_config2);

//ICM20789_LP_MODE_CTRL 
err += ICM_AG_writeOneRegister(ICM20789_LP_MODE_CTRL, settings_p.lp_mode_ctrl);

//ICM20789_ACCEL_WOM_X_THR
err += ICM_AG_writeOneRegister(ICM20789_ACCEL_WOM_X_THR, settings_p.accel_wom_x_thr);

//ICM20789_ACCEL_WOM_Y_THR
err += ICM_AG_writeOneRegister(ICM20789_ACCEL_WOM_Y_THR, settings_p.accel_wom_y_thr);

//ICM20789_ACCEL_WOM_Z_THR
err += ICM_AG_writeOneRegister(ICM20789_ACCEL_WOM_Z_THR, settings_p.accel_wom_z_thr);

//ICM20789_FIFO_EN 
err += ICM_AG_writeOneRegister(ICM20789_FIFO_EN, settings_p.fifo_en);

//ICM20789_INT_PIN_CFG
err += ICM_AG_writeOneRegister(ICM20789_INT_PIN_CFG, settings_p.int_pin_cfg);

//ICM20789_INT_ENABLE
err += ICM_AG_writeOneRegister(ICM20789_INT_ENABLE, settings_p.int_enable);

//ICM20789_ACCEL_INTEL_CTRL
err += ICM_AG_writeOneRegister(ICM20789_ACCEL_INTEL_CTRL, settings_p.accel_intel_ctrl);

//ICM20789_USER_CTRL
err += ICM_AG_writeOneRegister(ICM20789_USER_CTRL, settings_p.user_ctrl);

//ICM20789_PWR_MGMT_1
err += ICM_AG_writeOneRegister(ICM20789_PWR_MGMT_1, settings_p.pwr_mgmt_1);

//ICM20789_PWR_MGMT_2 
err += ICM_AG_writeOneRegister(ICM20789_PWR_MGMT_2 , settings_p.pwr_mgmt_2);

return err;
}

//----------------------Read Data------------------------------------------------
//read accel data from the sensor by reading its registers containig the data
//returns a struct with the x,y,z data where each value is a 16 bit integer. 
//this 16 bit number is the raw sensor data. Not the value in g or m/s^2
//old. use seperate functions for x,y,z instead
/*
ICM20789_AccelGyro_HAL::TYP_XYZ_rawData_16Bit ICM20789_AccelGyro_HAL::GetAccelData(void){

  ICM20789_AccelGyro_HAL::TYP_XYZ_rawData_16Bit data16;

  uint8_t successCount = 0;

  uint8_t x_H = 0;
  uint8_t x_L = 0;

  uint8_t y_H = 0;
  uint8_t y_L = 0;

  uint8_t z_H = 0;
  uint8_t z_L = 0;

  

  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_XOUT_H, &x_H);
  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_XOUT_L, &x_L);
  
  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_YOUT_H, &y_H);
  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_YOUT_L, &y_L);

  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_ZOUT_H, &z_H);
  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_ZOUT_L, &z_L);

  //Serial.println("SuccesCount (expected = 6) " + String(successCount)); //debug
  
  
  data16.x = x_L | (x_H << 8);
  data16.y = y_L | (y_H << 8);
  data16.z = z_L | (z_H << 8);
  
  return data16;
}
*/

int16_t ICM20789_AccelGyro_HAL::GetAccel_x_raw(void){

  int16_t raw16 = 0;
  
  uint8_t successCount = 0;

  uint8_t x_H = 0;
  uint8_t x_L = 0;
  
  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_XOUT_H, &x_H);
  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_XOUT_L, &x_L);

  //Serial.println("SuccesCount (expected = 2) " + String(successCount)); //debug

  raw16 = x_L | (x_H << 8);
  
  return raw16;
}

int16_t ICM20789_AccelGyro_HAL::GetAccel_y_raw(void){
  int16_t raw16 = 0;
  
  uint8_t successCount = 0;

  uint8_t y_H = 0;
  uint8_t y_L = 0;
  
  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_YOUT_H, &y_H);
  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_YOUT_L, &y_L);

  //Serial.println("SuccesCount (expected = 2) " + String(successCount)); //debug

  raw16 = y_L | (y_H << 8);
  
  return raw16;
}

int16_t ICM20789_AccelGyro_HAL::GetAccel_z_raw(void){
  int16_t raw16 = 0;
  
  uint8_t successCount = 0;

  uint8_t z_H = 0;
  uint8_t z_L = 0;
  
  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_ZOUT_H, &z_H);
  successCount += ICM_AG_readOneRegister(ICM20789_ACCEL_ZOUT_L, &z_L);

  //Serial.println("SuccesCount (expected = 2) " + String(successCount)); //debug

  raw16 = z_L | (z_H << 8);
  
  return raw16;
}

//read gyro data from the sensor by reading its registers containig the data
//returns a struct with the x,y,z data where each value is a 16 bit integer. 
//this 16 bit number is the raw sensor data. Not the value in dps
//old
/*
ICM20789_AccelGyro_HAL::TYP_XYZ_rawData_16Bit ICM20789_AccelGyro_HAL::GetGyroData(void){

  ICM20789_AccelGyro_HAL::TYP_XYZ_rawData_16Bit data16;

  uint8_t successCount = 0;

  uint8_t x_H = 0;
  uint8_t x_L = 0;

  uint8_t y_H = 0;
  uint8_t y_L = 0;

  uint8_t z_H = 0;
  uint8_t z_L = 0;

  

  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_XOUT_H, &x_H);
  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_XOUT_L, &x_L);
  
  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_YOUT_H, &y_H);
  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_YOUT_L, &y_L);

  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_ZOUT_H, &z_H);
  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_ZOUT_L, &z_L);

  //Serial.println("SuccesCount (expected = 6) " + String(successCount)); //debug
  
  
  data16.x = x_L | (x_H << 8);
  data16.y = y_L | (y_H << 8);
  data16.z = z_L | (z_H << 8);
  
  return data16;
}
*/

int16_t ICM20789_AccelGyro_HAL::GetGyro_x_raw(void){
  int16_t raw16 = 0;
  
  uint8_t successCount = 0;

  uint8_t x_H = 0;
  uint8_t x_L = 0;
  
  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_XOUT_H, &x_H);
  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_XOUT_L, &x_L);

  //Serial.println("SuccesCount (expected = 2) " + String(successCount)); //debug

  raw16 = x_L | (x_H << 8);
  
  return raw16;
} //int16_t ICM20789_AccelGyro_HAL::GetGyro_x_raw(void)

int16_t ICM20789_AccelGyro_HAL::GetGyro_y_raw(void){
  int16_t raw16 = 0;
  
  uint8_t successCount = 0;

  uint8_t y_H = 0;
  uint8_t y_L = 0;
  
  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_YOUT_H, &y_H);
  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_YOUT_L, &y_L);

  //Serial.println("SuccesCount (expected = 2) " + String(successCount)); //debug

  raw16 = y_L | (y_H << 8);
  
  return raw16;
}//int16_t ICM20789_AccelGyro_HAL::GetGyro_y_raw(void)

int16_t ICM20789_AccelGyro_HAL::GetGyro_z_raw(void){
  int16_t raw16 = 0;
  
  uint8_t successCount = 0;

  uint8_t z_H = 0;
  uint8_t z_L = 0;
  
  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_ZOUT_H, &z_H);
  successCount += ICM_AG_readOneRegister(ICM20789_GYRO_ZOUT_L, &z_L);

  //Serial.println("SuccesCount (expected = 2) " + String(successCount)); //debug

  raw16 = z_L | (z_H << 8);
  
  return raw16;
} //int16_t ICM20789_AccelGyro_HAL::GetGyro_z_raw(void)

//read temperature data from the sensor by reading its registers containig the data
//returns a struct with the x,y,z data where each value is a 16 bit integer. 
//this 16 bit number is the raw sensor data. Not the value °C
//The values are not very stable. For this project the temperature of the accel, gyro senor is not needed
//the temperature of the pressure sensor (on the same IC) can be used instead

uint16_t ICM20789_AccelGyro_HAL::GetDIE_Temperature_val_accelGyro(){
  uint8_t t_H8 = 0;
  uint8_t t_L8 = 0;
  uint8_t successCount = 0;
  successCount += ICM_AG_readOneRegister(ICM20789_TEMP_OUT_H, &t_H8);
  successCount += ICM_AG_readOneRegister(ICM20789_TEMP_OUT_L, &t_L8);
  //Serial.println(successCount); //debug. expected 2. has expected value
  //Serial.println(String(t_H8, BIN) + " " + String(t_L8, BIN));
  //problem: t_H is allways 254 or 253 and sometimes suddenly 0, t_L moves along with temperature increase or falling but seems to overflow sometimes
  //success is in each case the expected value of 2.
  //maybe i broke the sensor a little bit while soldering?
  //Serial.println(String(t_H8) + " " + String(t_L8)); // + " " + String(successCount * 100));

  uint16_t t_H16 = t_H8;
  uint16_t t_L16 = t_L8;
  
  uint16_t t_val = t_L16 | (t_H16 << 8);
  

  return t_val;
}






