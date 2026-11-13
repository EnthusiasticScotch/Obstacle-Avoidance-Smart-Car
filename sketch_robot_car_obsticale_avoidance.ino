#include <Servo.h>

const int STBY = 3;
const int PWMA = 5;
const int PWMB = 6;
const int AIN1 = 7;
const int BIN1 = 8;

const int trig = 13;
const int echo = 12;

Servo servo;

long duration;
int distance = 100;

const int STOP_DISTANCE  = 30;
const int CLEAR_DISTANCE = 40;

unsigned long stateStartTime = 0;
enum State { FORWARD, BACKWARD };
State currentState = FORWARD;

unsigned long lastPingTime = 0;
const unsigned long PING_INTERVAL = 100;

int servoAngle = 65;
int servoStep = 1;
const int SERVO_MIN = 20;
const int SERVO_MAX = 110;

unsigned long lastServoTime = 0;
const unsigned long SERVO_INTERVAL = 5;

void setup() {
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(BIN1, OUTPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  servo.attach(10);
  servo.write(65);
  delay(500);

  digitalWrite(STBY, HIGH);
  Serial.begin(4800);
}

void loop() {
  unsigned long now = millis();

  if (now - lastPingTime >= PING_INTERVAL) {
    lastPingTime = now;

    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    duration = pulseIn(echo, HIGH, 30000);
    if (duration > 0) {
      distance = duration * 0.034 / 2;
    }

    Serial.print("Distance: ");
    Serial.println(distance);
  }

  if (now - lastServoTime >= SERVO_INTERVAL) {
    lastServoTime = now;

    servoAngle += servoStep;

    if (servoAngle >= SERVO_MAX) {
      servoAngle = SERVO_MAX;
      servoStep = -servoStep;
    } else if (servoAngle <= SERVO_MIN) {
      servoAngle = SERVO_MIN;
      servoStep = -servoStep;
    }

    servo.write(servoAngle);
  }

  if (currentState == FORWARD && distance <= STOP_DISTANCE) {
    currentState = BACKWARD;
    stateStartTime = now;
  } else if (currentState == BACKWARD && distance >= CLEAR_DISTANCE) {
    currentState = FORWARD;
    stateStartTime = now;
  }

  if (currentState == BACKWARD) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(BIN1, LOW);
    analogWrite(PWMA, 100);
    analogWrite(PWMB, 100);
  } else {
    digitalWrite(AIN1, HIGH);
    digitalWrite(BIN1, HIGH);
    analogWrite(PWMA, 150);
    analogWrite(PWMB, 150);
  }
}