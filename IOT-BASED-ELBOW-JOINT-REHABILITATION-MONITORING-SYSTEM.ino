#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int flexs = A0;
int ledRed = 7;
int ledYellow = 6;
int ledGreen = 5;
int buzzerPin = 4; // Define the pin for the buzzer

int transmissionCount = 0;
bool inCycle = false;

void setup() {
  Serial.begin(9600);
  pinMode(flexs, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(buzzerPin, OUTPUT); // Set the buzzer pin as an output

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2); // Set text size to 2
}

void loop() {
  int flexResistance = analogRead(flexs);
  int elbowAngle;

  if (flexResistance <= 250) {
    elbowAngle = 0; // Set angle to 0 for resistance less than or equal to 250 ohms
  } else {
    elbowAngle = ((flexResistance - 250) / 3) * 10; // Increment 10 degrees for each 3 ohms rise in resistance
    if (elbowAngle > 180) {
      elbowAngle = 180;
    }
  }

  // Update LED states based on elbow angle
  if (elbowAngle < 45 || elbowAngle == 0) {
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledGreen, LOW);
  } else if (elbowAngle >= 45 && elbowAngle <= 110) { // Modified condition to turn on green LED when angle is less than or equal to 110
    digitalWrite(ledRed, LOW);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledGreen, LOW);
  } else {
    digitalWrite(ledRed, LOW);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledGreen, HIGH);
  }

  // Check for cycle transition
  if (!inCycle && elbowAngle >= 110) {
    inCycle = true;
  } else if (inCycle && elbowAngle <= 10) {
    transmissionCount++;
    // Activate the buzzer when count increments
    digitalWrite(buzzerPin, HIGH);
    delay(100); // Buzzer on time
    digitalWrite(buzzerPin, LOW);
    inCycle = false; // Reset cycle flag when elbowAngle goes below or equal to 10 degrees
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Ohms: ");
  display.print(flexResistance);
  display.println("");
  display.print("Angle: ");
  display.print(elbowAngle);
  display.println("");
  display.print("Count: ");
  display.println(transmissionCount);
  display.display();

  delay(500);

  Serial.print("Resistance: ");
  Serial.print(flexResistance);
  Serial.print(" ohm | Elbow Angle: ");
  Serial.print(elbowAngle);
  Serial.println(" deg");
  Serial.print("Cycle Count: ");
  Serial.println(transmissionCount);
}
