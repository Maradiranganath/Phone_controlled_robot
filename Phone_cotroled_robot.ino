#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// ---------- Pins ----------
#define AIN1 0
#define AIN2 1
#define BIN1 3
#define BIN2 4
#define OLED_SDA 8
#define OLED_SCL 9
#define SERVO_PIN 7

// ---------- Settings ----------
#define MOTOR_SPEED 200      // 0-255
#define NECK_CENTER 90
#define NECK_LEFT   130       // swap with NECK_RIGHT if direction is reversed
#define NECK_RIGHT  50
#define SERVO_STEP_MS 8      // lower = faster neck movement

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Servo neck;

// ---------- Eyes ----------
enum Mood { NORMAL, HAPPY, ANGRY, SAD, SURPRISED, MOOD_COUNT };
Mood mood = NORMAL;

float eyeX = 0, eyeY = 0, eyeH = 40;
int targetX = 0, targetY = 0;
unsigned long lastEyeUpdate = 0, nextBlink = 0, blinkStart = 0;
bool blinking = false;

void drawEyes() {
  const int w = 34;
  int h = (int)eyeH;
  if (mood == SURPRISED && !blinking) h += 8;
  int ox = (int)eyeX, oy = (int)eyeY;
  int lx = 18 + ox, rx = 76 + ox;
  int y = 32 - h / 2 + oy;

  display.clearDisplay();
  display.fillRoundRect(lx, y, w, h, 10, WHITE);
  display.fillRoundRect(rx, y, w, h, 10, WHITE);

  if (!blinking) {
    switch (mood) {
      case ANGRY:  // lids slope down toward the centre
        display.fillTriangle(lx, y - 1, lx + w, y - 1, lx + w, y + h / 3, BLACK);
        display.fillTriangle(rx, y - 1, rx + w, y - 1, rx, y + h / 3, BLACK);
        break;
      case SAD:    // lids slope down toward the outside
        display.fillTriangle(lx, y - 1, lx + w, y - 1, lx, y + h / 3, BLACK);
        display.fillTriangle(rx, y - 1, rx + w, y - 1, rx + w, y + h / 3, BLACK);
        break;
      case HAPPY:  // cheeks push up from below
        display.fillCircle(lx + w / 2, y + h + 2, w / 2 + 2, BLACK);
        display.fillCircle(rx + w / 2, y + h + 2, w / 2 + 2, BLACK);
        break;
      default: break;
    }
  }
  display.display();
}

void updateEyes() {
  unsigned long now = millis();
  if (now - lastEyeUpdate < 25) return;
  lastEyeUpdate = now;

  // smooth movement toward target
  eyeX += (targetX - eyeX) * 0.35;
  eyeY += (targetY - eyeY) * 0.35;

  // blinking
  if (!blinking && now > nextBlink) {
    blinking = true;
    blinkStart = now;
  }
  if (blinking) {
    unsigned long t = now - blinkStart;
    if (t < 100) eyeH = 40 - (t * 34 / 100);              // closing
    else if (t < 200) eyeH = 6 + ((t - 100) * 34 / 100);  // opening
    else {
      eyeH = 40;
      blinking = false;
      nextBlink = now + random(2500, 5000);
    }
  }
  drawEyes();
}

void setLook(int x, int y) { targetX = x; targetY = y; }

// ---------- Motors (DRV8833) ----------
void motor(int in1, int in2, int speed) {
  if (speed > 0)      { analogWrite(in1, speed); analogWrite(in2, 0); }
  else if (speed < 0) { analogWrite(in1, 0);     analogWrite(in2, -speed); }
  else                { analogWrite(in1, 0);     analogWrite(in2, 0); }
}

void drive(int left, int right) {
  motor(AIN1, AIN2, left);
  motor(BIN1, BIN2, right);
}

// ---------- Neck ----------
int neckPos = NECK_CENTER;
int neckTarget = NECK_CENTER;
unsigned long lastServo = 0;

void updateNeck() {
  if (millis() - lastServo < SERVO_STEP_MS) return;
  lastServo = millis();
  if (neckPos < neckTarget) neckPos++;
  else if (neckPos > neckTarget) neckPos--;
  neck.write(neckPos);
}

bool neckReached() { return abs(neckPos - neckTarget) <= 2; }

// ---------- Setup ----------
void setup() {
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  drive(0, 0);

  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  neck.setPeriodHertz(50);
  neck.attach(SERVO_PIN, 500, 2400);
  neck.write(NECK_CENTER);

  Dabble.begin("MaradiBot");   // Bluetooth name shown in Dabble app
  nextBlink = millis() + 2000;
}

// ---------- Loop ----------
bool lastTriangle = false;

void loop() {
  Dabble.processInput();

  bool up    = GamePad.isUpPressed();
  bool down  = GamePad.isDownPressed();
  bool left  = GamePad.isLeftPressed();
  bool right = GamePad.isRightPressed();

  bool sq  = GamePad.isSquarePressed();
  bool ci  = GamePad.isCirclePressed();
  bool cr  = GamePad.isCrossPressed();
  bool tri = GamePad.isTrianglePressed();

  static bool wasTurning = false;

  if (left || right) {
    // 1) neck turns first, eyes look that way
    neckTarget = left ? NECK_LEFT : NECK_RIGHT;
    setLook(left ? -12 : 12, 0);
    // 2) body turns only after the head has reached its position
    if (neckReached()) {
      if (left) drive(-MOTOR_SPEED, MOTOR_SPEED);
      else      drive(MOTOR_SPEED, -MOTOR_SPEED);
    } else {
      drive(0, 0);
    }
    wasTurning = true;
  }
  else if (up) {
    neckTarget = NECK_CENTER;
    setLook(0, -6);                 // looking ahead
    drive(MOTOR_SPEED, MOTOR_SPEED);
    wasTurning = false;
  }
  else if (down) {
    neckTarget = NECK_CENTER;
    setLook(0, 6);                  // looking down while reversing
    drive(-MOTOR_SPEED, -MOTOR_SPEED);
    wasTurning = false;
  }
  else {
    drive(0, 0);
    // turn finished -> head comes back, eyes back to centre
    if (wasTurning) {
      neckTarget = NECK_CENTER;
      setLook(0, 0);
      wasTurning = false;
    }

    // manual neck/eye buttons (only when not driving)
    if (sq)      { neckTarget = NECK_LEFT;  setLook(-12, 0); }
    else if (ci) { neckTarget = NECK_RIGHT; setLook(12, 0); }
    else if (cr) { neckTarget = NECK_CENTER; setLook(0, 0); mood = NORMAL; }

    if (tri && !lastTriangle) {     // change mood on each press
      mood = (Mood)((mood + 1) % MOOD_COUNT);
    }
  }
  lastTriangle = tri;

  updateNeck();
  updateEyes();
}