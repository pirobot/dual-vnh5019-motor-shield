#include "DualVNH5019MotorShield.h"

// Constructors ////////////////////////////////////////////////////////////////

#define ICR1 400

DualVNH5019MotorShield::DualVNH5019MotorShield()
{
  //Pin map
  _INA1 = 2;
  _INB1 = 4;
  _EN1DIAG1 = 6;
  _CS1 = A0; 
  _INA2 = 7;
  _INB2 = 8;
  _EN2DIAG2 = 12;
  _CS2 = A1;
}

DualVNH5019MotorShield::DualVNH5019MotorShield(unsigned char INA1, unsigned char INB1, unsigned char EN1DIAG1, unsigned char CS1, 
                                               unsigned char INA2, unsigned char INB2, unsigned char EN2DIAG2, unsigned char CS2)
{
  //Pin map
  //PWM1 and PWM2 cannot be remapped because the library assumes PWM is on timer1
  _INA1 = INA1;
  _INB1 = INB1;
  _EN1DIAG1 = EN1DIAG1;
  _CS1 = CS1;
  _INA2 = INA2;
  _INB2 = INB2;
  _EN2DIAG2 = EN2DIAG2;
  _CS2 = CS2;
}

// Public Methods //////////////////////////////////////////////////////////////
void DualVNH5019MotorShield::init()
{
// Define pinMode for the pins and set the frequency for timer1.

  pinMode(_INA1,OUTPUT);
  pinMode(_INB1,OUTPUT);
  pinMode(_PWM1,OUTPUT);
  pinMode(_EN1DIAG1,INPUT);
  pinMode(_CS1,INPUT);
  pinMode(_INA2,OUTPUT);
  pinMode(_INB2,OUTPUT);
  pinMode(_PWM2,OUTPUT);
  pinMode(_EN2DIAG2,INPUT);
  pinMode(_CS2,INPUT);
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__)
  // Timer 1 configuration
  // prescaler: clockI/O / 1
  // outputs enabled
  // phase-correct PWM
  // top of 400
  //
  // PWM frequency calculation
  // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz
  TCCR1A = 0b10100000;
  TCCR1B = 0b00010001;
  #elif defined(__AVR_ATmega2560__)
  // Timer 2 configuration
  // prescaler: clockI/O / 1
  // outputs enabled
  // phase-correct PWM
  // top of 400
  //
  // PWM frequency calculation
  // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz
  //TCCR2A = 0b10100000;
  //TCCR2B = 0b00010001;
  TCCR2B = TCCR2B & 0b11111000 | 0x01;
  #endif
}
// Set speed for motor 1, speed is a number betwenn -255 and 255
void DualVNH5019MotorShield::setM1Speed(int speed)
{
  unsigned char reverse = 0;

  if (speed < 0)
  {
    speed = -speed;  // Make speed a positive quantity
    reverse = 1;  // Preserve the direction
  }
  if (ICR1 * speed / 255 > ICR1)  // Max PWM dutycycle
    speed = 255;

  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__)
  OCR1A = ICR1 * speed / 255;
  #elif defined(__AVR_ATmega2560__)
  analogWrite(_PWM1,speed); // default to using analogWrite
  #else
  analogWrite(_PWM1,speed); // default to using analogWrite
  #endif
  if (reverse)
  {
    digitalWrite(_INA1,LOW);
    digitalWrite(_INB1,HIGH);
  }
  else
  {
    digitalWrite(_INA1,HIGH);
    digitalWrite(_INB1,LOW);
  }
}

// Set speed for motor 2, speed is a number betwenn -400 and 400
void DualVNH5019MotorShield::setM2Speed(int speed)
{
  unsigned char reverse = 0;

  if (speed < 0)
  {
    speed = -speed;  // Make speed a positive quantity
    reverse = 1;  // Preserve the direction
  }
  if (ICR1 * speed / 255 > ICR1)  // Max PWM dutycycle
    speed = 255;
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__)
  OCR1A = ICR1 * speed / 255;
  #elif defined(__AVR_ATmega2560__)
  analogWrite(_PWM2,speed); // default to using analogWrite
  #else
  analogWrite(_PWM2,speed); // default to using analogWrite
  #endif

  if (reverse)
  {
    digitalWrite(_INA2,LOW);
    digitalWrite(_INB2,HIGH);
  }
  else
  {
    digitalWrite(_INA2,HIGH);
    digitalWrite(_INB2,LOW);
  }
}

// Set speed for motor 1 and 2
void DualVNH5019MotorShield::setSpeeds(int m1Speed, int m2Speed)
{
  setM1Speed(m1Speed);
  setM2Speed(m2Speed);
}

// Brake motor 1, brake is a number between 0 and 400
void DualVNH5019MotorShield::setM1Brake(int brake)
{
  // normalize brake
  if (brake < 0)
  {
    brake = -brake;
  }
  if (brake > ICR1)  // Max brake
    brake = ICR1;
  digitalWrite(_INA1, LOW);
  digitalWrite(_INB1, LOW);
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__)
  OCR1A = brake;
  #elif defined(__AVR_ATmega2560__)
  analogWrite(_PWM2,brake); // default to using analogWrite
  #else
  analogWrite(_PWM2,brake); // default to using analogWrite
  #endif
}

// Brake motor 2, brake is a number between 0 and 400
void DualVNH5019MotorShield::setM2Brake(int brake)
{
  // normalize brake
  if (brake < 0)
  {
    brake = -brake;
  }
  if (brake > ICR1)  // Max brake
    brake = ICR1;
  digitalWrite(_INA2, LOW);
  digitalWrite(_INB2, LOW);
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__)
  OCR1B = brake;
  #elif defined(__AVR_ATmega2560__)
  analogWrite(_PWM2,brake); // default to using analogWrite
  #else
  analogWrite(_PWM2,brake); // default to using analogWrite
  #endif
}

// Brake motor 1 and 2, brake is a number between 0 and 400
void DualVNH5019MotorShield::setBrakes(int m1Brake, int m2Brake)
{
  setM1Brake(m1Brake);
  setM2Brake(m2Brake);
}

// Return motor 1 current value in milliamps.
unsigned int DualVNH5019MotorShield::getM1CurrentMilliamps()
{
  // 5V / 1024 ADC counts / 144 mV per A = 34 mA per count
  return analogRead(_CS1) * 34;
}

// Return motor 2 current value in milliamps.
unsigned int DualVNH5019MotorShield::getM2CurrentMilliamps()
{
  // 5V / 1024 ADC counts / 144 mV per A = 34 mA per count
  return analogRead(_CS2) * 34;
}

// Return error status for motor 1 
unsigned char DualVNH5019MotorShield::getM1Fault()
{
  return !digitalRead(_EN1DIAG1);
}

// Return error status for motor 2 
unsigned char DualVNH5019MotorShield::getM2Fault()
{
  return !digitalRead(_EN2DIAG2);
}
