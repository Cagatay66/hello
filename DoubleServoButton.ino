#include <Servo.h>


Servo servo360;
bool pressed=0;
long presstime=0;

void setup() {
  Serial.begin(9600);
  servo360.attach(9);
  servo360.write(175);
  for(int i=175;i<180;i++) {
    servo360.write(i);
    delay(5);
  }
  delay(50);
}

void loop() {
  for(int i=180;i>0;i--) {
    servo360.write(i);
    delay(5);
  }
  delay(100000);
  }
