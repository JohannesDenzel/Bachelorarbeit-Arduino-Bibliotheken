//For the library to work the raw input value must be uint16_t.
//it also works with different types, but if the results of functions from this library are printed against
//the inputs, it might look like the library is not working
//
//As Source for this Library the Code from the HeratRate Files in SparkFun_MAX3010x_Sensor_Library was used and just changed a little bit
//Also the heart beat Example code from this library was used
//https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library
//Basically i just put the code from the library files and the example into the PulseDetector class to be able
//to run the example code in a function in the class instead of the main loop of the ino file
//this way i can use private variables that are acessible by all the class functions but not from outside
//The goal is writing a UpdatePulseDetector function that returns 1 if a pulse was detected
//and a getter that returns the pulse freq. So All the compicated logic is done in the PulseDetector class instead of the main loop
//also i can use multiple instances of this class totetect different kinds of pulses (like heartbeat and movement)
#ifndef PulseDetector_h
#define PulseDetector_h

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "MovingAvg_uint16.h"

#define RATE_SIZE 4

class PulseDetector{

  private:

    int16_t IR_AC_Max; // = 20;
    int16_t IR_AC_Min; // = -20;

    int16_t IR_AC_Signal_Current; // = 0;
    int16_t IR_AC_Signal_Previous; //;
    int16_t IR_AC_Signal_min; // = 0;
    int16_t IR_AC_Signal_max; // = 0;
    int32_t IR_Average_Estimated;

    int16_t positiveEdge; // = 0;
    int16_t negativeEdge; // = 0;
    int32_t ir_avg_reg; // = 0;

    int16_t cbuf[32];
    uint8_t offset; // = 0;

    //const = read only
    const uint16_t FIRCoeffs[12] = {172, 321, 579, 927, 1360, 1858, 2390, 2916, 3391, 3768, 4012, 4096};

    
    uint8_t rates[RATE_SIZE]; //Array of pulses used for averaging
    uint8_t rateSpot;// = 0;
    long lastBeat;// = 0; //Time at which the last beat occurred

    float beatsPerMinute;// = 0;
    int16_t beatAvg;// = 0;


    uint8_t FIR_en;
	
	MovingAvg_uint16 movingAvg_16Bit;
	uint8_t movingAvg_en;
	
	uint8_t useFallingEdge;

    //private functions
    uint16_t averageDCEstimator(int32_t *p, uint16_t x);

    int32_t mul16(int16_t x, int16_t y);

	uint16_t debugPrint_Offset; //use as Offset value while printing out values for debug mode
	float debugPrint_ScalingFaktor; //multiply values with this scaling faktor
	
	uint16_t beatDetected_MarkerHight;

  public:

    PulseDetector(uint8_t fir_enable, uint8_t movingAvg_intervalSize, uint8_t beatOnFallingEdge_p);
	
	void Set_DebugPrint_Faktors(uint16_t offset, float scalingFaktor, uint16_t zeroCrossing_MarkerHight);

    bool checkForPulse(uint16_t sample, float range_AC_Min, float range_AC_Max, uint8_t debug);

    int16_t lowPassFIRFilter(int16_t din);

    float LowpassFilter(float yk0, float uk0, float n);

   uint8_t UpdatePulseDetector(uint16_t value, int16_t range_AC_Min, int16_t range_AC_Max, uint16_t avg_min_BPM, uint16_t avg_max_BMP, uint8_t debug);
 
   float Get_PulsesPerMinute(void);

    float Get_PulsesPerMinute_Avg(void);

    float Get_DCvalue(void);

};

#endif
