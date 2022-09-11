#include "ICM20789_Pressure.h"

ICM20789_Pressure::ICM20789_Pressure(const uint8_t readMode_ID):
    crc(ICM20789_P_CRC_GENERATOR, ICM20789_P_CRC_INIT)
{    

  meas_req_f = 0; //no measurement requested

  //init raw data struct
  rawDataStruct = {0,0,0, 0,0,0, 0,0,0};

  //init received raw data array
  //recRawData = {0,0,0, 0,0,0, 0,0,0};

  pressureVal = 0;
  temperature_C = 0;
  
  rawDataAvailable = 0;
  dataAvailable = 0;
  
  modeID = readMode_ID;
  if(readMode_ID == PRESSURE_FIRST_ULN){
   readMode_H = PRESSURE_FIRST_ULN_H;
   readMode_L = PRESSURE_FIRST_ULN_L;
  }else if(readMode_ID == PRESSURE_FIRST_LN){
    readMode_H = PRESSURE_FIRST_LN_H;
    readMode_L = PRESSURE_FIRST_LN_L;
  }else if(readMode_ID == PRESSURE_FIRST_N){
   readMode_H = PRESSURE_FIRST_N_H;
   readMode_L = PRESSURE_FIRST_N_L;
  }else{
   readMode_H = PRESSURE_FIRST_LP_H;
   readMode_L = PRESSURE_FIRST_LP_L;
  }
  
}//ICM20789_Pressure_HAL::ICM20789_Pressure_HAL(const uint8_t readMode_ID)

void ICM20789_Pressure::ConfigSensor(void){
  //crc settings
  crc.Init_crcTable_CRC8();
  //mode settings 
  
   
}

//request data from sensor
//after request the sensor can be "asked" for data and it will answer with NACK if it didnt finish the measurement
void ICM20789_Pressure::StartMeasurement(void){
  // if no measurement was requested yet -> request measurement
  // measurement cant be requested in every loop. Only after the sensor answered a new measurement can be requested

  //new measurement requested -> set availability flags to 0
  rawDataAvailable = 0;
  dataAvailable = 0;
  
  if (meas_req_f == 0){
    Wire.beginTransmission(ICM20789_P_ADR);

    //actually transmit adress and data
    //argument false = send start, true send stop, output 0 = success, 1,2,3,4 = error see doku of Wire Lib
    //send request
    Wire.write(readMode_H); //high byte of read mode
    Wire.write(readMode_L); //low byte of read mode
    uint8_t err = Wire.endTransmission(true); 
    meas_req_f = 1;
    //Serial.println("Err = " + String(err));
  }

} //uint8_t ICM20789_Pressure_HAL::StartMeasurement(void)


//"I2C_read_Data" writes received data to the buffer array given with buffer_adress
//Parameter:
//uint8_t i2c_adress_7Bit - 7 bit long address of the i2c device
//uint8_t buffer_size - buffer size of the i2c data buffer that the data should be written to
//uint8_t *buffer_adress - pointer to the first element of the buffer array. The recived bytes will be written to this array
//the max number of bytes that can be received is given by the parameter buffer_size, less bytes than that is possible more not.
//
//returns:
//uint8_t rec_bytes - number of received bytes, if 0 buffer is empty
uint8_t ICM20789_Pressure::I2C_read_Data(uint8_t i2c_adress_7Bit, uint8_t buffer_size, uint8_t *buffer_adress){
  
  uint8_t rec_bytes = Wire.requestFrom(i2c_adress_7Bit, buffer_size); 
    //Serial.println("rec_bytes = " + String(rec_bytes)); //debug
    if (rec_bytes > 0) {
      uint8_t i = 0;
      while (Wire.available()){
        *(buffer_adress + i) = Wire.read();
        //Serial.println("rec data " + String(i) + ": " + String(icm_pT_rec_data[i]));
        i++;
      }
    }

    return rec_bytes; //number of received bytes, if 0 buffer will be empty
}

void ICM20789_Pressure::WriteDataArrayToStruct(uint8_t rawDatArr[ICM20789_P_DATA_BUFFER_SIZE]){
  if(
         (modeID == PRESSURE_FIRST_ULN) 
      || (modeID == PRESSURE_FIRST_LN)
      || (modeID == PRESSURE_FIRST_N)
      || (modeID == PRESSURE_FIRST_LP)
    )
  {
    rawDataStruct.pressure_MMSB = rawDatArr[0];
    rawDataStruct.pressure_MLSB = rawDatArr[1];
    rawDataStruct.pressure_MCRC = rawDatArr[2];
    rawDataStruct.pressure_LMSB = rawDatArr[3];
    rawDataStruct.pressure_LLSB = rawDatArr[4];
    rawDataStruct.pressure_LCRC = rawDatArr[5];

    rawDataStruct.temperature_MSB = rawDatArr[6];
    rawDataStruct.temperature_LSB = rawDatArr[7];
    rawDataStruct.temperature_CRC = rawDatArr[8];
  }
}

//Only update struct if new data was received
//return 1 if successful
uint8_t ICM20789_Pressure::UpdateRawDataStruct(void){
  uint8_t recRawData[ICM20789_P_DATA_BUFFER_SIZE] = {0,0,0, 0,0,0, 0,0,0};
  uint8_t success = 0;
  //request bytes from Sensor
  //if sensor is not finished with measurement it will answer NACK on request. In this case Wire.request returns 0
  //try to read data from sensor. If the sensor is busy doing a measurement it will respond with NAC and rec_bytes will be 0
  uint8_t rec_bytes = I2C_read_Data(ICM20789_P_ADR, ICM20789_P_DATA_BUFFER_SIZE, &recRawData[0]);
  //if reading was successful, write data to struct and process it
  
  if (rec_bytes > 0){
    WriteDataArrayToStruct(recRawData);
    meas_req_f = 0; //a new measurement can be requested
    success = 1;
  }
  return success;
}//ICM20789_Pressure_HAL::UpdateRawDataStruct(void)


   
    

//Only update data (pressure, temperature) using struct if there is no CRC error
uint8_t ICM20789_Pressure::UpdateData(void){
  uint8_t errCount = 0;
  uint8_t crcErr = 1;
  rawDataAvailable = UpdateRawDataStruct();

  if(rawDataAvailable == 1){
    //struct_p_T_raw.pressure_MMSB += 1; //debug forced error -> works
	crcErr_found = 0;
    errCount = 0; 
    crcErr = Error_Check_CRC_sensorData(); //struct_p_T_raw); //number of crc errors in the recieved data bytes (9 bytes = 6 data, 3 crc)

    if(crcErr > 0){
      //transmission Error occoured!
      errCount += crcErr;
	  crcErr_found = 1;
      //Serial.println("Err Count " + String(errCount));
    }else{ 
      //No transmission error occoured
      //extract pressure_val, temperature_val from struct, calc temperature and print    
      pressureVal = RawDataStruct_To_PressureVal();//struct_p_T_raw);
      uint16_t temperature_val = RawDataStruct_To_Temperature_val(); //struct_p_T_raw);
      temperature_C = Convert_16Bit_val_To_Temperature_C(temperature_val);
      dataAvailable = 1;

    } //if(crcErr > 0){
  }

return dataAvailable; //return private Variable that is available in whole class but not outside. With return it is made available outside of the class.
}  
  

//only the first 9 bytes will be read into the struct
/*
ICM20789_Pressure_HAL::TYP_rawSensorData ICM20789_Pressure_HAL::RawDataArr_to_Struct(uint8_t icm_pT_data[ICM_DATA_BUFFER_SIZE]){
  ICM20789_Pressure_HAL::TYP_rawSensorData data_raw = {0,0,0,0, 0,0,0,0, 0};

  if((modeID == PRESSURE_FIRST_ULN) 
  || (modeID == PRESSURE_FIRST_N)){
    data_raw.pressure_MMSB = icm_pT_data[0];
    data_raw.pressure_MLSB = icm_pT_data[1];
    data_raw.pressure_MCRC = icm_pT_data[2];
    data_raw.pressure_LMSB = icm_pT_data[3];
    data_raw.pressure_LLSB = icm_pT_data[4];
    data_raw.pressure_LCRC = icm_pT_data[5];

    data_raw.temperature_MSB = icm_pT_data[6];
    data_raw.temperature_LSB = icm_pT_data[7];
    data_raw.temperature_CRC = icm_pT_data[8];
  }
  
  return data_raw;
}
*/


//ICM_p_first_Struct_To_Pressure(TYP_p_first_raw data_struct)
//This function reads out the bytes containing the pressure data from the struct with the raw data and combines these bytes to a 32 Bit Value
// In future Versions the pressure bytes can be verified using the CRC bytes from the struct, if CRC fails the function should return 0.
//
//Parameter: 
// TYP_p_first_raw data_struct = structure of the Type TYP_p_first_raw containing the received data
//
//Output:
//24 Bit Value that corresponds to the measured pressure. 
uint32_t ICM20789_Pressure::RawDataStruct_To_PressureVal(void){ //ICM20789_Pressure_HAL::TYP_rawSensorData data_struct){
  //Dataheet v1.4 p 51
  //Therefore, for retrieving the ADC pressure value, LLSB must be disregarded: 
  //pdout = MMSB ≪ 16 | MLSB ≪ 8| LMSB.
  uint32_t pressure_result = 0;
  
  uint32_t pressure_MMSB = 0;
  uint32_t pressure_MLSB = 0;
  uint32_t pressure_LMSB = 0;
 

  pressure_MMSB = rawDataStruct.pressure_MMSB << 16;
  //masking because without mask pressure_result can become bigger than 2^24
  //i suspect that the arduino doesnt fill the first byte of uint32_t with 0, or sth like that
  pressure_MMSB = pressure_MMSB & 0x00FF0000; 
  //Serial.println("MMSB8 = " + String(data_struct.pressure_MMSB)); //debug
  //Serial.println("MMSB32 = " + String(pressure_MMSB)); //debug
  
  pressure_MLSB = rawDataStruct.pressure_MLSB << 8;
  pressure_MLSB = pressure_MLSB & 0x0000FF00;
  //Serial.println("MLSB8 = " + String(data_struct.pressure_MLSB)); //debug
  //Serial.println("MLSB32 = " + String(pressure_MLSB)); //debug
  
  pressure_LMSB = rawDataStruct.pressure_LMSB;
  pressure_LMSB = pressure_LMSB & 0x000000FF;
  //Serial.println("LMSB8 = " + String(data_struct.pressure_LMSB)); //debug
  //Serial.println("LMSB32 = " + String(pressure_LMSB)); //debug
  
  pressure_result = pressure_MMSB | pressure_MLSB | pressure_LMSB;
  pressure_result = pressure_result & 0x00FFFFFF;
  
  return pressure_result;
}


//read out the 2 bytes of temperature data from struct and convert them to actual temperature val
uint16_t ICM20789_Pressure::RawDataStruct_To_Temperature_val(void){ //ICM20789_Pressure_HAL::TYP_rawSensorData data_struct){
  uint16_t temperature_MSB = 0;
  uint16_t temperature_LSB = 0;
  float result = 0;

  temperature_MSB = rawDataStruct.temperature_MSB << 8;
  temperature_MSB = temperature_MSB & 0xFF00;
  temperature_LSB = rawDataStruct.temperature_LSB;
  temperature_LSB = temperature_LSB & 0x00FF;
  result = (temperature_MSB | temperature_LSB);

  return result;
}

float ICM20789_Pressure::Convert_16Bit_val_To_Temperature_C(uint16_t value16){
  //Datasheet page 53
  //T = - 45°C + (175°C / 2^16) x t_dout
  return -45.0 + (175.0 / 65536.0) * value16; 
}

//-----------------------CRC check------------------------------------------

//The ICM20789 pressure Sensor always sends 2 data bytes and one crc byte, the crc of all 3 bytes
//sould be 0. If not there was a transmission error
//So this functions calculates the crc of 2 data bytes (High, low byte) + 1 crc byte
//and returns 0 if success, 1 at fail
//parameter: high, low data byte, received crc byte
uint8_t ICM20789_Pressure::Error_CRC_2bData_1bCRC(uint8_t data_H, uint8_t data_L, uint8_t rec_crc){
  uint8_t err = 0;
  uint8_t checkArray[3]; //byte array for 2 Data bytes + received crr byte

  checkArray[0] = data_H;
  checkArray[1] = data_L;
  checkArray[2] = rec_crc;

  //first check: MMLSB, MLSB of pressure data
  //if CRC of data + received CRC != 0 then there is at leas one wrong bit in rec data or rec CRC
  if (crc.Compute_CRC8(&checkArray[0], 3) != 0){
    err = 1;
  }

  return err;
}


//checks the Struct with the received Data using the CRC algorithm
//return the number of CRC errors in this struct, -> if return is 0 everything is fine, else there is at least one error
uint8_t ICM20789_Pressure::Error_Check_CRC_sensorData(void){ //ICM20789_Pressure_HAL::TYP_rawSensorData structRawData){
  uint8_t errCount = 0;

  uint8_t data_H = rawDataStruct.pressure_MMSB;
  uint8_t data_L = rawDataStruct.pressure_MLSB;
  uint8_t rec_crc = rawDataStruct.pressure_MCRC;

  errCount += Error_CRC_2bData_1bCRC(data_H, data_L, rec_crc);

  data_H = rawDataStruct.pressure_LMSB;
  data_L = rawDataStruct.pressure_LLSB;
  rec_crc = rawDataStruct.pressure_LCRC;

  errCount += Error_CRC_2bData_1bCRC(data_H, data_L, rec_crc);
  
  data_H = rawDataStruct.temperature_MSB;
  data_L = rawDataStruct.temperature_LSB;
  rec_crc = rawDataStruct.temperature_CRC;

  errCount += Error_CRC_2bData_1bCRC(data_H, data_L, rec_crc);

  return errCount;
} //uint8_t Error_Check_CRC_sensorData(void)


uint32_t ICM20789_Pressure::Get_pressureVal(void){
  return pressureVal;
}

float ICM20789_Pressure::Get_temperature_C(void){
  return temperature_C;
}

//call when UpdateData returned 0 to see if its because of an crc error
uint8_t ICM20789_Pressure::Get_crcErr_found(void){
	   return crcErr_found;
  }
