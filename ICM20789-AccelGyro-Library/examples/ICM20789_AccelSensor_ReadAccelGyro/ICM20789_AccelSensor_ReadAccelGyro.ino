//Read all registers and print it out

//arduino nano I2C pins: SDA=A4, SCL = A5


#include "Accel_Gyro.h"

//sensor settings according to settings struct in Sensor HAL
TYP_ICM20789_AccelGyro_config icm20789_config = {
  //ICM20789_SMPLRT_DIV
  icm20789_config.smplrt_div = 0,

  //ICM20789_CONFIG
  icm20789_config.icm_config = 0 | FIFO_FULL_REPL_OLD | DLPF_CFG_4, //gyro lowpass 20 Hz

  //ICM20789_GYRO_CONFIG
  icm20789_config.gyro_config = 0 | FCHOICE_B_00, // gyro lp filter enabeled

  //ICM20789_ACCEL_CONFIG
  //full scale accel = +-2,4,8,16g
  icm20789_config.accel_config = 0, // accelGyro_HAL.set_ACCEL_FS_BITS doesnt look like a good solution

  //ICM20789_ACCEL_CONFIG2
  icm20789_config.accel_config2 = 0 | DEC2_CFG_8 | ACCEL_FCHOICE_B_0 | ACCEL_DLPF_1, //average 8 samples, lowpass enabeled, 21 Hz lowpass

  //ICM20789_LP_MODE_CTRL
  icm20789_config.lp_mode_ctrl = 0, 

  //ICM20789_ACCEL_WOM_X_THR
  icm20789_config.accel_wom_x_thr = 0,

  //ICM20789_ACCEL_WOM_Y_THR
  icm20789_config.accel_wom_y_thr = 0, 

  //ICM20789_ACCEL_WOM_Z_THR
  icm20789_config.accel_wom_z_thr = 0, 

  //ICM20789_FIFO_EN
  icm20789_config.fifo_en = 0, //send nothing to fifo 

  //ICM20789_INT_PIN_CFG
  icm20789_config.int_pin_cfg = 0, //no int config 

  //ICM20789_INT_ENABLE
  icm20789_config.int_enable = 0, //all interrupts disabled 

  //ICM20789_ACCEL_INTEL_CTRL
  icm20789_config.accel_intel_ctrl = 0, //no Wake on motion logic

  //ICM20789_USER_CTRL
  icm20789_config.user_ctrl = 0, //fifo dmp disabeled

  //ICM20789_PWR_MGMT_1
  icm20789_config.pwr_mgmt_1 = 0, //never sleep 

  //ICM20789_PWR_MGMT_2 
  icm20789_config.pwr_mgmt_2 = 0 | LP_DISABLE, //disable lowpower

};

const uint16_t fullScale_accel = 2; //+-2 g
const uint16_t fullScale_gyro = 250; //+-250 dps
AccelGyro accelGyroSensor(fullScale_accel, fullScale_gyro);

//software Lowpass
//PT 1 Filter
 ///////////////////////////////////////////////////////////
// this function is a diskrete PT1 filter, its exact, as long uk0 konstant over one timestep
// the matimatical function would be: y[k+1] = u[k] * (1 - exp(-timestep * fknick)) + exp(-timestep * fknick) * y[k]
// inputs:
// yk0 - y[k] the last value of this function, or start value
// uk0 - input signal 
// timestep - timestep between two adc measurements
// fknick - kink (german knick) frequency where the transfere function knicks down

//alternativ 1/n if timestep=constant 
int16_t PT1_val(int16_t yk0, int16_t uk0, float n){
  float e = 2.71828;
  double a = pow(e, -1/n);
  int16_t yk1 = (int16_t) uk0 * (1-a) + a * yk0;
  return yk1;
}



void setup() {
  Wire.begin();
  delay(50);
  Serial.begin(115200);
  
  uint8_t err = accelGyroSensor.ConfigAccelGyro(icm20789_config);
  //Serial.println("Config Error (0 = success): " + String(err));
  //Wire.setClock(100000);
  delay(500);
}

void loop() {

 //static int16_t gyro_LP = 0;

 float accel_x_g = accelGyroSensor.GetAccel_x_g();
 float accel_y_g = accelGyroSensor.GetAccel_y_g();
 float accel_z_g = accelGyroSensor.GetAccel_z_g();
 
 float gyro_x_dps = accelGyroSensor.GetGyro_x_dps();
 float gyro_y_dps = accelGyroSensor.GetGyro_y_dps();
 float gyro_z_dps = accelGyroSensor.GetGyro_z_dps();

 //raw Sensor reading. No formula for calculating actual temperature from accel gyro sensor, only from pressure sensor
 //and that formula doest work here
 uint16_t accelGyro_TempVal = accelGyroSensor.GetDIE_Temperature_val();


//Software Lowpass Filter
//int16_t gyrox_LP = PT1_val(gyrox_LP, gyroRaw.x, 3);
//int16_t gyroy_LP = PT1_val(gyroy_LP, gyroRaw.y, 3);
//int16_t gyroz_LP = PT1_val(gyroz_LP, gyroRaw.z, 3);

//accel is more sensetive but if the value in g is used the value should be multiplied by 20 to have around the same amplitude as gyro in dps

 
//print accel in g 
 //Serial.print(String(accel_x_g) + " ");
 //Serial.print(String(accel_y_g) + " ");
 //Serial.print(String(accel_z_g) + " ");


//print gyro Lowpass Raw
 //Serial.print(String(gyrox_LP) + " ");
 //Serial.print(String(gyroy_LP) + " ");
 //Serial.print(String(gyroz_LP) + " ");

//print gyro in dps
 //Serial.print(String(gyro_x_dps) + " ");
 //Serial.print(String(gyro_y_dps) + " ");
 //Serial.print(String(gyro_z_dps) + " ");
 
 Serial.print(String(accelGyro_TempVal) + " ");

 Serial.println();


//delay(1);
}

