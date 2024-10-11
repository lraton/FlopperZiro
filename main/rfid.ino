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
    battery();          // Display the battery status

    // Universal key for NDEF and Mifare Classic communication
    uint8_t keyuniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t success;                                                             // Variable to hold success status of the RFID read operation
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);  // Read RFID tag

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
          display.print(uid[i]);             // Print UID byte
        } else {
          display.println(uid[i]);  // Print last UID byte with new line
        }
      }
      display.setCursor(15, 35);                                 // Set cursor position for length display
      display.print("Length: " + String(uidLength) + " Bytes");  // Display UID length
      scanning = 0;                                              // Stop scanning after reading UID
    }
  } else {
    scanbase();                            // Display base information if not currently scanning
    display.setCursor(20, 25);             // Set cursor position for UID display
    display.print("UID: ");                // Print UID label
    for (int i = 0; i < uidLength; i++) {  // Loop through UID bytes
      if (i + 1 != uidLength) {            // If not the last byte
        display.print(uid[i]);             // Print UID byte
      } else {
        display.println(uid[i]);  // Print last UID byte with new line
      }
    }
    display.setCursor(15, 35);                                 // Set cursor position for length display
    display.print("Length: " + String(uidLength) + " Bytes");  // Display UID length
  }
  battery();             // Display battery status
  checkModuleButton(2);  // Check the status of the module button
}

// Function to emulate RFID transmission
void emulateRfid() {
  uint8_t apdubuffer[255] = {}, apdulen = 0;  // Buffer for APDU data and its length
  // PPSE APDU data to mimic a smart card response
  uint8_t ppse[] = { 0x8E, 0x6F, 0x23, 0x84, 0x0E, 0x32, 0x50, 0x41, 0x59, 0x2E,
                     0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31,
                     0xA5, 0x11, 0xBF, 0x0C, 0x0E, 0x61, 0x0C, 0x4F, 0x07,
                     0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10, 0x87,
                     0x01, 0x01, 0x90, 0x00 };
  nfc.AsTarget();                                 // Set the NFC device as the target
  (void)nfc.getDataTarget(apdubuffer, &apdulen);  // Read initial APDU data
  if (apdulen > 0) {                              // If APDU data is received
    for (uint8_t i = 0; i < apdulen; i++) {       // Loop through the APDU data
      Serial.print(" 0x");                        // Print prefix for hex values
      Serial.print(apdubuffer[i], HEX);           // Print APDU byte in hexadecimal
    }
    Serial.println("");  // New line for clarity
  }
  nfc.setDataTarget(ppse, sizeof(ppse));     // Mimic a smart card response with PPSE APDU
  nfc.getDataTarget(apdubuffer, &apdulen);   // Read response from the Point of Sale (PoS) or terminal
  if (apdulen > 0) {                         // If APDU response data is received
    for (uint8_t i = 0; i < apdulen; i++) {  // Loop through the response data
      Serial.print(" 0x");                   // Print prefix for hex values
      Serial.print(apdubuffer[i], HEX);      // Print response byte in hexadecimal
    }
    Serial.println("");  // New line for clarity
  }
  delay(1000);  // Wait for 1 second before the next operation
}

// Function to save RFID data to an SD card
void saveRfid() {
  scanbase();                        // Display base information
  if (scanning == 0) {               // Proceed only if not currently scanning
    if (sdbegin) {                   // Check if SD card is initialized
      display.setCursor(33, 30);     // Set cursor position for saving message
      display.println("Saving...");  // Display saving message
      // Check if the file "prova.txt" already exists on the SD card
      if (SD.exists("ir/prova.txt")) {
        Serial.println("Already exists");  // Print message indicating the file already exists
      } else {
        file = SD.open("ir/prova.txt", FILE_WRITE);  // Open the file for writing
        for (int i = 0; i < 67; i++) {               // Loop to write data to the file
          file.print("ciao");                        // Write "ciao" to the file
        }
        file.close();  // Close the file after writing
      }
    } else {
      display.setCursor(33, 30);       // Set cursor position for SD error message
      display.println("SD Error...");  // Display SD card error message
    }
  } else {
    display.setCursor(30, 30);           // Set cursor position for no data message
    display.println("Nothing to send");  // Indicate there is no RFID data to send
  }
  battery();    // Display battery status
  delay(2000);  // Wait for 2 seconds before the next action
}
