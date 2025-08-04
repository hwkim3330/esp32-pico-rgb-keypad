#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <BleKeyboard.h>

// ── 핀 설정 ────────────────────────────────
#define LED_PIN  23       // WS2812 DIN  (Keypad GP0)
#define NUM_LED  16
#define SDA_PIN  21       // Keypad GP4
#define SCL_PIN  22       // Keypad GP5

// ── TCA9555 레지스터 ─────────────────────
#define TCA_ADDR     0x20
#define REG_IN0      0x00
#define REG_IN1      0x01
#define REG_CFG0     0x06
#define REG_CFG1     0x07

Adafruit_NeoPixel leds(NUM_LED, LED_PIN, NEO_GRB + NEO_KHZ800);
BleKeyboard        kb("HexPad-ESP32", "KETI", 100);

uint16_t prevState = 0xFFFF;          // 1=released, 0=pressed
const char keyChar[16] = {
  '0','1','2','3','4','5','6','7',
  '8','9','A','B','C','D','E','F'
};

// ── I2C 헬퍼 ──────────────────────────────
inline void tcaWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(reg); Wire.write(val); Wire.endTransmission();
}
inline uint8_t tcaReadReg(uint8_t reg) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(reg); Wire.endTransmission(false);   // restart
  Wire.requestFrom(TCA_ADDR, (uint8_t)1);         // 두 인수만!
  return Wire.read();
}
inline uint16_t readKeys() {
  return (uint16_t)tcaReadReg(REG_IN1) << 8 | tcaReadReg(REG_IN0);
}

// ── SETUP ────────────────────────────────
void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  tcaWrite(REG_CFG0, 0xFF);           // 모두 입력
  tcaWrite(REG_CFG1, 0xFF);

  leds.begin();
  leds.setBrightness(60);             // USB 전류 제한
  leds.show();

  kb.begin();
  Serial.println("HexPad WS2812 ready");
}

// ── LOOP ─────────────────────────────────
void loop() {
  uint16_t now  = readKeys();
  uint16_t diff = now ^ prevState;
  if (diff) { delay(8); now = readKeys(); diff = now ^ prevState; }

  if (diff) {
    for (uint8_t i = 0; i < 16; ++i) {
      uint16_t m = 1 << i;
      if (diff & m) {
        bool down = !(now & m);               // 0 = pressed
        leds.setPixelColor(i, down ? leds.Color(0,180,40) : 0);
        if (kb.isConnected()) {
          if (down) kb.press(keyChar[i]);
          else      kb.release(keyChar[i]);
        }
      }
    }
    leds.show();
    prevState = now;
  }
  delay(1);                                   // WDT
}
