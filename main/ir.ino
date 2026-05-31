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

// Routes the IR sub-menu pages.
void ir() {
  currentModuleType = 3;
  switch (selectedSubMenu) {
    case 0: drawSubMenu();   break;  // Sub-menu selection screen
    case 1: scanIr();        break;  // Active scan / result display
    case 2: drawSdMenu(3);   break;  // SD file browser for /ir/
  }
}

// ─── Scan ─────────────────────────────────────────────────────────────────────
// Waits for a decoded IR signal and displays the protocol and value.
void scanIr() {
  if (isScanning == 1) {
    drawScanScreen();

    if (IrReceiver.decode()) {
      printIrInfo();
      printIrCode();
      Serial.println();

      drawScanBase();
      display.setCursor(20, 25);
      display.println("Protocol: " + irProtocol);
      display.setCursor(20, 35);
      display.println("Value: 0x" + irHexValue);

      IrReceiver.resume();  // Ready to receive the next signal
      isScanning = 0;
    }
  } else {
    // Display the last captured result.
    drawScanBase();
    display.setCursor(20, 25);
    display.println("Protocol: " + irProtocol);
    display.setCursor(20, 35);
    display.println("Value: 0x" + irHexValue);
  }

  displayBattery();
  handleModuleButtons(3);
}

// ─── Helpers ──────────────────────────────────────────────────────────────────

// Prints the protocol name of a Panasonic-style code to the Serial monitor.
void printIrValue(decode_results *results) {
  if (results->decode_type == PANASONIC) {
    Serial.print(results->address, HEX);
    Serial.print(":");
  }
  Serial.print(results->value, HEX);
}

// Resolves the protocol name and stores it in irProtocol.
void getIrProtocolName() {
  switch (IrReceiver.decodedIRData.protocol) {
    default:
    case UNKNOWN:    irProtocol = "UNKNOWN";   Serial.print("UNKNOWN");   break;
    case NEC:        irProtocol = "NEC";        Serial.print("NEC");       break;
    case SONY:       irProtocol = "SONY";       Serial.print("SONY");      break;
    case RC5:        irProtocol = "RC5";        Serial.print("RC5");       break;
    case RC6:        irProtocol = "RC6";        Serial.print("RC6");       break;
    case SHARP:      irProtocol = "SHARP";      Serial.print("SHARP");     break;
    case JVC:        irProtocol = "JVC";        Serial.print("JVC");       break;
    case BOSEWAVE:   irProtocol = "BOSEWAVE";   Serial.print("BOSEWAVE");  break;
    case SAMSUNG:    irProtocol = "SAMSUNG";    Serial.print("SAMSUNG");   break;
    case LG:         irProtocol = "LG";         Serial.print("LG");        break;
    case WHYNTER:    irProtocol = "WHYNTER";    Serial.print("WHYNTER");   break;
    case KASEIKYO:   irProtocol = "KASEIKYO";   Serial.print("KASEIKYO");  break;
    case PANASONIC:  irProtocol = "PANASONIC";  Serial.print("PANASONIC"); break;
    case DENON:      irProtocol = "DENON";      Serial.print("Denon");     break;
  }
}

// Prints decoded IR signal info (protocol, value, bit count) to the Serial monitor.
void printIrInfo() {
  if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
    Serial.println("IR code too long — increase RAWLEN in IRremoteInt.h");
    return;
  }
  Serial.print("Protocol : ");
  getIrProtocolName();
  Serial.println();
  Serial.print("Value    : 0x");
  Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
  Serial.print(" (");
  Serial.print(IrReceiver.decodedIRData.numberOfBits, DEC);
  Serial.println(" bits)");
}

// Builds irRawData[] from the decoder buffer, stores irHexValue, and prints
// the raw timing array to the Serial monitor in source-code format.
void printIrCode() {
  Serial.print("uint16_t rawData[");
  Serial.print(IrReceiver.decodedIRData.rawDataPtr->rawlen - 1, DEC);
  Serial.print("] = {");

  for (int i = 1; i < IrReceiver.decodedIRData.rawDataPtr->rawlen; i++) {
    irRawData[i - 1] = IrReceiver.decodedIRData.rawDataPtr->rawbuf[i] * USECPERTICK;
    Serial.print(irRawData[i - 1], DEC);
    if (i < IrReceiver.decodedIRData.rawDataPtr->rawlen - 1) Serial.print(",");
    if (!(i & 1)) Serial.print(" ");
  }

  Serial.print("};  // ");
  getIrProtocolName();
  Serial.print(" ");
  Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
  Serial.println();

  // Store the decoded value as a hex string.
  char hexBuf[17];
  sprintf(hexBuf, "%lX", IrReceiver.decodedIRData.decodedRawData);
  irHexValue = String(hexBuf);
  Serial.println(irHexValue);
}

// ─── Emulate ──────────────────────────────────────────────────────────────────
// Re-transmits the last captured IR signal as raw timing data at 38 kHz.
void emulateIr() {
  drawScanBase();
  display.setCursor(33, 30);
  display.println("Sending...");
  displayBattery();
  IrSender.sendRaw(irRawData, 67, irFrequency);
  delay(2000);
}

// ─── Save ─────────────────────────────────────────────────────────────────────
// Saves the current IR signal to the SD card.
//
// File format:
//   Line 1: protocol name
//   Line 2: hex value string
//   Line 3: 67 raw timing values separated by spaces
//
// Files are auto-numbered: /ir/ir_00.txt … /ir/ir_99.txt
void saveIr() {
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
      path = "/ir/ir_0" + String(i) + ".txt";
    } else {
      path = "/ir/ir_"  + String(i) + ".txt";
    }

    if (!SD.exists(path)) {
      file = SD.open(path, FILE_WRITE);
      file.println(irProtocol);
      file.println(irHexValue);

      for (int j = 0; j < 67; j++) {
        file.print(irRawData[j]);
        if (j != 66) file.print(" ");
      }
      file.print(" ");
      file.println();
      file.close();
      break;
    }
  }

  displayBattery();
}
