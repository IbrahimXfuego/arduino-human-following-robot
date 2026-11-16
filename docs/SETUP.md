# Arduino IDE setup and calibration

## Install

1. Install Arduino IDE 2.x.
2. Use Boards Manager to install **Arduino AVR Boards** if absent; select **Arduino Uno** under Tools → Board.
3. In Library Manager, install the original **Adafruit Motor Shield library** with `AFMotor.h` for **v1 L293D shields**. If it is not listed, download the official repository at https://github.com/adafruit/Adafruit-Motor-Shield-library and use Sketch → Include Library → Add .ZIP Library.
4. Install **Servo by Arduino** if the IDE requests it. No NewPing library is required; this version uses bounded `pulseIn()` measurements.
5. Open `firmware/HumanFollower/HumanFollower.ino`, with its two headers in the same folder.
6. Select the Uno's USB port, click Verify, then Upload.

## First run — wheels lifted

The robot starts scanning automatically after reset and may move after acquisition. Begin with wheels clear of the floor and motor power off.

1. Check [WIRING.md](WIRING.md), including the power jumper and motor battery polarity.
2. Open Serial Monitor at 115200 baud. Send `X` to latch a halt.
3. With the servo horn unloaded, confirm centre and travel. If 0°/180° strains the bracket or servo, narrow `SCAN_MIN` / `SCAN_MAX`; nominal servo angle is not guaranteed mechanical travel.
4. Attach the ultrasonic sensor centred forward at 90° or update `SERVO_CENTRE`.
5. Switch on motor power while wheels remain lifted. Send `R` with a target in view.
6. Check M1/M2 move the left side and M3/M4 the right. Correct `INVERT_M*` flags if individual wheels run backward.
7. If alignment turns away from the detected direction, check motor polarity and `HIGH_ANGLE_POINTS_LEFT`.
8. Confirm ultrasonic distances against a ruler and IR polarity against the module indicator LEDs.

## Floor calibration

Use an open area, clear the wheels, and keep access to the motor switch. Start with low PWM settings.

- `FOLLOW_GAP_CM = 25`: chosen gap default, not a recovered original measurement.
- `GAP_DEADBAND_CM = 5`: stop at or below 30 cm with the default target. The controller approaches from further away but does not reverse to correct a too-small gap.
- `TARGET_MAX_CM = 100`: readings beyond this are treated as a lost target.
- `MIN_DRIVE_PWM` / `MAX_DRIVE_PWM`: enough torque to roll, low enough for controlled behaviour.
- `ALIGN_MS_PER_DEGREE` / `ALIGN_MAX_MS`: brief pivot duration. There are no encoders, so turns need empirical tuning. If the candidate is not in front after alignment, the robot stops instead of continuing blindly.
- IR modules: adjust thresholds and aim them slightly to either side of the centre. Short-range reflective modules may provide steering cues only at close distances.

Move a hand or stand in front, then move slowly. A valid target should cause forward approach when too far away and stop near the gap. Side IR detection biases wheel speeds. Remove the target: the first invalid range stops drive; three consecutive misses latch LOST.

`R` starts a fresh stationary scan; `X` or space halts; reset restarts the automatic sequence. Lost mode remains stopped until reset or `R`.

## Timing and filtering

During the stationary scan, each angle uses a median of three pings separated by 55 ms after the servo settles. Following takes a bounded single ping about every 60 ms and uses a rolling three-sample median. Any invalid/out-of-range raw reading stops the motors and clears the rolling history. Three valid samples are needed before drive resumes. Sonar pulse wait is capped at 10 ms.

Serial stop processing continues between operations, but the three-ping scan measurement can delay it by around 140 ms in the worst case. The physical motor switch remains the immediate way to remove drive power.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| AFMotor.h missing | Original v1 library installed; v2 library is different |
| Motors stay still | Motor battery, switch, EXT_PWR polarity, correct outputs and sufficient PWM |
| Uno resets on movement | Battery sag, logic supply, jumper routing, servo current/noise |
| Robot turns opposite | Wheel grouping, motor inversion and servo angle direction |
| It follows a chair/wall | Expected limitation: sensors detect surfaces, not identity |
| Repeated LOST | Sonar alignment, target range, reflective surface angle, power and wiring |
| Servo chatters/stalls | Regulated 5 V, mechanical travel, sensor weight and supply headroom |
| Robot never reverses | Intentional: no rear sensing; too-close readings stop drive |
