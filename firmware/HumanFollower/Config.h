#pragma once
// Code-matched wiring based on the inspiration project's shield arrangement.
constexpr uint8_t LEFT_IR_PIN = A5;
constexpr uint8_t RIGHT_IR_PIN = A1;
constexpr uint8_t SONAR_TRIG_PIN = A2;
constexpr uint8_t SONAR_ECHO_PIN = A3;
constexpr uint8_t SERVO_PIN = 10;
constexpr uint8_t IR_DETECTED_LEVEL = LOW;

// Confirmed SG90 servo. Calibrate centre and travel before attaching the sensor.
constexpr int SERVO_CENTRE = 90;
constexpr int SCAN_MIN = 0;
constexpr int SCAN_MAX = 180;
constexpr int SCAN_STEP = 15;
constexpr unsigned long SERVO_SETTLE_MS = 180;
constexpr unsigned long SONAR_TIMEOUT_US = 10000;
constexpr unsigned long CONTROL_INTERVAL_MS = 60;
constexpr uint8_t LOST_AFTER_MISSES = 3;

// Change these to suit the chassis. Tune for the chassis.
constexpr int FOLLOW_GAP_CM = 25;
constexpr int GAP_DEADBAND_CM = 5;
constexpr int TARGET_MAX_CM = 100;
constexpr int ACQUIRE_MIN_CM = 8;
constexpr int MIN_DRIVE_PWM = 105;
constexpr int MAX_DRIVE_PWM = 165;
constexpr int GAP_GAIN_PWM_PER_CM = 3;
constexpr int ALIGN_PWM = 100;
constexpr int ALIGN_MS_PER_DEGREE = 4;
constexpr unsigned long ALIGN_MAX_MS = 350;

// M1/M2 = left wheels; M3/M4 = right wheels in this generated mapping.
// Flip only the motors whose physical forward direction is reversed.
constexpr bool INVERT_M1 = false;
constexpr bool INVERT_M2 = false;
constexpr bool INVERT_M3 = false;
constexpr bool INVERT_M4 = false;
// A normal front-facing pan bracket points LEFT at higher angles. Reverse if
// yours differs.
constexpr bool HIGH_ANGLE_POINTS_LEFT = true;
