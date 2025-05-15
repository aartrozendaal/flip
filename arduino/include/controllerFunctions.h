#pragma once 
#include <allLibs.h>

void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);
void processGamepad(ControllerPtr gamepad);
void controllerUpdate();
void processJoysticks(int servo1 = -1, int servo2 = -1, int servo3 = -1, int servo4 = -1);
void processButtons();
void processDpad();
void processJoysticksDrive();
void goToState(int goState);