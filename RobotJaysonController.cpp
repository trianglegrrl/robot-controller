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

RobotJaysonController::RobotJaysonController() {
  setupBluetoothMate();
}

/* =================================================================
 * setupBluetoothMate() - Configure BluetoothMate for commanding
 */
void RobotJaysonController::setupBluetoothMate() {
  robotControllerBluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  robotControllerBluetooth.print("$");  // Print three times individually
  robotControllerBluetooth.print("$");
  robotControllerBluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  robotControllerBluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  delay(100);

  robotControllerBluetooth.begin(9600);
}


RobotJaysonController::parseControllerState(String stateString) {
  fwdSpeed          = getValue(stateString, '|', 0);
  bkwdSpeed         = getValue(stateString, '|', 1);
  bankLeftSpeed     = getValue(stateString, '|', 2);
  bankRightSpeed    = getValue(stateString, '|', 3);
  rotateLeftSpeed   = getValue(stateString, '|', 4);
  rotateRightSpeed  = getValue(stateString, '|', 5);
  headYaw           = getValue(stateString, '|', 6);
  headPitch         = getValue(stateString, '|', 7);
  sound             = getValue(stateString, '|', 8);
  lightStates       = getValue(stateString, '|', 9);
  autonomyState     = getValue(stateString, '|', 10);
}

/* =================================================================
 * requestControllerState() - Get the controller's current state.
 * Used to get the driving commands.
 * FWDSPEED|BKWDSPEED|BANKLEFTSPEED|BANKRIGHTSPEED|ROTATELEFTSPEED|ROTATERIGHTRSPEED|HEADYAW|HEADPITCH|SOUND|
 * LIGHTSTATES|AUTONOMYSTATE
*/
RobotJaysonController::requestControllerState() {
  int startTime = millis();
  int hitTimeout = 0;
  String commandString = "";
  char nextControllerChar = '';

  // if you haven't heard from the controller in N requests, freeze and play sad sound
  // else get the state
  robotControllerBluetooth.println("?"); // Request state from controller

  // Use these examples later, e.g. for a watchdog timeout
  // timer.setTimer(1200, TenTimesTask, 10);
  // void restartTimer(int timerId)

  while (!robotControllerBluetooth.available()) {
    if (millis() > (startTime + CONTROLLER_REQUEST_TIMEOUT)) { hitTimeout = 1; break; }
  }

  if (hitTimeout) {
    sayICantHearMyController();
    // Also, get data from serial port (useful for debugging)
    while (Serial.available()) {
      delay(20);

      nextControllerChar = Serial.read();

      if (c == '\n') { break; }

      commandString += c;
    }

    if (commandString.length()) {
      parseControllerState(commandString);
    }
  } else {
    // Get data from bluetooth connection to robot
    while (robotControllerBluetooth.available()) {
      delay(20);

      nextControllerChar = robotControllerBluetooth.read();

      if (c == '\n') { break; }

      commandString += c;
    }

    if (commandString.length()) {
      parseControllerState(commandString);
    }
  }

}

RobotJaysonController::sayICantHearMyController() {
  // sad face
  Serial.println("Aw, I can't hear my controller.");
}
