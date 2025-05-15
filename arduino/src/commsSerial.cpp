#include <allLibs.h>

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

extern unsigned long highestBodyTimer;
extern unsigned long peopleTimer;

String inputString = "";

/**
 * @brief Reads the microphone and sets soundDirection 
 */
void readMicrophone() {
  int tempSound = analogRead(MICROPHONE);
  if (tempSound> SOUNDMEAN + SOUNDTHRESHOLD) {
    soundDirection = 1;
  } else if (tempSound < SOUNDMEAN - SOUNDTHRESHOLD) {
    soundDirection = 1;
  } else {
    soundDirection = 0;
  }
}


/**
 * @brief Receives and processes all data from the Pi via Serial
 */
void receiveData() {
    unsigned long tempTime = millis();
    if (Serial.available()) {
        inputString = Serial.readStringUntil('\n');
        inputString.trim();  // remove whitespace
        int values[10];
        int lastIndex = 0;
        for (int i = 0; i < 10; i++) {
          int commaIndex = inputString.indexOf(',', lastIndex);
          if (commaIndex == -1 && i < 9) return; // malformed data
          String part = (i == 9) ? inputString.substring(lastIndex) : inputString.substring(lastIndex, commaIndex);
          values[i] = part.toInt();
          lastIndex = commaIndex + 1;
        }
      
    numFaces = values[0]; 
    faceX = values[1];
    faceY = values[2];
    if(faceSize = 0) deltaFaceSize = 0;
    else deltaFaceSize = values[3] - faceSize;
    faceSize = values[3];
    numBodies = values[4];
    bodyX = values[5];
    bodyY = values[6];
    if(bodySize = 0) deltaBodySize = 0;
    else deltaBodySize = values[7] - bodySize;
    bodySize = values[7];
    //sound
    //soundDirection = values[8];
    //emotion
    emotion = values[9];
    //people
    if(numBodies == 0 && numFaces == 0) person = false;
    else {
      person = true;
      peopleTimer = millis();
    }
  }
}



  
