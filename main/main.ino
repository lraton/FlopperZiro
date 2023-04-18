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

//pin rfid
#define IRQ 1
#define RESET 0

//ir receiver
#define IR_RECEIVE_PIN 6
#define IR_SEND_PIN 9
int freq_ir = 38;
String irproducer = "";
uint16_t rawData[67];
String data = "";
int scanning = 1;

//pin sd
#define SPI_SPEED SD_SCK_MHZ(4)
#define SD_PIN A5
bool sdbegin = false;
SdFat SD;
File file;

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
#define button2Pin (A0)
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

//rfid display
Adafruit_PN532 nfc(1, 0);
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

Adafruit_SSD1306 display(128, 64);

void setup() {
  Serial.begin(9600);
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
  /*
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
    sdbegin=true;
    Serial.println("SD");
  } else {
    sdbegin=false;
    Serial.println("NO SD");
  }
  */
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
