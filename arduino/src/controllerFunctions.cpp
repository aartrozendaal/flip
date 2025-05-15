#include <allLibs.h>

extern ControllerPtr myControllers[BP32_MAX_CONTROLLERS];
extern bool buttonA;
extern bool buttonB;
extern bool buttonX;
extern bool buttonY;
extern bool joystickLUp;
extern bool joystickLDown;
extern bool joystickLLeft;
extern bool joystickLRight;
extern bool joystickRUp;
extern bool joystickRDown;
extern bool joystickRLeft;
extern bool joystickRRight;
extern bool DpadUp;
extern bool DpadDown;
extern bool DpadLeft;
extern bool DpadRight;
extern int joystickLY;
extern int joystickLX;
extern int joystickRY;
extern int joystickRX;
extern bool updateFlag;
extern bool enterStateFlag;
extern bool enterStateFlag2;
extern int state;

extern uint16_t servoStartAngle[NUMBEROFSERVOS];
extern uint16_t servoAngle[NUMBEROFSERVOS];
extern uint16_t servoEndAngle[NUMBEROFSERVOS];
extern unsigned long startTime[NUMBEROFSERVOS];
extern int servoStepsize;
extern bool manualMode;
extern unsigned long previousWiggleMillis;
extern unsigned long previousShiverMillis;
extern unsigned long stateTimer;
extern unsigned long peopleTimer;
extern unsigned long previousSteer;


/**
 * @brief Takes care of connection of controllers
 * @param gamepad The controller property
 */
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      if(DEBUGFLAG){
        Serial.print("CALLBACK: Controller is connected, index=");
        Serial.println(i);
      }
      myControllers[i] = ctl;
      foundEmptySlot = true;

      // Optional, once the gamepad is connected, request further info about the
      // gamepad.
      ControllerProperties properties = ctl->getProperties();
      if (DEBUGFLAG) {
        char buf[80];
        sprintf(buf,
              "BTAddr: %02x:%02x:%02x:%02x:%02x:%02x, VID/PID: %04x:%04x, "
              "flags: 0x%02x",
              properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
              properties.btaddr[3], properties.btaddr[4], properties.btaddr[5],
              properties.vendor_id, properties.product_id, properties.flags);
        Serial.println(buf);
      }
      break;
    }
  }
  if (!foundEmptySlot) {
    if(DEBUGFLAG){
        Serial.println(
            "CALLBACK: Controller connected, but could not found empty slot");
    }
  }
}

/**
 * @brief Takes care of disconnection of controllers
 * @param gamepad The controller property
 */
void onDisconnectedController(ControllerPtr ctl) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
        if(DEBUGFLAG){
            Serial.print("CALLBACK: Controller is disconnected from index=");
            Serial.println(i);
        }
        myControllers[i] = nullptr;
        foundGamepad = true;
        break;
    }
  }

  if (!foundGamepad) {
    if(DEBUGFLAG){
        Serial.println(
            "CALLBACK: Controller disconnected, but not found in myControllers");
    }
  }
}

/**
 * @brief Processes the buttons and joysticks
 * @param gamepad The controller property
 */
void processGamepad(ControllerPtr gamepad) {

    buttonB = gamepad->a();
    buttonA = gamepad->b();
    buttonX = gamepad->y();
    buttonY = gamepad->x();

    joystickLY = gamepad->axisY();
    joystickLX = gamepad->axisX();
    joystickRY = gamepad->axisRY();
    joystickRX = gamepad->axisRX();
    
    if (joystickLY < -JOYSTICKTHRESHOLD){
        joystickLUp = true;
        joystickLDown = false;
    } else if (joystickLY > JOYSTICKTHRESHOLD){
        joystickLUp = false;
        joystickLDown = true;
    } else {
        joystickLUp = false;
        joystickLDown = false;
    }

    if (joystickRY < -JOYSTICKTHRESHOLD){
        joystickRUp = true;
        joystickRDown = false;
    } else if (joystickRY > JOYSTICKTHRESHOLD){
        joystickRUp = false;
        joystickRDown = true;
    } else {
        joystickRUp = false;
        joystickRDown = false;
    }

    if (joystickLX < -JOYSTICKTHRESHOLD){
        joystickLLeft = true;
        joystickLRight = false;
    } else if (joystickLX > JOYSTICKTHRESHOLD){
        joystickLLeft = false;
        joystickLRight = true;
    } else {
        joystickLLeft = false;
        joystickLRight = false;
    }

    if (joystickRX < -JOYSTICKTHRESHOLD){
        joystickRLeft = true;
        joystickRRight = false;
    } else if (joystickRX > JOYSTICKTHRESHOLD){
        joystickRLeft = false;
        joystickRRight = true;
    } else {
        joystickRLeft = false;
        joystickRRight = false;
    }

    DpadDown = gamepad->dpad() & 2;
    DpadUp = gamepad->dpad() & 1;
    DpadLeft = gamepad->dpad() & 8;
    DpadRight = gamepad->dpad() & 4;

    /*
    if(joystickLUp || joystickLDown || joystickLLeft || joystickLLeft) updateFlag = true;
    if(joystickRUp || joystickRDown || joystickRLeft || joystickRLeft) updateFlag = true;
    if(DpadUp || DpadDown || DpadLeft || DpadLeft) updateFlag = true;
    if(buttonA || buttonB || buttonX || buttonY) updateFlag = true;
    */

    if(DEBUGFLAG){

        if (joystickLUp) Serial.println("JSLeft up");
        if (joystickLDown) Serial.println("JSLeft down");
        if (joystickLLeft) Serial.println("JSLeft left");
        if (joystickLRight) Serial.println("JSLeft right");
        if (joystickRUp) Serial.println("JSRight up");
        if (joystickRDown) Serial.println("JSRight down");
        if (joystickRLeft) Serial.println("JSRight left");
        if (joystickRRight) Serial.println("JSRight right");

        if (DpadDown) Serial.println("Dpad down");
        if (DpadUp) Serial.println("Dpad up");
        if (DpadLeft) Serial.println("Dpad left");
        if (DpadRight) Serial.println("Dpad right");

        if (buttonA) Serial.println("A");
        if (buttonB) Serial.println("B");
        if (buttonX) Serial.println("X");
        if (buttonY) Serial.println("Y");
     
    }

    /*
    if(DEBUGFLAG){
        char buf[256];
        snprintf(buf, sizeof(buf) - 1,
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, "
        "axis L: %4li, %4li, axis R: %4li, %4li, "
        "misc: 0x%02x, "
        ,
        gamepad->index(),        // Gamepad Index
        gamepad->dpad(),         // DPad
        gamepad->buttons(),      // bitmask of pressed buttons
        gamepad->axisX(),        // (-511 - 512) left X Axis
        gamepad->axisY(),        // (-511 - 512) left Y axis
        gamepad->axisRX(),       // (-511 - 512) right X axis
        gamepad->axisRY(),       // (-511 - 512) right Y axis
        gamepad->miscButtons()  // bitmask of pressed "misc" buttons
        );
        Serial.println(buf);
    } */
  
}

/**
 * @brief Updates controller values for callbacks and checks connection
 */
void controllerUpdate(){
    BP32.update();

  for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
    ControllerPtr myController = myControllers[i];

    if (myController && myController->isConnected()) {
      if (myController->isGamepad())
        processGamepad(myController);
    }
  }
  return;
}

/**
 * @brief Couples the angles of the given servos to the four axes of the joysticks
 * @param servo1 connects this servos angle to the y axis of the left joystick  (not used -1)
 * @param servo2 connects this servos angle to the x axis of the left joystick  (not used -1)
 * @param servo3 connects this servos angle to the y axis of the right joystick (not used -1)
 * @param servo4 connects this servos angle to the x axis of the right joystick (not used -1)
 */
void processJoysticks(int servo1 = -1, int servo2 = -1, int servo3 = -1, int servo4 = -1){
  
  // left joystick Y axis
  if (servo1 != -1){
    if(SERVODIR[servo1]){
      if (joystickLUp && servoAngle[servo1] <= (SERVOMAXANGLE[state][servo1]-servoStepsize)) {
        servoGoTo(servoAngle[servo1]+servoStepsize, servo1);
      } else if (joystickLDown && servoAngle[servo1] > SERVOMINANGLE[state][servo1]+servoStepsize) {
        servoGoTo(servoAngle[servo1]-servoStepsize, servo1);
      }
    }else{
      if (joystickLDown && servoAngle[servo1] <= (SERVOMAXANGLE[state][servo1]-servoStepsize)) {
        servoGoTo(servoAngle[servo1]+servoStepsize, servo1);
      } else if (joystickLUp && servoAngle[servo1] > SERVOMINANGLE[state][servo1]+servoStepsize) {
        servoGoTo(servoAngle[servo1]-servoStepsize, servo1);
      }
    }
  }

  // left joystick X axis
  if (servo2 != -1){
    if(SERVODIR[servo2]){
      if (joystickLRight && servoAngle[servo2] <= (SERVOMAXANGLE[state][servo2]-servoStepsize)) {
        servoGoTo(servoAngle[servo2]+servoStepsize, servo2);
      } else if (joystickLLeft && servoAngle[servo2] > SERVOMINANGLE[state][servo2]+servoStepsize) {
        servoGoTo(servoAngle[servo2]-servoStepsize, servo2);
      }
    }else{
      if (joystickLLeft && servoAngle[servo2] <= (SERVOMAXANGLE[state][servo2]-servoStepsize)) {
        servoGoTo(servoAngle[servo2]+servoStepsize, servo2);
      } else if (joystickLRight && servoAngle[servo2] > SERVOMINANGLE[state][servo2]+servoStepsize) {
        servoGoTo(servoAngle[servo2]-servoStepsize, servo2);
      }
    }
  } 

  // Right joystick Y axis
  if (servo3 != -1){
    if(SERVODIR[servo3]){
      if (joystickRUp && servoAngle[servo3] <= (SERVOMAXANGLE[state][servo3]-servoStepsize)) {
        servoGoTo(servoAngle[servo3]+servoStepsize, servo3);
      } else if (joystickRDown && servoAngle[servo3] > SERVOMINANGLE[state][servo3]+servoStepsize) {
        servoGoTo(servoAngle[servo3]-servoStepsize, servo3);
      }
    }else{
      if (joystickRDown && servoAngle[servo3] <= (SERVOMAXANGLE[state][servo3]-servoStepsize)) {
        servoGoTo(servoAngle[servo3]+servoStepsize, servo3);
      } else if (joystickRUp && servoAngle[servo3] > SERVOMINANGLE[state][servo3]+servoStepsize) {
        servoGoTo(servoAngle[servo3]-servoStepsize, servo3);
      }
    }
  } 

  // Right joystick X axis
  if (servo4 != -1){
    if(SERVODIR[servo4]){
      if (joystickRRight && servoAngle[servo4] <= (SERVOMAXANGLE[state][servo4]-servoStepsize)) {
        servoGoTo(servoAngle[servo4]+servoStepsize, servo4);
      } else if (joystickRLeft && servoAngle[servo4] > SERVOMINANGLE[state][servo4]+servoStepsize) {
        servoGoTo(servoAngle[servo4]-servoStepsize, servo4);
      }
    }else{
      if (joystickRLeft && servoAngle[servo4] <= (SERVOMAXANGLE[state][servo4]-servoStepsize)) {
        servoGoTo(servoAngle[servo4]+servoStepsize, servo4);
      } else if (joystickRRight && servoAngle[servo4] > SERVOMINANGLE[state][servo4]+servoStepsize) {
        servoGoTo(servoAngle[servo4]-servoStepsize, servo4);
      }
    }
  } 

  return;
}

/**
 * @brief Processes the Joystick for driving and steering
 */
void processJoysticksDrive(){
  bool directionForward;
  int throttle;
  int steering;
  int steerInPlace;
  if(joystickLY < 0){
    directionForward = true;
    throttle = -joystickLY;
    steering = joystickRX;
    dcMotorDrive(directionForward, throttle, steering);
  } else if (joystickLY == 0 && joystickRX == 0){
    dcMotorStop();
  } else if (joystickLY > 0) {
    directionForward = false;
    throttle = joystickLY;
    steering = joystickLX;
    dcMotorDrive(directionForward, throttle, steering);
  }
  if (DEBUGFLAG) {
    Serial.print("\t throttle: \t");Serial.print(throttle);
  }
  //Serial.print("\t steering: \t");Serial.print(steering);
  steerInPlace = joystickRX;
  dcMotorSteer(steerInPlace);

}

/**
 * @brief Goes to the state and resets appropriate values and timers
 * @param goState - int the state you want to go to
 */

void goToState(int goState){
  state = goState;
  unsigned long tempTime = millis();
  stateTimer = tempTime;
  peopleTimer = tempTime;
  previousWiggleMillis = tempTime;
  previousShiverMillis = tempTime;
  previousSteer = tempTime;
  enterStateFlag = true;
  enterStateFlag2 = true;
  for (int i = 0; i < NUMBEROFSERVOS; i++)
  {
    servoEndAngle[i] = servoAngle[i];
    servoStartAngle[i] = servoAngle[i];
    startTime[i] = tempTime;
  }
}

/**
 * @brief Processes the buttons and can attach functions to them
 */
void processButtons(){
  if (buttonY) {
    if(manualMode){
      state = STATEANTENNA;
      resetBlinkTimer();
      servoStepsize = 5;
    } else {
      goToState(STATEDEFENSIVE);
    }
  }
  if (buttonB) {
    //manualMode = !manualMode;
    goToState(STATEGOTOSLEEP);
  }
  if (buttonA) {
    if(manualMode){
      state = STATEWHEELS;
      resetBlinkTimer();
      servoStepsize = 5;
    } else {
      goToState(STATEWAKEUP);
    }
  }
  if (buttonX) {
    if (manualMode)
    {
      state = STATEEYES;
      resetBlinkTimer();
      servoStepsize = 2;
    } else {
      goToState(STATEFEARFUL);
    }
  }

}

/**
 * @brief Processes Dpad inputs and can attach functions to them
 */
void processDpad(){
  // increase or decrease stepsize (similar to speed)
  if(DpadUp) {
    goToState(STATEPLAYFULL);
  }
  if(DpadDown) {
    goToState(STATERELAXED);
  }
  if(DpadLeft) {
    goToState(STATESTRESSED);
  }
  if(DpadRight) {
    goToState(STATEALERT);
  }
  return;
}