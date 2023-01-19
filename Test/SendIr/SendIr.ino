#define IR_SEND_PIN 9
#include <IRremote.h>
 

unsigned int  rawData[67] = {4350,4550, 550,1650, 450,1800, 500,1700, 500,600, 550,550, 550,550, 550,600, 500,600, 500,1650, 550,1700, 500,1700, 550,550, 550,600, 500,600, 500,600, 500,600, 500,600, 500,1700, 550,550, 550,600, 500,600, 500,600, 500,600, 500,600, 500,1700, 550,550, 550,1700, 500,1650, 550,1700, 500,1700, 550,1650, 550,1700, 500};  // NEC 20DF10EF
int freq_ir = 38; //Khz 

void setup() {
  Serial.begin(9600);    
  IrSender.begin();
  Serial.println("pronto");
}
  
void loop() {   
   IrSender.sendRaw(rawData, 67, freq_ir);
   Serial.println(" ");
   delay(150);
}
