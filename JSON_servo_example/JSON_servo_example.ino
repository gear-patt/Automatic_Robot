#include <ArduinoJson.h>
#include <Servo.h>
#include "configuration.h"
// Allocate the JSON document
//
// Inside the brackets, 200 is the capacity of the memory pool in bytes.
// Don't forget to change this value to match your JSON document.
// Use arduinojson.org/v6/assistant to compute the capacity.
double Deadband(double value,double limit);
int OutputToMotor1(int value);
StaticJsonDocument<200> doc;
unsigned long previousLoopTime = 0;
unsigned long loopTime = 0;
int currentLimit = 5;
Servo servo1;
Servo servo2;
float currentValue1 = 0.0;
int out1 = 0;
int num = 0;

// StaticJsonDocument<N> allocates memory on the stack, it can be
// replaced by DynamicJsonDocument which allocates in the heap.
//
// DynamicJsonDocument doc(200);


void setup() {
  // Initialize serial port
  pinMode(INA1,OUTPUT);
  pinMode(INB1,OUTPUT);
  pinMode(PWM1,OUTPUT);
  digitalWrite(INA1,LOW);
  digitalWrite(INB1,LOW);
  analogWrite(PWM1,0);
  Serial.begin(115200);
  while (!Serial) continue;

  servo1.attach(44);
  servo2.attach(46);  

//    Example data
//    {"servo1":90,"servo2":120}
}

void loop() {
  // Check if there is data in serial buffer
  if (Serial.available()){
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, Serial);

    // Test if parsing succeeds.
    if (!error) {
      if(num < 3){
        int angle1 = doc["servo1"];
        int angle2 = doc["servo2"];
        servo1.write(angle1); //only 180(หุบ)-120(กางสุด)
        servo2.write(angle2);
        num = num+1;
      }
      loopTime = micros()-previousLoopTime;
      if(loopTime >= 10000) 
      {
        // Set new loop time
        if(num == 3){
          previousLoopTime = micros();
          currentValue1 = analogRead(CS1)*0.035;
          int value1 = doc["motor1"];
          if(currentValue1 < currentLimit) out1 = OutputToMotor1(value1);
            else out1 = OutputToMotor1(0);
          analogWrite(PWM1,out1);
        }
      }
    }
  }
}

int OutputToMotor1(int value)
  {
    int temp = 0;
    if(value >= 0)
    {
      // CW
      digitalWrite(INA1,LOW);
      digitalWrite(INB1,HIGH);
      temp = map(value,0,500,0,255); //original=500
    }else{
      // CCW
      digitalWrite(INA1,HIGH);
      digitalWrite(INB1,LOW);
      temp = map(-value,0,500,0,255); //original=500
    }
    return temp;
  }
