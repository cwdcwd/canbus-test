#include <Arduino.h>
#include "StepperMotor.h"

StepperMotor::StepperMotor() {
  // constructor
}

void StepperMotor::init(int _pulsePin, int _dirPin, unsigned long _delayTime, unsigned int _direction)
{
  pulsePin = _pulsePin;
  dirPin = _dirPin;
  delayTime = _delayTime;
  direction = _direction;

  togglePulse = LOW;
  enable = 0;

  pinMode(pulsePin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void StepperMotor::stop(void)
{
  enable = 0;
}

void StepperMotor::start(void)
{
  enable = 1;
}

void StepperMotor::control(void)
{
  currentTime = micros();
  digitalWrite(dirPin, direction);
  if (enable == 1)
  {
    if ((currentTime - deltaTime) > delayTime)
    {
      pulseCount++;

      // Each HIGH or LOW is a "pulse"
      // But each step of the motor requires two "pulses"
      if (pulseCount % 2 == 0)
      {
        stepCount++;
      }

      togglePulse = togglePulse == LOW ? HIGH : LOW;
      digitalWrite(pulsePin, togglePulse);
      deltaTime = currentTime;
    }
  }
  }

unsigned int StepperMotor::getDirection(void)
{
  return direction;
}

void StepperMotor::changeDirection(unsigned int _direction)
{
  direction = _direction;
}

unsigned long StepperMotor::steps(void)
{
  return stepCount;
}

unsigned long StepperMotor::pulses(void)
{
  return pulseCount;
}

unsigned long StepperMotor::getSpeed(void)
{
  return delayTime;
}

void StepperMotor::changeSpeed(unsigned long _speed)
{
  delayTime = _speed;
}

