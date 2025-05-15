
#include <allLibs.h>
// ===============================================❀ Global variables ❀================================================

int state = STATESLEEP;
bool motionComplete[NUMBEROFSERVOS];
unsigned long startTime[NUMBEROFSERVOS];


// ===============================================❀ Controller ❀================================================
ControllerPtr myControllers[BP32_MAX_CONTROLLERS];
bool buttonA = false;
bool buttonB = false;
bool buttonX = false;
bool buttonY = false;
bool joystickLUp = false;
bool joystickLDown = false;
bool joystickLLeft = false;
bool joystickLRight = false;
bool joystickRUp = false;
bool joystickRDown = false;
bool joystickRLeft = false;
bool joystickRRight = false;
bool DpadUp = false;
bool DpadDown = false;
bool DpadLeft = false;
bool DpadRight = false;
int joystickLY = 0;
int joystickLX = 0;
int joystickRY = 0;
int joystickRX = 0;
bool updateFlag = false;
bool manualMode = false;
bool enterStateFlag = false;
bool enterStateFlag2 = false;

int throttleLeft = 0;
int throttleRight = 0;
bool directionLeft = false;
bool directionRight = false;

// ===============================================❀ Servo ❀================================================

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x60);
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);
uint16_t servoStartAngle[NUMBEROFSERVOS];
uint16_t servoAngle[NUMBEROFSERVOS];
uint16_t servoEndAngle[NUMBEROFSERVOS];
int servoStepsize = SERVOMAXSTEPSIZE/2;   //stepsize for the servos in degrees


bool winkFlag = false;
bool blinkFlag = false;
bool wiggleFlag = false;

unsigned long stateTimer;
unsigned long peopleTimer;
unsigned long highestBodyTimer;

unsigned long previousBlinkMillis = 0;  // Stores the last time blink occurred
unsigned long previousWiggleMillis = 0;
unsigned long previousShiverMillis = 0;
unsigned long previousSteer = 0;
bool steerDirection;
unsigned long blinkInterval;       // Random interval between blinks
unsigned long eyeFollowTimer = 0;
unsigned long eyeFollowTimerThreshold = 2000;

unsigned long measureTime = 0;
unsigned long measureTimeMax = 0;

// ===============================================❀ Serial comms ❀================================================
uint8_t numFaces;
uint16_t faceX;
uint16_t faceY;
uint32_t faceSize;
uint8_t numBodies;
uint16_t bodyX;
uint16_t bodyY;
uint32_t bodySize;
uint8_t soundDirection;
uint8_t emotion;
bool person;
int deltaBodySize;
int deltaFaceSize;

// ===============================================❀ Setup ❀================================================
void setup() {
  

  if(DEBUGFLAG){
    Serial.begin(9600); // init serial communication if in debug mode
    while (!Serial) {
      ;
    }
    Serial.println("Serial open");
    Serial.println("Setup begin");
  }else{
    Serial.begin(115200);
  }
  
  //Controller
  if(CONTROLLERUSEFLAG){
    BP32.setup(&onConnectedController, &onDisconnectedController); // Creates controller call backs.
    if(DEBUGFLAG) Serial.println("Controller setup done");
  } else {
    state = STATESLEEP;
  }
  
  //Motors
  pwm.begin();
  pwm.setOscillatorFrequency(25000000); // calculated and checked with an oscilloscope 
  pwm.setPWMFreq(SERVOFREQ);

  pinMode(LEFTPWM,OUTPUT);
  pinMode(LEFTFORWARD,OUTPUT);
  pinMode(RIGHTPWM,OUTPUT);
  pinMode(RIGHTFORWARD,OUTPUT);

  servoHome();
  dcMotorStop();
  goToState(STATESLEEP);  // NEW

  if(DEBUGFLAG) Serial.println("Servo setup done");

  delay(SETUPDELAY);  

  if(DEBUGFLAG) Serial.println("Setup Done");
}


// ===============================================❀ Loop ❀================================================
void loop() {

  if(CONTROLLERUSEFLAG){
    controllerUpdate(); //update controller values
    processButtons(); //processes the buttons and performs actions
    if(state == STATEANTENNA) {
      processJoysticks(SERVO3,SERVO4,SERVO1,SERVO2); 
    }
    if(state == STATEWHEELS) {
      processJoysticksDrive();
    }
    if(state == STATEEYES) {
      processJoysticks(SERVO6,SERVO5,SERVO8,SERVO7);
    }    
    processDpad();  //processes the Dpad and performs actions
  }
  
  if(DEBUGFLAG) measureTime = micros();
  receiveData();
  readMicrophone();
  doState();
  if(DEBUGFLAG) measureTime = micros() - measureTime;
  if(DEBUGFLAG && measureTime > measureTimeMax) measureTimeMax = measureTime;
  // print 
  if(DEBUGFLAG) {
    Serial.print("state: ");
    if(state == STATEANTENNA) Serial.print("Antenna");
    if(state == STATEWHEELS) Serial.print("Wheels");
    if(state == STATEEYES) Serial.print("Eyes");
    if(state == STATEALERT) Serial.print("Alert");
    if(state == STATEPLAYFULL) Serial.print("Playfull");
    if(state == STATESTRESSED) Serial.print("Stressed");
    if(state == STATERELAXED) Serial.print("Relaxed");
    if(state == STATEFEARFUL) Serial.print("Fearful");
    if(state == STATEDEFENSIVE) Serial.print("defensive");
    if(state == STATESLEEP) Serial.print("sleep");
    if(state == STATEGOTOSLEEP) Serial.print("Go to sleep");
    if(state == STATEWAKEUP) Serial.print("wake up");
    if(state == STATEWAKEUPFAST) Serial.print("wake fast");
    Serial.print("\t Stepsize:\t");
    Serial.print(servoStepsize);
    Serial.print("\t Angles:");
    for (int i = 0; i < NUMBEROFSERVOS; i++)
    {
      Serial.print("\t");
      Serial.print(servoAngle[i]);
    }
    Serial.print("\t Time: ");
    Serial.print(measureTime);
    Serial.print("\t");
    Serial.print(measureTimeMax);
    Serial.print("\t Sound: ");
    Serial.print(soundDirection);
    //Serial.print("\t Manual:");
    //if(manualMode) Serial.print("Yes");
    //if(!manualMode) Serial.print("No");
    Serial.println("");
  }
  if(CONTROLLERUSEFLAG) delay(LOOPDELAY);
  //delay(LOOPDELAY); // if required adds a delay to the loop (find in constants)
}