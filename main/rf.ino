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

// The rf function handles the RF menu options
void rf() {
  type = 4;  // Set the type to indicate RF functionality
  switch (sceltaSubMenu) {
    case 0:
      subMenuDisplay();  // Display the submenu options
      break;
    case 1:
      scanRf();  // Start the RF scanning process
      break;
    case 2:
      sdMenuDisplay(4);  // Display the SD card menu for RF options
      break;
  }
}

//+=============================================================================
// Scan for RF signals and display the received information
//
void scanRf() {
  if (scanning == 1) {                                // Check if scanning is active
    graficascan();                                    // Update the display with scanning graphics
    if (mySwitch.available()) {                       // Check if a new RF signal is available
      Serial.print("Received ");                      // Print received RF value to Serial Monitor
      Serial.println(mySwitch.getReceivedValue());    // Print the value received
      Serial.print(mySwitch.getReceivedBitlength());  // Print the bit length of the received value
      Serial.println(" bit ");
      Serial.print("Protocol: ");                      // Print the protocol used for the RF signal
      Serial.println(mySwitch.getReceivedProtocol());  // Print the received protocol
      rfvalue = mySwitch.getReceivedValue();           // Store the received value
      rfbit = mySwitch.getReceivedBitlength();         // Store the bit length of the received value
      rfprotocol = mySwitch.getReceivedProtocol();     // Store the protocol used
      mySwitch.resetAvailable();                       // Reset the availability of the RF signal
      scanning = 0;                                    // Stop scanning
    }
  } else {
    scanbase();                                         // Display base information if not scanning
    display.setCursor(25, 25);                          // Set cursor position on the display
    display.println("Value " + String(rfvalue));        // Display the received RF value
    display.setCursor(20, 35);                          // Set cursor position for bit length display
    display.println("Bit " + String(rfbit));            // Display the bit length
    display.setCursor(60, 35);                          // Set cursor position for protocol display
    display.println("Protocol " + String(rfprotocol));  // Display the RF protocol used
  }
  battery();             // Display battery status
  checkModuleButton(4);  // Check the status of the module button
}

// Function to emulate RF transmission
void emulateRf() {
  scanbase();                     // Display base information
  display.setCursor(33, 30);      // Set cursor position to indicate sending
  display.println("Sending...");  // Display sending message
  battery();                      // Display battery status
  mySwitch.send(rfvalue, rfbit);  // Send the stored RF value and bit length
  delay(2000);                    // Wait for 2 seconds to allow transmission to complete
}

// Function to save the received RF data to an SD card
void saveRf() {
  scanbase();                          // Display base information
  if (scanning == 0) {                 // Proceed only if not currently scanning
    if (sdbegin) {                     // Check if SD card is initialized
      display.setCursor(33, 30);       // Set cursor position for saving message
      display.println("Saving...");    // Display saving message
      for (int i = 0; i < 100; i++) {  // Loop to find an available file slot

        String title;  // Variable to hold the file name
        // Generate the file name based on the index
        if (i < 10 && i >= 0) {
          title = "/rf/rf_0" + String(i) + ".txt";  // Format for single-digit index
        } else {
          title = "/rf/rf_" + String(i) + ".txt";  // Format for double-digit index
        }

        // Check if the file already exists on the SD card
        if (SD.exists(title)) {
          // Uncomment below to display message if file exists
          //display.setCursor(33, 30);
          //display.println("Already exists");  // Indicate that the file already exists
        } else {
          file = SD.open(title, FILE_WRITE);  // Open the file for writing
          file.println(rfvalue);              // Write the RF value to the file
          file.println(rfbit);                // Write the bit length to the file
          file.println(rfprotocol);           // Write the protocol to the file
          file.close();                       // Close the file after writing
          break;                              // Exit the loop after saving the data
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
