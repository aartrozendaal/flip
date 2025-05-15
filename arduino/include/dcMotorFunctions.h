#pragma once 
#include <allLibs.h>

void dcMotorStop();
void dcMotorDrive(bool direction, int throttle, int steering);
void dcMotorSteer(int steeringInPlace);
float wiggle(unsigned long periodMs, unsigned long startTime, float amplitude);
float wiggleOnce(unsigned long periodMs, unsigned long startTime, float amplitude);
void bounce(unsigned long periodMs, unsigned long startTime, float amplitude);
bool moveForward(unsigned long periodMs, unsigned long startTime, float amplitude);
bool moveBackward(unsigned long periodMs, unsigned long startTime, float amplitude);
float steerTo(unsigned long periodMs, unsigned long startTime, float amplitude);