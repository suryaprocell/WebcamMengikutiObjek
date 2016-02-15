#include <Servo.h>
#include <PID.h>
 
Servo myservoX;
Servo myservoY;

int servoMax = 2400;
int servoMin = 544;
int servoCenterposX,servoCenterposY;

PID myPIDX(0, 0.2, 0); // KP, KI, KD
PID myPIDY(0, 0.2, 0); // KP, KI, KD

int Xaxis, Yaxis;
double pwmX, pwmY, setpointX,setpointY;

String string;
char karakter;
 
void setup() 
{ 
  Serial.begin(9600);
  
  myservoX.attach(7);
  myservoY.attach(8);
  myservoX.write(90);
  myservoY.write(90);
  delay(1000);
  servoCenterposX = myservoX.readMicroseconds();
  servoCenterposY = myservoY.readMicroseconds();
  
  myPIDX.LimitP(0,0); //Pmax , Pmin
  myPIDX.LimitI(1000,-1000); //Imax , Imin
  myPIDY.LimitP(0,0); //Pmax , Pmin
  myPIDY.LimitI(1000,-1000); //Imax , Imin
  setpointX = 160;
  setpointY = 120;
} 
 
void loop() 
{ 
  cek_serial();
  //delay(15); 
} 

void cek_serial(){
  while (Serial.available()) {
    karakter = Serial.read();
    if(karakter != '\n'){
      string = string + karakter;
    }else{
      if(string.startsWith("x")){
        Xaxis = string.substring(1).toInt();
        prosesX();
      }else if(string.startsWith("y")){
        Yaxis = string.substring(1).toInt();
        prosesY();
      }
      string = "";
    }
  }
}

void prosesX(){
  

  pwmX = servoCenterposX + myPIDX.Calculate(setpointX, Xaxis);
  if(pwmX>servoMax)
  {
    pwmX = servoMax;
  }
  if(pwmX<servoMin)
  {
    pwmX = servoMin;
  }
  myservoX.writeMicroseconds(pwmX);
  
}

void prosesY(){

  pwmY = servoCenterposY - myPIDY.Calculate(setpointY, Yaxis);
  if(pwmY>servoMax)
  {
    pwmY = servoMax;
  }
  if(pwmY<servoMin)
  {
    pwmY = servoMin;
  }
  myservoY.writeMicroseconds(pwmY);
}
