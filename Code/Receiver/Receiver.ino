#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

#define ENA 5
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 4
#define ENB 3

struct Data {
  byte throttle;
  byte steering;
  byte extraBtn1;
  byte extraBtn2;
};

Data data;

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

unsigned long lastRecvTime = 0;
const unsigned long timeout = 1000;

void setup() {
  Serial.begin(9600);
  
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
  
  resetData();
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(Data));
    lastRecvTime = millis();
  }
  
  if (millis() - lastRecvTime > timeout) {
    resetData();
  }
  
  controlMotors();
}

void resetData() {
  data.throttle = 127;
  data.steering = 127;
  data.extraBtn1 = 0;
  data.extraBtn2 = 0;
}

void controlMotors() {
  int throttleVal = data.throttle;
  int steeringVal = data.steering;
  
  if (throttleVal > 137) {
    int motorSpeed = map(throttleVal, 138, 255, 50, 255);
    forward(motorSpeed);
  }
  else if (throttleVal < 117) {
    int motorSpeed = map(throttleVal, 116, 0, 50, 255);
    backward(motorSpeed);
  }
  else {
    stop();
  }
  
  if (steeringVal > 137) {
    int turnSpeed = map(steeringVal, 138, 255, 0, 50);
    rightTurn(turnSpeed);
  }
  else if (steeringVal < 117) {
    int turnSpeed = map(steeringVal, 116, 0, 0, 50);
    leftTurn(turnSpeed);
  }
  
  if (data.extraBtn1) {
    honk();
  }
  
  if (data.extraBtn2) {
    lights();
  }
}

void forward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void backward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void rightTurn(int adjust) {
  int currentENA = analogRead(ENA);
  int currentENB = analogRead(ENB);
  
  if (currentENA > 0)
    analogWrite(ENA, max(currentENA - adjust, 30));
    
  if (currentENB > 0)
    analogWrite(ENB, currentENB);
}

void leftTurn(int adjust) {
  int currentENA = analogRead(ENA);
  int currentENB = analogRead(ENB);
  
  if (currentENA > 0)
    analogWrite(ENA, currentENA);
    
  if (currentENB > 0)
    analogWrite(ENB, max(currentENB - adjust, 30));
}

void honk() {
  // Add code for horn if connected
}

void lights() {
  // Add code for lights if connected
}