#pragma once 
#include <allLibs.h>

void servoHome();
float servoDegree(uint16_t angle, uint16_t servoNumber);
void servoMax();
void servoMin();
void servoGoTo(uint16_t angle, uint16_t servoNumber);
void servoAngleSet(uint16_t angle ,uint16_t servoNumber);
void servoAngleUpdate();
float getRandomAngle(float currentAngle, int servoNumber);
float calculateAngle(float startAngle, float endAngle, float maxSpeed, float maxAccel, unsigned long servoStartTime);
unsigned long getRandomDelay(int servoNumber);