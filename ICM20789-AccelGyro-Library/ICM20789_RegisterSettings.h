#ifndef ICM20789_REGISTERSETTINGS_H
#define ICM20789_REGISTERSETTINGS_H

//only high bits have to be used in register settings. By adding all bits of a register together with "OR" the high bits will stay high while the rest low because in the Bit definions the other bits are 0.

#ifdef ICM20789_SMPLRT_DIV //FIFO sample rate divisor. SAMPLE_RATE = 1 / (1 + SMPLRT_DIV). 		
	//Bits [7:0]
  #define	SMPLRT_DIV_1 	1
  #define	SMPLRT_DIV_10 	10
  #define	SMPLRT_DIV_100 	100
  #define	SMPLRT_DIV_255 	255
#endif 

#ifdef ICM20789_CONFIG 
	//Bit 6
	#define FIFO_FULL_REPL_OLD	0x00 //default. if FIFO full, new writes will replace old
	#define FIFO_FULL_NO_WRITES 0b01000000
	
	//Bits [2:0] Low Pass Filter Setting see Datasheet v1.4 table 21. FCHOICE_B Bits in GYRO_CONFIG must be 0 for CFG Settings to apply
	//CFG = config
	#define DLPF_CFG_0	0
	#define DLPF_CFG_1	1
	#define DLPF_CFG_2	2
	#define DLPF_CFG_3	3
	#define DLPF_CFG_4	4
	#define DLPF_CFG_5	5
	#define DLPF_CFG_6	6
	#define DLPF_CFG_7	7
	
#endif

#ifdef ICM20789_GYRO_CONFIG 

	//Bit 7
	#define XGYRO_ST_EN 128 //(=0b1000 0000) x gyro selftest enable 
	//Bit 6
	#define YGYRO_ST_EN 64
	//Bit 5
	#define ZGYRO_ST_EN 32
	//Bits [4:3]
	#define GYRO_FS_SEL_250_DPS		0 //Gyro Fullscale select +-250dps
	#define GYRO_FS_SEL_500_DPS		0b00001000
	#define GYRO_FS_SEL_1000_DPS	0b00010000 
	#define GYRO_FS_SEL_2000_DPS	0b00011000
	//Bits [1:0]
	#define FCHOICE_B_00	0 //Low pass Filter Settings see datasheet table 21
	#define FCHOICE_B_01	0b00000001
	#define FCHOICE_B_10	0b00000010
	
#endif
 
#ifdef ICM20789_ACCEL_CONFIG 		
	//Bit 7
	#define XACCEL_ST_EN	128 //x accel selftest enable
	//Bit 6
	#define YACCEL_ST_EN	64
	//Bit 5
	#define ZACCEL_ST_EN	32
	//Bits 4:3
	#define ACCEL_FS_SEL_2G		0	//Accel full scale select +-2g
	#define ACCEL_FS_SEL_4G		0b00001000
	#define ACCEL_FS_SEL_8G		0b00010000
	#define ACCEL_FS_SEL_16G	0b00011000
	
#endif 

#ifdef ICM20789_ACCEL_CONFIG2 		
	//Bits 7:6 Reset FIFO after setting size! Register USER_CTRL Bit FIFO_RST
	#define FIFO_BUF_SIZE_512	0 //FIFO Buffer size 512 bytes
	#define FIFO_BUF_SIZE_1K	0b01000000 //FIFO Buffer size 1 kB
	#define FIFO_BUF_SIZE_2K	0b10000000
	#define FIFO_BUF_SIZE_3K	0b11000000
	
	//Bits 5:4
	#define DEC2_CFG_4 	0 //4 samples averaged in accel
	#define DEC2_CFG_8 	0b00010000
	#define DEC2_CFG_16 0b00100000
	#define DEC2_CFG_32 0b00110000
	
	//Bit 3
	#define ACCEL_FCHOICE_B_1	0b00001000 //Bypass accel lowpass see table 22 in datasheet
	#define ACCEL_FCHOICE_B_0	0
	
	//Bits 2:0
	#define ACCEL_DLPF_0	0	//Accel lowpass Filter setting see table 22 in datasheet
	#define ACCEL_DLPF_1	1
	#define ACCEL_DLPF_2	2
	#define ACCEL_DLPF_3	3
	#define ACCEL_DLPF_4	4
	#define ACCEL_DLPF_5	5
	#define ACCEL_DLPF_6	6
	#define ACCEL_DLPF_7	7
		
#endif 

#ifdef ICM20789_LP_MODE_CTRL 		
	//Bit 7
	#define GYRO_CYCLE_EN 128 //enable gyro duty cycling.
	
	//Bits 6:4 averaging Filter configuration for gyro duty cycling
	//not depedent on DLPF_CFG[2:0] Bits of Register CONFIG (0x1A)
	//see table 24 in datasheet
	#define GYRO_CYC_AVG_1		0
	#define GYRO_CYC_AVG_2		0b00010000
	#define GYRO_CYC_AVG_4		0b00100000
	#define GYRO_CYC_AVG_8		0b00110000
	#define GYRO_CYC_AVG_16		0b01000000
	#define GYRO_CYC_AVG_32		0b01010000
	#define GYRO_CYC_AVG_64		0b01100000	
	#define GYRO_CYC_AVG_128	0b01110000
	
#endif

#ifdef ICM20789_ACCEL_WOM_X_THR //Accel Wake on Motion X Threshold

#endif 

#ifdef ICM20789_ACCEL_WOM_Y_THR 	

#endif 

#ifdef ICM20789_ACCEL_WOM_Z_THR 	

#endif 

#ifdef ICM20789_FIFO_EN 
//write selected data to FIFO
//if enabeled buffering of data will occour even if data path is on standby

	//Bit 7 write TEMP_OUT_H/L to FIFO
	#define FIFO_TEMP_OUT_EN		128 
	
	//Bit 6
	#define FIFO_GYRO_XOUT_EN		64
	
	//Bit 5
	#define FIFO_GYRO_YOUT_EN		32
	
	//Bit 4
	#define FIFO_GYRO_ZOUT_EN		16
	
	//Bit 3
	#define FIFO_ACCEL_XYZ_OUT_EN	8

#endif

#ifdef ICM20789_INT_PIN_CFG //Interrupt Pin Configuration
	//Bit 7 int pin active low or high
	#define ACTIVE_LOW	128
	#define ACTIVE_HIGH	0
	
	//Bit 6 int pin open drain or push pull
	#define OPENDRAIN	64
	#define PUSHPULL	0
	
	//Bit 5
	#define LATCH_INT_EN	32 //Int pin level held until interrupt status is cleared
	#define INT_PULSE_50US	0  //interrupt pulse on int pin is 50us	

	//Bit 4
	#define INT_ANY_READ_TO_CLEAR 16 //interrupt status is cleared if any read operation is performed
	#define INT_STATUS_REG_READ_TO_CLEAR 0 //interrupt status is cleared only by reading INT_STATUS register
	
	//Bit 3
	//INT_FSYNC_PIN_ACTIVE_LOW	8 //logic level of FSYNC Pin as interrupt is active low. unimplemented in dev board because FSYNC connected to GND
	
	//Bit 2
	//FSYNC_INT_MODE_EN	4 //enable FSYNC Pin as interrupt pin. unimplemented
	
#endif 

#ifdef ICM20789_INT_ENABLE 		
//Enable interrupts to propagate to interrupt Pin
	//Bit 7
	#define WOM_X_INT_EN	128 //enable wake on motion interrupt on accel x axis
	
	//Bit 6
	#define WOM_Y_INT_EN	64
	
	//Bit 5
	#define WOM_Z_INT_EN	32
	
	//Bit 4
	#define FIFO_OVERFLOW_INT_EN	16 //enable interrupt for FIFO overflow
	
	//Bit 2
	#define GDRIVE_RDY_INT_EN	4 //Gyro drive ready interrupt
	
	//Bit 1
	#define DMP_INT_EN	2	//DMP Interrupt
	
	//Bit 0
	#define RAW_RDY_INT_EN	//Raw Sensor Data ready interrupt
	
#endif


//#ifdef ICM20789_DMP_INT_STATUS 	no setting
//#endif
//#ifdef ICM20789_INT_STATUS 	no setting, interrupt statuses
//#endif

//#ifdef ICM20789_SIGNAL_PATH_RESET //reset signal paths, unused	
//#endif 

#ifdef ICM20789_ACCEL_INTEL_CTRL 	
	//accel intelligence control (some WOM Logic Settings)
	
	//Bit 7
	#define ACCEL_WOM_EN 	128 //enable WOM Logic
	
	//Bit 6
	#define ACCEL_COMPARE_PRE_SAMPLE	64	//compare current sample to previous sample
	#define ACCEL_COMPARE_INIT_SAMPLE	0	//compare current sample to initial smaple. (all future samples are compared do init sample)
	
#endif 

#ifdef ICM20789_USER_CTRL 	
	//Bit 7 
	#define DMP_EN	128
	
	//Bit 6
	#define FIFO_EN	64
	
	//Bit 4
	#define I2C_RESET	16 
	
	//Bit 3
	#define DMP_RESET	8 //auto clears
	
	//Bit 2
	#define FIFO_RESET	4 //auto clears. do FIFO_RESET after FIFO settings
	
	//Bit 0
	#define SIG_COND_RESET	1 //reset all gyro, accel, temperaturs digiatal data signal paths and clear all sensor registers
	

#endif

#ifdef ICM20789_PWR_MGMT_1 	

	//Bit 7
	#define DEVICE_RESET 128 //reset internal registers and restore default settings
	
	//Bit 6
	#define SLEEP	64	//set chip to sleep mode (default settings)
	
	//Bit 5
	#define ACCEL_SLEEP_CYCLE	32 //when SLEEP and STANDBY are not set to 1, the chip will cycle between sleep and taking a single accelerometer sample at a rate determined by SMPLRT_DIV
	
	//Bit 4
	#define GYRO_STANDBY	16 //low power mode that allows quick enabeling of the gyro sensors
	
	//Bit 3
	#define TEMP_DISABLE	8 //when set the temperature sensor is disabled
	
	//Bits 2:0
	//CLKSEL clock select (internal or external)

#endif

#ifdef ICM20789_PWR_MGMT_2 	

	//Bit 7
	#define LP_DISABLE 128 //Low Power Disable bit. if = 0 (not set to 1) the system will enter sleep when gyro is disabled and accel is off while duty cycling. 
	
	//Bit 6
	#define DMP_LP_DISABLE	64 //When set DMP will not execute in low power accel mode. 
	//When cleared DMP will execute in low power accel mode.
	
	//Bit 5
	#define DISABLE_X_ACCEL 32 //x accel is disabled
	
	//Bit 4
	#define DISABLE_Y_ACCEL	16
	
	//Bit 3
	#define DISABLE_Z_ACCEL	8
	
	//Bit 2
	#define DISABLE_X_GYRO	4 //x gyro is disabled
	
	//Bit 1
	#define DISABLE_Y_GYRO	2
	
	//Bit 0
	#define DISABLE_Z_GYRO	1

#endif

//#ifdef ICM20789_FIFO_COUNTH //no setting. read COUNTH/L to get number of bytes in sensor FIFO buffer	
//#endif 

//#ifdef ICM20789_FIFO_COUNTL 		
//#endif

//#ifdef ICM20789_FIFO_R_W 	//used to Read or Write FIFO buffer	
//#endif


#ifdef ICM20789_XA_OFFS_H //XA_OFFS_H/L will be subtracted from the measured raw sensor value before writing to register or FIFO 		
#endif

#ifdef ICM20789_XA_OFFS_L	 	
#endif

#ifdef ICM20789_YA_OFFS_H 		
#endif

#ifdef ICM20789_YA_OFFS_L 		
#endif

#ifdef ICM20789_ZA_OFFS_H 		
#endif

#ifdef ICM20789_ZA_OFFS_L 		
#endif


#endif //ICM20789_REGISTERSETTINGS_H
