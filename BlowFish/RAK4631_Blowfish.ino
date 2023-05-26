#include <Arduino.h>
//#include <Adafruit_TinyUSB.h>
#include "Blowfish.h"

void hexDump(unsigned char *buf, uint16_t len) {
  char alphabet[17] = "0123456789abcdef";
  Serial.print(F("   +------------------------------------------------+ +----------------+\n"));
  Serial.print(F("   |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f | |      ASCII     |\n"));
  for (uint16_t i = 0; i < len; i += 16) {
    if (i % 128 == 0)
      Serial.print(F("   +------------------------------------------------+ +----------------+\n"));
    char s[] = "|                                                | |                |\n";
    uint8_t ix = 1, iy = 52;
    for (uint8_t j = 0; j < 16; j++) {
      if (i + j < len) {
        uint8_t c = buf[i + j];
        s[ix++] = alphabet[(c >> 4) & 0x0F];
        s[ix++] = alphabet[c & 0x0F];
        ix++;
        if (c > 31 && c < 128) s[iy++] = c;
        else s[iy++] = '.';
      }
    }
    uint8_t index = i / 16;
    if (i < 256) Serial.write(' ');
    Serial.print(index, HEX); Serial.write('.');
    Serial.print(s);
  }
  Serial.print(F("   +------------------------------------------------+ +----------------+\n"));
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(2000);
  Serial.println("\nBlowfish test");
  CBlowFish bf;
  BYTE plain[256];
  BYTE encrypted[256];
  BYTE decrypted[256];
  BYTE myKey[17] = "YELLOW SUBMARINE";
  Serial.println("\nmyKey");
  hexDump(myKey, 16);
  bf.Initialize(myKey, 16);
  for (int i = 0; i < 256; i++) plain[i] = i;
  Serial.println("\nplain");
  hexDump(plain, 256);
  bf.Encode(plain, encrypted, 256);
  Serial.println("\nEncrypted:");
  hexDump(encrypted, 256);
  bf.Decode(encrypted, decrypted, 256);
  Serial.println("\nDecrypted:");
  hexDump(decrypted, 256);
  Serial.println("\nTHE END");

  // void CBlowFish::Initialize(BYTE key[], int keybytes) {
  // DWORD CBlowFish::Encode(BYTE * pInput, BYTE * pOutput, DWORD lSize) {
  // void CBlowFish::Decode(BYTE * pInput, BYTE * pOutput, DWORD lSize) {

}

void loop() {
  // put your main code here, to run repeatedly:

}
