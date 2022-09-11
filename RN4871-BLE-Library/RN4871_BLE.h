/*
  RN4871_BLE.h - Library for using the RN4870 Bluetooth Low Energy Module.
  Created by Johannes G. Denzel, April 26, 2022.
*/
#ifndef RN4871_BLE_H
#define RN4871_BLE_H

#include "Arduino.h" //use standart arduino Types and functions

class RN4871_BLE
{
  public:

    //Constructor
    RN4871_BLE();

    //wait for BLE_response then return it
    //wait until the response matches at least the expected response then return the full response
    //the Module sometimes doesnt send the full response at once (or the full response isnt in the serial buffer)
    //ideally the expected response should match the received response fully. if not there might be a problem in the next command, becaus then the response
    //also contains the old response. To prevent that the serial buffer is cleared.
    String Get_expected_BLE_Response(uint32_t timeout_ms, const char expectedResponse[]);

    //return 1 if AOK, 0 if ERR or no response. wait for timeout_ms milliseconst until response
    uint8_t SendCommand(const char command[], const char expectedResponse[], uint32_t timeout_ms);

    //start command mode (CMD)
    uint8_t StartCMD_Mode(uint32_t timeout_ms);

    //send command and wait till reboot finished ble module will send %REBOOT%
    uint8_t Reboot_BLE_Module(uint32_t timeout_ms);
	
	//returns 4 if everything went as expected
	//const String ble_Service bluetooth Service 
	//    currently only implemented: ble_Service = "Service_transparent_UART" 
	//const uint8_t echoOn - turn echo Mode on or off on ble Module: ble module sends back selected modes
	//uint32_t timeout_ms - timeout waiting for ble module to answer. must be > 600 ms because of reboot time
	uint8_t Init_BLE(const String ble_Service, uint32_t timeout_ms);
	
	//get text from UART2 -> String that was sent by ble module 
	String Get_BLE_String(void);
 
};

#endif //#ifndef RN4870_BLE_H

