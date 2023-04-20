#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RCSwitch.h>
#include <stdint.h>
#include <IRremote.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <PN532_I2C.h>
#include "PN532.h"

//pin rfid
#define IRQ 1
#define RESET 0

//pin sd
#define SD_PIN A5
File file;

uint32_t versiondata;
//rfid display
PN532_I2C pn(Wire);
PN532 nfc(pn);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("porcodddd");
  display.display();
  //Setup rfid/nfc
  nfc.begin();
   versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1)
      ;
  }
  nfc.SAMConfig();
/*
  if (SD.begin(SD_PIN)) {
    Serial.println("SD");
  } else {
    Serial.println("errore");
    return;
  }
*/
}

void loop() {
    if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    //while (1);
  }else{
    Serial.print("PN53x found");
  }
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.print("UID Value: ");
    nfc.PrintHex(uid, uidLength);
    for (int i = 0; i < uidLength; i++) {
      Serial.println(uid[i]);
    }
  }
}
