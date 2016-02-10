#include <Servo.h>
#include <PID.h>
 
Servo myservo1;
Servo myservo2;

int servoMax = 2400;
int servoMin = 544;

PID myPID1(0.25, 0, 0); // KP, KI, KD
PID myPID2(0.25, 0, 0); // KP, KI, KD

int Xaxis, Yaxis;
double pwmX, pwmY, setpointX,setpointY;

String string;
char karakter;
 
void setup() 
{ 
  Serial.begin(9600);
  myservo1.attach(7);
  myservo2.attach(8);
  myservo1.write(90);
  myservo2.write(90);
  myPID1.LimitP(500,-500); //Pmax , Pmin
  myPID1.LimitI(100,-100); //Imax , Imin
  myPID2.LimitP(500,-500); //Pmax , Pmin
  myPID2.LimitI(100,-100); //Imax , Imin
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
  

  pwmX = myservo1.readMicroseconds() + myPID1.Calculate(setpointX, Xaxis);
  if(pwmX>servoMax)
  {
    pwmX = servoMax;
  }
  if(pwmX<servoMin)
  {
    pwmX = servoMin;
  }
  myservo1.writeMicroseconds(pwmX);
  
}

void prosesY(){

  pwmY = myservo2.readMicroseconds() - myPID2.Calculate(setpointY, Yaxis);
  if(pwmY>servoMax)
  {
    pwmY = servoMax;
  }
  if(pwmY<servoMin)
  {
    pwmY = servoMin;
  }
  myservo2.writeMicroseconds(pwmY);
}
