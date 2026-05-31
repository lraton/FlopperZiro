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

// Routes the RF 433 MHz sub-menu pages.
void rf() {
  currentModuleType = 4;
  switch (selectedSubMenu) {
    case 0: drawSubMenu();   break;  // Sub-menu selection screen
    case 1: scanRf();        break;  // Active scan / result display
    case 2: drawSdMenu(4);   break;  // SD file browser for /rf/
  }
}

// ─── Scan ─────────────────────────────────────────────────────────────────────
// Polls the RCSwitch receiver for a new 433 MHz signal and displays the result.
void scanRf() {
  if (isScanning == 1) {
    drawScanScreen();

    if (rfSwitch.available()) {
      rfReceivedValue    = rfSwitch.getReceivedValue();
      rfBitLength        = rfSwitch.getReceivedBitlength();
      rfReceivedProtocol = rfSwitch.getReceivedProtocol();

      Serial.print("Received: ");
      Serial.println(rfReceivedValue);
      Serial.print(rfBitLength);
      Serial.println(" bit");
      Serial.print("Protocol: ");
      Serial.println(rfReceivedProtocol);

      rfSwitch.resetAvailable();
      isScanning = 0;
    }
  } else {
    // Display the last captured result.
    drawScanBase();
    display.setCursor(25, 25);
    display.println("Value: "    + String(rfReceivedValue));
    display.setCursor(20, 35);
    display.println("Bits: "     + String(rfBitLength));
    display.setCursor(60, 35);
    display.println("Protocol: " + String(rfReceivedProtocol));
  }

  displayBattery();
  handleModuleButtons(4);
}

// ─── Emulate ──────────────────────────────────────────────────────────────────
// Re-transmits the last captured RF signal using the FS1000A transmitter.
void emulateRf() {
  drawScanBase();
  display.setCursor(33, 30);
  display.println("Sending...");
  displayBattery();
  rfSwitch.send(rfReceivedValue, rfBitLength);
  delay(2000);
}

// ─── Save ─────────────────────────────────────────────────────────────────────
// Saves the current RF signal to the SD card.
//
// File format (one value per line):
//   <rfReceivedValue>
//   <rfBitLength>
//   <rfReceivedProtocol>
//
// Files are auto-numbered: /rf/rf_00.txt … /rf/rf_99.txt
void saveRf() {
  drawScanBase();

  if (isScanning != 0) {
    display.setCursor(30, 30);
    display.println("Nothing to save");
    displayBattery();
    return;
  }

  if (!sdReady) {
    display.setCursor(33, 30);
    display.println("SD Error...");
    displayBattery();
    return;
  }

  display.setCursor(33, 30);
  display.println("Saving...");

  for (int i = 0; i < 100; i++) {
    String path;
    if (i < 10) {
      path = "/rf/rf_0" + String(i) + ".txt";
    } else {
      path = "/rf/rf_"  + String(i) + ".txt";
    }

    if (SD.exists(path)) {
      display.setCursor(33, 30);
      display.println("Already exists");
    } else {
      file = SD.open(path, FILE_WRITE);
      file.println(rfReceivedValue);
      file.println(rfBitLength);
      file.println(rfReceivedProtocol);
      file.close();
      break;
    }
  }

  displayBattery();
}
