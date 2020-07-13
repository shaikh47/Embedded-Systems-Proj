#include <Servo.h>

Servo jaw;
Servo jawTwist;
Servo secondaryArm;
Servo mainRotation;
Servo primaryArm;


void setup() {
  Serial.begin(9600);
  
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
}

void loop() {
  String x[5];
  int from=0;
  int i1=0;
  int gyroX,gyroY,gyroZ;
  
  
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

    //gyroX=x[0].toInt();
    //gyroY=x[1].toInt();
    //gyroZ=x[2].toInt();
    int varTwist=x[0].toInt();
    int varPitchPalm=x[1].toInt();
    int varYaw=x[2].toInt();
    int varPitchArm=x[3].toInt();
    int varJaw=x[4].toInt();

    varTwist=map(varTwist,-80,60,0,180);//roll
    varPitchPalm=map(varPitchPalm,-60,60,25,160);//pitch palm
    varYaw=map(varYaw,-50,50,20,160);//yaw
    varPitchArm=map(varPitchArm,-40,40,80,0);//pitch arm
    varJaw=map(varJaw,125,470,124,60);//jaw

    /*varTwist=constrain(varTwist,0,180);//roll
    varPitchPalm=constrain(varPitchPalm,25,160);//pitch palm
    varYaw=constrain(varYaw,0,180);//yaw
    varPitchArm=constrain(varPitchArm,80,0);//pitch arm
    varJaw=constrain(varJaw,124,65);//jaw*/
    
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
    //Serial.println(a);
  }
}
