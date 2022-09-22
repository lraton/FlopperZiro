#include <SPI.h>
#include <SD.h>

File file

void setup() {
  Serial.begin(9600);
  if(!SD.begin(10)){
    Serial.println("errore");
    return;
  }
  Serial.println("SD");

  if(SD.exists("prova.txt")){
    SD.remove("prova.txt");
  }
   file = SD.open("prova.txt", FILE_WRITE);
   file.write("hello");
   file.close();
}

void loop() {
  
}
