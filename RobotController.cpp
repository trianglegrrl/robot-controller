#include "RobotController.hpp"

#define DEBUG_OUTPUT Serial.println

SoftwareSerial robotControllerBluetooth(BLUETOOTH_TX_PIN, BLUETOOTH_RX_PIN);

/* =================================================================
 * getValue() - Helper function to parse a delimited string.
 * Sexy sexy from here: http://arduino.stackexchange.com/questions/1013/how-do-i-split-an-incoming-string
 */
String getValue(String data, char separator, int index)
 {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for(int i=0; i<=maxIndex && found<=index; i++) {
    if(data.charAt(i)==separator || i==maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

RobotController::RobotController() {
  setupBluetoothMate();
}

/* =================================================================
 * setupBluetoothMate() - Configure BluetoothMate for commanding
 */
void RobotController::setupBluetoothMate() {
  robotControllerBluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  robotControllerBluetooth.print("$");  // Print three times individually
  robotControllerBluetooth.print("$");
  robotControllerBluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  robotControllerBluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  delay(100);

  robotControllerBluetooth.begin(9600);
}

void RobotController::parseControllerState(String stateString) {
  DEBUG_OUTPUT("I am parsing the controller state");

  fwdSpeed          = getValue(stateString, '|', 0).toInt();
  bkwdSpeed         = getValue(stateString, '|', 1).toInt();
  bankLeftSpeed     = getValue(stateString, '|', 2).toInt();
  bankRightSpeed    = getValue(stateString, '|', 3).toInt();
  rotateLeftSpeed   = getValue(stateString, '|', 4).toInt();
  rotateRightSpeed  = getValue(stateString, '|', 5).toInt();
  headYaw           = getValue(stateString, '|', 6).toInt();
  headPitch         = getValue(stateString, '|', 7).toInt();
  sound             = getValue(stateString, '|', 8).toInt();
  lightStates       = getValue(stateString, '|', 9).toInt();
  autonomyState     = getValue(stateString, '|', 10).toInt();

  DEBUG_OUTPUT("---forward speed:---");
  DEBUG_OUTPUT(fwdSpeed);
  DEBUG_OUTPUT("---end parsing---");
}

/* =================================================================
 * requestControllerState() - Get the controller's current state.
 * Used to get the driving commands.
 * FWDSPEED|BKWDSPEED|BANKLEFTSPEED|BANKRIGHTSPEED|ROTATELEFTSPEED|ROTATERIGHTRSPEED|HEADYAW|HEADPITCH|SOUND|
 * LIGHTSTATES|AUTONOMYSTATE
*/
void RobotController::requestControllerState() {
  int startTime = millis();
  int hitTimeout = 0;
  String commandString = "";
  char nextControllerChar;
  bool begin = false;

  // if you haven't heard from the controller in N requests, freeze and play sad sound
  // else get the state
  robotControllerBluetooth.println("?"); // Request state from controller

  while (!robotControllerBluetooth.available()) {
    if (millis() > (startTime + CONTROLLER_REQUEST_TIMEOUT)) { hitTimeout = 1; break; }
  }

  if (hitTimeout) {
    sayICantHearMyController();
    DEBUG_OUTPUT("I hit the timeout");
    // Also, get data from serial port (useful for debugging)
    begin = false;

    while (Serial.available() || !begin) {
      nextControllerChar = Serial.read();

      if (nextControllerChar == '{') { begin = true; }

      if (begin) { commandString += (nextControllerChar); }

      if (nextControllerChar == '}') { break; }

      delay(1);
    }

    if (commandString.length()) {
      parseControllerState(commandString.substring(1, (commandString.length() - 1)));
    }
  } else {
    DEBUG_OUTPUT("I did not hit the timeout");
    // Get data from bluetooth connection to robot
    begin = false;

    while (robotControllerBluetooth.available() || !begin) {
      nextControllerChar = robotControllerBluetooth.read();

      if (nextControllerChar == '{') { begin = true; }

      if (begin) { commandString += (nextControllerChar); }

      if (nextControllerChar == '}') { break; }

      delay(1);
    }

    if (commandString.length()) {
      parseControllerState(commandString);
    }
  }

}

void RobotController::sayICantHearMyController() {
  // sad face
  Serial.println("Bweeeep boop boop bweeeee. (\"I can't hear my controller.\")");
}
