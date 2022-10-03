
#include "PulseDetector.h"

//movingAvg_intervalSize = 0 -> moving Average disabeled
//beatOnFallingEdge = 0 -> detect beat on rising edge, 1 -> detect beat on falling edge
PulseDetector::PulseDetector(uint8_t fir_enable, uint8_t movingAvg_intervalSize, uint8_t beatOnFallingEdge_p):
	movingAvg_16Bit(movingAvg_intervalSize)
{
  //init all variables
  FIR_en = fir_enable;
  
  IR_AC_Max = 20;
  IR_AC_Min = -20;

  IR_AC_Signal_Current = 0;
  IR_AC_Signal_Previous = 0; 
  IR_AC_Signal_min = 0;
  IR_AC_Signal_max = 0;
  IR_Average_Estimated = 0;

  positiveEdge = 0;
  negativeEdge = 0;
  ir_avg_reg = 0;

  offset = 0;

  rateSpot = 0;
  lastBeat = 0; //Time at which the last beat occurred

  beatsPerMinute = 0;
  beatAvg = 0;
  
  useFallingEdge = beatOnFallingEdge_p;
  
  debugPrint_Offset = 0; //use as Offset value while printing out values for debug mode
  debugPrint_ScalingFaktor = 1; //multiply values with this scaling faktor

  
  if(movingAvg_intervalSize > 0){
	movingAvg_en = 1;
	movingAvg_16Bit.begin();
  }else{
	movingAvg_en = 0;
  }
  

  //in header
  //cbuf[32];
  //const uint16_t FIRCoeffs[12] = {172, 321, 579, 927, 1360, 1858, 2390, 2916, 3391, 3768, 4012, 4096};

}

//Set Scaling faktors for debug Print modes
//raw uint16_t input value is multiplied with scalingFaktor
//the offset is added to the result
void PulseDetector::Set_DebugPrint_Faktors(uint16_t offset, float scalingFaktor, uint16_t zeroCrossing_MarkerHight){
	debugPrint_Offset = offset; //use as Offset value while printing out values for debug mode
	debugPrint_ScalingFaktor = scalingFaktor; //multiply values with this scaling faktor
	beatDetected_MarkerHight = zeroCrossing_MarkerHight;
}


//  Returns true if a pulse is detected
//  A running average of four samples is recommended for display on the screen.
// range_AC_Min, range_AC_Max - range_AC_Min/Max are the minimum/maximum difference between the local minimum and local maximum of the ac signal
// heart beat: range_AC_Min = 20, range_AC_Max = 1000
// debug: if debug = 0, normal operation mode, if debug = 1 print ac Signal and actual calculated local min max values of the signal (Serial.print(ac_signal + " "))
bool PulseDetector::checkForPulse(uint16_t sample, float range_AC_Min, float range_AC_Max, uint8_t debug)
{
  bool beatDetected = false;
  
  

  //  Save current state
  IR_AC_Signal_Previous = IR_AC_Signal_Current;
  
  
  
  //  Process next data sample
  if(movingAvg_en){
	IR_Average_Estimated = movingAvg_16Bit.reading(sample);
  }else{
	IR_Average_Estimated = averageDCEstimator(&ir_avg_reg, sample);
  }

  
  if(FIR_en == 1){
    IR_AC_Signal_Current = lowPassFIRFilter(sample - IR_Average_Estimated);
  }else{
    IR_AC_Signal_Current = sample - IR_Average_Estimated;
  }

  //This is good to view for debugging
  //Serial.print("Signal_Current: ");
  //Serial.print(String(sample) + " ");
  //Serial.print(String(IR_Average_Estimated) + " ");

  //if(debug == 1){
  //  Serial.print(String(IR_AC_Signal_Current) + " ");
  //  Serial.print(String(IR_AC_Max) + " ");
  //  Serial.print(String(IR_AC_Min) + " ");
  //}
 
  
 

  //  Detect positive zero crossing (rising edge)
  if ((IR_AC_Signal_Previous < 0) & (IR_AC_Signal_Current >= 0))
  {
  
    IR_AC_Max = IR_AC_Signal_max; //Adjust our AC max and min
    IR_AC_Min = IR_AC_Signal_min;

    positiveEdge = 1;
    negativeEdge = 0;
    IR_AC_Signal_max = 0;

	if(!useFallingEdge){
		//if ((IR_AC_Max - IR_AC_Min) > 100 & (IR_AC_Max - IR_AC_Min) < 1000)
		if (((IR_AC_Max - IR_AC_Min) > range_AC_Min) & ((IR_AC_Max - IR_AC_Min) < range_AC_Max))
		{
		//Heart beat!!!
		beatDetected = true;
		}
	}
  }

  //  Detect negative zero crossing (falling edge)
  if ((IR_AC_Signal_Previous > 0) & (IR_AC_Signal_Current <= 0))
  {
    positiveEdge = 0;
    negativeEdge = 1;
    IR_AC_Signal_min = 0;
	if(useFallingEdge){
		//if ((IR_AC_Max - IR_AC_Min) > 100 & (IR_AC_Max - IR_AC_Min) < 1000)
		if (((IR_AC_Max - IR_AC_Min) > range_AC_Min) & ((IR_AC_Max - IR_AC_Min) < range_AC_Max))
		{
		//Heart beat!!!
		beatDetected = true;
		}
	}
  }

  //  Find Maximum value in positive cycle
  if (positiveEdge & (IR_AC_Signal_Current > IR_AC_Signal_Previous))
  {
    IR_AC_Signal_max = IR_AC_Signal_Current;
  }

  //  Find Minimum value in negative cycle
  if (negativeEdge & (IR_AC_Signal_Current < IR_AC_Signal_Previous))
  {
    IR_AC_Signal_min = IR_AC_Signal_Current;
  }
  
  //debugPrint_Offset = offset; //use as Offset value while printing out values for debug mode
  //debugPrint_ScalingFaktor = scalingFaktor; //multiply values with this scaling faktor
  if(debug == 1){
    Serial.print(String(debugPrint_ScalingFaktor * IR_AC_Signal_Current + debugPrint_Offset) + " ");
    Serial.print(String(debugPrint_ScalingFaktor * IR_AC_Max + debugPrint_Offset) + " ");
    Serial.print(String(debugPrint_ScalingFaktor * IR_AC_Min + debugPrint_Offset) + " ");
	Serial.print(String(beatDetected * beatDetected_MarkerHight + debugPrint_Offset) + " ");
  }else if(debug == 2){
	Serial.print(String(debugPrint_ScalingFaktor * sample + debugPrint_Offset) + " ");
	Serial.print(String(debugPrint_ScalingFaktor * IR_Average_Estimated + debugPrint_Offset) + " ");
  }
  
  
  return(beatDetected);
}

//  Average DC Estimator
uint16_t PulseDetector::averageDCEstimator(int32_t *p, uint16_t x)
{
  //*p += ((((long) x << 15) - *p) >> 4);
  //return (*p >> 15);
  
  ir_avg_reg += ((((long) x << 15) - ir_avg_reg) >> 4);
  return (ir_avg_reg >> 15);
  
}



//  Low Pass FIR Filter
int16_t PulseDetector::lowPassFIRFilter(int16_t din)
{  
  cbuf[offset] = din;

  int32_t z = mul16(FIRCoeffs[11], cbuf[(offset - 11) & 0x1F]);
  
  for (uint8_t i = 0 ; i < 11 ; i++)
  {
    z += mul16(FIRCoeffs[i], cbuf[(offset - i) & 0x1F] + cbuf[(offset - 22 + i) & 0x1F]);
  }

  offset++;
  offset %= 32; //Wrap condition

  return(z >> 15);
}

//  Integer multiplier
int32_t PulseDetector::mul16(int16_t x, int16_t y)
{
  return((long)x * (long)y);
}

// range_AC_Min, range_AC_Max - range in which the min, max peak of the AC Signal should be for beatDetected to be true
// range_AC_Min, range_AC_Max - range_AC_Min/Max are the minimum/maximum difference between the local minimum and local maximum of the ac signal
// heart beat: range_AC_Min = 20, range_AC_Max = 1000
// debug: if debug = 0, normal operation mode, if debug = 1 print ac Signal and actual calculated local min max values of the signal (Serial.print(ac_signal + " "))
//debug = 2 - print ac signal and pulse sensend
//avg_min/max_BMP if bpm is in this range the bmp value is used to calculate the average. hearbeat min = 20, max = 255
uint8_t PulseDetector::UpdatePulseDetector(uint16_t value, int16_t range_AC_Min, int16_t range_AC_Max, uint16_t avg_min_BPM, uint16_t avg_max_BPM, uint8_t debug){
  //heart beat detection
  //source Example 5 of pulsSensor library
  //https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library/tree/master/examples/Example5_HeartRate
  
  bool pulseSensed = checkForPulse(value, range_AC_Min, range_AC_Max, debug); 

  //if(debug == 2){
  //  Serial.print(String(pulseSensed * 100) + " ");
  //  Serial.print(String(IR_AC_Signal_Current) + " ");
  //}
  
  beatAvg = 0;
  if (pulseSensed == 1)
  {
    //We sensed a beat!
    long deltaBeat = millis() - lastBeat;
    lastBeat = millis();
    
    //Serial.println(deltaBeat);
    beatsPerMinute = 60 / (deltaBeat / 1000.0);
    

    if (beatsPerMinute < avg_max_BPM && beatsPerMinute > avg_min_BPM)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      //beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++){
        beatAvg += rates[x];
	  }
	beatAvg /= RATE_SIZE;
	  
    }
  } //if (checkForBeat(ir_raw) == true)

return pulseSensed;
} //uint8_t PulseDetector::UpdatePulseDetector(void)

float PulseDetector::Get_PulsesPerMinute(void){
  
  return beatsPerMinute;
}

float PulseDetector::Get_PulsesPerMinute_Avg(void){
  return beatAvg;
}

float PulseDetector::Get_DCvalue(void){
  return IR_Average_Estimated;
}

