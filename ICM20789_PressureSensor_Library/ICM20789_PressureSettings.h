#ifndef ICM20789_PressureSettings_h
#define ICM20789_PressureSettings_h

//Read Mode Settings
//Temperature first modes are not impemented in this library
/*
OPERATION_MODE        | TRANSMIT_T_FIRST | TRANSMIT_P_FIRST
LowPower (LP)         | 0x609C           | 0x401A
Normal (N)            | 0x6825           | 0x48A3
LowNoise (LN)         | 0x70DF           | 0x5059
Ultra-Low Noise (ULN) | 0x7866           | 0x58E0
*/

//Pressure first Ultra Low Noise 
//Send pressure first then temperature, ultra low noise mode 
//consumes most power, slowest, best signal to noise ratio
#define PRESSURE_FIRST_ULN 0

//Pressure first Low Noise mode
#define PRESSURE_FIRST_LN 1

//Pressure first Normal mode
#define PRESSURE_FIRST_N 2

//Pressure first Low Power mode
#define PRESSURE_FIRST_LP 3


#endif //ICM20789_PressureSettings_h
