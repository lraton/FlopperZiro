void battery() {
  sensorValue = analogRead(analogInPin);
  float voltage = (((sensorValue * 3.3) / 1024) * 2 - calibration); //multiply by two as voltage divider network is 100K & 100K Resistor

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
  if (bat_percentage < 100 && bat_percentage > 10) {
    display.setCursor(105, 5);
  } else {
    if (bat_percentage < 10) {
      display.setCursor(110, 5);
    } else {
      display.setCursor(100, 5);
    }
  }
  display.print(bat_percentage);
  display.println("%");
  display.display();
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
