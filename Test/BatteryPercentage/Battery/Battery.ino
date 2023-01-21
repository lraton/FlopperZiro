int analogInPin  = A4;    // Analog input pin
int sensorValue;          // Analog Output of Sensor
float calibration = 0.33; // Check Battery voltage using multimeter & add/subtract the value
int bat_percentage;

void setup() {
  pinMode(A4, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  
  sensorValue = analogRead(analogInPin);
  Serial.println(sensorValue);
  float voltage = (((sensorValue * 3.3) / 1024) * 2 + calibration); //multiply by two as voltage divider network is 100K & 100K Resistor
 
  bat_percentage = mapfloat(voltage, 2.8, 4.2, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
 
  if (bat_percentage >= 100)
  {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }
  
  Serial.print("Analog Value = ");
  Serial.print(sensorValue);
  Serial.print("\t Output Voltage = ");
  Serial.print(voltage);
  Serial.print("\t Battery Percentage = ");
  Serial.println(bat_percentage);
  checkMenuButton();
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//all button function when im in the menu

void checkMenuButton(){
  if(analogRead(analogInPin) == 0) {
    Serial.println("bottone");
  }
}
