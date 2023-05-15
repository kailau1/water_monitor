#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD object
LiquidCrystal_I2C lcd(0x20, 16, 2);

// Define the pins for the flow meter and pH sensor
const int flowPin = 2;
const int pHpin = A0;

// Variables to store the flow rate and pH reading
volatile int pulseCount;
float flowRate;
float pHreading;

// Variables for the flow meter interrupt
unsigned long currentTime;
unsigned long previousTime;
const float calibrationFactor = 4.5;

// Timer variables for console output
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

void setup() 
{
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

void loop() 
{
  // Calculate the flow rate
  currentTime = millis();
  float timeDiff = (float)(currentTime - previousTime) / 1000.0;
  flowRate = (pulseCount / calibrationFactor) / timeDiff * 60.0;

  // Read the pH sensor
  pHreading = analogRead(pHpin);
  pHreading = map(pHreading, 0, 1023, 0, 1400) / 100.0; // Map the pH reading to a range of 0-14 with two decimal places

  // Display the readings on the LCD
  lcd.setCursor(0,0);
  lcd.print("Flow: ");
  lcd.print(flowRate, 1);
  lcd.print(" L/min");

  lcd.setCursor(0,1);
  lcd.print("pH: ");
  lcd.print(pHreading);
  
  // Debugging output
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.print(" L/min\t pH reading: ");
    Serial.println(pHreading);
  }

  // Reset the flow count and time
  pulseCount = 0;
  previousTime = currentTime;

  // Wait for a moment before taking another reading
  delay(1000);
}

void flowISR()
{
  pulseCount++;
}
