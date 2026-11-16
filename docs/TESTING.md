# Checks and calibration

## Control-logic checks

```sh
g++ -std=c++11 -Wall -Wextra -Werror tests/control_test.cpp -o control_test
./control_test
```

Covers invalid/far range stops, stop-band behaviour, PWM limits and left/right steering.

## Arduino and chassis setup

1. Use Arduino IDE Verify, then upload with the Uno and v1 AFMotor library selected.
2. Lift wheels and verify all four motor directions.
3. Check SG90 centre, angle direction and mechanical travel.
4. Compare ultrasonic readings with measured distances.
5. Check the stop band and immediate stop on missing range.
6. Check IR steering polarity and X/R serial controls.
7. Tune the brief pivot for the chassis, then check front-target reacquisition.
8. Check supply stability while the motors and servo operate.
