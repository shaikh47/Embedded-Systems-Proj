#include <Wire.h>
const int MPU2 = 0x69, MPU1=0x68;

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ, gyroX, gyroY, gyroZ,rotX, rotY, rotZ;
long accelX2, accelY2, accelZ2;
float gForceX2, gForceY2, gForceZ2;

double ax1=0,ay1=0,az1=0;
double ax2=0,ay2=0,az2=0;

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
  

//error calculation for mpu1
  offx1=0;
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
  errorZ2=offz2/(100.0*131);
}
unsigned long previousMillis = 0;
void loop(){
  GetMpuValue(MPU1);
  String data="";
  int sensorValue = analogRead(clawPot);//read the pot value
  
  if(abs(rotX-errorX1)>6 || abs(rotY-errorY1)>6 || abs(rotZ-errorZ1)>6){
    ax1=ax1+rotX-errorX1;
    ay1=ay1+rotY-errorY1;
    az1=az1+rotZ-errorZ1;
  }

  data=String((int)ax1/150)+'$'+String((int)ay1/150)+'$'+String((int)az1/150)+'$';
  //Serial.print("gyro\t");
  //Serial.print(ax1/50);
  //Serial.print("\t");
 // Serial.print(ay1/50);
  //Serial.print("\t");
  //Serial.print(az1/50);
  
  //Serial.print("\t ||| \t");

  GetMpuValue(MPU2);
  if(abs(rotX-errorX2)>6 || abs(rotY-errorY2)>6 || abs(rotZ-errorZ2)>6){
    ax2=ax2+rotX-errorX2;
    ay2=ay2+rotY-errorY2;
    az2=az2+rotZ-errorZ2;
  }
  data=data+String((int)ay2/150)+'$'+sensorValue+'$'+'&';
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
  Wire.beginTransmission(MPU); 
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(MPU,6);
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); 
  accelY = Wire.read()<<8|Wire.read(); 
  accelZ = Wire.read()<<8|Wire.read();
  
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(MPU,6);
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read();
  gyroY = Wire.read()<<8|Wire.read();
  gyroZ = Wire.read()<<8|Wire.read(); 


  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0; 
  rotZ = gyroZ / 131.0;


  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);

  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);//degree
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);//degree
  z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);//degree
  /*Serial.print("\tAcc\t");
  Serial.print(gForceX);
  Serial.print("\t");
  Serial.print(gForceY);
  Serial.print("\t");
  Serial.print(gForceZ);*/
}


void reset(){
  ax1=0,ay1=0,az1=0;
  ax2=0,ay2=0,az2=0;
}
