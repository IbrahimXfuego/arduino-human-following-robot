# Wiring and power

## Shield and motors

Fit the L293D/AFMotor-compatible v1 shield to the Uno. It consumes digital pins internally for the shift-register and motor PWM signals; do not assign arbitrary additional digital pins without checking its schematic.

| Output | Assigned wheel |
| --- | --- |
| M1 | Left front |
| M2 | Left rear |
| M3 | Right front |
| M4 | Right rear |

This is a chosen mapping. With wheels lifted, check that positive drive rolls all wheels forward. Use `INVERT_M1` through `INVERT_M4` in `Config.h` or swap that motor's two leads. Do not change the left/right grouping accidentally.

Check each motor's stall current against the actual L293D shield rating. Four-wheel chassis can draw much more when pivoting than when lifted. A battery or voltage label alone does not establish driver compatibility.

## Sensors

| Module | Uno connection |
| --- | --- |
| Ultrasonic VCC | Regulated 5 V |
| Ultrasonic GND | GND |
| Ultrasonic TRIG | A2 |
| Ultrasonic ECHO | A3 |
| Left IR VCC / GND / OUT | 5 V / GND / A5 |
| Right IR VCC / GND / OUT | 5 V / GND / A1 |
| SG90 red | Regulated 5 V servo supply |
| SG90 brown/black | GND |
| SG90 orange/yellow | D10 servo signal header |

Use an HC-SR04-compatible ultrasonic module. Both IR modules use digital outputs; default detection is LOW. Check by observing the module indicator and Serial Monitor while bringing a hand into view. Change `IR_DETECTED_LEVEL` if opposite.

Read the servo header's **signal / +5V / GND** markings. Do not assume its board position identifies D10; clone labels vary. The SG90 sensor bracket must centre forward at `SERVO_CENTRE`. If increasing servo angles points right instead of left, set `HIGH_ANGLE_POINTS_LEFT` false.

## Motor power

Four AA cells in a switched holder feed the motor shield’s external-power terminals.

Battery positive → switch → shield **EXT_PWR +**.
Battery negative → shield **EXT_PWR GND**.

Do not connect the pack directly to the regulated 5V rail. Four AA alkaline cells are nominally 6 V; four AA NiMH cells nominally 4.8 V. Voltage under motor/servo load may be significantly lower.

## Recommended reproduction power arrangement

1. Connect the switched AA pack only to EXT_PWR for the DC motors.
2. **Remove the shield's PWR jumper.** Verify its function on the actual clone.
3. Supply the Uno through USB from a regulated 5 V source; USB power also feeds shield logic and its standard small-servo header on the original v1 design.
4. The fitted shield shares ground with the Uno. All sensors and servo need that same reference.
5. Check 5 V stability while sweeping the SG90. If a separate servo regulator is needed, isolate the servo positive from the shield's 5 V trace/header before powering it independently, and retain common ground.

The split-supply arrangement above keeps motor and logic power paths separate. For one-pack operation, use a properly designed regulated logic supply and avoid USB back-feeding; confirm the schematic first. Do not rely on a four-AA pack feeding Uno VIN through the shield as a guaranteed stable 5 V source.

For uploading, keep motor power switched off and wheels lifted. Never hot-swap motor wiring. Motor `RELEASE` stops drive but is not an active mechanical brake; the robot can coast.

References:
- https://learn.adafruit.com/adafruit-motor-shield/power-requirements
- https://learn.adafruit.com/adafruit-motor-shield/using-dc-motors
