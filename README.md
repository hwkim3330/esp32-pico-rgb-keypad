# ESP32-PICO RGB Keypad

Pimoroni **Pico RGB Keypad**를  
**ESP32-PICO-KIT**(내장 플래시) 보드와 점퍼 몇 가닥으로 연결해  
– 16개 버튼 : BLE HID(무선 키보드)로 ‘0’ ~ ‘F’ 전송  
– 16개 RGB LED : 각 키 상태 표시

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
