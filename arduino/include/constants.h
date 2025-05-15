#pragma once


// ===============================================❀ Configurations ❀================================================

// CHECK BEFORE UPLOADING #TODO check before uploading
#define DEBUGFLAG false
#define DCMOTORUSEFLAG true
#define CONTROLLERUSEFLAG false

// ===============================================❀ Servos ❀================================================
const int NUMBEROFSERVOS = 8;     // the number of servos used
const int NUMBEROFSTATES = 13;
//{yaxisleftantenna,xaxisleftantenna,yaxisrightantenna,xaxisrightantenna, yaxislefteye, xaxislefteye, yaxisrighteye, xaxisrighteye}
const bool SERVOSMALL[NUMBEROFSERVOS] = {false,true,false,true,true,true,false,false}; // True if it is a small servo
const bool SERVODIR[NUMBEROFSERVOS] = {true,true,false,true,true,false,false,true}; // only for controller
const int SERVOMINANGLE[NUMBEROFSTATES][NUMBEROFSERVOS] =   // The smalles angle allowed in the config (put 0 if NA)
{{ 10,  0, 10,  0, 50, 50, 60, 20}, //antenna
 { 10,  0, 10,  0, 50, 50, 60, 20}, //eye
 { 10,  0, 10,  0, 50, 50, 60, 20}, //wheels
 { 30,  0, 30,110, 50, 80, 60, 20}, //relaxed
 { 40, 70, 20, 70, 50, 70, 60, 20}, //playful
 { 70,  0, 10,160, 50, 60, 60, 20}, //alert/interested
 { 40,135, 30,  0, 50, 60, 60, 20}, //stressed 
 { 10,175, 80,  5, 50, 60, 60, 20}, //fearful
 { 10,  0, 80,160, 50, 85, 60, 20}, //defensive
 { 10, 45, 70,135, 90, 90, 60,125}, //Sleep
 { 10, 45, 80,135, 90, 90, 60,125}, //go to sleep
 { 30, 45, 50,135, 90, 90, 60, 20},  // wake up
 { 80, 45,  0,135, 50, 80, 60, 20}  // wake up fast
}; 

const int SERVOHOMEANGLE[NUMBEROFSTATES][NUMBEROFSERVOS] =  // The Homing angle
{{ 10, 45, 80,135, 90, 90, 60,125},//antenna
 { 10, 45, 80,135, 90, 90, 60,125},//eye
 { 10, 45, 80,135, 90, 90, 60,125},//wheels
 { 10, 45, 80,135, 90, 90, 60,125},//relaxed
 { 10, 45, 80,135, 90, 90, 60,125},//playful
 { 10, 45, 80,135, 90, 90, 60,125},//alert/interested
 { 10, 45, 80,135, 90, 90, 60,125},//stressed 
 { 10,175, 80,  5, 90, 90, 60,125},//fearful
 { 10, 45, 80,135, 90, 90, 60,125},//defensive
 { 10, 45, 80,135, 90, 90, 60,125},//Sleep
 { 10, 45, 80,135, 90, 90, 60,125},//go to sleep
 { 30, 45, 50,135, 90, 90,120, 65}, // wake up
 { 30, 45, 50,135, 90, 90,120, 65} // wake up fast
}; 

const int SERVOMAXANGLE[NUMBEROFSTATES][NUMBEROFSERVOS] =   // The largest angle allowed in the config (put 180 or 270 if NA)
{{ 80,180, 80,180,130,130,155,125}, //antenna
 { 80,180, 80,180,130,130,155,125},//eye
 { 80,180, 80,180,130,130,155,125},//wheels
 { 50, 70, 60,180,130,100,155,125},//relaxed
 { 70,110, 40,110,130,110,155,125},//playful
 { 80, 20, 20,180,130,120,155,125},//alert/interested
 { 60,180, 50, 45,130,120,155,125},//stressed 
 { 10,175, 80,  5,130,120,155,125},//fearful
 { 20, 20, 80,180,130, 95,155,125},//defensive
 { 20, 45, 80,135, 90, 90, 60,125},//Sleep
 { 10, 45, 80,135, 90, 90, 60,125},//go to sleep
 { 30, 45, 50,135, 90, 90, 60, 20},  // wake up
 { 80, 45,  0,135,130,110, 60, 20}  // wake up fast
}; 


const unsigned long MAXDELAY[NUMBEROFSTATES][NUMBEROFSERVOS] =
{{  0,  0,  0,  0,  0,  0,  0,  0}, //antenna
 {  0,  0,  0,  0,  0,  0,  0,  0},//eye
 {  0,  0,  0,  0,  0,  0,  0,  0},//wheels
 {3UL,3UL,3UL,3UL,3UL,3UL,3UL,3UL},//relaxed
 {0.5,0.5,0.5,0.5,  0,  0,  0,  0},//playful
 {  0,  0,  0,  0,  0,  0,  0,  0},//alert/interested
 {  0,  0,  0,  0,9UL,9UL,  0,  0},//stressed 
 {  0,  0,  0,  0,  0,  0,  0,  0},//fearful
 {  0,  0,  0,  0,  0,  0,  0,  0},//defensive
 {9UL,  0,9UL,  0,  0,  0,  0,  0},//Sleep
 {  0,  0,  0,  0,  0,  0,  0,  0},//go to sleep
 {  0,  0,  0,  0,  0,  0,  0,  0}, // wake up
 {  0,  0,  0,  0,  0,  0,  0,  0} // wake up
};

const int RANDOMANGLE[NUMBEROFSTATES][NUMBEROFSERVOS] = // threshold for the minimum movement in automatic states
{{  0,  0,  0,  0,  0,  0,  0,  0}, //antenna
 {  0,  0,  0,  0,  0,  0,  0,  0},//eye
 {  0,  0,  0,  0,  0,  0,  0,  0},//wheels
 {  0,  0,  0,  0,  3,  3,  0,  0},//relaxed
 {  0,  0,  0,  0,  4,  4,  0,  0},//playful
 {  0,  0,  0,  0,  0,  0,  0,  0},//alert/interested
 {  0,  0,  0,  0,  0,  0,  0,  0},//stressed 
 {  0,  0,  0,  0,  0,  0,  0,  0},//fearful
 {  0,  0,  0,  0,  0,  0,  0,  0},//defensive
 {  0,  0,  0,  0,  0,  0,  0,  0},//Sleep
 {  0,  0,  0,  0,  0,  0,  0,  0},//go to sleep
 {  0,  0,  0,  0,  0,  0,  0,  0},// wake up
 {  0,  0,  0,  0,  0,  0,  0,  0} // wake up
};

// degrees per second
const float MAXSPEED[NUMBEROFSTATES] = {0 , //antenna
                                        0 ,//eye
                                        0 ,//wheels
                                        30,//relaxed 
                                        50,//playful 
                                        50,//alert/interested
                                        120,//stressed  
                                        200 ,//fearful
                                        200 ,//defensive
                                        10 ,//Sleep
                                        20 ,//go to sleep
                                        20 ,  // wake up
                                        400
                                    };

const float MAXACCEL[NUMBEROFSTATES] = {0 , //antenna
                                        0 ,//eye
                                        0 ,//wheels
                                        40,//relaxed 
                                        60,//playful 
                                        70,//alert/interested
                                        120,//stressed  
                                        200 ,//fearful
                                        200,//defensive
                                        10,//Sleep
                                        20,//go to sleep
                                        20,   // wake up
                                        400
                                    };


// ===============================================❀ States ❀================================================

enum BehaviourStates {
    STATEANTENNA,
    STATEEYES,
    STATEWHEELS,

    STATERELAXED,
    STATEPLAYFULL,
    STATEALERT,
    STATESTRESSED,
    STATEFEARFUL,
    STATEDEFENSIVE,
    STATESLEEP,

    STATEGOTOSLEEP, 
    STATEWAKEUP,
    STATEWAKEUPFAST 
};

// ===============================================❀ Time constants ❀================================================

const int SETUPDELAY = 500;
const int LOOPDELAY = 3;

// ===============================================❀ Pins ❀================================================

//#define PIN 0
//#define PIN 1
//#define PIN 2 //~
#define LEFTFORWARD 3
#define RIGHTFORWARD 4
#define LEFTPWM 5 //~
#define RIGHTPWM 6 //~
//#define PIN 7
//#define PIN 8
//#define PIN 9 //~
//#define PIN 10 //~
//#define PIN 11
//#define PIN 12
//#define PIN 13 
#define MICROPHONE A0
//#define PIN A1
//#define PIN A2
//#define PIN A3
//#define PIN A4
//#define PIN A5

// ===============================================❀ Controller ❀================================================

const int JOYSTICKTHRESHOLD = 350; //threshold number for up or down detection

// ===============================================❀ Servo ❀================================================

const float MINPULSEWIDTH = 0.5;  // Minimum pulse width in us
const float MAXPULSEWIDTH = 2.5;  // Maximum pulse width in us
const int SERVOMINDEGREE = 0;          // the lowest angle in degrees
const int SERVOHOMEDEGREE = 135;       // the homing angle in degrees
const int SERVOMAXDEGREE = 270;        // the highest angle in degrees
const int SERVOSMALLMINDEGREE = 0;          // the lowest angle in degrees
const int SERVOSMALLHOMEDEGREE = 90;       // the homing angle in degrees
const int SERVOSMALLMAXDEGREE = 180;        // the highest angle in degrees
const int SERVOMINSTEPSIZE = 1;        // stepsize min in degrees
const int SERVOMAXSTEPSIZE = 8;        // stepsize max in degrees
enum ServoCount {
    SERVO1,
    SERVO2,
    SERVO3,
    SERVO4,
    SERVO5,
    SERVO6,
    SERVO7,
    SERVO8
};
const int PWMMAX = 255;
const int SERVOFREQ = 50; // Analog servos run at ~50 Hz updates
const double MICROSPERBIT = 4.8828125;

// ===============================================❀ Serial comms ❀================================================

const unsigned long BLINKDONETIMER = 400;
const unsigned long MINBLINKINTERVAL = 5000 + BLINKDONETIMER;   // Minimum interval (0.5 seconds)
const unsigned long MAXBLINKINTERVAL = 12000 + BLINKDONETIMER;  // Maximum interval (2 seconds)

const int SCREENX = 1280;
const int SCREENY = 720;

const int DEADBAND = 300;

// ===============================================❀ Behaviour ❀================================================

const int SCREENH1 = 200;
const int SCREENH2 = 250;

const unsigned long STATETIMERTHRESHOLD = 3000;
const unsigned long PEOPLETIMERTHRESHOLD = 30000;
const unsigned long HIGHESTBODYTIMERTHRESHOLD = 150; 

const int SOUNDTHRESHOLD = 250;
const int SOUNDMEAN = 330;
