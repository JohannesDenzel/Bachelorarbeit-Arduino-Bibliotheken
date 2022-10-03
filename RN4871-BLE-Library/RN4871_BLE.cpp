
#include "RN4871_BLE.h"

//Constructor
RN4871_BLE::RN4871_BLE(){
  debug_msg = 0;
}


//wait for BLE_response then return it
//wait until the response matches at least the expected response then return the full response
//the Module sometimes doesnt send the full response at once (or the full response isnt in the serial buffer)
//ideally the expected response should match the received response fully. if not there might be a problem in the next command, becaus then the response
//also contains the old response. To prevent that the serial buffer is cleared.
String RN4871_BLE::Get_expected_BLE_Response(uint32_t timeout_ms, const char expectedResponse[]){
  //wait until BLE responds
  uint32_t start = millis();
  uint8_t expectedLen = strlen(expectedResponse);
  while(Serial2.available() <= 0){
    if(millis() - start > timeout_ms) break;
  }

  String responseStr = "";
  start = millis(); 
  while(millis() - start < timeout_ms){ //debug wait until timeout for expected response
    
	while(Serial2.available() > 0){
      char inChar = Serial2.read();
      responseStr += String(inChar);
      //Serial.println((uint8_t) inChar); //debug. used to also see the "non-letter" characters
    }
	
	//responseStr = RN4871_BLE::Get_BLE_String(); do not use this here.
	
    if(String(responseStr).substring(0,expectedLen) == String(expectedResponse)){
      break;
    }
  } //while(millis() - start < timeout_ms)

    Serial2.flush(); //clear serial buffer
    return responseStr;
}

//return 1 if AOK, 0 if ERR or no response. wait for timeout_ms milliseconst until response
uint8_t RN4871_BLE::SendCommand(const char command[], const char expectedResponse[], uint32_t timeout_ms){
  uint8_t success = 0;
  uint8_t commandLength = strlen(command);
  uint8_t responseLength = strlen(expectedResponse);
  if(debug_msg == 1){
	Serial.print("send: ");
  }
  for(uint8_t chrID = 0; chrID < commandLength; chrID++){
    Serial2.write(command[chrID]);
    delay(5);
	if(debug_msg == 1){
		Serial.print(command[chrID]);
	}
		delay(5);
	
  }
  if(debug_msg == 1){
	Serial.println();
  }

  //wait until BLE responds
  String responseStr = Get_expected_BLE_Response(timeout_ms, expectedResponse);
  
  if(debug_msg == 1){
	Serial.println("RN4871_response:" + String(responseStr) + "\n" + "expected_response: " + String(expectedResponse) + "\n" +  "response_checked_substring: " + String(responseStr).substring(0,responseLength));
  }

  if(String(responseStr).substring(0,responseLength) == String(expectedResponse)){
    success = 1;
  }

  //clear both serial buffers
  Serial2.flush();
  Serial.flush();
  return success;
}

//start command Mode
uint8_t RN4871_BLE::StartCMD_Mode(uint32_t timeout_ms){
  return SendCommand("$$$", "CMD>", timeout_ms);
}

//send command and wait till reboot finished ble module will send %REBOOT%
uint8_t RN4871_BLE::Reboot_BLE_Module(uint32_t timeout_ms){
  uint8_t success = 0;
  //\r = CR ASCII 13, \n = LF ASCII 10
  //BLE Module answer to reboot command is
  //R,1AOK
  //CMD> Rebooting
  //then after some time
  //%REBOOT% then rebooting finished
  success = SendCommand("R,1\r", "R,1AOK\r\nCMD> \rRebooting\r\n%REBOOT%", timeout_ms);

  //first the module sends 
  //String responseStr = Get_expected_BLE_Response(timeout_ms, "%REBOOT%");
  //Serial.println("%REBOOT%: " + responseStr);
  return success;
}

//returns 4 if everything went as expected
//const String ble_Service bluetooth Service 
//    currently only implemented: ble_Service = "Service_transparent_UART" 
//const uint8_t echoOn - turn echo Mode on or off on ble Module: ble module sends back selected modes
//uint32_t timeout_ms - timeout waiting for ble module to answer. must be > 600 ms because of reboot time
uint8_t RN4871_BLE::Init_BLE(const String ble_Service, uint32_t timeout_ms, uint8_t debugMessages_ON){
  uint8_t success = 1;

   //if one function returns 0 success will be 0.

   //start command Mode
   success += RN4871_BLE::StartCMD_Mode(timeout_ms);
   if(debug_msg == 1){
	Serial.println("start CMD: " + String(success));
   }
   
   //enable ECHO Mode
   success += RN4871_BLE::SendCommand("+\r", "ECHO ON", timeout_ms); //turn echo on
   if(debug_msg == 1){
	Serial.println("echo on: " + String(success));
   }

	
   if(ble_Service.equals(String("Service_transparent_UART")) == 1){
    //Select Service (transparent UART)
    success += RN4871_BLE::SendCommand("SS,C0\r", "SS,C0", timeout_ms);
	if(debug_msg == 1){
		Serial.println("config: " + String(success));
	}
   }

   if(debug_msg == 1){
	Serial.println("Rebooting BLE Module:");
   }
   
   success += RN4871_BLE::Reboot_BLE_Module(timeout_ms);
   
   if(debug_msg == 1){
	Serial.println("reboot: " + String(success));
   }

   //delay(10000); 
   if(success > 0){
	if(debug_msg == 1){
		Serial.println("try connecting to BLE Module");
	}
   }
   
   return success;
}

String RN4871_BLE::Get_BLE_String(void){
  String recStr = "";
  while(Serial2.available() > 0){
    char inChar = Serial2.read();
    recStr += String(inChar);
  }
  return recStr;
}


