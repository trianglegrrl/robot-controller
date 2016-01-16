#ifndef RobotController_h
#define RobotController_h

#include <Arduino.h>
#include "SoftwareSerial.h" // Note that this is the replaced version required by the DynamixelSoftSerial - see link below.

#define CONTROLLER_REQUEST_TIMEOUT 1500
/*
 * Set up Bluetooth module
 */
#define BLUETOOTH_TX_PIN 11  // TX-O pin of bluetooth mate, Arduino D11
#define BLUETOOTH_RX_PIN 13  // RX-I pin of bluetooth mate, Arduino D13

class RobotController
{
    // Current string:
    // FWDSPEED|BKWDSPEED|BANKLEFTSPEED|BANKRIGHTSPEED|ROTATELEFTSPEED|ROTATERIGHTRSPEED|HEADYAW|HEADPITCH|SOUND|LIGHTSTATES|AUTONOMYSTATE|
  public:
    RobotController();

    // current values
    int fwdSpeed = 0;
    int bkwdSpeed = 0;
    int bankLeftSpeed = 0;
    int bankRightSpeed = 0;
    int rotateLeftSpeed = 0;
    int rotateRightSpeed = 0;
    int headYaw = 0;
    int headPitch = 0;
    int sound = 0;
    int lightStates = 0;
    int autonomyState = 0;

    void parseControllerState(String stateString);
    void requestControllerState();
    void sayICantHearMyController();
    void setupBluetoothMate();
};

#endif
