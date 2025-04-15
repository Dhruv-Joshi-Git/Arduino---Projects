#include <Wire.h>                // For I2C communication
#include <LiquidCrystal_I2C.h>   // LCD control over I2C
#include <EEPROM.h>              // For storing persistent data like high score

// === GAME CONFIGURATION ===
const int TURNS = 5;             // Maximum number of rounds (levels)
int randomSequence[TURNS];       // Stores the sequence for the current game

// === HARDWARE PINS ===
const int BUZZER = 2;            // Buzzer pin
const int FREQ[4] = {196, 220, 247, 262}; // Frequencies for each button/LED (G3, A3, B3, C4)
const int LED_PINS[4] = {13, 12, 11, 10}; // LEDs for visual feedback
const int BTN_PINS[4] = {6, 5, 4, 3};     // Input buttons

// === GAME STATE ===
int level = 1;                   // Current level (round)
int highScore = 1;               // High score (loaded/saved to EEPROM)
const int highScoreAddr = 0;     // EEPROM address to store high score
int seqDelay = 350;              // Delay for LED & tone duration

// === LCD DISPLAY ===
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns, 2 rows

// === INITIAL SETUP ===
void setup() {
  pinMode(BUZZER, OUTPUT);      // Set buzzer as output

  lcd.init();                   // Initialize LCD
  lcd.backlight();              // Turn on LCD backlight

  // Set all LED pins to output and buttons to INPUT_PULLUP
  for (int i = 0; i < 4; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    pinMode(BTN_PINS[i], INPUT_PULLUP);
  }

  // Random seed to make game unpredictable
  randomSeed(analogRead(A0));

  // Load high score from EEPROM
  EEPROM.get(highScoreAddr, highScore);
  if (highScore < 1 || highScore > TURNS) highScore = 1; // Safety check
}

// === MAIN GAME LOOP ===
void loop() {
  // Show current level and high score
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(level);
  lcd.setCursor(0, 1);
  lcd.print("High Score: ");
  lcd.print(highScore);

  delay(1000); // Pause to let the player see info

  if (level == 1) genRandomSequence(); // Generate new sequence on new game
  showSequence();                      // Show the sequence to follow
  readUserInput();                     // Read player input and validate
}

// === GENERATE RANDOM SEQUENCE ===
// Called at start of a game to generate new button sequence
void genRandomSequence() {
  for (int i = 0; i < TURNS; i++) {
    randomSequence[i] = random(0, 4); // Values between 0 and 3
  }
}

// === DISPLAY SEQUENCE TO PLAYER ===
// Uses LEDs and tones to show current level sequence
void showSequence() {
  delay(300); // Short pause before showing sequence
  for (int i = 0; i < level; i++) {
    int index = randomSequence[i];
    playLED(index); // Play tone and light for current step
    delay(50);      // Pause between steps
  }
}

// === PLAY LED & SOUND FOR GIVEN INDEX ===
// Abstracted function for lighting up and playing tone
void playLED(int index) {
  digitalWrite(LED_PINS[index], HIGH);
  tone(BUZZER, FREQ[index]);
  delay(seqDelay);
  noTone(BUZZER);
  digitalWrite(LED_PINS[index], LOW);
}

// === READ PLAYER INPUT ===
// Waits for the correct sequence of button presses
void readUserInput() {
  for (int i = 0; i < level; i++) {
    bool inputReceived = false;
    while (!inputReceived) {
      for (int j = 0; j < 4; j++) {
        if (digitalRead(BTN_PINS[j]) == LOW) { // Button pressed
          playLED(j); // Play button feedback
          if (j == randomSequence[i]) { // Correct input
            inputReceived = true;
            waitForRelease(j);         // Debounce and wait for release
          } else {
            handleDefeat();            // Wrong input
            return;
          }
        }
      }
    }
  }
  handleVictory(); // Player passed this level
}

// === WAIT FOR BUTTON RELEASE ===
// Prevents multiple readings from long button press
void waitForRelease(int btnIndex) {
  delay(200);
  while (digitalRead(BTN_PINS[btnIndex]) == LOW) {
    delay(20);
  }
}

// === HANDLE LEVEL SUCCESS ===
void handleVictory() {
  if (level < TURNS) {
    level++; // Go to next level
    if (level > highScore) {
      highScore = level;               // Update high score
      EEPROM.put(highScoreAddr, highScore); // Save to EEPROM
    }
  } else {
    showWinAnimation(); // Game completed successfully
    level = 1;          // Restart game
  }
}

// === HANDLE DEFEAT ===
// Triggers on wrong input
void handleDefeat() {
  blinkAllLEDs(3, 300); // Flash LEDs
  tone(BUZZER, 100);    // Play defeat tone
  delay(500);
  noTone(BUZZER);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You Lost :(");
  lcd.setCursor(0, 1);
  lcd.print("Try Again!");
  delay(2000);
  level = 1; // Reset game
}

// === BLINK ALL LEDs ===
// Utility function to flash all lights (used for defeat feedback)
void blinkAllLEDs(int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    for (int j = 0; j < 4; j++) digitalWrite(LED_PINS[j], HIGH);
    delay(delayTime);
    for (int j = 0; j < 4; j++) digitalWrite(LED_PINS[j], LOW);
    delay(delayTime);
  }
}

// === SHOW WIN ANIMATION ===
// Called when all levels are completed
void showWinAnimation() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Congratulations!");
  lcd.setCursor(0, 1);
  lcd.print("You Won :D");

  // Light show and tones
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 4; j++) {
      playLED(j);
      delay(50);
    }
  }

  delay(2000); // Wait before restarting
}
