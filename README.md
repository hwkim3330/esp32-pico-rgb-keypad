# ESP32-PICO RGB Keypad
<img width="852" height="78" alt="스크린샷 2025-08-04 125040" src="https://github.com/user-attachments/assets/b488fc70-6d98-4027-a814-e399ffe4f581" />

Pimoroni **Pico RGB Keypad**를  
**ESP32-PICO-KIT**(내장 플래시) 보드와 점퍼 몇 가닥으로 연결해  
– 16개 버튼 : BLE HID(무선 키보드)로 ‘0’ ~ ‘F’ 전송  
– 16개 RGB LED : 각 키 상태 표시
![IMG_3129](https://github.com/user-attachments/assets/a5082fe6-c8e4-464c-9716-944e98a1e355)
## 📌 핀 맵 (최종 확정)
![IMG_3134](https://github.com/user-attachments/assets/508b9190-78b7-48a5-a412-4a6cae07a347)

| 키패드 패키지 번호 | 키패드 라벨 | 기능               | ESP32-PICO-KIT 핀 | 비고             |
| ---------- | ------ | ---------------- | ---------------- | -------------- |
| **40**     | VBUS   | 5 V (LED 전원)     | **5 V**          | 빨강 점퍼          |
| **36**     | 3V3    | 3.3 V 로직 전원      | **3 V3**         | 주황             |
| **34**     | –      | GND              | **GND**          | 검정             |
| **24**     | MOSI   | **LED DATA**     | **GPIO 19**      | 초록             |
| **25**     | SCLK   | **LED CLK**      | **GPIO 18**      | 파랑             |
| **6**      | SDA    | I²C SDA          | **GPIO 21**      | 노랑             |
| **7**      | SCL    | I²C SCL          | **GPIO 22**      | 보라             |
| **4**      | INT    | TCA9555 INT (선택) | **GPIO 23**      | 회색, 사용 안 해도 무방 |
![IMG_3137](https://github.com/user-attachments/assets/36d22be3-b07a-4ae8-a674-01a20f51a51f)

> ● LED 타입: **APA102 (DotStar)**   
> ● ‘–’ 패드는 전부 GND ― 하나만 연결해도 무방

---

## 🎛️ 전체 예제 (버튼 → BLE ‘0’-‘F’ + LED 점등)

```cpp
#include <Wire.h>
#include <Adafruit_DotStar.h>
#include <BleKeyboard.h>

// ── 핀 -----------------------------------------------------------
#define DOT_DATA 19      // MOSI  (패키지 24)
#define DOT_CLK  18      // SCLK  (패키지 25)
#define SDA_PIN  21      // 패키지 6
#define SCL_PIN  22      // 패키지 7

// ── TCA9555 ------------------------------------------------------
#define TCA_ADDR     0x20
#define REG_IN0      0x00
#define REG_IN1      0x01
#define REG_CFG0     0x06
#define REG_CFG1     0x07

// ── 객체 ---------------------------------------------------------
Adafruit_DotStar leds(16, DOT_DATA, DOT_CLK, DOTSTAR_BGR);
BleKeyboard       kb("HexPad-ESP32", "PicoRGB", 100);

uint16_t prevState = 0xFFFF;                // 1 = released
const char keyChar[16] = {
  '0','1','2','3','4','5','6','7',
  '8','9','A','B','C','D','E','F'
};

// ── I²C 헬퍼 ------------------------------------------------------
inline void tcaWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(reg); Wire.write(val); Wire.endTransmission();
}
inline uint8_t tcaRead(uint8_t reg) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(reg); Wire.endTransmission(false);                 // restart
  Wire.requestFrom(static_cast<uint8_t>(TCA_ADDR),
                   static_cast<size_t>(1));                     // 2-arg
  return Wire.read();
}
inline uint16_t readKeys() {
  return (uint16_t)tcaRead(REG_IN1) << 8 | tcaRead(REG_IN0);
}

// ── SETUP --------------------------------------------------------
void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  tcaWrite(REG_CFG0, 0xFF);          // 16핀 전부 입력
  tcaWrite(REG_CFG1, 0xFF);

  leds.begin();
  leds.setBrightness(80);            // USB 전류 제한
  leds.show();

  kb.begin();
  Serial.println("HexPad ready – pair via BLE");
}

// ── LOOP ---------------------------------------------------------
void loop() {
  uint16_t now  = readKeys();
  uint16_t diff = now ^ prevState;
  if (diff) { delay(8); now = readKeys(); diff = now ^ prevState; } // debounce

  if (diff) {
    for (uint8_t i = 0; i < 16; ++i) {
      uint16_t mask = 1 << i;
      if (diff & mask) {
        bool down = !(now & mask);                     // 0 = pressed
        leds.setPixelColor(i, down ? leds.Color(0,180,40) : 0);
        if (kb.isConnected()) {
          if (down) kb.press  (keyChar[i]);
          else      kb.release(keyChar[i]);
        }
      }
    }
    leds.show();
    prevState = now;
  }
  delay(1);                                           // feed WDT
}
```

### IDE / 라이브러리 버전

| 항목             | 설정                                   |
| -------------- | ------------------------------------ |
| **esp32 core** | 2.0.17 (Boards Manager)              |
| **라이브러리**      | Adafruit DotStar, ESP32 BLE Keyboard |

### 사용 절차

1. 위 배선대로 연결
2. 스케치 업로드 → 시리얼 모니터에 `HexPad ready` 확인
3. PC/폰 블루투스에서 **HexPad-ESP32** 페어링
4. 16 버튼 누르면 ‘0’–‘F’ 입력 & 해당 LED 초록 ON/OFF

이제 LED와 BLE 키보드가 모두 정상 동작할 것입니다!

---

## 파일 구조

```

/firmware
├─ code1.c   ; DotStar(APA102) 버전
└─ code2.c   ; WS2812(NeoPixel) 버전

/docs
└─ README.md ; (본 문서)

````

> 보드에 실장된 LED 종류에 따라 **code1.c** 또는 **code2.c**를 컴파일하세요.  
> ※ 버튼은 두 버전 모두 동일하게 동작합니다.

---

## 필요한 부품

| 수량 | 품목 | 비고 |
|----:|------|------|
| 1 | ESP32-PICO-KIT (17 × 2 헤더) | V4 보드 테스트 |
| 1 | Pimoroni Pico RGB Keypad | 16× RGB + TCA9555 |
| 6 ~ 7 | 점퍼선(수→암) | INT까지 쓰면 7개 |
| 1 | USB-C 케이블 | 전원·펌웨어 업로드 |

---

## 핀 매핑

| 키패드 핀 | 기능 | ESP32-PICO-KIT 핀 |
|-----------|------|-------------------|
| VBUS  | 5 V (LED 전원) | 5 V |
| +3 V3 | 3.3 V 로직 전원 | 3 V3 |
| –     | GND | GND |
| GP0   | LED 데이터 (DIN) | GPIO 23 |
| GP1   | DotStar 전용 CLK | GPIO 18 *(DotStar만)* |
| GP4   | I²C SDA | GPIO 21 |
| GP5   | I²C SCL | GPIO 22 |
| GP3   | TCA9555 INT (옵션) | GPIO 19 |

- **DotStar(APA102)** → GP0-DATA + GP1-CLK 모두 사용  
- **WS2812(NeoPixel)**  → GP0-DIN 한 줄만 사용, GP1은 비연결

---

## Arduino IDE 설정

1. **Boards Manager**  
   esp32 by Espressif → *v 2.0.17* 설치(BLE-Keyboard 호환)
2. **라이브러리 Manager**  
   - Adafruit DotStar (code1 전용)  
   - Adafruit NeoPixel (code2 전용)  
   - ESP32 BLE Keyboard (또는 wachihkutchi 포크)
3. **보드** : ESP32 PICO-D4 선택  
4. Baud 115200 시리얼 모니터로 로그 확인

---

## 빌드·업로드

```text
code1.c  ; DotStar(APA102) 용
code2.c  ; WS2812(NeoPixel) 용
````

1. 자신의 LED 유형에 맞는 파일 열기
2. Tools → Upload
3. 시리얼 모니터 메시지

   ```
   HexPad ready – pair via BLE
   ```

   이후 PC/휴대폰 블루투스에서 **HexPad-ESP32** 검색·페어링

---

## 문제 해결

| 현상                            | 점검 사항                                                                                      |
| ----------------------------- | ------------------------------------------------------------------------------------------ |
| 버튼 OK, LED OFF                | • VBUS ↔ GND = 5 V 실측 <br>• LED 데이터 핀 위치 확인 (GP0→GPIO 23) <br>• DotStar ↔ WS2812 스케치 혼용 여부 |
| 컴파일 시 String / std::string 오류 | esp32 core 2.0.17 사용 또는 BLE-Keyboard 포크 설치                                                 |
| BLE 기기 검색 안 됨                 | 업로드 후 **EN** 리셋, 시리얼 로그 ‘ready’ 확인                                                         |

---

## 라이선스

MIT

```
::contentReference[oaicite:0]{index=0}
```
