#include "Config.h"
#include "Control.h"
#include <AFMotor.h>
#include <Arduino.h>
#include <Servo.h>

AF_DCMotor m1(1, MOTOR12_1KHZ), m2(2, MOTOR12_1KHZ);
AF_DCMotor m3(3, MOTOR34_1KHZ), m4(4, MOTOR34_1KHZ);
Servo head;
const follower::Settings settings = {FOLLOW_GAP_CM, GAP_DEADBAND_CM, TARGET_MAX_CM,
                                     MIN_DRIVE_PWM, MAX_DRIVE_PWM,   GAP_GAIN_PWM_PER_CM};
enum class State : uint8_t { Scanning, Aligning, Settling, Following, Lost, Halted };
State state = State::Halted;
int scanAngle = SCAN_MIN, bestAngle = SERVO_CENTRE, bestDistance = TARGET_MAX_CM + 1;
unsigned long stateAt = 0, lastControl = 0, alignDuration = 0;
uint8_t misses = 0, sampleCount = 0, sampleIndex = 0;
int followSamples[3] = {0, 0, 0};

void motor(AF_DCMotor &m, int speed, bool inverted) {
  if (inverted)
    speed = -speed;
  m.setSpeed(constrain(abs(speed), 0, 255));
  m.run(speed == 0 ? RELEASE : (speed > 0 ? FORWARD : BACKWARD));
}
void drive(int left, int right) {
  motor(m1, left, INVERT_M1);
  motor(m2, left, INVERT_M2);
  motor(m3, right, INVERT_M3);
  motor(m4, right, INVERT_M4);
}
void stopDrive() { drive(0, 0); }
void announce(const __FlashStringHelper *message) { Serial.println(message); }

int singleRange() {
  digitalWrite(SONAR_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(SONAR_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SONAR_TRIG_PIN, LOW);
  unsigned long duration = pulseIn(SONAR_ECHO_PIN, HIGH, SONAR_TIMEOUT_US);
  return duration ? int(duration / 58UL) : 0;
}
int filteredRange() {
  // Median of three: one isolated bad echo does not determine the measurement.
  int a = singleRange();
  delay(55);
  int b = singleRange();
  delay(55);
  int c = singleRange();
  if (a > b) {
    int v = a;
    a = b;
    b = v;
  }
  if (b > c) {
    int v = b;
    b = c;
    c = v;
  }
  if (a > b) {
    int v = a;
    a = b;
    b = v;
  }
  return b;
}
void beginScan() {
  stopDrive();
  misses = 0;
  bestDistance = TARGET_MAX_CM + 1;
  bestAngle = SERVO_CENTRE;
  scanAngle = SCAN_MIN;
  head.write(scanAngle);
  state = State::Scanning;
  stateAt = millis();
  announce(F("SCAN: chassis stopped; measuring at each angle"));
}
void loseTarget() {
  stopDrive();
  head.write(SERVO_CENTRE);
  state = State::Lost;
  announce(F("LOST: stopped. Send R to scan again, X to halt."));
}
void serialControls() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'x' || c == 'X' || c == ' ') {
      stopDrive();
      state = State::Halted;
      head.write(SERVO_CENTRE);
      announce(F("HALTED. Send R to restart acquisition."));
    } else if (c == 'r' || c == 'R')
      beginScan();
  }
}
void finishScan() {
  stopDrive();
  if (bestDistance > TARGET_MAX_CM) {
    loseTarget();
    return;
  }
  Serial.print(F("Candidate distance cm: "));
  Serial.print(bestDistance);
  Serial.print(F("; angle: "));
  Serial.println(bestAngle);
  head.write(SERVO_CENTRE);
  int offset = bestAngle - SERVO_CENTRE;
  alignDuration = min(static_cast<unsigned long>(abs(offset) * ALIGN_MS_PER_DEGREE), ALIGN_MAX_MS);
  stateAt = millis();
  if (abs(offset) <= SCAN_STEP / 2) {
    state = State::Settling;
  } else {
    bool left = HIGH_ANGLE_POINTS_LEFT ? offset > 0 : offset < 0;
    drive(left ? -ALIGN_PWM : ALIGN_PWM, left ? ALIGN_PWM : -ALIGN_PWM);
    state = State::Aligning;
    announce(F("ALIGN: brief calibrated pivot, then recheck"));
  }
}
void setup() {
  Serial.begin(115200);
  stopDrive();
  pinMode(LEFT_IR_PIN, INPUT);
  pinMode(RIGHT_IR_PIN, INPUT);
  pinMode(SONAR_TRIG_PIN, OUTPUT);
  pinMode(SONAR_ECHO_PIN, INPUT);
  digitalWrite(SONAR_TRIG_PIN, LOW);
  head.attach(SERVO_PIN);
  head.write(SERVO_CENTRE);
  announce(F("Arduino Human Follower | R=rescan X=stop | lift wheels for first "
             "setup"));
  delay(500);
  beginScan();
}
void loop() {
  serialControls();
  unsigned long now = millis();
  if (state == State::Scanning && now - stateAt >= SERVO_SETTLE_MS) {
    int cm = filteredRange();
    if (cm >= ACQUIRE_MIN_CM && cm <= TARGET_MAX_CM && cm < bestDistance) {
      bestDistance = cm;
      bestAngle = scanAngle;
    }
    if (scanAngle >= SCAN_MAX)
      finishScan();
    else {
      scanAngle = min(scanAngle + SCAN_STEP, SCAN_MAX);
      head.write(scanAngle);
      stateAt = millis();
    }
  } else if (state == State::Aligning && now - stateAt >= alignDuration) {
    stopDrive();
    state = State::Settling;
    stateAt = millis();
  } else if (state == State::Settling && now - stateAt >= SERVO_SETTLE_MS) {
    state = State::Following;
    misses = 0;
    sampleCount = sampleIndex = 0;
    lastControl = 0;
    announce(F("FOLLOW: gap control enabled"));
  } else if (state == State::Following && now - lastControl >= CONTROL_INTERVAL_MS) {
    // One bounded ping per interval; a rolling median avoids long control-loop
    // delays.
    int cm = singleRange();
    serialControls();
    if (state != State::Following)
      return;
    bool left = digitalRead(LEFT_IR_PIN) == IR_DETECTED_LEVEL;
    bool right = digitalRead(RIGHT_IR_PIN) == IR_DETECTED_LEVEL;
    if (cm <= 0 || cm > TARGET_MAX_CM) {
      stopDrive();
      sampleCount = sampleIndex = 0;
      if (++misses >= LOST_AFTER_MISSES)
        loseTarget();
    } else {
      misses = 0;
      followSamples[sampleIndex] = cm;
      sampleIndex = (sampleIndex + 1) % 3;
      if (sampleCount < 3)
        sampleCount++;
      if (sampleCount < 3 || cm <= FOLLOW_GAP_CM + GAP_DEADBAND_CM)
        stopDrive();
      else {
        int a = followSamples[0], b = followSamples[1], c = followSamples[2];
        if (a > b) {
          int v = a;
          a = b;
          b = v;
        }
        if (b > c) {
          int v = b;
          b = c;
          c = v;
        }
        if (a > b) {
          int v = a;
          a = b;
          b = v;
        }
        follower::Drive output = follower::decide(b, left, right, settings);
        drive(output.left, output.right);
      }
    }
    Serial.print(F("cm="));
    Serial.print(cm);
    Serial.print(F(" left="));
    Serial.print(left);
    Serial.print(F(" right="));
    Serial.println(right);
    lastControl = millis();
  }
}
