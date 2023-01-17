#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PN532.h>

#define IRQ   (8)
#define RESET (9)

int buttonUp= 2;   
int buttonDown = 3;
int buttonLeft = 4;   
int buttonSelect = 6; 
int currentPage = 0;
int scelta=0;
const int numPages = 3; 

int buf[]={115,232,15,186};
bool tag=false;

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

  //setup display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

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
  Serial.print("Current page: ");
  Serial.println(currentPage);
  
  //mostra in base alle scelte
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
  }
  


  delay(100);
}
