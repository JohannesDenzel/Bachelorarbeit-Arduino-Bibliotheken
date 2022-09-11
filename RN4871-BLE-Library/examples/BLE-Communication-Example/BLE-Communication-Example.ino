//ESP 32
//Serial 2
//RX Pin: G16
//TX Pin: G17

#include "RN4871_BLE.h"

RN4871_BLE bleModule;

void setup() {
 
  Serial.begin(115200);
  Serial2.begin(115200);

  delay(1000); //debug wait so that i can open the serial monitor
  Serial.println("begin");

  //Init_BLE(const String ble_Service, const uint8_t echoOn, long timeout_ms) 
  //returns 4 if everything went as expected
  //const String ble_Service bluetooth Service 
  //    currently only implemented: ble_Service = "Service_transparent_UART" 
  //const uint8_t echoOn - turn echo Mode on or off on ble Module: ble module sends back selected modes
  //long timeout_ms - timeout waiting for ble module to answer. must be > 600 ms because of reboot time
  uint8_t success = bleModule.Init_BLE("Service_transparent_UART", 10000);

  if(success == 0){
    Serial.println("Config didnt go as expected. BLE might not work");
    //while (1); //do not continue
   }
  
}


void loop() {

  String recString = bleModule.Get_BLE_String();
  if(recString.length() > 0){
    Serial.println("rec: " + recString);
    Serial2.println(recString);//send back to BLE Module
  }

}
