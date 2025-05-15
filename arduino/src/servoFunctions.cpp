#include <allLibs.h>

extern uint16_t servoAngle[NUMBEROFSERVOS];
extern Adafruit_PWMServoDriver pwm;
extern int state;


/**
 * @brief stores the angle for the given servo
 * @param angle Give new value for angle that will be stored.
 * @param servoNumber The angle for the selected servo will be stored.
 */
void servoAngleSet(uint16_t angle ,uint16_t servoNumber){
    servoAngle[servoNumber] = angle;
    return;
}

/**
 * @brief Homes the Servo to its neutral position (135 degrees)
 */
void servoHome(){ 
    for (int i = 0; i < NUMBEROFSERVOS; i++)
    {
        servoGoTo(SERVOHOMEANGLE[state][i], i);
    }
    
    return;
}

/**
 * @brief Brings the servos to the min
 */
void servoMin(){
    for (int i = 0; i < NUMBEROFSERVOS; i++)
    {
        servoGoTo(SERVOMINANGLE[state][i], i);
    }
    return;
}

/**
 * @brief Brings the servos to the max
 */
void servoMax(){
    for (int i = 0; i < NUMBEROFSERVOS; i++)
    {
        servoGoTo(SERVOMAXANGLE[state][i], i);
    }
    return;
}

/**
 * @brief Calculates the pulsewidth that corresponds to the given angle
 * @param angle The desired angle of the servo (must be between 0-270).
 * @param servoSmall is the servo small, default false.
 * @return The corresponding pulsewidth in ms.
 */
float servoDegree(uint16_t angle, uint16_t servoNumber){
    float degree;
    if (SERVOSMALL[servoNumber]) degree = float(angle)/(float(SERVOSMALLMAXDEGREE)-float(SERVOSMALLMINDEGREE)); //Normalize small servo
    else degree = float(angle)/(float(SERVOMAXDEGREE)-float(SERVOMINDEGREE)); //Normalize
    degree = degree*(MAXPULSEWIDTH-MINPULSEWIDTH) + MINPULSEWIDTH; // Pulsewidth (ms)
    return degree;
}

/**
 * @brief Selected servo goes to the given angle
 * @param angle The desired angle of the servo (must be between 0-270).
 * @param servoMotor number of the Servo motor that you want to go to the selected angle.
 * @param servoSmall is the servo small, default false.
 */
void servoGoTo(uint16_t angle, uint16_t servoNumber){
    // set the angle in the angle array
    if (servoAngle[servoNumber] != angle){
        servoAngle[servoNumber] = angle;
        // write the pwm signal via the PCA board
        pwm.setPWM(servoNumber,0,(servoDegree(servoAngle[servoNumber],servoNumber) * 1000.0)/MICROSPERBIT); // use the bit resolution
        //pwm.writeMicroseconds(servoNumber, servoDegree(servoAngle[servoNumber]) * 1000.0); // this doesnt work well
        //delay(rotationTime);
    }
    return;
}

/**
 * @brief All servos go to their stored angle
 */
void servoAngleUpdate(){
    for (int i = 0; i < NUMBEROFSERVOS; i++)
    {
        servoGoTo(servoAngle[i], i);
    }
}

/**
 * @brief Calculates the angle for a smooth motion from a start angle to an end angle 
 *          based on the starting time, max speed and max acceleration
 * @param startAngle - Float start angle of the motion
 * @param endAngle - Float end angle of the motion
 * @param maxSpeed - Float maximum speed of the motion
 * @param maxAccel - Float maximum acceleration of the motion
 * @param servoStartTime - unsigned long start time of the motion
 */
float calculateAngle(float startAngle, float endAngle, float maxSpeed, float maxAccel, unsigned long servoStartTime) {
    // Get the current time since the motion started
    unsigned long currentTime = millis();
    if(currentTime > servoStartTime){
        float elapsedTime = (currentTime - servoStartTime) / 1000.0; // Convert to seconds

        // Calculate the total distance to travel in degrees
        float distance = abs(endAngle - startAngle);

        // Calculate the time to reach max speed (acceleration phase)
        float tAccel = maxSpeed / maxAccel;

        // Calculate the distance covered during acceleration (and deceleration)
        float accelDistance = 0.5 * maxAccel * tAccel * tAccel;

        // Determine if we need a constant speed phase
        float cruiseDistance = 0;
        float tCruise = 0;
        if (2 * accelDistance < distance) {
            cruiseDistance = distance - 2 * accelDistance;
            tCruise = cruiseDistance / maxSpeed;
        } else {
            // Adjust for triangular profile
            tAccel = sqrt(distance / maxAccel);
            accelDistance = 0.5 * maxAccel * tAccel * tAccel;
        }

        // Total motion time
        float totalTime = 2 * tAccel + tCruise;

        // Calculate the current angle based on the time
        float currentAngle = startAngle;
        float direction = (endAngle > startAngle) ? 1 : -1; // Determine motion direction

        if (elapsedTime <= tAccel) {
            // Acceleration phase
            currentAngle += direction * 0.5 * maxAccel * elapsedTime * elapsedTime;
        } else if (elapsedTime <= tAccel + tCruise) {
            // Constant speed phase
            float t = elapsedTime - tAccel;
            currentAngle += direction * (accelDistance + maxSpeed * t);
        } else if (elapsedTime <= totalTime) {
            // Deceleration phase
            float t = elapsedTime - (tAccel + tCruise);
            currentAngle += direction * (accelDistance + cruiseDistance + maxSpeed * t - 0.5 * maxAccel * t * t);
        } else {
            // Motion complete
            currentAngle = endAngle;
        }

        if (direction == 1 && currentAngle > endAngle) return endAngle;
        if (direction == -1 && currentAngle < endAngle) return endAngle;
        return currentAngle;
    } else {
        return startAngle;
    }
    
    
}

/**
 * @brief Calculates a random angle from the current angle of the servo that is within the boundries 
 *          and larger than the minimum threshold
 * @param currentAngle - Float current angle of the servo
 * @param servoNumber - int the servo for which the angle is calculated
 * @return float a random angle inside the boundries larger than the threshold
 */
float getRandomAngle(float currentAngle, int servoNumber) {
    float newAngle;
    do {
        // Generate a random delta within the full range
        int randomDelta = random(-(SERVOMAXANGLE[state][servoNumber] - SERVOMINANGLE[state][servoNumber]), (SERVOMAXANGLE[state][servoNumber] - SERVOMINANGLE[state][servoNumber]));
        newAngle = currentAngle + randomDelta;

        // Clamp the new angle within boundaries
        if (newAngle < SERVOMINANGLE[state][servoNumber]) {
            newAngle = SERVOMINANGLE[state][servoNumber];
        } else if (newAngle > SERVOMAXANGLE[state][servoNumber]) {
            newAngle = SERVOMAXANGLE[state][servoNumber];
        }
    } while (abs(newAngle - currentAngle) < RANDOMANGLE[state][servoNumber]); // Ensure it meets the threshold

    return newAngle;
}

/**
 * @brief Calculates a random dealy for the given servo
 * @param servoNumber - int the servo for which the delay is calculated
 * @return unsigned long the random delay between 0 and the MAXDELAY
 */
unsigned long getRandomDelay(int servoNumber) {
    return random(0, 1000UL*MAXDELAY[state][servoNumber]); // `random` is exclusive of the upper bound, so add 1
}
