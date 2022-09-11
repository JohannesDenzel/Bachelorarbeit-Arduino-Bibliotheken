//Wrote based on CRC Tutorial on website: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html

#include "CRC.h"


//Example from ICM20789_Pressure_HAL.h
//Sensor specific CRC Settings
//#define CRC_NR_OF_BYTES 2 //Number of bytes per message eg. 1 CRC Byte per NR_OF_BYTES
//#define CRC_GENERATOR 0x31 //0b(1)0011 0001 the first 1 is discarded because its always 1.
//#define CRC_INIT 0xFF

CRC_Library::CRC_Library(const uint8_t crc_generator, const uint8_t crc_init){
      generator = crc_generator;
      init = crc_init;
}

//Init global Variable crcTable
void CRC_Library::Init_crcTable_CRC8(){
    //const byte generator = ;//0x1D;
    //uint8_t[256] crctable;
    /* iterate over all byte values 0 - 255 */
    for(uint16_t divident = 0; divident < CRC_TABLE_BYTES; divident++){ //divident musst be uint16_t because when div == 255 for loop is executed once more adding 1 after the loop causing div to overflow, thus the for loop is never exited 
        uint8_t currByte = (uint8_t) divident;
        /* calculate the CRC-8 value for current byte */
        
        for (uint8_t bit = 0; bit < 8; bit++){
            if ((currByte & 0x80) != 0) //0x80 = 1000 0000, -> if first 1 of currByte aligns with 0x80, then (currByte & 0x80) is != 0
            {
                currByte <<= 1; //shift left by 1 because CRC_GENERATOR is without the first 0b1 of the divisor (because 1XOR1 is always 0)
                //now currByte and CRC_GENERATOR are actually aligned
                currByte ^= generator; 
            }
            else
            {
                currByte <<= 1; //next bit
            }
        }//for (uint8_t bit = 0; bit < 8; bit++){
        
        /* store CRC value in lookup table */
        //Serial.println(String(divident, HEX) + " " + String(currByte, HEX));
        crcTable[divident] = currByte;
    }
    
    return;
}//public static void CalulateTable_CRC8()



//------------------------CRC Functions--------------------------------

//Calculate 8 bit CRC Value
//If the data array contains only data, the CRC will be calculated an can be compared to the received CRC
//If the data array contains data + the received CRC byte, the calculated CRC of this will be 0x00 if no bit errors occoured
//
//Parameter:
//uint8_t *adr_byte_arr - Pointer to the Data Array those CRC should be calculated
//uint8_t arr_numOfBytes - Number of Bytes in the Array
//
//returns calculated 8 bit CRC of the input bytes
uint8_t CRC_Library::Compute_CRC8(uint8_t *adr_byte_arr, uint8_t arr_numOfBytes){
    uint8_t crc = init; //Init
    
    for(uint8_t i = 0; i < arr_numOfBytes; i++){  //for each (byte b in bytes)
        uint8_t b = *(adr_byte_arr + i);
        /* XOR-in next input byte */
        uint8_t data = (uint8_t)(b ^ crc);
        /* get current CRC value = remainder */
        crc = (uint8_t)(crcTable[data]);
    }

    return crc;
   
}//public static byte Compute_CRC8(byte[] bytes)


