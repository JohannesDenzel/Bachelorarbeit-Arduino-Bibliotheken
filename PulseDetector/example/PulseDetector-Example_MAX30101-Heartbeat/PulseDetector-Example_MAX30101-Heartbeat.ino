 //Gesamtbeispiel mit allen Sensoren und BLE Modul

//-----------------------------------Einbinden der Bibliotheken--------------------------------------------------------
#include <PulseDetector.h>
#include <MAX30105.h>


//----------------------------Einstellungen und Objektinitialisierung--------------------------------------------------

//----------------------------------MAX30101 Herzfrequenz---------------------
MAX30105 herzfreqSensor;

//movingAvg_intervalSize = 0 -> moving Average disabeled (interval size = number of uint16_t elements in array)
//beatOnFallingEdge = 0 -> detect beat on rising edge, 1 -> detect beat on falling edge
//PulseDetector::PulseDetector(uint8_t fir_enable, uint8_t movingAvg_intervalSize, uint8_t beatOnFallingEdge_p)
PulseDetector heartBeatDetector(0,50,1); //0 = FIR on input disabeled, 1 dc = moving average, 0 dc = estimated average 

//------------------------------------------Setup-Funktion-------------------------------------------------------
void setup() {
  Serial.begin(115200);
  

  // Initialize heartbeat sensor
  if (!herzfreqSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30101 was not found. Please check wiring/power. ");
    delay(5000); //give some time to read the message in Serial Monitor
  }

  //Setup to sense a nice looking saw tooth on the plotter
  const byte ledBrightness = 31; //Options: 0=Off to 255=50mA
  const byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  const byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  const int sampleRate = 1000; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  const int pulseWidth = 411; //Options: 69, 118, 215, 411
  const int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  herzfreqSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

  delay(5000); //debug. give some time to read all messages in Serial Monitor

} //void setup()

//--------------------------------------Loop-Funktion--------------------------------------------------------

void loop() {

  //delay(10); //debug current measurement. 

  static float heartBeat_avg_sum = 0;
  static uint32_t heartBeat_avg_N = 0;
  
  uint16_t ir_raw = 0;
  uint16_t red_raw = 0;
  uint16_t green_raw = 0;
  float heartRate_die_Temp = 0; //die temperature

  uint8_t heartBeatDetected = 0;
  float heartBeat = 0;
  float heartBeat_avg = 0;
  float ir_DC = 0;
  
  
  //Herzfrequenz Sensor MAX30101
  //Rohdaten lesen
  ir_raw = herzfreqSensor.getIR();
  red_raw = herzfreqSensor.getRed();
  //green_raw = herzfreqSensor.getGreen();

  //heartRate_die_Temp = herzfreqSensor.readTemperature();

  //Herzfreq. Pulsdetektor
  //uint8_t UpdatePulseDetector(uint16_t value, float range_AC_Min, float range_AC_Max, uint16_t avg_min_BPM, uint16_t avg_max_BMP, uint8_t debug){
  //debug can be 0,1,2
  heartBeatDetected = heartBeatDetector.UpdatePulseDetector(ir_raw, -1000, 1000, 20, 255, 1); //val, minVal, maxVal, minBPM, maxBPM, debug=0
  heartBeat = heartBeatDetector.Get_PulsesPerMinute();
  heartBeat_avg = heartBeatDetector.Get_PulsesPerMinute_Avg();
  ir_DC = heartBeatDetector.Get_DCvalue();

  //avg is also 0 if no pulse detected
  if (heartBeatDetected > 0) {
    heartBeat_avg_sum += heartBeat_avg;
    heartBeat_avg_N++;
  }


  //Data Output to serial Monitor for debugging

  
  Serial.print(String(heartBeat) + " "); // /120000 * 10 - 8) + " ");
  //Serial.print(String(heartBeat_avg) + " ");
  //Serial.print(String(ir_DC) + " ");
  //Serial.print(String(ir_raw) + " "); // 

  //Serial.print(String(green_raw) + " ");
  //Serial.print(String(heartRate_die_Temp) + " "); //die/chip temperature for temperature offset calculation of sensor


  Serial.println();


 
}
