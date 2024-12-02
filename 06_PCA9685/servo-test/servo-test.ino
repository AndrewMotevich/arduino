#include <Wire.h>                       // Подключаем библиотеку Wire
#include <Adafruit_PWMServoDriver.h>    // Подключаем библиотеку Adafruit_PWMServoDriver
 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40); // Установка адреса I2C 0x40
 
#define SERVOMIN  150                   // Минимальная длительность импульса для сервопривода
#define SERVOMAX  600                   // Максимальная длина импульса для сервопривода
#define SERVO_1 0                       // Номер порта (0 - 15)
#define SERVO_2 4                       // Номер порта (0 - 15)
 
void setup() 
{
  pwm.begin();                          // Инициализация
  pwm.setPWMFreq(60);                   // Частота следования импульсов 60 Гц
  delay(10);                            // Пауза
}
 
void loop() 
{
  // Вращение сервопривода 1 влево
  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) 
  {
    pwm.setPWM(SERVO_1, 0, pulselen);
  }
  delay(250);
  // Вращение сервопривода 1 вправо
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) 
  {
    pwm.setPWM(SERVO_1, 0, pulselen);
  }
  delay(250);
}
