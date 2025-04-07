#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

struct Data {
  byte throttle;
  byte steering;
  byte extraBtn1;
  byte extraBtn2;
};

Data data;

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

const int throttlePin = A0;
const int steeringPin = A1;
const int button1Pin = 2;
const int button2Pin = 3;

void setup() {
  Serial.begin(9600);
  
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
  
  data.throttle = 127;
  data.steering = 127;
  data.extraBtn1 = 0;
  data.extraBtn2 = 0;
}

void loop() {
  data.throttle = map(analogRead(throttlePin), 0, 1023, 0, 255);
  data.steering = map(analogRead(steeringPin), 0, 1023, 0, 255);
  
  data.extraBtn1 = !digitalRead(button1Pin);
  data.extraBtn2 = !digitalRead(button2Pin);
  
  radio.write(&data, sizeof(Data));
  delay(15);
}