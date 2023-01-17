#include <SPI.h>
#include <SD.h>
#define SD_PIN A5

File file;

void setup() {
  Serial.begin(9600);
  if(!SD.begin(SD_PIN)){
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
   file = SD.open("prova.txt");
   if (file) {
      Serial.println("prova.txt:");
  
      // read from the file until there's nothing else in it:
      while (file.available()) {
        Serial.write(file.read());
      }
      // close the file:
      file.close();
   } else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
   }

}

void loop() {
  
}
