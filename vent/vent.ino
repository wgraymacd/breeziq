#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <LiquidCrystal.h>
#include <Servo.h>

RF24 radio(9, 10); // CE, CNS
const byte address1[6] = "00001";
const byte address2[6] = "00002";

Servo myServo;


void setup() {
  Serial.begin(9600);

  // radio
  radio.begin();
  radio.openReadingPipe(0, address2);
  radio.openWritingPipe(address1);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening(); // sets this module as a receiver

  myServo.attach(3);
}

void loop() {
  delay(250);
  radio.startListening();
  int in = 0;
  radio.read(&in, sizeof(in));
  Serial.print("in = ");
  Serial.println(in);

  // servo logic
  // if in == 0, vent close
  // if in == 1, vent open
  if (in == 0) {
    myServo.write(100);
  } else {
    myServo.write(115);
  }

  delay(500);
  radio.stopListening();

  int out = 1;
  radio.write(&out, sizeof(out));
}
