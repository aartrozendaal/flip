#include <allLibs.h>

extern Adafruit_PWMServoDriver pwm;
extern bool wiggleFlag;
extern unsigned long previousWiggleMillis;
extern unsigned long previousSteer;
extern int throttleLeft;
extern int throttleRight;
extern bool directionLeft;
extern bool directionRight;
extern bool steerDirection;

/**
 * @brief sets all motors according to:  directionLeft, directionRight, throttleLeft, throttleRight
 */
void setMotor(){
    if(DCMOTORUSEFLAG){
        digitalWrite(LEFTFORWARD,directionLeft);
        digitalWrite(RIGHTFORWARD,directionRight);
        analogWrite(LEFTPWM,throttleLeft);
        analogWrite(RIGHTPWM,throttleRight); 
    }
}


/**
 * @brief stops al dc motors
 */
void dcMotorStop(){
    directionLeft = false;
    directionRight = false;
    throttleLeft = 0;
    throttleRight =  0; 
    setMotor();
}

/**
 * @brief lets the robot bounce forward and backwards
 * @param periodMs - unsigned long the length in ms that the full movement takes
 * @param startTime - the startime of the movement (millis)
 * @param amplitude - the topspeed of the movement
 */
void bounce(unsigned long periodMs, unsigned long startTime, float amplitude){
    int tempWiggle = wiggle(periodMs,startTime,amplitude);
    if(tempWiggle>0) dcMotorDrive(0,tempWiggle,0);
    if(tempWiggle<0) dcMotorDrive(1,-tempWiggle,0);
}

/**
 * @brief Lets the robot move forward and return when done
 * @param periodMs - unsigned long the length in ms that the full movement takes
 * @param startTime - the startime of the movement (millis)
 * @param amplitude - the topspeed of the movement
 * @return bool false when done
 */
bool moveForward(unsigned long periodMs, unsigned long startTime, float amplitude){
    int tempWiggle = wiggle(periodMs,startTime,periodMs);
    if(tempWiggle>0){
        dcMotorDrive(1,tempWiggle,0);
        return true;
    } else {
        dcMotorStop();
        return false;
    }
}

/**
 * @brief Lets the robot move Backwards and return when done
 * @param periodMs - unsigned long the length in ms that the full movement takes
 * @param startTime - the startime of the movement (millis)
 * @param amplitude - the topspeed of the movement
 * @return bool false when done
 */
bool moveBackward(unsigned long periodMs, unsigned long startTime, float amplitude){
    int tempWiggle = wiggle(periodMs,startTime,periodMs);
    if(tempWiggle>0){
        dcMotorDrive(0,tempWiggle,0);
        return true;
    } else {
        dcMotorStop();
        return false;
    }
}

/**
 * @brief lets the robot Wiggle side to side once
 * @param periodMs - unsigned long the length in ms that the full movement takes
 * @param startTime - the startime of the movement (millis)
 * @param amplitude - the topspeed of the movement
 */
float wiggleOnce(unsigned long periodMs, unsigned long startTime, float amplitude) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;
  
    if (elapsedTime >= periodMs) {
      wiggleFlag = true;
      return 0.0;  // Done wiggling, return zero speed (or keep last value if you prefer)
    }
  
    // Map elapsed time to angle in radians
    float angle = 2.0 * PI * ((float)elapsedTime / periodMs);
  
    // Calculate sine wave speed from -500 to 500
    float speed = amplitude * sin(angle);

    if (speed >= 0) {
        speed = map(speed, 0, amplitude, DEADBAND, amplitude);
    } else {
        speed = map(speed, 0, -amplitude, -DEADBAND, -amplitude);
    }
  
    return speed;
  }

/**
 * @brief lets the robot to a direction
 * @param periodMs - unsigned long the length in ms that the full movement takes
 * @param startTime - the startime of the movement (millis)
 * @param amplitude - the topspeed of the movement
 */
float steerTo(unsigned long periodMs, unsigned long startTime, float amplitude) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    if (elapsedTime >= periodMs/2) {
        wiggleFlag = true;
        previousSteer = random(10000,20000)+ millis();
        dcMotorStop();
        return 0.0;  // Done wiggling, return zero speed (or keep last value if you prefer)
    }
    if(startTime > currentTime){
        steerDirection = checkDirection();
        dcMotorStop();
        return 0.0;
    }
  
    // Map elapsed time to angle in radians
    float angle = 2.0 * PI * ((float)elapsedTime / periodMs);
  
    // Calculate sine wave speed from -500 to 500
    float speed;
    if (steerDirection) speed = amplitude * sin(angle);
    else speed = -amplitude * sin(angle);

    if (speed >= 0) {
        speed = map(speed, 0, amplitude, DEADBAND, amplitude);
    } else {
        speed = map(speed, 0, -amplitude, -DEADBAND, -amplitude);
    }
  
    return speed;
  }

/**
 * @brief lets the robot Wiggle side to side
 * @param periodMs - unsigned long the length in ms that the full movement takes
 * @param startTime - the startime of the movement (millis)
 * @param amplitude - the topspeed of the movement
 * @return throttle 
 */
float wiggle(unsigned long periodMs, unsigned long startTime, float amplitude) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;
  
    // Wrap elapsed time within the wave period
    elapsedTime = elapsedTime % periodMs;
  
    // Map elapsed time to angle in radians (0 to 2π)
    float angle = 2.0 * PI * ((float)elapsedTime / periodMs);

    // Calculate sine wave speed from -500 to 500
    float speed = amplitude * sin(angle);

    // Determine direction of sine wave to decide jump
    if (speed >= 0) {
        speed = map(speed, 0, amplitude, DEADBAND, amplitude);
    } else {
        speed = map(speed, 0, -amplitude, -DEADBAND, -amplitude);
    }

    return speed;
  }

/**
 * @brief Steers the robot in place
 * @param steeringInPlace int: between -511 and 512 with minus steering left and plus to the right
 */
void dcMotorSteer(int steeringInPlace){

    if (steeringInPlace > 0) {
        throttleLeft = steeringInPlace;
        throttleRight =  steeringInPlace;
        throttleLeft = map(throttleLeft, 0, 512,0,PWMMAX);
        throttleRight = map(throttleRight, 0, 512,PWMMAX,0); 
        
        directionLeft = false; 
        directionRight = true;

        setMotor();
    } else if(steeringInPlace == 0){
        throttleLeft = 0;
        throttleRight = 0;
    } else {
        throttleLeft = -steeringInPlace;
        throttleRight = -steeringInPlace;
        throttleLeft = map(throttleLeft, 0, 512,PWMMAX,0);
        throttleRight = map(throttleRight, 0, 512,0, PWMMAX); 

        directionLeft = true; 
        directionRight = false;
        
        setMotor();
    }

}


/**
 * @brief Drives the DC Motors
 * @param tempdirection bool: if 1 goes forward and if 0 goes backwards
 * @param throttle int: between 0 and 511 should tell how fast the robot should move
 * @param steering int: between -511 and 512 with minus steering left and plus to the right
 */
void dcMotorDrive(bool tempdirection, int throttle, int steering) {

    if (steering > 0) {
        float mappedSteering = (512 - steering) / 511.0;
        throttleLeft = throttle;
        throttleRight = throttle * mappedSteering;
    } else if(steering == 0){
        throttleLeft = throttle;
        throttleRight = throttle;
    } else {
        float mappedSteering =  (512 + steering) / 511.0;
        throttleLeft = throttle * mappedSteering;
        throttleRight = throttle;
    }

    if(!tempdirection){  
        
        throttleLeft = map(throttleLeft, 0, 512,PWMMAX,0);
        throttleRight = map(throttleRight, 0, 512,PWMMAX,0);   
        directionLeft = true;
        directionRight = true;
        setMotor();
    } else {
     
        throttleLeft = map(throttleLeft, 0, 512, 0, PWMMAX);
        throttleRight = map(throttleRight, 0, 512, 0, PWMMAX); 
        directionLeft = false;
        directionRight = false;
        setMotor();
    }
    if(DEBUGFLAG){
        Serial.print("\t throttleR: \t");Serial.print(throttleLeft);
        Serial.print("\t throttleL: \t");Serial.print(throttleRight);
        Serial.print("\t Direction: \t");Serial.print(directionLeft);Serial.print("\t");Serial.print(directionRight);
    }
        

}

