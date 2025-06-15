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
}

// Function to save RFID data to an SD card
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
        if (SD.exists(title)) {
        } else {
          file = SD.open(title, FILE_WRITE);  // Open the file for writing

          file.println(uidLength);  // Write the uidlenght on the file

          for (int i = 0; i < uidLength; i++) {  // Loop through UID bytes
            if (i + 1 != uidLength) {            // If not the last byte
              file.println(uid[i]);              // Write UID byte
            } else {
              file.println(uid[i]);  // Write last UID byte with new line
            }
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
  delay(2000);  // Wait for 2 seconds before the next action
}
