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

// The rfid function handles the RFID menu options
void rfid() {
  type = 2;  // Set the type to indicate RFID functionality
  switch (sceltaSubMenu) {
    case 0:
      subMenuDisplay();  // Display the submenu options
      break;
    case 1:
      scanRfid();  // Start the RFID scanning process
      break;
    case 2:
      sdMenuDisplay(2);  // Display the SD card menu for RFID options
      break;
  }
}

//+=============================================================================
// Scan for RFID tags and display the received information
//
void scanRfid() {
  if (scanning == 1) {  // Check if scanning is active
    graficascan();      // Update the display with scanning graphics

    // Universal key for NDEF and Mifare Classic communication
    uint8_t keyuniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t success;  // Variable to hold success status of the RFID read operation

    //Non-blocking reding

    if (!detectionStarted) {
      nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);
      detectionStarted = true;
    }

    success = nfc.readDetectedPassiveTargetID(uid, &uidLength);

    if (success) {  // If a tag is found
      scanbase();   // Display base information
      // Display some basic information about the found card
      Serial.println("Found an ISO14443A card");  // Print card type to Serial Monitor
      Serial.print("  UID Length: ");             // Print UID length message
      Serial.print(uidLength, DEC);               // Print UID length in decimal
      Serial.println(" bytes");                   // Print bytes label
      Serial.print("  UID Value: ");              // Print UID value message
      nfc.PrintHex(uid, uidLength);               // Print UID value in hexadecimal format
      Serial.println("");                         // New line for clarity

      // Display UID value on the screen
      display.setCursor(20, 25);             // Set cursor position for UID display
      display.print("UID: ");                // Print UID label
      for (int i = 0; i < uidLength; i++) {  // Loop through UID bytes
        if (i + 1 != uidLength) {            // If not the last byte
          display.print(uid[i], HEX);        // Print UID byte
          display.print(" ");                // Print UID byte
        } else {
          display.println(uid[i], HEX);  // Print last UID byte with new line
        }
      }

      switch (uidLength) {
        case 4:
          // We probably have a Mifare Classic card ...
          Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

          cardType = "Mifare Classic";

          // Now we need to try to authenticate it for read/write access
          // Try with the factory default keyuniversal: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
          Serial.println("Trying to authenticate block 4 with default keyuniversal value");

          // Start with block 4 (the first block of sector 1) since sector 0
          // contains the manufacturer data and it's probably better just
          // to leave it alone unless you know what you're doing
          success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keyuniversal);

          if (success) {
            Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
            uint8_t data[16];

            // Try to read the contents of block 4
            success = nfc.mifareclassic_ReadDataBlock(4, data);

            if (success) {
              // Data seems to have been read ... spit it out
              Serial.println("Reading Block 4:");
              nfc.PrintHexChar(data, 16);
              Serial.println("");

            } else {
              Serial.println("Ooops ... unable to read the requested block.  Try another key?");
            }
          } else {
            Serial.println("Ooops ... authentication failed: Try another key?");
          }
          break;
        case 7:
          // We probably have a Mifare Ultralight card ...
          Serial.println("Seems to be a Mifare Ultralight or NTAG2xx tag (7 byte UID)");

          Serial.println("Trying Mifare Ultralight tag (7 byte UID)");

          cardType = "Mifare UltraLight";

          // Try to read the first general-purpose user page (#4)
          Serial.println("Reading page 4");
          uint8_t data[32];
          success = nfc.mifareultralight_ReadPage(4, data);
          if (success) {
            // Data seems to have been read ... spit it out
            nfc.PrintHexChar(data, 4);
            Serial.println("");
          } else {
            Serial.println("Ooops ... unable to read the requested page Mifare!?");

            // We probably have an NTAG2xx card (though it could be Ultralight as well)
            Serial.println("Trying NTAG2xx tag (7 byte UID)");

            cardType = "NTAG2xx";

            for (uint8_t i = 0; i < 42; i++) {
              success = nfc.ntag2xx_ReadPage(i, data);

              // Display the current page number
              Serial.print("PAGE ");
              if (i < 10) {
                Serial.print("0");
                Serial.print(i);
              } else {
                Serial.print(i);
              }
              Serial.print(": ");

              // Display the results, depending on 'success'
              if (success) {
                // Dump the page data
                nfc.PrintHexChar(data, 4);
              } else {
                Serial.println("Unable to read the requested page!");
              }
            }
          }
          break;
      }

      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(cardType + " " + String(uidLength) + " B", 0, 0, &x1, &y1, &w, &h);
      int16_t x = (SCREEN_WIDTH - w) / 2;                        // Calculate x position to center the text
      display.setCursor(x, 35);                                  // Center the text vertically
      display.print(cardType + " " + String(uidLength) + " B");  // Display UID length

      scanning = 0;  // Stop scanning after reading UID
    }
  } else {
    scanbase();  // Display base information if not currently scanning
    // Display UID value on the screen
    display.setCursor(20, 25);             // Set cursor position for UID display
    display.print("UID: ");                // Print UID label
    for (int i = 0; i < uidLength; i++) {  // Loop through UID bytes
      if (i + 1 != uidLength) {            // If not the last byte
        display.print(uid[i], HEX);        // Print UID byte
        display.print(" ");                // Print UID byte
      } else {
        display.println(uid[i], HEX);  // Print last UID byte with new line
      }
    }

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(cardType + " " + String(uidLength) + " B", 0, 0, &x1, &y1, &w, &h);
    int16_t x = (SCREEN_WIDTH - w) / 2;                        // Calculate x position to center the text
    display.setCursor(x, 35);                                  // Center the text vertically
    display.print(cardType + " " + String(uidLength) + " B");  // Display UID length
  }

  battery();             // Display battery status
  checkModuleButton(2);  // Check the status of the module button
}

// Function to emulate RFID transmission
// Uses PN532 tgInitAsTarget to present as a passive ISO14443A card.
// Supports:
//   4-byte UID  → Mifare Classic  (ATQA=0x0004, SAK=0x08)
//   7-byte UID  → NTAG/Ultralight (ATQA=0x0044, SAK=0x00)
void emulateRfid() {
  if (uidLength == 0) {
    // No tag scanned/loaded yet — tell the user
    scanbase();
    display.setCursor(20, 30);
    display.print("Scan first!");
    display.display();
    battery();
    return;
  }

  // ---- Build UI ----
  scanbase();
  display.setCursor(33, 20);
  display.println("Emulating...");
  display.setCursor(10, 30);
  display.print("UID: ");
  for (int i = 0; i < uidLength; i++) {
    if (uid[i] < 0x10) display.print("0");  // zero-pad single hex digit
    display.print(uid[i], HEX);
    if (i < uidLength - 1) display.print(" ");
  }
  display.display();

  // ---- Build tgInitAsTarget params buffer ----
  //  Byte 0   : Mode (0x00 = passive-only, accept ISO14443A/Mifare)
  //  Bytes 1-2: SENS_RES / ATQA (2 bytes)
  //  Byte 3   : SEL_RES  / SAK  (1 byte)
  //  Byte 4   : NFCIDLength     (4 or 7)
  //  Bytes 5..: NFCID1 bytes
  uint8_t params[13];
  uint8_t paramLen;

  if (uidLength == 4) {
    // Mifare Classic 1K — ATQA 0x00 0x04, SAK 0x08
    params[0] = 0x00;         // mode: passive
    params[1] = 0x04;         // ATQA[0]
    params[2] = 0x00;         // ATQA[1]
    params[3] = 0x08;         // SAK — Mifare Classic 1K
    params[4] = 0x04;         // NFCIDLength = 4
    params[5] = uid[0];
    params[6] = uid[1];
    params[7] = uid[2];
    params[8] = uid[3];
    paramLen  = 9;
  } else {
    // 7-byte UID: NTAG / Mifare Ultralight — ATQA 0x00 0x44, SAK 0x00
    params[0]  = 0x00;        // mode: passive
    params[1]  = 0x44;        // ATQA[0]
    params[2]  = 0x00;        // ATQA[1]
    params[3]  = 0x00;        // SAK — NTAG / Ultralight
    params[4]  = 0x07;        // NFCIDLength = 7
    params[5]  = uid[0];
    params[6]  = uid[1];
    params[7]  = uid[2];
    params[8]  = uid[3];
    params[9]  = uid[4];
    params[10] = uid[5];
    params[11] = uid[6];
    paramLen   = 12;
  }

  // ---- Present as target (blocking up to 3 s, then return) ----
  // tgInitAsTarget returns true when an initiator (reader) contacts us.
  // With timeout=3000 ms we hand control back quickly if no reader is near,
  // which keeps the button-check loop responsive.
  nfc.tgInitAsTarget(params, paramLen, 3000);

  battery();
}

// Function to save RFID data to an SD card.
// File format (one value per line):
//   <cardType>
//   <uidLength>
//   <uid[0]>
//   <uid[1]>
//   ...
//   <uid[uidLength-1]>
void saveRfid() {
  scanbase();                          // Display base information
  if (scanning == 0) {                 // Proceed only if not currently scanning
    if (sdbegin) {                     // Check if SD card is initialized
      display.setCursor(33, 30);       // Set cursor position for saving message
      display.println("Saving...");    // Display saving message
      for (int i = 0; i < 100; i++) {  // Loop to find an available file slot

        String title;  // Variable to hold the file name
        // Generate the file name based on the index
        if (i < 10 && i >= 0) {
          title = "/rfid/rfid_0" + String(i) + ".txt";  // Format for single-digit index
        } else {
          title = "/rfid/rfid_" + String(i) + ".txt";  // Format for double-digit index
        }

        // Check if the file already exists on the SD card
        if (!SD.exists(title)) {
          file = SD.open(title, FILE_WRITE);  // Open the file for writing

          file.println(cardType);    // Line 1: card type string
          file.println(uidLength);   // Line 2: UID length in bytes

          // Lines 3+: one UID byte per line as decimal integer
          // NOTE: uid[j] is uint8_t — do NOT use uid[j] + " " (arithmetic!)
          for (int j = 0; j < uidLength; j++) {
            file.println(uid[j]);
          }

          file.close();  // Close the file after writing
          break;         // Exit the loop after saving the data
        }
      }
    } else {
      display.setCursor(33, 30);       // Set cursor position for SD error message
      display.println("SD Error...");  // Display SD card error message
    }
  } else {
    display.setCursor(30, 30);           // Set cursor position for no data message
    display.println("Nothing to send");  // Indicate there is no RF data to send
  }
  battery();    // Display battery status
}
