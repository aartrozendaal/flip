#include <allLibs.h>

extern int state;
extern uint16_t servoStartAngle[NUMBEROFSERVOS];
extern uint16_t servoAngle[NUMBEROFSERVOS];
extern uint16_t servoEndAngle[NUMBEROFSERVOS];
extern unsigned long startTime[NUMBEROFSERVOS];

extern bool enterStateFlag;
extern bool enterStateFlag2;
extern bool winkFlag;
extern bool blinkFlag;
extern unsigned long previousBlinkMillis;  // Stores the last time blink occurred
extern unsigned long previousWiggleMillis;
extern unsigned long previousSteer;
extern unsigned long blinkInterval; 
extern unsigned long eyeFollowTimer;
extern unsigned long eyeFollowTimerThreshold;
extern unsigned long previousShiverMillis;
extern bool steerDirection;

extern unsigned long peopleTimer;
extern unsigned long stateTimer;

extern uint8_t numFaces;
extern uint16_t faceX;
extern uint16_t faceY;
extern uint32_t faceSize;

extern uint8_t numBodies;
extern uint16_t bodyX;
extern uint16_t bodyY;
extern uint32_t bodySize;

extern uint8_t soundDirection;
extern uint8_t emotion;

extern bool person;
extern int deltaBodySize;
extern int deltaFaceSize;

/**
 * @brief Converts the eyelid level to a servo angle
 * @param servo -which servo 
 * @param level -int between 0 and 100 with 0 is closed and 100 is fully open
 * @return the angle corresponding to the given level
 */
uint16_t eyeLidLevelToAngle (int servo, int level){
    float levelFloat = level/100.0;
    levelFloat *= 95; // 95 = difference between max and min
    level = levelFloat;
    if (servo == SERVO7) return level + 60;
    else if (servo == SERVO8) return 125 - level;
    else return SERVOHOMEANGLE[state][servo];
    }

/**
 * @brief sets the openess of the eyelids with the speed and acell in constants
 * @param level -int between 0 and 100 with 0 is closed and 100 is fully open
 * @param speed -speed of the movement
 * @param accel -acceleration of the movement
 * @return true when done
 */
bool eyeLidLevelSlow (int level, int speed, int accel){  
    servoEndAngle[SERVO7] = eyeLidLevelToAngle(SERVO7,level);
    servoEndAngle[SERVO8] = eyeLidLevelToAngle(SERVO8,level);
    if (servoAngle[SERVO7] == servoEndAngle[SERVO7]) // get a new end angle 
    {   
        servoStartAngle [SERVO7] = servoAngle[SERVO7];
        servoStartAngle [SERVO8] = servoAngle[SERVO8];
        return true;
    } else { // go to end angle
        servoGoTo(calculateAngle(float(servoStartAngle[SERVO7]),float(servoEndAngle[SERVO7]),speed,accel,startTime[SERVO7]),SERVO7);
        servoGoTo(calculateAngle(float(servoStartAngle[SERVO8]),float(servoEndAngle[SERVO8]),speed,accel,startTime[SERVO8]),SERVO8);
        return false;
    }   
    
}

/**
 * @brief performs blink and returns true when blink is done
 */
bool blink(int speed, int accel){
    return eyeLidLevelSlow(0,speed,accel);    
}

/**
 * @brief Sets eyelid level and performs blinks
 * @param level -int between 0 and 100 with 0 is closed and 100 is fully open
 * @param speed -max blink speed
 * @param accel -max blink acceleration
 */
void doEyelid(int level, int speed = MAXSPEED[state], int accel = MAXACCEL[state]){
    bool temp;
    if (blinkFlag == 0)
    {
        temp = eyeLidLevelSlow(level, speed, accel);
        if (temp) {
            unsigned long timeToStart = millis();
            blinkInterval = random(MINBLINKINTERVAL, MAXBLINKINTERVAL);  // Random interval
            startTime[SERVO7] = timeToStart + blinkInterval;
            startTime[SERVO8] = timeToStart + blinkInterval;
            blinkFlag = true;
        }
    } else {
        temp = blink(speed,accel);
        if (temp) {
            unsigned long timeToStart = millis();
            startTime[SERVO7] = timeToStart;
            startTime[SERVO8] = timeToStart;
            blinkFlag = false;
        }
    }
}

/**
 * @brief does random Eye movements within the set boundaries
 */
void doRandomEye(){
    for (int i = SERVO5; i <= SERVO6; i++){    
        if (servoAngle[i] == servoEndAngle[i]) // get a new end angle 
        {   
            servoStartAngle [i] = servoAngle[i];
            servoEndAngle[i] = getRandomAngle(servoAngle[i],i);
            startTime[i] = millis() + getRandomDelay(i); // could be + random time
        } else { // go to end angle
            servoGoTo(calculateAngle(float(servoStartAngle[i]),float(servoEndAngle[i]),MAXSPEED[state],MAXACCEL[state],startTime[i]),i);
        }   
    }
}

/**
 * @brief does random antennae movements within the set boundaries
 */
void doRandomAntennae(){
    for (int i = SERVO1; i <= SERVO4; i++){    
        if (servoAngle[i] == servoEndAngle[i]) // get a new end angle 
        {   
            servoStartAngle [i] = servoAngle[i];
            servoEndAngle[i] = getRandomAngle(servoAngle[i],i);
            startTime[i] = millis() + getRandomDelay(i); // could be + random time
        } else { // go to end angle
            servoGoTo(calculateAngle(float(servoStartAngle[i]),float(servoEndAngle[i]),MAXSPEED[state],MAXACCEL[state],startTime[i]),i);
        }   
    }
}

/**
 * @brief Uses the face coordinates or else the body coordinates to look at a person. 
 *          if none than it moves randomly within the set boundaries
 */
void eyeFollow(){
    
    if (numFaces>0) {
        eyeFollowTimer = millis();
        // Map coordinates to servo angles (0-180)
        int angleX = map(faceX, SCREENX, 0, SERVOMINANGLE[0][SERVO5], SERVOMAXANGLE[0][SERVO5]);  // Adjust screen width if needed
        int angleY = map(faceY, 0, SCREENY, SERVOMINANGLE[0][SERVO6], SERVOMAXANGLE[0][SERVO6]);  // Adjust screen height if needed

        if(angleX > SERVOMAXANGLE[state][SERVO5]) angleX = SERVOMAXANGLE[state][SERVO5];
        if(angleX < SERVOMINANGLE[state][SERVO5]) angleX = SERVOMINANGLE[state][SERVO5];
        if(angleY > SERVOMAXANGLE[state][SERVO6]) angleY = SERVOMAXANGLE[state][SERVO6];
        if(angleY < SERVOMINANGLE[state][SERVO6]) angleY = SERVOMINANGLE[state][SERVO6];
    
        // Move the servos
        servoGoTo(angleX,SERVO5); servoEndAngle[SERVO5] = servoAngle[SERVO5];
        servoGoTo(angleY,SERVO6); servoEndAngle[SERVO6] = servoAngle[SERVO6];

    } else if (numBodies>0)
    {
        eyeFollowTimer = millis();
        int angleX = map(bodyX, SCREENX, 0, SERVOMINANGLE[0][SERVO5], SERVOMAXANGLE[0][SERVO5]);  // Adjust screen width if needed
        int angleY = map(bodyY, 0, SCREENY, SERVOMINANGLE[0][SERVO6], SERVOMAXANGLE[0][SERVO6]);  // Adjust screen height if needed

        if(angleX > SERVOMAXANGLE[state][SERVO5]) angleX = SERVOMAXANGLE[state][SERVO5];
        if(angleX < SERVOMINANGLE[state][SERVO5]) angleX = SERVOMINANGLE[state][SERVO5];
        if(angleY > SERVOMAXANGLE[state][SERVO6]) angleY = SERVOMAXANGLE[state][SERVO6];
        if(angleY < SERVOMINANGLE[state][SERVO6]) angleY = SERVOMINANGLE[state][SERVO6];
    
        // Move the servos
        servoGoTo(angleX,SERVO5); servoEndAngle[SERVO5] = servoAngle[SERVO5];
        servoGoTo(angleY,SERVO6); servoEndAngle[SERVO6] = servoAngle[SERVO6];

    }
    

    if(millis() - eyeFollowTimer > eyeFollowTimerThreshold){
        doRandomEye();
    }  

}

/**
 * @brief sets the openess of the eyelids
 * @param level -int between 0 and 100 with 0 is closed and 100 is fully open
 */
void eyeLidLevel(int level){
    servoGoTo(eyeLidLevelToAngle(SERVO7,level), SERVO7);
    servoGoTo(eyeLidLevelToAngle(SERVO8,level), SERVO8);
}

/**
 * @brief Resets the blink timer and determines next blink
 */
void resetBlinkTimer(){
    blinkInterval = random(MINBLINKINTERVAL, MAXBLINKINTERVAL);  // Random interval
    previousBlinkMillis = millis();  // Reset the timer
    startTime[SERVO7], startTime[SERVO8]= blinkInterval + previousBlinkMillis;
}

/**
 * @brief sets the openess of the eyelids
 * @param startTime - unsigned long The time the movement is started
 * @param delayBetweenMovenent -int the delay in ms between the movements
 * @return true when done
 */
bool eyeScanRoom(unsigned long startTime,int delayBetweenMovenent) {
    unsigned long tempTime = millis();
    int standardDelay = 500;
    if (tempTime - startTime < standardDelay )
    {
        servoGoTo(90,SERVO5);
        servoGoTo(90,SERVO6);
        return false;
    } else if (tempTime - startTime < standardDelay+1*delayBetweenMovenent){
        servoGoTo(SERVOMAXANGLE[state][SERVO5],SERVO5);
        servoGoTo(SERVOMAXANGLE[state][SERVO6],SERVO6);
        return false;
    } else if (tempTime - startTime < standardDelay+2*delayBetweenMovenent){
        servoGoTo(SERVOMINANGLE[state][SERVO5],SERVO5);
        servoGoTo(SERVOMINANGLE[state][SERVO6],SERVO6);
        return false;
    } else if (tempTime - startTime < standardDelay+3*delayBetweenMovenent){
        servoGoTo(SERVOMINANGLE[state][SERVO5],SERVO5);
        servoGoTo(SERVOMAXANGLE[state][SERVO6],SERVO6);
        return false;
    } else if (tempTime - startTime < standardDelay+4*delayBetweenMovenent){
        servoGoTo(SERVOMAXANGLE[state][SERVO5],SERVO5);
        servoGoTo(SERVOMINANGLE[state][SERVO6],SERVO6);
        return false;
    } else {
        servoGoTo(90,SERVO5);
        servoGoTo(90,SERVO6);
        return true;
    }
}

/**
 * @brief Perform shiver with a certain frequency and amount of degrees
 * @param degrees - int how many degrees deviation from the homing angle
 */
void shiver(int degrees){

    for (int i = SERVO1; i <= SERVO4; i++){     
        servoGoTo(random(SERVOHOMEANGLE[state][i]-degrees, SERVOHOMEANGLE[state][i]+degrees+1 ),i);
      }

}

/**
 * @brief Checks Direction in which to move depending on where the closest person is on the screen if no one than random
 */
bool checkDirection(){
    if (numFaces>0){
        if(faceX > SCREENX/2) return true;
        else return false;
    } else if (numBodies>0) {
        if(bodyX > SCREENX/2) return true;
        else return false;
    } else {
        return random(2);
    } 
}

/**
 * @brief Performs all actions in the current state, for states see the constants
 */
void doState (){
    unsigned long tempStateTime = millis();
    switch (state)
    {
    case STATERELAXED:
        if (enterStateFlag) {
            steerDirection = checkDirection();
            enterStateFlag = false;
        }
        doEyelid(60,400,400);
        doRandomAntennae();
        eyeFollow();
        dcMotorStop();
        //dcMotorSteer(steerTo(1000,previousSteer,400));
        if (!CONTROLLERUSEFLAG){
            if(tempStateTime - stateTimer > STATETIMERTHRESHOLD){
                if(tempStateTime - peopleTimer > PEOPLETIMERTHRESHOLD) goToState(STATEGOTOSLEEP);
                else if (numFaces > 0) goToState(STATEALERT);
                else if (numBodies >= 2 || soundDirection == 1) goToState(STATESTRESSED);
            }   
        }
        break;
    case STATEPLAYFULL:
        doEyelid(75,400,400);        
        doRandomAntennae();
        eyeFollow();
        bounce(2000,previousWiggleMillis,320);
        if (!CONTROLLERUSEFLAG){
            if(tempStateTime - stateTimer> STATETIMERTHRESHOLD){
                if (numFaces == 0) goToState(STATEALERT);
                else if (numBodies >= 2 || (numFaces > 0 && faceY < SCREENH1)) goToState(STATESTRESSED);
            }   
        }
        break;        
    case STATEALERT:    
        if (enterStateFlag) enterStateFlag = moveForward(1500,previousWiggleMillis,320);
        else dcMotorStop();       
        doEyelid(85,600,600);        
        doRandomAntennae();
        eyeFollow();
        if (!CONTROLLERUSEFLAG){
            if(tempStateTime - stateTimer> STATETIMERTHRESHOLD){
                if (numBodies >= 2) goToState(STATESTRESSED);
                else if (numFaces == 0) goToState(STATERELAXED);
                else if (numFaces > 0 && faceY > SCREENH1) goToState(STATEPLAYFULL);
            }   
        }
        break;
    case STATESTRESSED:
        if (enterStateFlag) enterStateFlag = moveBackward(1500,previousWiggleMillis,350);
        else dcMotorStop();    
        doEyelid(100,600,600);        
        doRandomAntennae();
        eyeFollow();  
        if (!CONTROLLERUSEFLAG){
            if(tempStateTime - stateTimer> STATETIMERTHRESHOLD){
                if (numFaces > 0 && faceY > SCREENH2) goToState(STATEDEFENSIVE);
                else if (numBodies <= 1) goToState(STATERELAXED);
                else if (numBodies >= 3 || (numFaces > 0 && faceY < SCREENH2) || soundDirection == 1) goToState(STATEFEARFUL);
            }   
        } 
        break;
    case STATEFEARFUL:
        if (enterStateFlag) enterStateFlag = moveBackward(2000,previousWiggleMillis,350); 
        doEyelid(100,600,600);        
        shiver(4);
        eyeFollow(); 
        if (!CONTROLLERUSEFLAG){
            if(tempStateTime - stateTimer> STATETIMERTHRESHOLD){
                if (numBodies <= 2 && numFaces == 0) goToState(STATESTRESSED);
            }   
        }  
        break; 
    case STATEDEFENSIVE:
        if (enterStateFlag) {
            enterStateFlag = moveForward(2000,previousWiggleMillis,350);
            if (!enterStateFlag) previousWiggleMillis = millis();  
        }
        //else {
            //enterStateFlag = !moveBackward(2000,previousWiggleMillis,320);
            //if (enterStateFlag) previousWiggleMillis = millis();  
        //}
        doEyelid(40,600,600);        
        doRandomAntennae();
        eyeFollow();  
        if (!CONTROLLERUSEFLAG){
            if(tempStateTime - stateTimer> STATETIMERTHRESHOLD){
                if (numBodies >= 3 || (numFaces > 0 && faceY < SCREENH2)) goToState(STATEFEARFUL);
                else if (numFaces == 0) goToState(STATESTRESSED);
            }   
        }
        break;
    case STATESLEEP:
        //doEyelid(0,MAXSPEED[state],MAXACCEL[state]);
        if (enterStateFlag) {
            eyeLidLevel(0);
            enterStateFlag = false;
        } 
        doRandomAntennae();        
        eyeFollow(); 
        dcMotorStop();
        if (!CONTROLLERUSEFLAG){
            if(tempStateTime - stateTimer> STATETIMERTHRESHOLD){
                if(soundDirection == 1) goToState(STATEWAKEUPFAST);
                else if(person == 1) goToState(STATEWAKEUP);
            }   
        }  
        break;
    case STATEGOTOSLEEP:
        if (enterStateFlag) enterStateFlag = !eyeLidLevelSlow(0,MAXSPEED[state],MAXACCEL[state]); 
        else goToState(STATESLEEP);
        doRandomAntennae();
        doRandomEye();   
        dcMotorStop();
        break;
    case STATEWAKEUP:
        if (enterStateFlag) enterStateFlag = !eyeLidLevelSlow(60,MAXSPEED[state],MAXACCEL[state]); 
        else goToState(STATERELAXED);
        doRandomAntennae();
        doRandomEye();  
        dcMotorStop(); 
        break;
    case STATEWAKEUPFAST:
        if (enterStateFlag){
            //servoAngle[SERVO7] = eyeLidLevelToAngle(SERVO7,100);
            //servoAngle[SERVO8] = eyeLidLevelToAngle(SERVO8,100);
            if (enterStateFlag2) enterStateFlag2 = !eyeLidLevelSlow(100,MAXSPEED[state],MAXACCEL[state]);
            else enterStateFlag = !eyeScanRoom(stateTimer,1000);
        } else {
            if (numFaces > 0 || numBodies >= 3) goToState(STATEFEARFUL);
            else if (numBodies > 0) goToState(STATESTRESSED);
            else goToState(STATERELAXED); 
        } 
        doRandomAntennae();  
        dcMotorStop(); 
        break;
    default:
        break;
    }
    return;
}