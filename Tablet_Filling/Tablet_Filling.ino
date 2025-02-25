
char ssid[] = "Wifi";
char pass[] = "wifi1234";

#define servo1P 4
#define servo2P 27

#define relay1 18
#define relay2 19

#define opto1 32
#define opto2 35
#define opto3 34

#define echo 26
#define trig 27

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

//#include <Servo.h>
#include <ESP32Servo.h>
Servo servo1;
Servo servo2;

#define servo1open 0
#define servo1close 150

#define servo2open 0
#define servo2close 100


int tablet = 0,     //fbase input
    stage = 0,      //fbase output
    swStage = 0,
    bottleCount = 0,//fbase bottle count
    finish = 0;


int sonicDistance();

#include "fbase.h"
#include"display.h"


void setup()
{
  Serial.begin(115200);

  display.begin();
  display.wait();

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);

  pinMode(opto1, INPUT_PULLUP);
  pinMode(opto2, INPUT_PULLUP);
  pinMode(opto3, INPUT_PULLUP);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  connectWifi();
  fbaseBegin();

  int minUs = 1000,
      maxUs = 2000;

  servo1.setPeriodHertz(50);      // Standard 50hz servo
  servo2.setPeriodHertz(50);
  servo1.attach(servo1P, minUs, maxUs);
  servo2.attach(servo2P, minUs, maxUs);
}


void loop()
{
  display.home(bottleCount, stage, tablet);

  switch (swStage)
  {
    case 0:
      {
        servo2.write(servo2open);
        servo1.write(servo1close);
        digitalWrite(relay1, 1);

        fbaseRead(1);
        digitalWrite(relay2, !tablet);

        //if (!digitalRead(opto1))
        swStage = 1;
        delay(3000);
      }
      break;

    case 1:
      {
        //Serial.println("case:1");

        stage = 0;
        finish = 0;
        fbaseSend(1);

        while (sonicDistance() >= 5)
        {
          //fbaseSend();
          display.home(bottleCount, stage, tablet);
        }

        if (sonicDistance() < 5)
        {
          digitalWrite(relay1, 0);
          servo1.write(servo1open);
          //delay(3000);
          //servo1.write(servo1close);

          bottleCount++;
          stage = 1;
          swStage = 2;

          fbaseSend(1);
          fbaseRead(1);
          digitalWrite(relay2, !tablet);
          
          display.home(bottleCount, stage, tablet);
          delay(3000);
          servo1.write(servo1close);
        }
      }
      break;

    case 2:
      {
        //Serial.println("case:2");
        digitalWrite(relay2, !tablet);

        if (!digitalRead(opto2))
        {
          //digitalWrite(relay1, 1);
          servo1.write(servo1close);
          swStage = 3;
        }
      }
      break;

    case 3:
      {
        //Serial.println("case:3");
        stage = 2;

        bool j = !digitalRead(opto3);
        uint32_t ptime = millis();

        while (j)
        {
          servo2.write(servo2close);
          stage = 3;
          fbaseSend();
          fbaseRead();
          display.home(bottleCount, stage, tablet);
          stage = swStage = 0;

          if (millis() - ptime > 2000)
          {
            j = !digitalRead(opto3);

            if (j) finish = 1;
            else {
              servo2.write(servo2open);
              delay(3000);
            }
          }
        }
      }
      break;
  }

  fbaseSend();


  //  if (!digitalRead(opto1))
  //  {
  //    while (1)
  //    {
  //      if (sonicDistance() < 5)
  //      {
  //        digitalWrite(relay1, 0);
  //        servo1.write(servo1open);
  //
  //        delay(3000);
  //
  //        servo1.write(servo1close);
  //
  //        delay(1000);
  //        break;
  //      }
  //    }
  //    Serial.println("break;");
  //    digitalWrite(relay1, 1);
  //    servo1.write(servo1close);
  //  }
  //  servo1.write(servo1close);
  //
  //  if (!digitalRead(opto1)) stage = 1;
  //  else if (!digitalRead(opto2)) stage = 2;
  //  else if (!digitalRead(opto3)) stage = 3;
  //  else stage = 0;

  //
  //  if (!digitalRead(opto2)) {
  //    servo2.write(servo2close);
  //    delay(300);
  //  }
  //  else {
  //    servo2.write(servo2open);
  //  }


  //  if (Serial.available() > 1) {
  //    int x = Serial.parseInt();
  //    Serial.println(x);
  //    servo2.write(x);
  //  }
}






int sonicDistance()
{
  static uint32_t duration,
         ptime;

  static int distanceCm;

  if (millis() - ptime > 50)
  {
    ptime = millis();

    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    duration = pulseIn(echo, HIGH);
  }

  distanceCm = duration * SOUND_SPEED / 2;

  Serial.println(distanceCm);

  return distanceCm;
}
