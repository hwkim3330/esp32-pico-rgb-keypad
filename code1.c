#include <Wire.h>
#include <Adafruit_DotStar.h>
#include <BleKeyboard.h>

#define DOT_DATA 23
#define DOT_CLK  18
#define SDA_PIN  21
#define SCL_PIN  22

#define TCA_ADDR      0x20
#define REG_INPUT_0   0x00
#define REG_INPUT_1   0x01
#define REG_CONFIG_0  0x06
#define REG_CONFIG_1  0x07

Adafruit_DotStar leds(16, DOT_DATA, DOT_CLK, DOTSTAR_BGR);
BleKeyboard       kb("HexPad-ESP32", "KETI", 100);

uint16_t prevState = 0xFFFF;

// ASCII chars for 0-F
const char keyChar[16] = {
  '0','1','2','3','4','5','6','7',
  '8','9','A','B','C','D','E','F'
};

inline void tcaWrite(uint8_t r, uint8_t v){
  Wire.beginTransmission(TCA_ADDR); Wire.write(r); Wire.write(v); Wire.endTransmission();
}
inline uint8_t tcaRead(uint8_t r){
  Wire.beginTransmission(TCA_ADDR); Wire.write(r); Wire.endTransmission(false);
  Wire.requestFrom(TCA_ADDR,(uint8_t)1); return Wire.read();
}
inline uint16_t readKeys(){
  return (uint16_t)tcaRead(REG_INPUT_1)<<8 | tcaRead(REG_INPUT_0);
}

void setup(){
  Serial.begin(115200);
  Wire.begin(SDA_PIN,SCL_PIN);
  tcaWrite(REG_CONFIG_0,0xFF); tcaWrite(REG_CONFIG_1,0xFF);
  leds.begin(); leds.setBrightness(80); leds.clear(); leds.show();
  kb.begin();
}

void loop(){
  uint16_t now=readKeys(), diff=now^prevState;
  if(diff){ delay(8); now=readKeys(); diff=now^prevState; }
  if(diff){
    for(uint8_t i=0;i<16;++i){
      uint16_t m=1<<i;
      if(diff&m){
        bool down=!(now&m);
        leds.setPixelColor(i, down?leds.Color(0,150,40):0);
        if(kb.isConnected()){
          if(down) kb.press  (keyChar[i]);
          else     kb.release(keyChar[i]);
        }
      }
    }
    leds.show();
    prevState=now;
  }
  delay(1);
}
