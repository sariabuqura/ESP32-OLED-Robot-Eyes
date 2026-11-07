#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Face frame -------------------------------------------------------------
const int FRAME_RADIUS = 14;

// --- Eye geometry -----------------------------------------------------------
const int EYE_Y   = 20;
const int EYE_W   = 23;
const int EYE_H   = 26;
const int L_EYE_X = 30;
const int R_EYE_X = 75;
const int RADIUS  = 6;

// Animated state
float curOffX = 0, curOffY = 0;
float tgtOffX = 0, tgtOffY = 0;
float curSquishL = 1.0, curSquishR = 1.0;
float tgtSquishL = 1.0, tgtSquishR = 1.0;
float curBrow = 0, tgtBrow = 0;
float curSad  = 0, tgtSad  = 0;

bool heartEyes = false;

// --- Fixed rounded border ---------------------------------------------------
void drawBorder() {
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, FRAME_RADIUS, SH110X_WHITE);
}

// --- Eye shape (plain, no pupil) --------------------------------------------
void drawEye(int x, int y, int w, int h, float squish) {
  int sh = max(2, (int)(h * squish));
  int sy = y + (h - sh) / 2;
  display.fillRoundRect(x, sy, w, sh, RADIUS, SH110X_WHITE);
}

void drawHeart(int cx, int cy, int size) {
  int r = size / 4;
  int lx = cx - r;
  int rx = cx + r;
  int ly = cy - r / 2;
  display.fillCircle(lx, ly, r, SH110X_WHITE);
  display.fillCircle(rx, ly, r, SH110X_WHITE);
  display.fillTriangle(
    lx - r, ly,
    rx + r, ly,
    cx,     cy + size / 2,
    SH110X_WHITE
  );
}

// --- Brows ------------------------------------------------------------------
void drawAngryBrows(float amount) {
  if (amount <= 0.02) return;
  int thickness = 3;
  int drop = (int)(8 * amount);
  int topY = EYE_Y - 6;

  int lx1 = L_EYE_X - 2,          ly1 = topY;
  int lx2 = L_EYE_X + EYE_W + 2,  ly2 = topY + drop;
  for (int t = 0; t < thickness; t++)
    display.drawLine(lx1, ly1 + t, lx2, ly2 + t, SH110X_WHITE);

  int rx1 = R_EYE_X - 2,          ry1 = topY + drop;
  int rx2 = R_EYE_X + EYE_W + 2,  ry2 = topY;
  for (int t = 0; t < thickness; t++)
    display.drawLine(rx1, ry1 + t, rx2, ry2 + t, SH110X_WHITE);
}

void drawSadBrow(int ex, int ey, int ew, int eh, bool leftEye, float amount) {
  if (amount <= 0.02) return;
  int cut = (int)(eh * 0.5 * amount);
  int x1, y1, x2, y2, x3, y3;
  if (leftEye) {
    x1 = ex;      y1 = ey;
    x2 = ex + ew; y2 = ey;
    x3 = ex;      y3 = ey + cut;
  } else {
    x1 = ex + ew; y1 = ey;
    x2 = ex;      y2 = ey;
    x3 = ex + ew; y3 = ey + cut;
  }
  display.fillTriangle(x1, y1, x2, y2, x3, y3, SH110X_BLACK);
}

// --- Compose the full face --------------------------------------------------
void drawFace() {
  display.clearDisplay();
  drawBorder();

  if (heartEyes) {
    drawHeart(L_EYE_X + EYE_W / 2 + curOffX, EYE_Y + EYE_H / 2 + curOffY, EYE_H);
    drawHeart(R_EYE_X + EYE_W / 2 + curOffX, EYE_Y + EYE_H / 2 + curOffY, EYE_H);
  } else {
    drawEye(L_EYE_X + curOffX, EYE_Y + curOffY, EYE_W, EYE_H, curSquishL);
    drawEye(R_EYE_X + curOffX, EYE_Y + curOffY, EYE_W, EYE_H, curSquishR);
    drawSadBrow(L_EYE_X + curOffX, EYE_Y + curOffY, EYE_W, EYE_H, true,  curSad);
    drawSadBrow(R_EYE_X + curOffX, EYE_Y + curOffY, EYE_W, EYE_H, false, curSad);
  }

  drawAngryBrows(curBrow);
  display.display();
}

// --- Animation helper -------------------------------------------------------
void glide(int steps = 8, int frameDelay = 15) {
  for (int i = 0; i < steps; i++) {
    curOffX    += (tgtOffX    - curOffX)    * 0.3;
    curOffY    += (tgtOffY    - curOffY)    * 0.3;
    curSquishL += (tgtSquishL - curSquishL) * 0.4;
    curSquishR += (tgtSquishR - curSquishR) * 0.4;
    curBrow    += (tgtBrow    - curBrow)    * 0.35;
    curSad     += (tgtSad     - curSad)     * 0.35;
    drawFace();
    delay(frameDelay);
  }
}

// --- Expressions ------------------------------------------------------------
void neutral() {
  heartEyes = false;
  tgtSquishL = tgtSquishR = 1.0;
  tgtBrow = 0; tgtSad = 0;
  tgtOffX = 0; tgtOffY = 0;
  glide(8, 15);
}

void blink() {
  tgtSquishL = tgtSquishR = 0.08; glide(5, 10);
  tgtSquishL = tgtSquishR = 1.0;  glide(5, 10);
}

void doubleBlink() { blink(); delay(80); blink(); }

void wink(bool rightEye) {
  if (rightEye) tgtSquishR = 0.08; else tgtSquishL = 0.08;
  glide(5, 12); delay(250);
  tgtSquishL = tgtSquishR = 1.0;
  glide(5, 12);
}

void squint() {
  tgtSquishL = tgtSquishR = 0.45; glide(10, 20);
  delay(600); neutral();
}

void surprised() {
  tgtSquishL = tgtSquishR = 1.25; glide(6, 15);
  delay(500); neutral();
}

void sleepy() {
  tgtSquishL = tgtSquishR = 0.3; glide(15, 30);
  delay(900);
  tgtOffY = 4; glide(8, 30);
  tgtOffY = 0; glide(8, 30);
  neutral();
}

void lookAround() {
  int positions[][2] = {{-6, 0}, {6, 0}, {0, -4}, {0, 4}, {-5, -3}, {5, 3}};
  for (auto &p : positions) {
    tgtOffX = p[0]; tgtOffY = p[1];
    glide(6, 20); delay(200);
  }
  tgtOffX = 0; tgtOffY = 0;
  glide(8, 20);
}

void shifty() {
  tgtOffX = -6; glide(4, 12); delay(140);
  tgtOffX =  6; glide(4, 12); delay(140);
  tgtOffX =  0; glide(4, 12);
  neutral();
}

void crazyDart() {
  for (int i = 0; i < 6; i++) {
    tgtOffX = random(-7, 8);
    tgtOffY = random(-4, 5);
    glide(3, 8);
  }
  tgtOffX = 0; tgtOffY = 0;
  glide(5, 10); neutral();
}

void angry() {
  tgtBrow = 1.0;
  tgtSquishL = tgtSquishR = 0.75;
  glide(10, 20); delay(900);
  for (int i = 0; i < 3; i++) {
    tgtOffX = -2; glide(2, 15);
    tgtOffX =  2; glide(2, 15);
  }
  tgtOffX = 0; glide(4, 15);
  delay(400); neutral();
}

void furious() {
  tgtBrow = 1.0;
  tgtSquishL = tgtSquishR = 0.55;
  glide(8, 18);
  for (int i = 0; i < 8; i++) {
    tgtOffX = random(-4, 5);
    tgtOffY = random(-2, 3);
    glide(2, 10);
  }
  tgtOffX = tgtOffY = 0;
  glide(4, 15); delay(300); neutral();
}

void sad() {
  tgtSad = 1.0;
  tgtSquishL = tgtSquishR = 0.75;
  tgtOffY = 3;
  glide(14, 30); delay(1200);
  tgtSquishL = tgtSquishR = 0.1;  glide(6, 25);
  tgtSquishL = tgtSquishR = 0.75; glide(6, 25);
  delay(500); neutral();
}

void love() {
  heartEyes = true;
  for (int i = 0; i < 4; i++) {
    tgtOffY = -2; glide(3, 20);
    tgtOffY =  1; glide(3, 20);
  }
  tgtOffY = 0;
  glide(4, 20); delay(400); neutral();
}

void dizzy() {
  for (int i = 0; i < 24; i++) {
    float a = i * 0.5;
    tgtOffX = cos(a) * 6;
    tgtOffY = sin(a) * 3;
    glide(2, 15);
  }
  tgtOffX = tgtOffY = 0;
  glide(5, 20); neutral();
}

void confused() {
  tgtSquishL = 1.2; tgtSquishR = 0.6; glide(8, 20); delay(700);
  tgtSquishL = 0.6; tgtSquishR = 1.2; glide(8, 20); delay(700);
  neutral();
}

void laugh() {
  for (int i = 0; i < 4; i++) {
    tgtSquishL = tgtSquishR = 0.25; tgtOffY = -2; glide(3, 15);
    tgtSquishL = tgtSquishR = 0.45; tgtOffY =  2; glide(3, 15);
  }
  neutral();
}

// --- Setup / loop -----------------------------------------------------------
void setup() {
  Serial.begin(115200);
  if (!display.begin(0x3C, true)) {
    Serial.println(F("SH110X allocation failed"));
    for (;;);
  }
  randomSeed(analogRead(0));
  display.clearDisplay();
  display.display();
  drawFace();
  delay(500);
}

void loop() {
  int mood = random(0, 100);

  if      (mood < 15) blink();
  else if (mood < 22) doubleBlink();
  else if (mood < 30) wink(random(0, 2));
  else if (mood < 36) squint();
  else if (mood < 42) surprised();
  else if (mood < 48) sleepy();
  else if (mood < 56) lookAround();
  else if (mood < 62) shifty();
  else if (mood < 68) crazyDart();
  else if (mood < 76) angry();
  else if (mood < 80) furious();
  else if (mood < 86) sad();
  else if (mood < 90) love();
  else if (mood < 94) dizzy();
  else if (mood < 97) confused();
  else                laugh();

  tgtOffX = random(-2, 3);
  tgtOffY = random(-1, 2);
  glide(4, 25);
  delay(random(400, 1000));
}