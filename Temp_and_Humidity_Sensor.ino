#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// --- Pin Definitions ---
#define DHTPIN A3
#define DHTTYPE DHT11
#define BUTTON_PIN 2

// --- Globals ---
bool useFahrenheit = false;
int displayMode = 0;
const int maxDisplayMode = 1;

unsigned long buttonPressTime = 0;
bool buttonState = HIGH;
bool lastButtonState = HIGH;
bool buttonPressed = false;
const unsigned long longPressDuration = 1000;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Custom Character for Bar Display ---
byte barChar[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.createChar(0, barChar); // create bar char once
  Serial.println("DHT11 Sensor Ready");
}

void loop() {
  buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    // button just pressed
    buttonPressTime = millis();
    buttonPressed = true;
  }

  if (buttonState == HIGH && lastButtonState == LOW && buttonPressed) {
    // button just released
    unsigned long pressDuration = millis() - buttonPressTime;
    buttonPressed = false;

    if (pressDuration >= longPressDuration) {
      // long press: toggle display mode
      displayMode = (displayMode + 1) % (maxDisplayMode + 1);
    } else {
      // short press: toggle °C/°F
      useFahrenheit = !useFahrenheit;
    }
  }

  lastButtonState = buttonState;

  // --- Sensor Readings ---
  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();

  if (isnan(humidity) || isnan(tempC)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error");
    Serial.println("Sensor Error");
    delay(2000);
    return;
  }

  float temperature = useFahrenheit ? (tempC * 9.0 / 5.0 + 32.0) : tempC;

  // --- Display Logic ---
  lcd.clear();

  if (displayMode == 0) {
    // Normal text display
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temperature, 1);
    lcd.print(useFahrenheit ? "F " : "C ");
    lcd.print("H:");
    lcd.print(humidity, 1);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Uptime:");
    lcd.print(millis() / 1000);
    lcd.print("s");

  } else if (displayMode == 1) {
    // Bar display
    int tempBars = map((int)temperature, 0, useFahrenheit ? 122 : 50, 0, 16);
    int humidBars = map((int)humidity, 0, 100, 0, 16);

    lcd.setCursor(0, 0);
    lcd.print("Temp ");
    for (int i = 0; i < tempBars && i < 16; i++) lcd.write(byte(0));

    lcd.setCursor(0, 1);
    lcd.print("Hum  ");
    for (int i = 0; i < humidBars && i < 16; i++) lcd.write(byte(0));
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Temp: ");
  Serial.print(temperature);
  Serial.println(useFahrenheit ? " F" : " C");

  delay(500);
}
