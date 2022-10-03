#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <SPI.h>
#include <Wire.h>

Adafruit_SSD1306 display(128, 64, &Wire, 4);

void setup()   {   
  if(display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){ //MOdifica Indirizzo tramite i2c scanner
    while(1);
  }
  
  display.clearDisplay();  //Pulisce il buffer da inviare al display
  display.setTextSize(1);  //Imposta la grandezza del testo
  display.setTextColor(WHITE); //Imposta il colore del testo (Solo bianco)
  display.setCursor(0,0); //Imposta la posizione del cursore (Larghezza,Altezza)
  display.println("Cazzo guardi"); //Stringa da visualizzare
  display.display(); //Invia il buffer da visualizzare al display
}
void loop() {
 
}