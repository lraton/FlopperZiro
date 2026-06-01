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

// ─── IR ──────────────────────────────────────────────────────────────────────
#define IR_RECEIVE_PIN 6   // Pin for receiving IR signals
#define IR_SEND_PIN    9   // Pin for sending IR signals
int     irFrequency = 38;  // IR carrier frequency in kHz
String  irProtocol  = "";  // Decoded IR protocol name
String  irHexValue  = "";  // Decoded IR value as hex string
uint16_t irRawData[67];    // Raw IR timing data (microseconds)

// ─── Scanning state ──────────────────────────────────────────────────────────
int  isScanning = 1;  // 1 = actively scanning, 0 = result ready / tag loaded

// ─── SD card ─────────────────────────────────────────────────────────────────
#define SPI_SPEED SD_SCK_MHZ(4)  // SPI clock speed for SD card
#define SD_CS_PIN A0              // Chip-select pin for SD card
SdFat  SD;                        // SdFat SD object
File   file;                      // Active file handle
bool   sdReady       = false;     // True when SD card initialised successfully
float  sdFreePercent = 0.0f;      // Percentage of SD space remaining
char   sdFileName[50];            // Buffer for a file name
char   sdSelectedFile[50];        // Name of the file the user has chosen

// ─── Module / menu state ─────────────────────────────────────────────────────
int    currentModuleType  = 0;  // Active module type (mirrors selectedModule, used by SD browser)
int    sdFileSelected     = 0;  // 0 = browsing, ≥1 = file index selected
int    sdSelectedFileNum  = 1;  // 1-based index of the highlighted SD file
int    sdFileCount        = 0;  // Total number of files in the open directory
String sdReadBuffer;            // Temporary buffer for SD file reads

// ─── Main menu ───────────────────────────────────────────────────────────────
int        menuCurrentPage  = 0;  // Currently visible main-menu page
int        selectedModule   = 0;  // Active module (0=menu,1=badusb,2=rfid,3=ir,4=rf)
const int  MENU_PAGE_COUNT  = 5;  // Total number of main-menu pages

// ─── Sub-menu ────────────────────────────────────────────────────────────────
int        subMenuCurrentPage  = 0;  // Currently visible sub-menu page
int        selectedSubMenu     = 0;  // Selected sub-menu option (0=submenu,1=scan,2=SD)
int        SUB_MENU_PAGE_COUNT = 3;  // Total number of sub-menu pages

// ─── Tamagotchi / button pins ────────────────────────────────────────────────
#define SOUND_PIN   0   // PWM pin for buzzer / sound output
#define BUTTON1_PIN A3
#define BUTTON2_PIN A5
#define BUTTON3_PIN A2
int button1State = 0;
int button2State = 0;
int button3State = 0;
#define BUTTON_ACTIVE LOW  // Logic level when a button is pressed

// ─── Navigation buttons ───────────────────────────────────────────────────────
#define BTN_UP     A4
#define BTN_DOWN   A0
#define BTN_LEFT   A3
#define BTN_SELECT A2
#define BTN_RIGHT  A1

// ─── Battery ─────────────────────────────────────────────────────────────────
#define BATTERY_ADC_PIN A4         // Analog pin connected to the voltage-divider mid-point
int   batterySensorValue = 0;      // Raw ADC reading
float BATTERY_CALIBRATION = 2.25f; // Calibration offset for the voltage-divider circuit
int   batteryPercent      = 0;     // Computed battery percentage (0-100)

// ─── RFID lock ────────────────────────────────────────────────────────────────
// Set deviceUnlocked = false to require a specific card to unlock the main menu.
// authorizedUid[] holds the expected UID bytes (4 bytes).
int  authorizedUid[] = { 115, 232, 15, 186 };
bool deviceUnlocked  = true;  // true = unlocked (no card required)

// ─── RF (433 MHz) ─────────────────────────────────────────────────────────────
#define RF_RECEIVE_PIN  3   // Digital pin for RF receiver data
#define RF_TRANSMIT_PIN A6  // Digital pin for RF transmitter data
RCSwitch rfSwitch = RCSwitch();
int  rfReceivedValue    = 0;  // Decoded RF signal value
int  rfBitLength        = 0;  // Number of bits in the decoded signal
int  rfReceivedProtocol = 0;  // RCSwitch protocol index

// ─── RFID / NFC (PN532 over I²C) ─────────────────────────────────────────────
#define RFID_IRQ_PIN   1  // PN532 IRQ pin (active-low interrupt)
#define RFID_RESET_PIN 0  // PN532 RSTPDN pin
Adafruit_PN532 nfc(RFID_IRQ_PIN, RFID_RESET_PIN, &Wire);
uint8_t rfidUid[8]  = { 0 };  // Buffer for the tag UID (up to 8 bytes)
uint8_t rfidUidLen  = 0;      // Actual UID length in bytes (4 or 7)
uint8_t ntagData[256] = { 0 }; // Buffer for NTAG/Ultralight memory data (64 pages * 4 bytes)
String  rfidCardType = "";    // Human-readable card type string
static bool rfidDetectionStarted = false;  // Non-blocking read state flag

// ─── OLED display (SSD1306, 128×64, I²C 0x3C) ───────────────────────────────
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

// ─────────────────────────────────────────────────────────────────────────────
void setup() {

  Serial.begin(115200);
  Wire.begin();

  // Navigation buttons
  pinMode(BTN_UP,     INPUT_PULLUP);
  pinMode(BTN_DOWN,   INPUT_PULLUP);
  pinMode(BTN_LEFT,   INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_RIGHT,  INPUT_PULLUP);

  // Auxiliary buttons and sound
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(SOUND_PIN,   OUTPUT);

  // OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  if (deviceUnlocked) {
    drawBootScreen();
  }

  // RF 433 MHz
  rfSwitch.enableReceive(RF_RECEIVE_PIN);
  rfSwitch.enableTransmit(RF_TRANSMIT_PIN);

  // IR
  IrReceiver.begin(IR_RECEIVE_PIN);
  IrSender.begin(IR_SEND_PIN);

  // RFID / NFC
  nfc.begin();
  uint32_t nfcVersion = nfc.getFirmwareVersion();
  if (!nfcVersion) {
    display.setCursor(52, 33);
    display.print("Connect RFID");
    display.display();
    while (1);
  }
  nfc.SAMConfig();

  // SD card
  if (SD.begin(SD_CS_PIN, SPI_SPEED)) {
    sdReady      = true;
    sdFreePercent = getSdFreePercent();
  } else {
    sdReady = false;
  }
}

// ─────────────────────────────────────────────────────────────────────────────
void loop() {
  switch (selectedModule) {
    case 0: handleMainMenu(); break;  // Show the main menu
    case 1: badUsb();         break;  // BadUSB / DuckyScript
    case 2: rfid();           break;  // RFID / NFC
    case 3: ir();             break;  // Infrared
    case 4: rf();             break;  // RF 433 MHz
    default:
      menuCurrentPage  = 0;
      selectedModule   = 0;
      break;
  }
}
