#include "ICM20789_Pressure.h"

//select one by uncommenting 

//use low power mode
ICM20789_Pressure pressureSensor(PRESSURE_FIRST_LP);

//use normal mode
//ICM20789_Pressure pressureSensor(PRESSURE_FIRST_N);

//use low noise mode
//ICM20789_Pressure pressureSensor(PRESSURE_FIRST_LN);

//use ultra low noise mode
//ICM20789_Pressure pressureSensor(PRESSURE_FIRST_ULN);

void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  delay(50); //wait for Serial, Wire to init
  pressureSensor.ConfigSensor();

}

//-----------------------Loop----------------------------------

void loop() 
{
  //--------------Init Variables---------------------------------
  
  static long errCount = 0;
  static uint32_t pressureVal = 0;
  float temperature = 0;
  //-------------------------------------------------------------

  
  //if no measurement was requested yet -> request measurement
  //measurement cant be requested in every loop. Only after the sensor answered a new measurement can be requested
  pressureSensor.StartMeasurement();
  uint8_t dataAvailable = pressureSensor.UpdateData();
  
  if(dataAvailable == 1){
    pressureVal = pressureSensor.Get_pressureVal();
    temperature = pressureSensor.Get_temperature_C();
  } 

  Serial.print(String(pressureVal) + " ");
  //Serial.print(String(temperature) + " ");
  Serial.println();
  
}//void loop() 






