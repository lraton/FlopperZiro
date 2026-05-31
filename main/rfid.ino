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

// Routes the RFID sub-menu pages.
void rfid() {
  currentModuleType = 2;
  switch (selectedSubMenu) {
    case 0: drawSubMenu();    break;  // Sub-menu selection screen
    case 1: scanRfid();       break;  // Active scan / result display
    case 2: drawSdMenu(2);    break;  // SD file browser for /rfid/
  }
}

// ─── Scan ─────────────────────────────────────────────────────────────────────
// Non-blocking passive target detection.
// Starts a detection round on the first call, then polls for a result on
// subsequent calls without blocking the display update loop.
void scanRfid() {
  if (isScanning == 1) {
    drawScanScreen();

    uint8_t keyUniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    // Kick off detection if not already started.
    if (!rfidDetectionStarted) {
      nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);
      rfidDetectionStarted = true;
    }

    bool tagFound = nfc.readDetectedPassiveTargetID(rfidUid, &rfidUidLen);

    if (tagFound) {
      drawScanBase();

      // ── Serial debug output ────────────────────────────────────────────────
      Serial.println("Found an ISO14443A card");
      Serial.print("  UID Length: ");
      Serial.print(rfidUidLen, DEC);
      Serial.println(" bytes");
      Serial.print("  UID Value: ");
      nfc.PrintHex(rfidUid, rfidUidLen);
      Serial.println();

      // ── Display UID ────────────────────────────────────────────────────────
      display.setCursor(20, 25);
      display.print("UID: ");
      for (int i = 0; i < rfidUidLen; i++) {
        display.print(rfidUid[i], HEX);
        if (i + 1 < rfidUidLen) display.print(" ");
        else                    display.println();
      }

      // ── Card-type-specific handling ────────────────────────────────────────
      switch (rfidUidLen) {
        case 4: {
          // Likely a Mifare Classic card
          Serial.println("Seems to be a Mifare Classic card (4-byte UID)");
          rfidCardType = "Mifare Classic";

          Serial.println("Trying to authenticate block 4 with default key");
          bool authOk = nfc.mifareclassic_AuthenticateBlock(
              rfidUid, rfidUidLen, 4, 0, keyUniversal);

          if (authOk) {
            Serial.println("Sector 1 (Blocks 4-7) authenticated");
            uint8_t blockData[16];
            if (nfc.mifareclassic_ReadDataBlock(4, blockData)) {
              Serial.println("Block 4 data:");
              nfc.PrintHexChar(blockData, 16);
            } else {
              Serial.println("Unable to read block 4 — try another key?");
            }
          } else {
            Serial.println("Authentication failed — try another key?");
          }
          break;
        }

        case 7: {
          // Likely Mifare Ultralight or NTAG2xx
          Serial.println("Seems to be a Mifare Ultralight or NTAG2xx (7-byte UID)");
          rfidCardType = "Mifare UltraLight";

          uint8_t pageData[32];
          if (nfc.mifareultralight_ReadPage(4, pageData)) {
            Serial.println("Mifare Ultralight page 4:");
            nfc.PrintHexChar(pageData, 4);
          } else {
            Serial.println("Ultralight read failed — trying NTAG2xx");
            rfidCardType = "NTAG2xx";

            for (uint8_t page = 0; page < 42; page++) {
              Serial.print("PAGE ");
              if (page < 10) Serial.print("0");
              Serial.print(page);
              Serial.print(": ");

              if (nfc.ntag2xx_ReadPage(page, pageData)) {
                nfc.PrintHexChar(pageData, 4);
              } else {
                Serial.println("Unable to read page");
              }
            }
          }
          break;
        }
      }

      // ── Display card type ──────────────────────────────────────────────────
      String cardLabel = rfidCardType + " " + String(rfidUidLen) + " B";
      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(cardLabel, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH - w) / 2, 35);
      display.print(cardLabel);

      isScanning = 0;  // Stop scanning — result is now on screen
    }

  } else {
    // ── Result display (not scanning) ─────────────────────────────────────────
    drawScanBase();
    display.setCursor(20, 25);
    display.print("UID: ");
    for (int i = 0; i < rfidUidLen; i++) {
      display.print(rfidUid[i], HEX);
      if (i + 1 < rfidUidLen) display.print(" ");
      else                    display.println();
    }

    String cardLabel = rfidCardType + " " + String(rfidUidLen) + " B";
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(cardLabel, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 35);
    display.print(cardLabel);
  }

  displayBattery();
  handleModuleButtons(2);
}

// --- Emulate -----------------------------------------------------------------
// Sends the PN532 TgInitAsTarget command (code 0x8C) via the Adafruit library's
// low-level sendCommandCheckAck / readResponse API.
//
// The Adafruit_PN532 library (v1.3.3) does NOT expose tgInitAsTarget() as a
// named method, but the raw command pipe is fully public.
//
// PN532 TgInitAsTarget command layout (UM10232 section 7.3.9):
//   cmd[0]    : 0x8C  -- command code
//   cmd[1]    : Mode  -- 0x00 = passive-only (ISO14443A / Mifare)
//   cmd[2..20]: MifareParams (19 bytes)
//                ATQA[0], ATQA[1]  (2 bytes)
//                SAK               (1 byte)
//                NFCIDLength       (1 byte, must be 4 or 7)
//                NFCID1            (NFCIDLength bytes)
//                zeros             (padding to 19 bytes total)
//   cmd[21..38]: FeliCaCodes (18 bytes, all 0 for Type-A only)
//
// Card emulation parameters:
//   4-byte UID -- Mifare Classic  (ATQA = 0x04 0x00, SAK = 0x08)
//   7-byte UID -- NTAG/Ultralight (ATQA = 0x44 0x00, SAK = 0x00)
//
// Blocks for up to 3 s waiting for a reader, then returns.
void emulateRfid() {
  if (rfidUidLen == 0) {
    // No tag has been scanned or loaded yet.
    drawScanBase();
    display.setCursor(20, 30);
    display.print("Scan first!");
    display.display();
    displayBattery();
    return;
  }

  // Build the OLED display
  drawScanBase();
  display.setCursor(33, 20);
  display.println("Emulating...");
  display.setCursor(10, 30);
  display.print("UID: ");
  for (int i = 0; i < rfidUidLen; i++) {
    if (rfidUid[i] < 0x10) display.print("0");  // Zero-pad single hex digit
    display.print(rfidUid[i], HEX);
    if (i < rfidUidLen - 1) display.print(" ");
  }
  display.display();

  // Build the raw TgInitAsTarget command
  // Total: 1 (cmd code) + 1 (mode) + 19 (MifareParams) + 18 (FeliCaCodes) = 39 bytes
  uint8_t cmd[39];
  memset(cmd, 0x00, sizeof(cmd));

  cmd[0] = 0x8C;  // TgInitAsTarget command code
  cmd[1] = 0x00;  // Mode: passive-only

  // MifareParams starts at index 2, exactly 19 bytes.
  // Layout: ATQA(2) + SAK(1) + NFCIDLen(1) + NFCID1(<=10) + zeros(padding)
  if (rfidUidLen == 4) {
    cmd[2] = 0x04;          // ATQA[0] -- Mifare Classic
    cmd[3] = 0x00;          // ATQA[1]
    cmd[4] = 0x08;          // SAK     -- Mifare Classic 1K
    cmd[5] = 0x04;          // NFCIDLength = 4
    cmd[6] = rfidUid[0];
    cmd[7] = rfidUid[1];
    cmd[8] = rfidUid[2];
    cmd[9] = rfidUid[3];
    // Bytes 10-20: padding (already 0x00)
  } else {
    // 7-byte UID: NTAG / Mifare Ultralight
    cmd[2]  = 0x44;         // ATQA[0] -- NTAG / Ultralight
    cmd[3]  = 0x00;         // ATQA[1]
    cmd[4]  = 0x00;         // SAK     -- NTAG / Ultralight
    cmd[5]  = 0x07;         // NFCIDLength = 7
    cmd[6]  = rfidUid[0];
    cmd[7]  = rfidUid[1];
    cmd[8]  = rfidUid[2];
    cmd[9]  = rfidUid[3];
    cmd[10] = rfidUid[4];
    cmd[11] = rfidUid[5];
    cmd[12] = rfidUid[6];
    // Bytes 13-20: padding (already 0x00)
  }
  // Bytes 21-38: FeliCaCodes (18 bytes, all 0x00 -- already zeroed by memset)

  // Send command and wait for a reader to contact us.
  // sendCommandCheckAck transmits the raw PN532 frame and waits for:
  //   1. ACK from the PN532   (fast)
  //   2. Response-ready signal (arrives when a reader initiates communication
  //      or when the 3 s timeout elapses)
  if (nfc.sendCommandCheckAck(cmd, sizeof(cmd), 3000)) {
    // nfc.readResponse() is private in Adafruit_PN532 v1.3.3, so we drain
    // the PN532's response buffer directly via I2C to keep the bus clean.
    // PN532_I2C_ADDRESS (0x24) is a public #define in Adafruit_PN532.h.
    Wire.requestFrom((uint8_t)PN532_I2C_ADDRESS, (uint8_t)20);
    while (Wire.available()) Wire.read();
  }

  displayBattery();
}


// ─── Save ─────────────────────────────────────────────────────────────────────
// Saves the current tag data to the SD card.
//
// File format (one value per line):
//   <rfidCardType>
//   <rfidUidLen>
//   <rfidUid[0]>
//   …
//   <rfidUid[rfidUidLen-1]>
//
// Files are auto-numbered: /rfid/rfid_00.txt … /rfid/rfid_99.txt
void saveRfid() {
  drawScanBase();

  if (isScanning != 0) {
    // Nothing has been scanned yet.
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
      path = "/rfid/rfid_0" + String(i) + ".txt";
    } else {
      path = "/rfid/rfid_"  + String(i) + ".txt";
    }

    if (!SD.exists(path)) {
      file = SD.open(path, FILE_WRITE);
      file.println(rfidCardType);  // Line 1: card type
      file.println(rfidUidLen);    // Line 2: UID length

      // Lines 3+: one UID byte per line as a decimal integer.
      // IMPORTANT: rfidUid[j] is uint8_t — writing rfidUid[j] + " " would
      // perform integer arithmetic (adds 32), not string concatenation.
      for (int j = 0; j < rfidUidLen; j++) {
        file.println(rfidUid[j]);
      }

      file.close();
      break;
    }
  }

  displayBattery();
}
