# Phone Controlled DIY Robot Car With a Moving Head and Animated Eyes

An ESP32-C3 robot car controlled from your phone with the Dabble Bluetooth gamepad. The robot has a head made from an OLED display mounted on a servo (the neck). The OLED shows animated eyes. Before the car turns, the neck turns first, then the car follows. When the turn stops, the neck returns to the centre. No 3D printing is needed.

## Video

Watch the full build on YouTube: https://youtu.be/3HLcX7RYXNU

## Features

- Phone control using the Dabble gamepad over Bluetooth
- Forward, backward, left and right driving
- Neck turns first, then the car turns
- Neck returns to centre when the turn stops
- Animated eyes with blinking and looking left, right, up and down
- Five eye moods: normal, happy, angry, sad, surprised
- Extra buttons to control the neck and the eyes manually

## Components List

| No. | Component | Quantity |
|---|---|---|
| 1 | ESP32-C3 development board | 1 |
| 2 | DRV8833 motor driver module | 1 |
| 3 | N20 gear motors with wheels | 4 |
| 4 | SSD1306 0.96" I2C OLED display | 1 |
| 5 | Servo motor (SG90 or similar) | 1 |
| 6 | Battery (3.7V to 6V) with switch | 1 |
| 7 | Chassis, jumper wires | as needed |

## Pin Connections

### ESP32-C3 to DRV8833 (motor driver)

| DRV8833 pin | ESP32-C3 pin | Purpose |
|---|---|---|
| AIN1 | GPIO 0 | Left motors |
| AIN2 | GPIO 1 | Left motors |
| BIN1 | GPIO 3 | Right motors |
| BIN2 | GPIO 4 | Right motors |
| SLP | 3.3V | Keeps the driver awake |
| VM | Battery + | Motor power (3.7V to 6V) |
| GND | Common GND | Ground |

### Motors to DRV8833

| Motors | DRV8833 output |
|---|---|
| Two left N20 motors (in parallel) | OUT1 / OUT2 |
| Two right N20 motors (in parallel) | OUT3 / OUT4 |

### ESP32-C3 to OLED (SSD1306)

| OLED pin | ESP32-C3 pin |
|---|---|
| SDA | GPIO 8 |
| SCL | GPIO 9 |
| VCC | 3.3V |
| GND | GND |

### ESP32-C3 to Servo

| Servo wire | Connection |
|---|---|
| Signal (orange/yellow) | GPIO 7 |
| Power (red) | Battery or 5V (not the ESP32 3.3V pin) |
| Ground (brown/black) | Common GND |

### Notes

- Connect all grounds together (battery, ESP32-C3, DRV8833, servo, OLED).
- If a motor spins the wrong way, swap its two wires.
- If the neck turns the wrong way, swap the `NECK_LEFT` and `NECK_RIGHT` values in the code.
- GPIO 8 and 9 are boot pins on the ESP32-C3. If upload or boot fails, unplug the OLED, upload, then plug it back in.

## Mounting

The OLED is fixed on the servo horn so it works as the head. The servo is fixed on the front of the chassis and works as the neck.

## Software Setup

1. Install the Arduino IDE.
2. Install the ESP32 board package, **version 3.x** (the code uses `analogWrite`).
3. Install these libraries from the Library Manager:
   - DabbleESP32
   - Adafruit SSD1306
   - Adafruit GFX
   - ESP32Servo
4. Select the ESP32-C3 board, open the code file and upload it.

## Phone Setup

1. Install the **Dabble** app on your phone.
2. Turn on the robot and open the Dabble app.
3. Connect to the Bluetooth device named **MaradiBot**.
4. Open the **Gamepad** module.

## Controls

| Button | Action |
|---|---|
| Up | Move forward |
| Down | Move backward |
| Left | Neck turns left, then car turns left |
| Right | Neck turns right, then car turns right |
| Square | Neck looks left |
| Circle | Neck looks right |
| Cross | Neck to centre and normal eyes |
| Triangle | Change eye mood |

## Settings in the Code

| Setting | Use |
|---|---|
| `MOTOR_SPEED` | Car speed (0 to 255) |
| `NECK_CENTER`, `NECK_LEFT`, `NECK_RIGHT` | Neck angles |
| `SERVO_STEP_MS` | Neck speed (lower is faster) |

## Author

Maradi Innovations
YouTube: https://youtu.be/3HLcX7RYXNU
GitHub: https://github.com/Maradiranganath

If you like this project, please like the video, subscribe to the channel and leave a comment.
