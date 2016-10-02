#include <Arduino.h>
#include <Servo.h>
#include "Learning.h"
#include "Pins.h"

extern Servo elbowJoint;
extern Servo baseJoint;

ArmState currentState = {0, 0, 0};
ArmState previousState = {0, 0, 0};

void resetArm() {
    elbowJoint.write(10);
    baseJoint.write(10);
    delay(100);
    currentState = previousState = {10, 10, 0};
    
}

void logArmState() {
    Serial.print(currentState.basePosition);
    Serial.print(" ");
    Serial.print(currentState.elbowPosition);
    Serial.print(" ");
    Serial.println(currentState.ledOn); 
}

/// Adjusts the arm's state.
void apply(ArmAction action) {

    // Let's keep track of this for the learner.
    previousState = currentState;
    // We've carefully ensured that the ledOn component is the lowest bit
    currentState.ledOn = (uint8_t)action & 1;

    int8_t baseMovement = 0;
    int8_t elbowMovement = 0;
    switch (action) {
        // No need for StayStay cases. Default values handle that.
        case StayLeftOff:
        case StayLeftOn:
            elbowMovement -= 10;
        break;
        case StayRightOff:
        case StayRightOn:
            elbowMovement += 10;
        break;
        case LeftStayOff:
        case LeftStayOn:
            baseMovement -= 10;
        break;
        case LeftLeftOff:
        case LeftLeftOn:
            baseMovement -= 10;
            elbowMovement -= 10;
        break;
        case LeftRightOff:
        case LeftRightOn:
            baseMovement -= 10;
            elbowMovement += 10;
        break;
        case RightStayOff:
        case RightStayOn:
            baseMovement += 10;
        break;
        case RightLeftOff:
        case RightLeftOn:
            baseMovement += 10;
            elbowMovement -= 10;
        break;
        case RightRightOff:
        case RightRightOn:
            baseMovement += 10;
            elbowMovement -= 10;
        break;
        
    }
    currentState.basePosition += baseMovement;
    currentState.elbowPosition += elbowMovement;

    // Limit the range of the joints before we send them the new state
    if (currentState.basePosition < 10) {
        currentState.basePosition = 10;
    }

    if (currentState.elbowPosition < 10) {
        currentState.elbowPosition = 10;
    }

    if (currentState.basePosition > 170) {
        currentState.basePosition = 170;
    }

    if (currentState.elbowPosition > 170) {
        currentState.elbowPosition = 170;
    }
    
    baseJoint.write(currentState.basePosition);
    // If you get a lot of servo jitter, it's likely because your power supply can't handle the initial current spike.
    // Try adding another delay here avoid moving both motors at once.
    elbowJoint.write(currentState.elbowPosition);
    digitalWrite(ledPin, currentState.ledOn);
    delay(200);
}
