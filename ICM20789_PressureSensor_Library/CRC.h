//Wrote based on CRC Tutorial on website: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html

#ifndef CRC_H
#define CRC_H

#include "Arduino.h"



#define CRC_TABLE_BYTES 256 //number of bytes in the crc table, must be 256 



 class CRC_Library{ 
  private:
    uint8_t generator;
    uint8_t init;

    
    
  public:
    uint8_t crcTable[CRC_TABLE_BYTES]; 
    
    CRC_Library(const uint8_t crc_generator, const uint8_t crc_init);


    inline uint8_t Get_CRC_Init(void){
      return init;
    }

    //Init crcTable
    void Init_crcTable_CRC8(void);

    //Calculate 8 bit CRC Value
    //If the data array contains only data, the CRC will be calculated an can be compared to the received CRC
    //If the data array contains data + the received CRC byte, the calculated CRC of this will be 0x00 if no bit errors occoured
    //
    //Parameter:
    //uint8_t *adr_byte_arr - Pointer to the Data Array those CRC should be calculated
    //uint8_t arr_numOfBytes - Number of Bytes in the Array
    //
    //returns calculated 8 bit CRC of the input bytes
    uint8_t Compute_CRC8(uint8_t *adr_byte_arr, uint8_t arr_numOfBytes);
 
}; //class CRC{







#endif //#ifndef CRC_H
