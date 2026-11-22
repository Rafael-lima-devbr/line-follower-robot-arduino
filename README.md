# Line Follower Robot with PD Control

Arduino line-follower robotics project using 5 analog IR sensors and PD (Proportional + Derivative) control for real-time trajectory correction. The repository also contains specialized robot variants built from the same line-following base.

**Status:** Experimental robotics project

## Repository Structure

```text
line-follower-robot-arduino/
├── line_follower.ino
├── variants/
│   └── mnr_2026/
│       └── mnr_2026.ino
├── LICENSE
└── README.md
```

- `line_follower.ino` — original line-following implementation used across multiple robot builds.
- `variants/mnr_2026/mnr_2026.ino` — MNR 2026 version combining line following with environmental sensing and alert logic.

## Prototype

This control code was tested and used across multiple line-follower robot builds. The two photos below show different robots that used the same code base during development and testing.

<p align="center">
  <img width="47%" alt="Line-follower robot build using the PD control code" src="https://github.com/user-attachments/assets/889902a1-e428-4598-9a29-89738003815b" />
  <img width="47%" alt="Another line-follower robot build using the same control code" src="https://github.com/user-attachments/assets/ec44f46a-6178-44b0-8706-ebb24cbca951" />
</p>

## Demo

<p align="center">
  <img width="85%" alt="Line-follower robot running on the track" src="https://github.com/user-attachments/assets/48d61bf6-a217-47c8-99e7-691b84fe5141" />
</p>

<p align="center"><sub>Short test run demonstrating the control code in operation on the track.</sub></p>

---

## Base Line-Following System

**1. Calibration**  
Runs for ~5 seconds on startup. Captures the minimum and maximum values of each sensor to normalize readings across different environments.

**2. Sensor Normalization**  
Raw sensor values are mapped to a 0–100 scale, ensuring more consistent behavior across lighting and surface variations.

**3. Line Position Calculation**  
Uses a weighted average with weights `[-2, -1, 0, 1, 2]` to estimate the line position.

**4. PD Control**

```text
error = setpoint (0) - position
output = kp * error + kd * derivative
```

**5. Motor Control**  
Base speed is adjusted by the controller output, including support for forward and reverse motor direction and minimum-power handling.

---

## MNR 2026 Variant

The MNR 2026 implementation extends the line-following robot with environmental sensing and alert behavior while keeping the same sensor-based navigation approach.

It currently integrates:

- 5 analog sensors for line following;
- PD-based trajectory correction;
- ultrasonic distance sensing;
- analog temperature sensing;
- gas sensing;
- green, yellow and red status LEDs;
- buzzer alerts;
- normal, warning and critical operating states;
- motor stopping while a critical close obstacle remains detected.

The MNR-specific source code is available at:

`variants/mnr_2026/mnr_2026.ino`

---

## Special Line-Following Logic

| Condition | Behavior |
|---|---|
| Strong left sensors activated | Sharp left curve |
| Strong right sensors activated | Sharp right curve |
| All sensors detect line | Timed verification followed by a stop condition |
| Line not detected | Continue using the gap-handling behavior |

---

## Hardware

### Base robot

- Arduino-compatible board
- 5 analog IR sensors
- motor driver
- 2 DC motors

### MNR 2026 additions

- ultrasonic distance sensor
- analog temperature sensor
- gas sensor
- green, yellow and red LEDs
- buzzer

---

## Parameters

The control parameters vary between robot versions and should be tuned for the specific hardware, track and sensor calibration.

The MNR 2026 variant currently uses:

| Parameter | Value |
|---|---|
| `kp` | 130 |
| `kd` | 2 |
| `velocidade_base` | 140 |
| Sensor weights | `[-2, -1, 0, 1, 2]` |
| Motor output range | -255 to 255 |
| Minimum motor threshold | ±100 |

---

## License

MIT License
