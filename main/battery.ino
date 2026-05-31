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

// Reads the battery voltage via a 100 kΩ / 100 kΩ voltage divider on BATTERY_ADC_PIN,
// converts it to a percentage, and renders the battery + SD status on the OLED.
void displayBattery() {
  batterySensorValue = analogRead(BATTERY_ADC_PIN);

  // Voltage at the divider mid-point × 2 (divider ratio) − calibration offset.
  float voltage = ((batterySensorValue * 3.3f) / 1024.0f) * 2.0f - BATTERY_CALIBRATION;

  // Map 2.8 V (0 %) → 4.2 V (100 %).
  batteryPercent = (int)mapFloat(voltage, 2.8f, 4.2f, 0.0f, 100.0f);
  batteryPercent = constrain(batteryPercent, 1, 100);

  if (selectedModule == 0 && menuCurrentPage == 0) {
    // ── Home screen: SD and battery in the bottom-right corner ───────────────
    if (sdReady) {
      if      (sdFreePercent < 10)  display.setCursor(110, 53);
      else if (sdFreePercent < 100) display.setCursor(105, 53);
      else                          display.setCursor(100, 53);

      drawSdIcon(85, 52);
      display.print((int)sdFreePercent);
      display.println("%");
    } else {
      drawSdIcon(85, 52);
      display.setCursor(100, 53);
      display.print("NoSD");
    }

    if      (batteryPercent < 10)  display.setCursor(110, 43);
    else if (batteryPercent < 100) display.setCursor(105, 43);
    else                           display.setCursor(100, 43);

    drawBatteryIcon(85, 43);
    display.print(batteryPercent);
    display.println("%");
    display.display();

  } else {
    // ── All other screens: SD and battery in the top-left / top-right ────────
    if (sdReady) {
      display.setCursor(5, 5);
      display.print((int)sdFreePercent);
      display.print("%");
      drawSdIcon(display.getCursorX(), 4);
    } else {
      display.setCursor(5, 5);
      display.print("No SD");
      drawSdIcon(display.getCursorX(), 4);
    }

    if      (batteryPercent < 10)  display.setCursor(110, 5);
    else if (batteryPercent < 100) display.setCursor(105, 5);
    else                           display.setCursor(100, 5);

    drawBatteryIcon(85, 5);
    display.print(batteryPercent);
    display.println("%");
    display.display();
  }
}

// Maps a float value from one range to another (analogous to Arduino map() for floats).
float mapFloat(float x, float inMin, float inMax, float outMin, float outMax) {
  return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}
