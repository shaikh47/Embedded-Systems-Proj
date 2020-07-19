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

int button=2;
int record=4;

int clawOpen=3;
int clawClose=7;

int joy1x=0;
int joy1y=0;
int joy2x=0;
int joy2y=0;

byte mode=0;
boolean modeFlag=true;

int posA;
int posB;
int posC;
int posD;
int posE;

int flag=1;

byte arrow[8] = {
  B01000,
  B01100,
  B11110,
  B11111,
  B11110,
  B01100,
  B01000,
};

byte jawVal[10];
byte jawTwistVal[10];
byte secondaryArmVal[10];
byte mainRotationVal[10];
byte primaryArmVal[10];
byte recordLength;

void setup() {
  Serial.begin(9600);
  lcd.begin(20,4);
  lcd.backlight();//Power on the back light

  lcd.createChar(1, arrow);

  pinMode(button, INPUT_PULLUP);
  pinMode(record, INPUT);
  pinMode(clawOpen, INPUT);
  pinMode(clawClose, INPUT);
  attachInterrupt(digitalPinToInterrupt(button), modeChange, RISING);
  
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
  posE=90;
  recordLength=0;
}

void loop() {
  String x[5];
  int from=0;
  int i1=0;
  int gyroX,gyroY,gyroZ;
  
  while(modeFlag==true){
    menu();
  }
  if(mode==0){
    lcd.setCursor(0,0);
    lcd.print("MANUAL         ");
    lcd.setCursor(0,1);
    lcd.print("                    ");
    joy1x=analogRead(A0);
    joy1y=analogRead(A1);
    joy2x=analogRead(A2);
    joy2y=analogRead(A3);

    if(joy1x > 700 || joy1x < 300){
      float diff=(joy1x-507)/250.0;
      posA=posA+diff;
      posA=constrain(posA,20,160);
      delay(35);
    }if(joy1y > 700 || joy1y < 300){
      float diff=(joy1y-507)/250.0;
      posB=posB+diff;
      posB=constrain(posB,25,160);
      delay(35);
    }if(joy2x > 700 || joy2x < 300){
      float diff=(joy2x-507)/250.0;
      posC=posC+diff;
      posC=constrain(posC,30,110);
      delay(35);
    }if(joy2y > 700 || joy2y < 300){
      float diff=(joy2y-507)/250.0;
      posD=posD+diff;
      posD=constrain(posD,0,80);
      delay(35);
    }
    if(digitalRead(clawOpen)==HIGH){
      posE=posE+1;
      posE=constrain(posE,60,124);
      delay(35);
    }
    if(digitalRead(clawClose)==HIGH){
      posE=posE-1;
      posE=constrain(posE,60,124);
      delay(35);
    }
    if(digitalRead(record)==HIGH){
      lcd.clear();
      lcd.setCursor(0,2);
      lcd.print("RECORDED");
      
      jawVal[recordLength]=posE;
      jawTwistVal[recordLength]=posA;
      secondaryArmVal[recordLength]=posB;
      mainRotationVal[recordLength]=posC;
      primaryArmVal[recordLength]=posD;
      recordLength++;
      delay(400);
      lcd.clear();
    }
    
    String valDisplay1="A: "+String(posA)+" B: "+String(posB)+"   "+"C: "+String(posC);
    String valDisplay2="D: "+String(posD)+" E: "+String(posE);
    lcd.setCursor(0,2);
    lcd.print(valDisplay1);
    lcd.setCursor(0,3);
    lcd.print(valDisplay2);
    
    /*Serial.print(digitalRead(clawOpen));
    Serial.print("   ");
    Serial.print(digitalRead(button));
    Serial.print("   ");
    Serial.print(digitalRead(record));
    Serial.print("   ");
    Serial.print(digitalRead(clawClose));
    Serial.print("   ");
    Serial.println("   ");*/

    jawTwist.write(posA);
    secondaryArm.write(posB);
    mainRotation.write(posC);
    primaryArm.write(posD);
    jaw.write(posE);
    flag=1;
  }
  else if(mode==1){
    if(flag==1)
      lcd.clear();
    flag=0;
    lcd.setCursor(0,0);
    lcd.print("BIONIC CONTROL");
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
  else if(mode==2){//playback recorded moves
    lcd.setCursor(0,0);
    lcd.print("AUTO MIMIC         ");
    lcd.setCursor(0,1);
    lcd.print("                   ");
    lcd.setCursor(0,2);
    lcd.print("                   ");
    lcd.setCursor(0,3);
    lcd.print("                   ");

    byte k=0;
    while(mode==2){
      byte stepLength1=abs(jawVal[k]-jawVal[k+1]);
      byte stepLength2=abs(jawTwistVal[k]-jawTwistVal[k+1]);
      byte stepLength3=abs(secondaryArmVal[k]-secondaryArmVal[k+1]);
      byte stepLength4=abs(mainRotationVal[k]-mainRotationVal[k+1]);
      byte stepLength5=abs(primaryArmVal[k]-primaryArmVal[k+1]);

      byte step1;
      byte step2;
      byte step3;
      byte step4;
      byte step5;

      
      
      byte maxNum=biggest(abs(stepLength1),abs(stepLength2),abs(stepLength3),abs(stepLength4),abs(stepLength5));
      byte minNum=smallest(abs(stepLength1),abs(stepLength2),abs(stepLength3),abs(stepLength4),abs(stepLength5));
      byte diff=maxNum-minNum;
      
      step1=jawVal[k];
      step2=jawTwistVal[k];
      step3=secondaryArmVal[k];
      step4=mainRotationVal[k];
      step5=primaryArmVal[k];

   
      for(int s=0;s<recordLength;s++){
         Serial.print(jawVal[s]);
         Serial.print(" ");
         Serial.print(jawTwistVal[s]);
         Serial.print(" ");
         Serial.print(secondaryArmVal[s]);
         Serial.print(" ");
         Serial.print(mainRotationVal[s]);
         Serial.print(" ");
         Serial.print(primaryArmVal[s]);
         Serial.println(" ");
         Serial.println("- - - - - - - - - - - - - - - -- -  ");
      }
       delay(1000);
      
      for(byte x=0;x<diff;x++){
         
         if(step1!=jawVal[k+1]){
             if(jawVal[k]<jawVal[k+1]){
                step1++;
                jaw.write(step1);
             }
             else if(jawVal[k]>jawVal[k+1]){
                step1--;
                jaw.write(step1);
             }
         }
         if(step2!=jawTwistVal[k+1]){
             if(jawTwistVal[k]<jawTwistVal[k+1]){
                step2++;
                jawTwist.write(step2);
             }
             else if(jawTwistVal[k]>jawTwistVal[k+1]){
                step2--;
                jawTwist.write(step2);
             }
         }
         if(step3!=secondaryArmVal[k+1]){
             if(secondaryArmVal[k]<secondaryArmVal[k+1]){
                step3++;
                secondaryArm.write(step3);
             }
             else if(secondaryArmVal[k]>secondaryArmVal[k+1]){
                step3--;
                secondaryArm.write(step3);
             } 
         }
         if(step4!=mainRotationVal[k+1]){
             if(mainRotationVal[k]<mainRotationVal[k+1]){
                step4++;
                mainRotation.write(step4);
             }
             else if(mainRotationVal[k]>mainRotationVal[k+1]){
                step4--;
                mainRotation.write(step4);
             }   
         }
         if(step5!=primaryArmVal[k+1]){
             if(primaryArmVal[k]<primaryArmVal[k+1]){
                step5++;
                primaryArm.write(step5);
             }
             else if(primaryArmVal[k]>primaryArmVal[k+1]){
                step5--;
                primaryArm.write(step5);
             }   
         }
         delay(40);
         Serial.print(step1);
         Serial.print(" ");
         Serial.print(step2);
         Serial.print(" ");
         Serial.print(step3);
         Serial.print(" ");
         Serial.print(step4);
         Serial.print(" ");
         Serial.print(step5);
         Serial.print(" ");
         Serial.print(x);
         Serial.println("");
      }
      
      if(k>=recordLength-2){
        Serial.println("loop ended");
        delay(5000);
        k=0;
      }
      else{
        k++; 
      }
    }

    
  }
}


void modeChange(){
    modeFlag=!modeFlag;
}

void menu(){
  int var=analogRead(A1);

  lcd.setCursor(0,mode);
  lcd.write(byte(1));

  lcd.setCursor(1,0);
  lcd.print("1.MANUAL MODE");
  lcd.setCursor(1,1);
  lcd.print("2.BIONIC MIMIC MODE");
  lcd.setCursor(1,2);
  lcd.print("3.TEACH MODE");

  if(var>700 && mode<2){
    mode++;
    mode=constrain(mode,0,2);
    
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("1.MANUAL MODE");
    lcd.setCursor(1,1);
    lcd.print("2.BIONIC MIMIC MODE");
    lcd.setCursor(1,2);
    lcd.print("3.TEACH MODE");
    
    lcd.setCursor(0,mode);
    lcd.write(byte(1));

    delay(300);
  }
  else if(var<300 && mode>0){
    mode--;
    mode=constrain(mode,0,2);
    
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("1.MANUAL MODE");
    lcd.setCursor(1,1);
    lcd.print("2.BIONIC MIMIC MODE");
    lcd.setCursor(1,2);
    lcd.print("3.TEACH MODE");

    lcd.setCursor(0,mode);
    lcd.write(byte(1));
    
    delay(300);
  }
}


byte biggest(byte a,byte b,byte c,byte d,byte e){
  byte var=max(a,b);
  var=max(var,c);
  var=max(var,d);
  var=max(var,e);
  return var;
}
byte smallest(byte a,byte b,byte c,byte d,byte e){
  byte var=min(a,b);
  var=min(var,c);
  var=min(var,d);
  var=min(var,e);
  return var;
}
