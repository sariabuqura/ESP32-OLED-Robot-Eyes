# ESP32 OLED Robot Eyes 👀

An ESP32 drives a 1.3" SH1106 OLED display to show a cute animated face. The eyes blink, wink, look around, and randomly switch between emotions such as angry, sad, sleepy, in love, dizzy, and more.

![Final result on the breadboard](images/wiring-photo.jpg)

---

## Features

- Smooth animated eyes (easing/interpolation between states)
- 16 expressions chosen at random: blink, double blink, wink, squint, surprised, sleepy, look around, shifty, crazy dart, angry, furious, sad, love (heart eyes), dizzy, confused, laugh
- Rounded face frame drawn around the screen
- No images or bitmaps: everything is drawn with simple shapes, so the code is small and easy to modify

---

## Hardware

| Part | Notes |
|------|-------|
| ESP32 DevKit (30/38-pin, ESP32-WROOM-32) | Any ESP32 dev board works |
| 1.3" OLED 128x64, **SH1106**, I2C (4 pins: VCC, GND, SCK, SDA) | Usually blue/white; I2C address `0x3C` |
| Breadboard | |
| 4 jumper wires (male-to-male) | |
| Micro-USB cable | Power and programming |

> ⚠️ Check your display chip. Many 1.3" modules use the **SH1106**, while 0.96" modules usually use the **SSD1306**. This code is written for the SH1106 (see [Troubleshooting](#troubleshooting)).

---

## Wiring

The display uses I2C. The ESP32's default I2C pins are used, so no pin configuration is needed in the code.

| OLED Pin | ESP32 Pin | Note |
|----------|-----------|------|
| **VCC** | **3V3** | Use 3.3 V |
| **GND** | **GND** | |
| **SCK** (SCL) | **GPIO 22** (D22) | I2C clock |
| **SDA** | **GPIO 21** (D21) | I2C data |

```
   ESP32                    OLED (SH1106)
 ┌─────────┐              ┌──────────────┐
 │     3V3 ├──────────────┤ VCC          │
 │     GND ├──────────────┤ GND          │
 │ D22/SCL ├──────────────┤ SCK (SCL)    │
 │ D21/SDA ├──────────────┤ SDA          │
 └─────────┘              └──────────────┘
```

On the breadboard photo, the **blue** and **white** wires carry the I2C signals (SDA/SCL) from the ESP32's bottom header (D21 and D22) to the display. Power (3V3) and GND complete the four connections.

![Wiring on the breadboard](images/wiring-photo.jpg)

---

## Software Setup

### 1. Install the Arduino IDE
Download it from <https://www.arduino.cc/en/software>.

### 2. Add ESP32 board support
1. Open **File → Preferences**.
2. In *Additional boards manager URLs*, add:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. Open **Tools → Board → Boards Manager**, search for **esp32** (by Espressif Systems) and install it.

### 3. Install the libraries
Open **Sketch → Include Library → Manage Libraries** and install:

- **Adafruit GFX Library**
- **Adafruit SH110X**
- (**Adafruit BusIO** is installed automatically as a dependency)

### 4. Select the board and port
- **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
- **Tools → Port →** your ESP32's COM/serial port

### 5. Upload
1. Open `esp32_oled_eyes/esp32_oled_eyes.ino`.
2. Click **Upload**.
3. If it shows `Connecting......`, hold the **BOOT** button on the ESP32 until upload starts.

---

## How It Works

### Drawing the face
- `drawBorder()` draws a rounded rectangle around the screen.
- `drawEye()` draws each eye as a filled rounded rectangle. A `squish` value (0.08 to 1.25) changes its height, which gives blinking, squinting, and wide-eyed looks.
- `drawHeart()` draws heart-shaped eyes from two circles and a triangle.
- `drawAngryBrows()` draws slanted brow lines; `drawSadBrow()` cuts a black triangle from the top corner of each eye.

### Animation
Every property has a **current** value (`cur...`) and a **target** value (`tgt...`). The `glide()` function moves the current values a fraction of the way toward the targets each frame, so the motion is smooth instead of jumping.

```cpp
curOffX += (tgtOffX - curOffX) * 0.3;
```

To create an expression, set targets and call `glide(steps, frameDelay)`.

---

## Customization

| Want to change... | Edit |
|-------------------|------|
| Eye size | `EYE_W`, `EYE_H`, `RADIUS` |
| Eye position / spacing | `EYE_Y`, `L_EYE_X`, `R_EYE_X` |
| Frame roundness | `FRAME_RADIUS` |
| How often an expression appears | The `if / else if` ranges in `loop()` |
| Animation speed | `glide(steps, frameDelay)` values |
| Trigger an expression yourself | Call it directly, e.g. `love();` |

---

## Troubleshooting

| Problem | Fix |
|---------|-----|
| Blank screen | Check wiring (SDA to D21, SCL to D22, 3V3, GND). Try address `0x3D` in `display.begin(0x3C, true)`. |
| Serial prints `SH110X allocation failed` | The display isn't detected. Recheck the wires and the I2C address. |
| Image shifted, noisy edge, or garbage pixels | Your display may be an **SSD1306**. Use `Adafruit_SSD1306` instead of `Adafruit_SH110X`. |
| Upload fails | Hold **BOOT** while uploading, try another USB cable (some are charge-only), and check the port. |
| Screen flickers | Use shorter wires and make sure the breadboard connections are firm. |

To find the I2C address, upload any "I2C scanner" sketch and read the result in the Serial Monitor.

---

## Repository Structure

```
.
├── README.md
├── esp32_oled_eyes/
│   └── esp32_oled_eyes.ino
└── images/
    └── wiring-photo.jpg
```

---

## License

MIT. Feel free to use, modify, and share.
