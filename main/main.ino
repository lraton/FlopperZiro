#include <Wire.h>
#include <RCSwitch.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PN532.h>

//pin rfid
#define IRQ   (1)
#define RESET (0)

//ir receiver
#define IR_RECEIVE_PIN 6
#define IR_SEND_PIN 9
#include <IRremote.h>

//button
#define buttonUp (A4)   
#define buttonDown  (A0)
#define buttonLeft  (A3)   
#define buttonSelect  (A2)

int currentPage = 0;
int scelta=0;
const int numPages = 5; 

//tamaguino
const int sound = 0;
#define button1Pin  (A3)
#define button2Pin  (A0) 
#define button3Pin  (A2) 
int button1State = 0;
int button2State = 0;
int button3State = 0;
#define ACTIVATED LOW

//battery
#define analogInPin  A4    // Analog input pin
int sensorValue;          // Analog Output of Sensor
float calibration = 2.33; 
int bat_percentage;

//carta per sbloccare 
int buf[]={115,232,15,186};
bool tag=false;

//Rf definition
#define rfreceive 3
#define rftransmit A6
RCSwitch mySwitch = RCSwitch();

//rfid display
Adafruit_PN532 nfc(IRQ, RESET);
Adafruit_SSD1306 display(128, 64);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  //setup button
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonSelect, INPUT_PULLUP);

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
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);

  //Setup rfid/nfc
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1);
  }
  nfc.SAMConfig();
}

void loop() {
  switch(scelta){
    case 0:
      displayMenu();
      break;
    case 1:
      graficausb();
      badusb();
      break;
    case 2:
      graficarfid();
      rfid();
      break;
    case 3:
      graficarfid();
      ir();
      break;
    default:
      currentPage=0;
      scelta=0;
      break;
  }
}