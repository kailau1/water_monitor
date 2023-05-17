#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD object
LiquidCrystal_I2C lcd(0x20, 16, 2);

// Define the pins for the flow meter and pH sensor
const byte flowPin = 2;
const byte pHpin = A0;

// Variables to store the flow rate and pH reading
volatile unsigned long pulseCount;
float flowRate;
float pHreading;

// Variables for the flow meter interrupt
unsigned long currentTime;
unsigned long previousTime;
constexpr float calibrationFactor = 4.5;

// Previous values for LCD update
float previousFlowRate = 0.0;
float previouspHreading = 0.0;

// Timer variables for LCD update and console output
unsigned long lcdUpdateMillis = 0;

unsigned long loopDelayMillis = 1000; // Minimum loop delay time in milliseconds
unsigned long previousLoopMillis = 0;

void setup() {
  // Set up the LCD display
  lcd.init();
  lcd.backlight();

  // Set up the flow meter interrupt
  pulseCount = 0;
  previousTime = millis();
  attachInterrupt(digitalPinToInterrupt(flowPin), flowISR, RISING);

  // Set up the serial connection for debugging
  Serial.begin(9600);
}

void loop() {
  currentTime = millis(); // Current time in milliseconds

  // Calculate the flow rate
  unsigned long timeDiff = currentTime - previousTime;
  flowRate = (pulseCount * 60.0) / (calibrationFactor * timeDiff);

  // Read the pH sensor
  int pHraw = analogRead(pHpin);
  if (pHraw >= 0 && pHraw <= 1023) {
    pHreading = map(pHraw, 0, 1023, 0, 1400);
    pHreading /= 100.0;
  }

  // Update the LCD if the flow rate or pH reading has changed
  if (flowRate != previousFlowRate || pHreading != previouspHreading) {
    lcd.setCursor(0, 0);
    lcd.print("Flow: ");
    lcd.print(flowRate, 1);
    lcd.print(" L/min");

    lcd.setCursor(0, 1);
    lcd.print("pH: ");
    lcd.print(pHreading);

    // Update previous values
    previousFlowRate = flowRate;
    previouspHreading = pHreading;
  }

  // Debugging output
  if (currentTime - previousTime >= 1000) {
    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.print(" L/min\t pH reading: ");
    Serial.println(pHreading);

    previousTime = currentTime; // Update the last console output time
  }

  pulseCount = 0; // Reset the flow count

  // Other code or operations can be placed here

  // Non-blocking delay to ensure a minimum loop duration
  unsigned long currentLoopMillis = millis();
  if (currentLoopMillis - previousLoopMillis >= loopDelayMillis) {
    // Update previous loop time
    previousLoopMillis = currentLoopMillis;
  }
}

void flowISR() {
  pulseCount++;
}
