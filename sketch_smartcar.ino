// Author: Dhruv Joshi
//June 3rd 2025

#include <IRremote.h>

// Pin definitions
const int IR_RECEIVE_PIN = 12;  // Define the pin number for the IR Sensor
const int A_1B = 5;
const int A_1A = 6;
const int B_1B = 9;
const int B_1A = 10;
const int echoPin = 4;
const int trigPin = 3;
const int rightIR = 7;
const int leftIR = 8;
const int lineTrackPin = 2;
const int buzzerPin = 11; // Active buzzer connected here

// Security System 
String passcode = "1234";
String enteredCode = "";
int attempts = 3;
bool unlocked = false;

// Calibration values for motor adjustments
float leftOffset = 1.0;         // Left motor speed offset
float rightOffset = 0.9;        // Right motor speed offset

// Variable declaration 
int speed = 150;
String flag = "NONE";
String lastKey = "NONE";  // Variable to store the last key pressed

// Initialize setup
void setup() {
  Serial.begin(9600);

  // motor
  pinMode(A_1B, OUTPUT);
  pinMode(A_1A, OUTPUT);
  pinMode(B_1B, OUTPUT);
  pinMode(B_1A, OUTPUT);

  // ultrasonic
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  // IR obstacle
  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  // Line Track Module
  pinMode(lineTrackPin, INPUT);
  
  // Active - Buzzer 
  pinMode(buzzerPin, OUTPUT);

  // IR remote
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  // Start the IR receiver // Start the receiver
  Serial.println("REMOTE CONTROL START");

}

// Main loop to handle different modes and IR input
void loop() {

  if (!unlocked) {
    if (IrReceiver.decode()) {
      String key = decodeKeyValue(IrReceiver.decodedIRData.command);

      if (key == "POWER") {
        resetLock();  // Owner can reset the system
      } 
      else if (key.length() == 1 && key[0] >= '0' && key[0] <= '9') {
        enteredCode += key;
        Serial.println("Entered: " + enteredCode);

        if (enteredCode.length() == 4) {
          if (enteredCode == passcode) {
            unlocked = true;
            Serial.println("Access Granted");
          } 
          else {
            attempts--;
            Serial.println("Access Denied. Attempts left: " + String(attempts));
            beepError();
            if (attempts <= 0) {
              Serial.println("Too many wrong attempts. Locked until reset.");

              while (true) {
                beepError();

                if (IrReceiver.decode()) {
                  String key = decodeKeyValue(IrReceiver.decodedIRData.command);
                  if (key == "POWER") {
                    resetLock();
                    IrReceiver.resume();
                    break;  // exit the while loop, unlock system
                  }
                    IrReceiver.resume();
                }
              }
            }

        }
          enteredCode = "";
        }
      }

      IrReceiver.resume();
    }
  return;  // Skip rest of loop if still locked
  }
  if (IrReceiver.decode()) {
    //    Serial.println(results.value,HEX);
    String key = decodeKeyValue(IrReceiver.decodedIRData.command);
    if (key != "ERROR") {
      Serial.println(key);

      // Handle various key inputs to control speed, movement, and modes
      if (key == "+") {
        speed += 25;
        Serial.println(speed);
      } 
      else if (key == "-") {
        speed -= 25;
        Serial.println(speed);
      } 
      else if (key == "2") {
        moveForward(speed);
        delay(1000);
      } 
      else if (key == "1") {
        moveLeft(speed);
      } else if (key == "3") {
        moveRight(speed);
      } 
      else if (key == "4") {
        turnLeft(speed);
      } 
      else if (key == "6") {
        turnRight(speed);
      } 
      else if (key == "7") {
        backLeft(speed);
      } 
      else if (key == "9") {
        backRight(speed);
      } 
      else if (key == "8") {
        moveBackward(speed);
        delay(1000);
      }
      else if( key == "POWER") { 
        resetLock();
      } 
      else if (key == "CYCLE") {
        flag = "LINE";
      } 
      else if (key == "U/SD") {
        flag = "AUTO";
      } 
      else if (key == "0") {
        flag = "NONE";
        stopMove();
      } 
      else if (key == "FORWARD") {
        flag = "ULTR";
      } 
      else if (key == "BACKWARD" && lastKey == "0") {
        flag = "IROB";
      } 
      else if (key == "EQ") {
        flag = "FOLW";
      }
      
      lastKey = key;

      if (speed >= 255) {
        speed = 255;
      }
      if (speed <= 0) {
        speed = 0;
      }
      delay(500);
      stopMove();
    }

    IrReceiver.resume();  // Enable receiving of the next value
  }

  // Handle different modes based on the flag
  if (flag == "AUTO") {
    AutoDrive(speed);
  } 
  else if (flag == "LINE") {
    lineTrack(speed);
  } 
  else if (flag == "ULTR") {
    ultraSonic(speed);
  } 
  else if (flag == "IROB" && (flag != "FOLW" && flag != "AUTO")) {
    irObstacle(speed);
  } 
  else if (flag == "FOLW") {
    following(speed);
  }
}

float readSensorData() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float distance = pulseIn(echoPin, HIGH) / 58.00;  //Equivalent to (340m/s*1us)/2
  return distance;
}

// Motor Control Functions 

// Depending on what body configuration is ideal for each mode, move forward and back may alter in the code (some reuire the head to move forward and some require the tail)
void moveForward(int speed) {
  analogWrite(A_1B, 0);
  analogWrite(A_1A, int(speed * leftOffset));
  analogWrite(B_1B, int(speed * rightOffset));
  analogWrite(B_1A, 0);
}

void moveBackward(int speed) {
  analogWrite(A_1B, speed);
  analogWrite(A_1A, 0);
  analogWrite(B_1B, 0);
  analogWrite(B_1A, speed);
}

void turnRight(int speed) {
  analogWrite(A_1B, speed);
  analogWrite(A_1A, 0);
  analogWrite(B_1B, speed);
  analogWrite(B_1A, 0);
}

void turnLeft(int speed) {
  analogWrite(A_1B, 0);
  analogWrite(A_1A, speed);
  analogWrite(B_1B, 0);
  analogWrite(B_1A, speed);
}

void moveLeft(int speed) {
  analogWrite(A_1B, 0);
  analogWrite(A_1A, speed);
  analogWrite(B_1B, 0);
  analogWrite(B_1A, 0);
}

void moveRight(int speed) {
  analogWrite(A_1B, 0);
  analogWrite(A_1A, 0);
  analogWrite(B_1B, speed);
  analogWrite(B_1A, 0);
}

void backLeft(int speed) {
  analogWrite(A_1B, speed);
  analogWrite(A_1A, 0);
  analogWrite(B_1B, 0);
  analogWrite(B_1A, 0);
}

void backRight(int speed) {
  analogWrite(A_1B, 0);
  analogWrite(A_1A, 0);
  analogWrite(B_1B, 0);
  analogWrite(B_1A, speed);
}

// Gentle forward RIGHT turn
void softTurnRight(int speed) {
  analogWrite(A_1B, speed);           // Left motor forward (reversed wiring)
  analogWrite(A_1A, 0);
  analogWrite(B_1B, speed / 2);       // Right motor forward (slower)
  analogWrite(B_1A, 0);
}

// Gentle forward LEFT turn
void softTurnLeft(int speed) {
  analogWrite(A_1B, 0);           // Left motor forward (faster)
  analogWrite(A_1A, speed / 2);
  analogWrite(B_1B, 0);       // Right motor forward (slower)
  analogWrite(B_1A, speed);
}

void stopMove() {
  analogWrite(A_1B, 0);
  analogWrite(A_1A, 0);
  analogWrite(B_1B, 0);
  analogWrite(B_1A, 0);
}

void beepOn() {
  digitalWrite(buzzerPin, HIGH);
}

void beepOff() {
  digitalWrite(buzzerPin, LOW);
}

void beepError() {
  for (int i = 0; i < 3; i++) {
    beepOn(); delay(200);
    beepOff(); delay(200);
  }
}

void resetLock() {
  attempts = 3;
  unlocked = false;
  enteredCode = "";
  Serial.println("Lock reset");
}

// Function for each mode 

void AutoDrive(int speed) {
  int left = digitalRead(leftIR);  // 0: Obstructed   1: Empty
  int right = digitalRead(rightIR);

  float distance = readSensorData();
  Serial.println(distance);

  // Buzzer on if object is too close (<= 2 cm or zero reading)
  if (distance <= 2) {
    beepOn();
    stopMove();
    delay(1000);
    beepOff();
  }

  if (!left && right) {
    moveForward(120);
    delay(1000);
    backRight(speed);
    delay(1000);
  } 
  else if (left && !right) {
    moveForward(120);
    delay(1000);
    backLeft(speed);
    delay(1000);
  } 
  else if (!left && !right) {
    moveForward(speed);
  } 
  else {
    if (distance > 50) {  // Safe
      moveBackward(200);  
    } 
    else if (distance < 10 && distance > 2) {  // Attention range
      moveForward(200); 
      delay(1000);
      backLeft(150);
      delay(500);
    }
    else {
      moveBackward(120);
    }
  }
}

void following(int speed) {
  float distance = readSensorData();

  int left = digitalRead(leftIR);  // 0: Obstructed   1: Empty
  int right = digitalRead(rightIR);

  if (!left && !right) {
    moveBackward(speed);
  } else if (!left && right) {
    softTurnLeft(speed);
  } else if (left && !right) {
    softTurnRight(speed);
  } else {
    stopMove();
  }
}

void lineTrack(int speed) {
  int lineColor = digitalRead(lineTrackPin);  // 0:white  1:black
  Serial.println(lineColor);
  if (lineColor) {
    moveLeft(speed);
  } else {
    moveRight(speed);
  }
}

void irObstacle(int speed) {
  int left = digitalRead(leftIR);  // 0: Obstructed   1: Empty
  int right = digitalRead(rightIR);

  if (!left && right) {
    backLeft(speed);
  } else if (left && !right) {
    backRight(speed);
  } else if (!left && !right) {
    moveForward(speed);
  } else {
    stopMove();
  }
}

void ultraSonic(int speed) {
  float distance = readSensorData();
  Serial.println(distance);
  if (distance > 25) {
    moveBackward(speed);
  } else if (distance < 10 && distance > 2) {
    moveForward(speed);  
  } else {
    stopMove();
  }
}

// Function decodes the hex values recieved by the IR remote signal 
String decodeKeyValue(long result)
{
  switch(result){
    case 0x16:
      return "0";
    case 0xC:
      return "1"; 
    case 0x18:
      return "2"; 
    case 0x5E:
      return "3"; 
    case 0x8:
      return "4"; 
    case 0x1C:
      return "5"; 
    case 0x5A:
      return "6"; 
    case 0x42:
      return "7"; 
    case 0x52:
      return "8"; 
    case 0x4A:
      return "9"; 
    case 0x9:
      return "+"; 
    case 0x15:
      return "-"; 
    case 0x7:
      return "EQ"; 
    case 0xD:
      return "U/SD";
    case 0x19:
      return "CYCLE";         
    case 0x44:
      return "PLAY/PAUSE";   
    case 0x43:
      return "FORWARD";   
    case 0x40:
      return "BACKWARD";   
    case 0x45:
      return "POWER";   
    case 0x47:
      return "MUTE";   
    case 0x46:
      return "MODE";       
    case 0x0:
      return "ERROR";   
    default :
      return "ERROR";
    }
}
