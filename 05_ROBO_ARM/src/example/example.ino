/*servo motor driver board control
   Home Page
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver srituhobby = Adafruit_PWMServoDriver();

#define servoMIN 150
#define servoMAX 600

void setup() {
  Serial.begin(9600);
  srituhobby.begin();
  srituhobby.setPWMFreq(60);
}

void loop() {
  for (int servo = 0; servo < 4; servo++ ) {
    srituhobby.setPWM(servo, 0, servoMIN);
    Serial.println(servo);
    delay(300);
  }

  for (int servo = 3; servo >= 0; servo-- ) {
    srituhobby.setPWM(servo, 0, servoMAX);
    Serial.println(servo);
    delay(300);
  }
}
