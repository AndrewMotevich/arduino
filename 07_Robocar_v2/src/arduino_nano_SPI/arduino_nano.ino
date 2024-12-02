#include <Servo.h>

#define MOTOR_IN1 2
#define MOTOR_IN2 3
#define MOTOR_IN3 4
#define MOTOR_IN4 5
#define MOTOR_EN1 10
#define MOTOR_EN2 11

Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(9600);

  // DC Motor pins
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  pinMode(MOTOR_EN1, OUTPUT);
  pinMode(MOTOR_EN2, OUTPUT);

  // Attach servos to PWM pins
  servo1.attach(6); // Connect servo 1 to pin 6
  servo2.attach(9); // Connect servo 2 to pin 9

  // Initialize servos to 90 degrees (neutral position)
  servo1.write(90);
  servo2.write(90);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read the entire command line

//    if (command.startsWith("M:")) {
//      // Control motor speed with command M:<speed>
//      int speed = command.substring(2).toInt();
//      speed = constrain(speed, 0, 255); // Constrain speed to 0-255 range
//      analogWrite(MOTOR_EN1, speed);
//      analogWrite(MOTOR_EN2, speed);
//    }
    digitalWrite(MOTOR_EN1, HIGH); 
    digitalWrite(MOTOR_EN2, HIGH);
    if (command.startsWith("S1:")) {
      // Set servo1 angle with command S1:<angle>
      int angle = command.substring(3).toInt();
      angle = constrain(angle, 0, 180); // Constrain angle to 0-180 degrees
      servo1.write(angle);
    }
    else if (command.startsWith("S2:")) {
      // Set servo2 angle with command S2:<angle>
      int angle = command.substring(3).toInt();
      angle = constrain(angle, 0, 180); // Constrain angle to 0-180 degrees
      servo2.write(angle);
    }
    else {
      // DC motor directional control commands
      char motorCommand = command.charAt(0);

      switch (motorCommand) {
        case 'F': // Forward
          digitalWrite(MOTOR_IN1, HIGH);
          digitalWrite(MOTOR_IN2, LOW);
          digitalWrite(MOTOR_IN3, HIGH);
          digitalWrite(MOTOR_IN4, LOW);
          break;
          
        case 'B': // Backward
          digitalWrite(MOTOR_IN1, LOW);
          digitalWrite(MOTOR_IN2, HIGH);
          digitalWrite(MOTOR_IN3, LOW);
          digitalWrite(MOTOR_IN4, HIGH);
          break;
          
        case 'L': // Left
          digitalWrite(MOTOR_IN1, LOW);
          digitalWrite(MOTOR_IN2, HIGH);
          digitalWrite(MOTOR_IN3, HIGH);
          digitalWrite(MOTOR_IN4, LOW);
          break;
          
        case 'R': // Right
          digitalWrite(MOTOR_IN1, HIGH);
          digitalWrite(MOTOR_IN2, LOW);
          digitalWrite(MOTOR_IN3, LOW);
          digitalWrite(MOTOR_IN4, HIGH);
          break;
          
        case 'S': // Stop
          digitalWrite(MOTOR_IN1, LOW);
          digitalWrite(MOTOR_IN2, LOW);
          digitalWrite(MOTOR_IN3, LOW);
          digitalWrite(MOTOR_IN4, LOW);
          break;
      }
    }
  }
}
