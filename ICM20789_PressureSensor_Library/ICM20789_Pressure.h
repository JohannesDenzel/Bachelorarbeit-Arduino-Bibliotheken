#ifndef ICM20789_Pressure_h
#define ICM20789_Pressure_h

#include "ICM20789_PressureSettings.h"
#include "CRC.h"
#include "Arduino.h"
#include <Wire.h>

//Sensor Request Commands

//Temperature (T) first modes are not impemented in this library
/*
OPERATION_MODE        | TRANSMIT_T_FIRST | TRANSMIT_P_FIRST
LowPower (LP)         | 0x609C           | 0x401A
Normal (N)            | 0x6825           | 0x48A3
LowNoise (LN)         | 0x70DF           | 0x5059
Ultra-Low Noise (ULN) | 0x7866           | 0x58E0
*/

//Data Modes

//PRESSURE_FIRST_ULN 0
#define PRESSURE_FIRST_ULN_H 0x58 //High byte of pressure first ultra low noise mode {Wire.write(0x58); Wire.write(0xE0);} //request 9 byte
#define PRESSURE_FIRST_ULN_L 0xE0 //Low byte

//Pressure first Low Noise mode
//PRESSURE_FIRST_LN 1
#define PRESSURE_FIRST_LN_H 0x50
#define PRESSURE_FIRST_LN_L 0x59

//Pressure first Normal mode
//PRESSURE_FIRST_N 2
#define PRESSURE_FIRST_N_H 0x48  //pressure first normal mode high byte {Wire.write(0x48); Wire.write(0xA3);} //normal mode pressure first
#define PRESSURE_FIRST_N_L 0xA3  //Low byte

//Pressure first Low Power mode
//PRESSURE_FIRST_LP 3
#define PRESSURE_FIRST_LP_H 0x40
#define PRESSURE_FIRST_LP_L 0x1A


#define ICM20789_P_DATA_BUFFER_SIZE 9 //ICM_DATA_BUFFER_SIZE 9 //9 bytes buffer. Must be 9 bytes or more!
#define ICM20789_P_ADR 0x63//ICM_P_ADR 0x63 //pressure Sensor I2C adress
//Sensor specific CRC Settings
//#define CRC_NR_OF_BYTES 2 //Number of bytes per message eg. 1 CRC Byte per NR_OF_BYTES
#define ICM20789_P_CRC_GENERATOR 0x31//CRC_GENERATOR 0x31 //0b(1)0011 0001 the first 1 is discarded because its always 1.
#define ICM20789_P_CRC_INIT 0xFF//CRC_INIT 0xFF

class ICM20789_Pressure{

  private:

//-------------private variables-------------------------------
    uint8_t meas_req_f; //measurement requested Flag
    
    CRC_Library crc; 

    uint8_t modeID;
    uint8_t readMode_H; //high byte of read mode
    uint8_t readMode_L; //low byte of read mode

    
    
    //struct used to hold raw data from sensor
    struct TYP_rawSensorData{
  
      uint8_t pressure_MMSB; //MMSB = Most Most Significant Bit
      uint8_t pressure_MLSB; //MLSB = Most Least Significant Bit
      uint8_t pressure_MCRC; //CRC = Cyclic Redundancy check, checksum after each 2 bytes M
      uint8_t pressure_LMSB; //LMSB = Least Most Significant Bit
      uint8_t pressure_LLSB;
      uint8_t pressure_LCRC;

      uint8_t temperature_MSB;
      uint8_t temperature_LSB;
      uint8_t temperature_CRC;
    }rawDataStruct;

      uint32_t pressureVal;
      float temperature_C;
      uint8_t rawDataAvailable; 
      uint8_t dataAvailable; 
	  
	  uint8_t crcErr_found;
      
//-------------Private functions-----------------------------
 
  //"I2C_read_Data" writes received data to the buffer array given with buffer_adress
  //Parameter:
  //uint8_t i2c_adress_7Bit - 7 bit long address of the i2c device
  //uint8_t buffer_size - buffer size of the i2c data buffer that the data should be written to
  //uint8_t *buffer_adress - pointer to the first element of the buffer array. The recived bytes will be written to this array
  //the max number of bytes that can be received is given by the parameter buffer_size, less bytes than that is possible more not.
  //
  //returns:
  //uint8_t rec_bytes - number of received bytes, if 0 buffer is empty
  uint8_t I2C_read_Data(uint8_t i2c_adress_7Bit, uint8_t buffer_size, uint8_t *buffer_adress);

  void WriteDataArrayToStruct(uint8_t rawDatArr[ICM20789_P_DATA_BUFFER_SIZE]);

  //Only update struct if new data was received
  //return 1 if successful
  uint8_t UpdateRawDataStruct(void);

    //ICM_p_first_Struct_To_Pressure(TYP_p_first_raw data_struct)
  //This function reads out the bytes containing the pressure data from the struct with the raw data and combines these bytes to a 32 Bit Value
  // In future Versions the pressure bytes can be verified using the CRC bytes from the struct, if CRC fails the function should return 0.
  //
  //Parameter: 
  // TYP_p_first_raw data_struct = structure of the Type TYP_p_first_raw containing the received data
  //
  //Output:
  //24 Bit Value that corresponds to the measured pressure. 
  uint32_t RawDataStruct_To_PressureVal(void);


  //read out the 2 bytes of temperature data from struct and convert them to actual temperature val
  uint16_t RawDataStruct_To_Temperature_val(void);

  float Convert_16Bit_val_To_Temperature_C(uint16_t value16);

//-----------------------CRC check------------------------------------------

  //The ICM20789 pressure Sensor always sends 2 data bytes and one crc byte, the crc of all 3 bytes
  //sould be 0. If not there was a transmission error
  //So this functions calculates the crc of 2 data bytes (High, low byte) + 1 crc byte
  //and returns 0 if success, 1 at fail
  //parameter: high, low data byte, received crc byte
  uint8_t Error_CRC_2bData_1bCRC(uint8_t data_H, uint8_t data_L, uint8_t rec_crc);


  //checks the Struct with the received Data using the CRC algorithm
  //return the number of CRC errors in this struct, -> if return is 0 everything is fine, else there is at least one error
  uint8_t Error_Check_CRC_sensorData(void); 

//---------------------------------------------------------------------------------------------

public:


//----------------public functions---------------------------
    ICM20789_Pressure(const uint8_t readMode_ID);

    inline uint8_t Get_readMode_H(void){
      return readMode_H;
    }

    inline uint8_t Get_readMode_L(void){
      return readMode_L;
    }
 
  void ConfigSensor(void);

  //request data from sensor
  //after request the sensor can be "asked" for data and it will answer with NACK if it didnt finish the measurement
  void StartMeasurement(void);


  //Only update data (pressure, temperature) using struct if there is no CRC error
  uint8_t UpdateData(void);  

  uint32_t  Get_pressureVal(void);

  float Get_temperature_C(void);
  
  //call when UpdateData returned 0 to see if its because of an crc error
  uint8_t Get_crcErr_found(void);




}; //class ICM20789_Pressure_HAL{




#endif //#define ICM20789_Pressure_HAL_h
