#include <SPI.h>
#include <SD.h>
#define SD_PIN A5

File file;

void setup() {
  Serial.begin(9600);


}

void loop() {
    if(SD.begin(A5)){
     Serial.println("SD");
  }else{
    Serial.println("errore");
    return;
  }
   if(SD.exists("prova.txt")){
    Serial.println("gia esistente");
  }else{   
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
  delay(500);
}
