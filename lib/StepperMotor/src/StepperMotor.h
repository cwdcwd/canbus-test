#ifndef StepperMotor_h
#define StepperMotor_h

class StepperMotor
{
  public:
    StepperMotor();
   
    void init(int _pulsePin, int _dirPin, unsigned long _delayTime, bool _direction);
    void stop(void);
    void start(void);
    void control(void);
    bool getDirection(void);
    void changeDirection(bool _direction);
    unsigned long steps(void);
    unsigned long pulses(void);
    unsigned long getSpeed(void);
    void changeSpeed(unsigned long _speed);

  private:
    unsigned long delayTime, deltaTime, currentTime;
    unsigned long pulseCount = 0;
    unsigned long stepCount = 0;
    int pulsePin, dirPin;
    bool direction, togglePulse, enable;
};

#endif