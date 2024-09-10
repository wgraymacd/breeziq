#include <printf.h>
// radio includes
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
// lcd includes
#include <LiquidCrystal.h>
// temp sensor includes
#include <Wire.h>
#include "Adafruit_ADT7410.h"

// Create the ADT7410 temperature sensor object
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();

RF24 radio(9, 10); // CE, CNS
const byte address1[6] = "00001";
const byte address2[6] = "00002";

const int rs = 2, en = 4, d4 = 6, d5 = 5, d6 = 3, d7 = 7;
const int lcdOnPin = 8; // controls lcd brightness (on and off)
int cyclesLeftOn = 60;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

float degreesC, degreesF;
float settemp = 70;
bool lcdOn = true;

void setup() {
  Serial.begin(9600);

  // radio
  radio.begin();
  radio.openWritingPipe(address2); // set address of receiver to which we send signals
  radio.openReadingPipe(1, address1);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening(); // sets this module as a transmitter

  // lcd
  lcd.clear();
  lcd.begin(16, 2);
  lcd.print("Loading...");
  Serial.println("printing");

  if (!tempsensor.begin()) {
    Serial.println("Couldn't find ADT7410!");
    while (1)
      ;
  }

  pinMode(lcdOnPin, OUTPUT);
  digitalWrite(lcdOnPin, HIGH);

  delay(250); // sensor takes 250 ms to get first readings
}

void loop() {
  if (cyclesLeftOn <= 0) {
    digitalWrite(lcdOnPin, LOW);
  } else if (cyclesLeftOn == 60) {
    digitalWrite(lcdOnPin, HIGH);
  } else {
    cyclesLeftOn--;
  }

  // lcd.clear();
  lcd.home();

  degreesC = tempsensor.readTempC() - 1;
  degreesF = degreesC * (9.0 / 5.0) + 32;

  lcd.print("Temp:  ");
  lcd.print(degreesF);
  lcd.print("F");
  lcd.setCursor(15, 0);

  Serial.print("settemp = "); Serial.println(settemp);
  Serial.print("degreesF = "); Serial.println(degreesF);

  // button logic
  int buttonHotter = digitalRead(15);
  int buttonColder = digitalRead(16);
  if (buttonHotter == 1) {
    cyclesLeftOn = 60; // in miliseconds
    settemp++;
  }
  if (buttonColder == 1) {
    cyclesLeftOn = 60; // in miliseconds
    settemp--;
  }

  lcd.setCursor(0, 1);
  lcd.print("Set:   ");
  lcd.print(settemp);
  lcd.print("F");

  delay(500);
  radio.stopListening();

  // the servo rotation values
  int out = 0;
  if (degreesF > settemp) {
    out = 0; // close
  } else if (degreesF < settemp - 1) {
    out = 1; // open
  }
  Serial.print("out before radio write = "); Serial.println(out);
  radio.write(&out, sizeof(out));

  delay(250);

  // radio recieving
  radio.startListening();
  if (radio.available()) {
    int in = 0;
    radio.read(&in, sizeof(in));
    if (in = 1) {
      lcd.setCursor(15, 1);
      lcd.print("-");
      // Serial.println("acquired");
    }
  } else {
    lcd.clear();
    lcd.print("lost");
    // Serial.println("lost");
  }

  Serial.println("-----------------------------------");
}
