/*
 * Copyright (c) 2024, lraton 
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <SPI.h>
#include "SdFat.h"
#include "sdios.h"
#include <Wire.h>
#include <RCSwitch.h>
#include <stdint.h>
#include <IRremote.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PN532.h>
#include <string.h>
#include "Keyboard.h"

//ir receiver
#define IR_RECEIVE_PIN 6
#define IR_SEND_PIN 9
int freq_ir = 38;
String irproducer = "";
uint16_t rawData[67];
String data = "";
int scanning = 1;

//Sd variables
#define SPI_SPEED SD_SCK_MHZ(4)
#define SD_PIN A0
SdFat SD;
File file;
bool sdbegin = false;
char fileName[50];
char selectedFile[50];
int type = 0;
int sceltaSd = 0;
int selectedFileNumber = 1;
int fileCount = 0;  //da aggiungere funzione che conta quanti file ci sono
String buffer;

//pin button
#define buttonUp (A4)
#define buttonDown (A0)
#define buttonLeft (A3)
#define buttonSelect (A2)
#define buttonRight (A1)

//menu iniziale
int currentPage = 0;
int scelta = 0;
const int numPages = 5;

//sub menu
int currentPageSubMenu = 0;
int sceltaSubMenu = 0;
int numPagesSubMenu = 3;

//tamaguino
const int sound = 0;
#define button1Pin (A3)
#define button2Pin (A5)
#define button3Pin (A2)
int button1State = 0;
int button2State = 0;
int button3State = 0;
#define ACTIVATED LOW

//battery
#define analogInPin A4  // Analog input pin
int sensorValue;        // Analog Output of Sensor
float calibration = 2.25;
int bat_percentage;

//carta per sbloccare
int buf[] = { 115, 232, 15, 186 };
bool tag = false;

//Rf definition
#define rfreceive 3
#define rftransmit A6
RCSwitch mySwitch = RCSwitch();
int rfvalue;
int rfbit;
int rfprotocol;

//pin rfid
#define IRQ 1
#define RESET 0
//rfid
Adafruit_PN532 nfc(IRQ, RESET, &Wire);
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

//Display
Adafruit_SSD1306 display(128, 64);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  //setup button
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonSelect, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);

  //setup tamaguino
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(sound, OUTPUT);

  //setup display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  flopperblockedimage();

  //setup rf
  mySwitch.enableReceive(rfreceive);
  mySwitch.enableTransmit(rftransmit);

  //ir receiver sender
  IrReceiver.begin(IR_RECEIVE_PIN);
  IrSender.begin(IR_SEND_PIN);

  //Setup rfid/nfc
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("RFID error");
    display.setCursor(33, 30);
    display.println("RFID error");
    display.display();
    //while (1);
  }
  nfc.SAMConfig();

  if (SD.begin(SD_PIN, SPI_SPEED)) {
    sdbegin = true;
    Serial.println("SD");
  } else {
    sdbegin = false;
    Serial.println("NO SD");
  }
}

void loop() {
  switch (scelta) {
    case 0:
      displayMenu();  //Mostra  il menu
      break;
    case 1:
      badusb();
      break;
    case 2:
      rfid();
      break;
    case 3:
      ir();
      break;
    case 4:
      rf();
      break;
    default:
      currentPage = 0;
      scelta = 0;
      break;
  }
}
