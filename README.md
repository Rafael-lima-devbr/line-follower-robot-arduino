# Line Follower Robot with PD Control

Line follower robot using 5 analog IR sensors and a PD (Proportional + Derivative) controller for real-time trajectory correction. The system includes automatic calibration, sensor normalization, line position calculation, and logic for sharp curves and stopping conditions.

**Status:** Experimental robotics project

<img width="1264" height="948" alt="image" src="https://github.com/user-attachments/assets/889902a1-e428-4598-9a29-89738003815b" />

<img width="1264" height="948" alt="image" src="https://github.com/user-attachments/assets/ec44f46a-6178-44b0-8706-ebb24cbca951" />

<img width="720" height="404" alt="line-follower-demo-horizontal-fixed" src="https://github.com/user-attachments/assets/48d61bf6-a217-47c8-99e7-691b84fe5141" />


---

## How It Works

**1. Calibration**  
Runs for ~5 seconds on startup. Captures the minimum and maximum values of each sensor to normalize readings across different environments.

**2. Sensor Normalization**  
Raw sensor values are mapped to a 0–100 scale, ensuring more consistent behavior across lighting and surface variations.

**3. Line Position Calculation**  
Weighted average using weights `[-2, -1, 0, 1, 2]`. If the line is lost, the last valid position is used instead of resetting to zero.

**4. PD Control**

```text
error = setpoint (0) - position
output = kp * error + kd * derivative
```

Derivative is calculated using `dt = (now - lasttime) / 1000.0`.

**5. Motor Control**  
Base speed is adjusted by the PD output. A minimum power threshold prevents dead zones. Supports forward and reverse direction.

---

## Special Logic

| Condition | Behavior |
|---|---|
| Strong left sensors activated | Sharp left curve |
| Strong right sensors activated | Sharp right curve |
| All sensors detect line | Wait briefly; if center sensor is lost → full stop |

---

## Hardware

- Arduino (Uno or Nano)
- 5 analog IR sensors
- Motor driver (e.g. L298N)
- 2 DC motors

---

## Parameters

| Parameter | Value |
|---|---|
| `kp` | 130 |
| `kd` | 2 |
| `base_speed` | 170 |
| Sensor weights | `[-2, -1, 0, 1, 2]` |
| Motor output range | -255 to 255 |
| Minimum power threshold | ±120 |

---

## Program Flow

```text
setup()
├── initialization
├── pin configuration
└── calibration

loop()
└── control()
    ├── digital sensor reading
    ├── special curve/stop logic
    └── PD calculation + motor output
```

---

## License

MIT License
