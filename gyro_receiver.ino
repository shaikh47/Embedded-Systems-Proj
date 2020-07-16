#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//i2c pins
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //

Servo jaw;
Servo jawTwist;
Servo secondaryArm;
Servo mainRotation;
Servo primaryArm;

int joy1x=0;
int joy1y=0;
int joy2x=0;
int joy2y=0;

byte mode=1;


int posA;
int posB;
int posC;
int posD;

void setup() {
  Serial.begin(9600);
  lcd.begin(20,4);
  lcd.backlight();//Power on the back light
  
  jaw.attach(11);
  jawTwist.attach(5);
  secondaryArm.attach(10);
  mainRotation.attach(9);
  primaryArm.attach(6);

  jaw.write(124);
  jawTwist.write(80);
  secondaryArm.write(160);
  mainRotation.write(60);
  primaryArm.write(0);

  posA=90;
  posB=90;
  posC=70;
  posD=40;
}

void loop() {
  String x[5];
  int from=0;
  int i1=0;
  int gyroX,gyroY,gyroZ;
  
  
  if(mode==1){
    lcd.setCursor(0,0);
    lcd.print("MANUAL");
    joy1x=analogRead(A0);
    joy1y=analogRead(A1);
    joy2x=analogRead(A2);
    joy2y=analogRead(A3);

    if(joy1x > 700 || joy1x < 300){
      float diff=(joy1x-507)/350.0;
      posA=posA+diff;
      posA=constrain(posA,20,160);
      delay(35);
    }else if(joy1y > 700 || joy1y < 300){
      float diff=(joy1y-507)/350.0;
      posB=posB+diff;
      posB=constrain(posB,25,160);
      delay(35);
    }else if(joy2x > 700 || joy2x < 300){
      float diff=(joy2x-507)/350.0;
      posC=posC+diff;
      posC=constrain(posC,30,110);
      delay(35);
    }else if(joy2y > 700 || joy2y < 300){
      float diff=(joy2y-507)/350.0;
      posD=posD+diff;
      posD=constrain(posD,0,80);
      delay(35);
    }
    String valDisplay1="posA: "+String(posA)+" posB: "+String(posB);
    String valDisplay2="posC: "+String(posC)+" posD: "+String(posD);
    lcd.setCursor(0,2);
    lcd.print(valDisplay1);
    lcd.setCursor(0,3);
    lcd.print(valDisplay2);
    
    Serial.print(posA);
    Serial.print("   ");
    Serial.print(posB);
    Serial.print("   ");
    Serial.print(posC);
    Serial.print("   ");
    Serial.print(posD);
    Serial.print("   ");
    Serial.println("   ");

    jawTwist.write(posA);
    secondaryArm.write(posB);
    mainRotation.write(posC);
    primaryArm.write(posD);
    
  }
  else{
    while(Serial.available()) {
    String a = Serial.readStringUntil('&');
    int len=a.length();
    for(int i=0;i<len;i++){
      if(a.charAt(i)=='$'){
        x[i1]=a.substring(from,i);
        from=++i;
        i1++;
      }
    }

    int varTwist=x[0].toInt();
    int varPitchPalm=x[1].toInt();
    int varYaw=x[2].toInt();
    int varPitchArm=x[3].toInt();
    int varJaw=x[4].toInt();

    varTwist=map(varTwist,-80,60,20,160);//roll
    varPitchPalm=map(varPitchPalm,-60,60,160,25);//pitch palm
    varYaw=map(varYaw,-20,20,30,110);//yaw
    varPitchArm=map(varPitchArm,-40,40,80,0);//pitch arm
    varJaw=map(varJaw,125,470,124,60);//jaw
    
    jaw.write(varJaw);
    jawTwist.write(varTwist);
    secondaryArm.write(varPitchPalm);
    mainRotation.write(varYaw);
    primaryArm.write(varPitchArm);
    
    /*Serial.print(x[0]);
    Serial.print("  ");
    Serial.print(x[1]);
    Serial.print("  ");
    Serial.print(x[2]);
    Serial.print("  ");
    Serial.print(x[3]);
    Serial.print("  ");
    Serial.print(x[4]);
    Serial.println("");*/
  }

  }
}
