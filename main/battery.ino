/*
 * Copyright (c) 2024, lraton 
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

void battery() {
  // Read the analog value from the sensor pin
  sensorValue = analogRead(analogInPin);

  // Convert the analog sensor value to a voltage.
  // The factor of 2 accounts for the voltage divider (100K & 100K resistors).
  // Subtract calibration to adjust the voltage reading.
  float voltage = (((sensorValue * 3.3) / 1024) * 2 - calibration);

  // Map the voltage value to a percentage (0% at 2.8V, 100% at 4.2V).
  bat_percentage = mapfloat(voltage, 2.8, 4.2, 0, 100);

  // Ensure battery percentage stays within 0-100% range
  if (bat_percentage >= 100) {
    bat_percentage = 100;  // Cap the percentage at 100%
  }
  if (bat_percentage <= 0) {
    bat_percentage = 1;  // Prevent percentage from being less than 1%
  }

  /*
  // Uncomment for debugging:
  // Print the analog value, voltage, and battery percentage to the serial monitor.
  // Serial.print("Analog Value = ");
  // Serial.print(sensorValue);
  // Serial.print("\t Output Voltage = ");
  // Serial.print(voltage);
  // Serial.print("\t Battery Percentage = ");
  // Serial.println(bat_percentage);
  */

  //Print SD and batetry percentage
  if (scelta == 0 && currentPage == 0) {
    //Print SD on homepage (bottom right)
    if (sdbegin) {
      // Adjust the display cursor position based on the length of the percentage value.
      if (int(SDpercentFree) < 100 && int(SDpercentFree) > 10) {
        display.setCursor(105, 53);  // Set cursor for 2-digit percentage
      } else {
        if (int(SDpercentFree) < 10) {
          display.setCursor(110, 53);  // Set cursor for 1-digit percentage
        } else {
          display.setCursor(100, 53);  // Set cursor for 100%
        }
      }
      // Display the sd remaining on the screen
      drawsd(85, 52);
      display.print(int(SDpercentFree));
      display.println("%");
    } else {
      // Display the NOSD on the screen
      drawsd(85, 52);
      display.setCursor(100, 53);
      display.print("NoSD");
    }

    //Print battery on homepage (bottom right)
    // Adjust the display cursor position based on the length of the percentage value. Battery
    if (bat_percentage < 100 && bat_percentage > 10) {
      display.setCursor(105, 43);  // Set cursor for 2-digit percentage
    } else {
      if (bat_percentage < 10) {
        display.setCursor(110, 43);  // Set cursor for 1-digit percentage
      } else {
        display.setCursor(100, 43);  // Set cursor for 100%
      }
    }

    // Display the battery percentage on the screen
    drawbattery(85, 43);
    display.print(bat_percentage);
    display.println("%");
    display.display();  // Refresh the display with the new data
  } else {
    //Print sd on the other menu (top left)
    if (sdbegin) {
      // Display the sd remaining on the screen
      display.setCursor(5, 5);
      display.print(int(SDpercentFree));
      display.print("%");
      drawsd(display.getCursorX(), 4);
    } else {
      // Display the NOSD on the screen
      display.setCursor(5, 5);
      display.print("No SD");
      drawsd(display.getCursorX(), 4);
    }

    //Print battery on the other menu (top right)
    // Adjust the display cursor position based on the length of the percentage value. Battery
    if (bat_percentage < 100 && bat_percentage > 10) {
      display.setCursor(105, 5);  // Set cursor for 2-digit percentage
    } else {
      if (bat_percentage < 10) {
        display.setCursor(110, 5);  // Set cursor for 1-digit percentage
      } else {
        display.setCursor(100, 5);  // Set cursor for 100%
      }
    }

    // Display the battery percentage on the screen
    drawbattery(85, 5);
    display.print(bat_percentage);
    display.println("%");
    display.display();  // Refresh the display with the new data
  }
}

// This function maps a floating-point value from one range to another.
// It takes the input value 'x' and maps it from the input range (in_min to in_max)
// to the output range (out_min to out_max).
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
