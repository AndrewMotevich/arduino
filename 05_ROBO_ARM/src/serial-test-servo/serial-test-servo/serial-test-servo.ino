#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define MIN_PULSE_WIDTH       650
#define MAX_PULSE_WIDTH       2500
#define FREQUENCY             50

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int hand = 11;
int wrist = 12;
int elbow = 13;                           //Assign Motors to pins on Servo Driver Board
int shoulder = 14;
int base = 15;

void setup() 
{
  delay(5000);                            // Time to get the controller to starting position
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
  pwm.setPWM(11, 0, 90);                  // Set Gripper to 90 degrees (Close Gripper)
  
  pinMode(13, INPUT_PULLUP);              // Button for hand control
  motorsToStart();
  motorsTest();
  Serial.begin(9600);                     // Start serial communication at 9600 baud rate
  Serial.println("Ready for motor control via serial. Send values 0-255 for each motor.");
}

void motorsTest() {
  delay(2000);
  moveMotor(base, 0);
  delay(2000); 
  moveMotor(base, 125);
  delay(2000); 
  moveMotor(shoulder, 100);
  delay(2000);
  moveMotor(shoulder, 0);
  delay(2000);
  moveMotor(elbow, 100);
  delay(2000);
  moveMotor(elbow, 255);
  delay(2000);
  moveMotor(wrist, 200);
  delay(2000);
  moveMotor(wrist, 120);
  delay(2000);
  moveMotor(hand, 255);
  delay(2000);
  moveMotor(hand, 0);
  delay(2000);
}

void moveMotorGradually(int motorOut, int targetInputValue, int speedInputValue) {
  // Convert the target input value (0-255) to pulse width
  int targetPulseWidth = mapValueToPulseWidth(targetInputValue);
  
  // Get the current pulse width (or a starting position)
  int currentPulseWidth = pwm.getPWM(motorOut);  // You can initialize this manually if needed

  // Map the speed input (0-255) to a delay value
  int speedDelay = map(speedInputValue, 0, 255, 100, 1);  // Slow speed = higher delay, fast speed = lower delay

  // Gradually move from the current position to the target position
  if (currentPulseWidth < targetPulseWidth) {
    for (int pulse = currentPulseWidth; pulse <= targetPulseWidth; pulse += 15) {
      pwm.setPWM(motorOut, 0, pulse);
      delay(speedDelay);  // The delay controls how fast the servo moves
    }
  } else {
    for (int pulse = currentPulseWidth; pulse >= targetPulseWidth; pulse -= 15) {
      pwm.setPWM(motorOut, 0, pulse);
      delay(speedDelay);  // The delay controls how fast the servo moves
    }
  }
}

void motorsToStart() {
  moveMotor(base, 125);
  moveMotor(shoulder, 0);
  moveMotor(elbow, 255);
  moveMotor(wrist, 120);
  moveMotor(hand, 0);
}
 
int mapValueToPulseWidth(int value) {
  return map(value, 0, 255, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}
 
void moveMotor(int motorOut, int value)
{
  int pulse_width = mapValueToPulseWidth(value);                // Map input value (0-255) to pulse width
  int pwm_value = int(float(pulse_width) / 1000000 * FREQUENCY * 4096);
  pwm.setPWM(motorOut, 0, pwm_value);
}

void loop() 
{
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // Read serial input until newline character
    char motor = input.charAt(0);                 // First character represents the motor
    int value = input.substring(1).toInt();       // Remaining part is the value (0-255)

    // Move the appropriate motor based on the input
    switch (motor) {
      case 'b':  // Base motor
        moveMotor(base, value);
        Serial.println("Base motor: " + String(value));
        break;
      case 's':  // Shoulder motor
        moveMotor(shoulder, value);
        Serial.println("Shoulder motor: " + String(value));
        break;
      case 'e':  // Elbow motor
        moveMotor(elbow, value);
        Serial.println("Elbow motor: " + String(value));
        break;
      case 'w':  // Wrist motor
        moveMotor(wrist, value);
        Serial.println("Wrist motor: " + String(value));
        break;
      case 'h':  // Hand motor (gripper)
        moveMotor(hand, value);
        Serial.println("Hand motor: " + String(value));
        break;
      default:
        Serial.println("Invalid motor code. Use 'b', 's', 'e', 'w', or 'h'.");
    }
  }
}
