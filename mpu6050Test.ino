#include <Wire.h>

const int MPU2 = 0x69, MPU1=0x68;

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ, gyroX, gyroY, gyroZ,rotX, rotY, rotZ;
long accelX2, accelY2, accelZ2;
float gForceX2, gForceY2, gForceZ2;

double ax1,ay1,az1;
double ax2,ay2,az2;

double offx1,offy1,offz1;
double offx2,offy2,offz2;
double errorX1=0;
double errorY1=0;
double errorZ1=0;

double errorX2=0;
double errorY2=0;
double errorZ2=0;

int button=3;
int clawPot=A7;


int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int minVal=265;
int maxVal=402;
double x;
double y;
double z;

//foreign
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c=0;

void setup(){
  Serial.begin(9600);
  pinMode(clawPot, INPUT);
  pinMode(button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button), reset, RISING);
  
  Wire.begin();
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B);
  Wire.write(0b00000000);
  Wire.endTransmission();  
  Wire.beginTransmission(MPU1);
  Wire.write(0x1B);
  Wire.write(0x00000000);
  Wire.endTransmission(); 
  Wire.beginTransmission(MPU1);
  Wire.write(0x1C);
  Wire.write(0b00000000);
  Wire.endTransmission(); 
  
  Wire.begin();
  Wire.beginTransmission(MPU2);
  Wire.write(0x6B);
  Wire.write(0b00000000); 
  Wire.endTransmission();  
  Wire.beginTransmission(MPU2); 
  Wire.write(0x1B);
  Wire.write(0x00000000);
  Wire.endTransmission(); 
  Wire.beginTransmission(MPU2);
  Wire.write(0x1C);
  Wire.write(0b00000000);
  Wire.endTransmission(); 

  //calculate_IMU_error();
  delay(20);

//error calculation for mpu1
  /*offx1=0;
  offy1=0;
  offz1=0;
  for(int k=0;k<100;k++){
    GetMpuValue(MPU1);
  
    offx1=offx1+gyroX;
    offy1=offy1+gyroY;
    offz1=offz1+gyroZ;
    delay(10);
  }
  errorX1=offx1/(100.0*131);
  errorY1=offy1/(100.0*131);
  errorZ1=offz1/(100.0*131);

  //error calculation for mpu2
  offx2=0;
  offy2=0;
  offz2=0;
  for(int k=0;k<100;k++){
    GetMpuValue(MPU2);
  
    offx2=offx2+gyroX;
    offy2=offy2+gyroY;
    offz2=offz2+gyroZ;
    delay(10);
  }
  errorX2=offx2/(100.0*131);
  errorY2=offy2/(100.0*131);
  errorZ2=offz2/(100.0*131);*/
}
unsigned long previousMillis = 0;
void loop(){
  GetMpuValue(MPU1);
  String data="";
  int sensorValue = analogRead(clawPot);//read the pot value
  
  data=String((int)ax1)+'$'+String((int)ay1)+'$'+String((int)az1)+'$';
  //Serial.print("gyro\t");
  //Serial.print(ax1/50);
  //Serial.print("\t");
 // Serial.print(ay1/50);
  //Serial.print("\t");
  //Serial.print(az1/50);
  
  //Serial.print("\t ||| \t");

  GetMpuValue(MPU2);
 
  data=data+String((int)ay2)+'$'+sensorValue+'$'+'&';
  //Serial.print("gyro\t");
  //Serial.print(ax2/50);
  //Serial.print("\t");
  //Serial.print(ay2/50);
  //Serial.print("\t");
  //Serial.print(az2/50);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 80) {
    previousMillis = currentMillis;
    Serial.println(data);
  }
}



void GetMpuValue(const int MPU){
  float accError1Y=-3.41;
  float gyroError1Y=-1.22;
  
  float accError2Y=-17.15;
  float gyroError2Y=0.69;
  
  float gyroError,accError;
  if(MPU==MPU1){
    gyroError=gyroError1Y;
    accError=accError1Y;
  }
  else{
    gyroError=gyroError2Y;
    accError=accError2Y;
  }
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 13.42; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + accError; // AccErrorY ~(-1.58)
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  GyroX = GyroX - 0.90; // GyroErrorX ~(-0.56)
  GyroY = GyroY + gyroError; // GyroErrorY ~(2)
  GyroZ = GyroZ + 1.94; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  
  if(MPU==MPU1){
    az1 =  az1 + GyroZ * elapsedTime;
    // Complementary filter - combine acceleromter and gyro angle values
    ax1 = 0.10 * gyroAngleX + 0.90 * accAngleX;
    ay1 = 0.10 * gyroAngleY + 0.90 * accAngleY;
  }
  else{
    ay2 = 0.10 * gyroAngleY + 0.90 * accAngleY;
  }

}


void reset(){
  ax1=0,ay1=0,az1=0;
  ax2=0,ay2=0,az2=0;
}


void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU1);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU1, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU1);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU1, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}
