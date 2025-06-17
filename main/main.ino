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

// IR receiver setup
#define IR_RECEIVE_PIN 6  // Pin for receiving IR signals
#define IR_SEND_PIN 9     // Pin for sending IR signals
int freq_ir = 38;         // IR signal frequency
String irproducer = "";   // IR signal producer string
uint16_t rawData[67];     // Array to hold raw IR data
String data = "";         // String to hold data from IR signal
int scanning = 1;         // Variable to indicate scanning state

// SD card variables
#define SPI_SPEED SD_SCK_MHZ(4)  // Speed of SPI communication
#define SD_PIN A0                // Pin for SD card communication
SdFat SD;                        // SD card object
File file;                       // File object for SD card
bool sdbegin = false;            // Flag to check if SD card is initialized
char fileName[50];               // Filename buffer
char selectedFile[50];           // Selected file buffer
int type = 0;                    // Variable to hold type
int sceltaSd = 0;                // Choice for SD card options
int selectedFileNumber = 1;      // Selected file number
int fileCount = 0;               // Number of files (function to count files is needed)
String buffer;                   // String buffer for file data

// Button pin definitions
#define buttonUp (A4)
#define buttonDown (A0)
#define buttonLeft (A3)
#define buttonSelect (A2)
#define buttonRight (A1)

// Initial menu variables
int currentPage = 0;     // Current page of the main menu
int scelta = 0;          // Selected option from the main menu
const int numPages = 5;  // Total number of pages in the main menu

// Sub-menu variables
int currentPageSubMenu = 0;  // Current page in the sub-menu
int sceltaSubMenu = 0;       // Selected option from the sub-menu
int numPagesSubMenu = 3;     // Total number of pages in the sub-menu

// "Tamaguino" variables (for some game or device control)
const int sound = 0;  // Pin for sound output
#define button1Pin (A3)
#define button2Pin (A5)
#define button3Pin (A2)
int button1State = 0;  // State of button 1
int button2State = 0;  // State of button 2
int button3State = 0;  // State of button 3
#define ACTIVATED LOW  // Define button "activated" state

// Battery variables
#define analogInPin A4     // Pin for analog input to measure battery
int sensorValue;           // Value from battery sensor
float calibration = 2.25;  // Calibration value for battery percentage
int bat_percentage;        // Battery percentage

// RFID card variables for unlocking
int buf[] = { 115, 232, 15, 186 };  // Array holding card data
bool tag = true;                    // Tag flag for lock device. FALSE for activate the device lock TRUE for deactivate

// RF (Radio Frequency) definitions
#define rfreceive 3              // Pin to receive RF signals
#define rftransmit A6            // Pin to transmit RF signals
RCSwitch mySwitch = RCSwitch();  // RF switch object
int rfvalue;                     // RF value received
int rfbit;                       // RF bit value
int rfprotocol;                  // RF protocol used

// RFID pin definitions
#define IRQ 1    // IRQ pin for RFID communication
#define RESET 0  // Reset pin for RFID communication

// RFID/NFC variables
Adafruit_PN532 nfc(IRQ, RESET, &Wire);       // RFID/NFC object
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0, 0 };  // UID buffer for RFID tag
uint8_t uidLength;                           // UID length (either 4 or 7 bytes)
String cardType = "";
static bool detectionStarted = false;        // For non-blocking reading

// Display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);  // OLED display object (128x64 resolution)

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  Wire.begin();          // Initialize I2C communication

  // Setup button pins as input with internal pull-up resistors
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonSelect, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);

  // Setup "Tamaguino" buttons and sound pin
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(sound, OUTPUT);  // Set sound pin as output

  // Setup OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize display with address 0x3C
  display.setTextColor(WHITE);                // Set text color to white
  flopperblockedimage();                      // Display image on screen

  // Setup RF communication
  mySwitch.enableReceive(rfreceive);    // Enable RF reception
  mySwitch.enableTransmit(rftransmit);  // Enable RF transmission

  // Setup IR receiver and sender
  IrReceiver.begin(IR_RECEIVE_PIN);  // Initialize IR receiver
  IrSender.begin(IR_SEND_PIN);       // Initialize IR sender

  // Setup RFID/NFC communication
  nfc.begin();                                      // Initialize NFC
  uint32_t versiondata = nfc.getFirmwareVersion();  // Get RFID firmware version
  if (!versiondata) {
    Serial.println("RFID error");  // If no RFID detected, print error
    display.setCursor(33, 30);     // Display error on screen
    display.println("RFID error");
    display.display();
    //while (1);  // Uncomment this to stop execution if RFID is not detected
  }
  nfc.SAMConfig();  // Configure NFC for SAM mode

  // Setup SD card
  if (SD.begin(SD_PIN, SPI_SPEED)) {
    sdbegin = true;  // SD card successfully initialized
    Serial.println("SD");
  } else {
    sdbegin = false;  // SD card initialization failed
    Serial.println("NO SD");
  }
}

void loop() {
  switch (scelta) {  // Main loop to handle different modes based on user choice
    case 0:
      displayMenu();  // Display the menu
      break;
    case 1:
      badusb();  // Handle bad USB functionality
      break;
    case 2:
      rfid();  // Handle RFID functionality
      break;
    case 3:
      ir();  // Handle IR functionality
      break;
    case 4:
      rf();  // Handle RF functionality
      break;
    default:
      currentPage = 0;  // Reset to the first page of the menu
      scelta = 0;       // Reset selected option
      break;
  }
}
