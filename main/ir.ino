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

// The function that handles the IR menu
void ir() {
  type = 3;                 // Set the type to 3 for IR
  switch (sceltaSubMenu) {  // Check the submenu choice
    case 0:
      subMenuDisplay();  // Display the submenu options
      break;
    case 1:
      scanIr();  // Start scanning for IR signals
      break;
    case 2:
      sdMenuDisplay(3);  // Display the SD card menu for saving IR codes
      break;
  }
}

// Function to scan and display IR code information
void scanIr() {
  if (scanning == 1) {                             // If scanning is active
    graficascan();                                 // Display scan graphic/UI
    decode_results results;                        // Create a structure to store the scan results
    if (IrReceiver.decode()) {                     // Decode the IR signal
      dumpInfo();                                  // Print information about the signal
      dumpCode();                                  // Print the raw IR code in source code format
      Serial.println("");                          // Add a blank line between entries
      scanbase();                                  // Update the base scan screen
      display.setCursor(20, 25);                   // Set display cursor for encoding info
      display.println("Encoding: " + irproducer);  // Show the IR encoding type
      display.setCursor(20, 35);                   // Set display cursor for data info
      display.println("Data: 0x" + data);          // Display the decoded data
      Serial.println(irproducer);                  // Print the encoding type to the Serial Monitor
      IrReceiver.resume();                         // Prepare to receive the next IR code
      scanning = 0;                                // Stop scanning
    }
  } else {
    // If scanning is inactive, just display the last known encoding and data
    scanbase();
    display.setCursor(20, 25);
    display.println("Encoding: " + irproducer);
    display.setCursor(20, 35);
    display.println("Data: 0x" + data);
  }
  battery();             // Check and display battery level
  checkModuleButton(3);  // Check for button inputs in module 3 (IR)
}

// Function to print the IR code in a specific format (for Panasonic codes)
void ircode(decode_results *results) {
  // For Panasonic protocol, print the address
  if (results->decode_type == PANASONIC) {
    Serial.print(results->address, HEX);
    Serial.print(":");
  }

  // Print the value of the IR code
  Serial.print(results->value, HEX);
}

// Function to display the encoding type based on the IR protocol
void encoding() {
  switch (IrReceiver.decodedIRData.protocol) {
    default:
    case UNKNOWN:
      irproducer = "UNKNOWN";  // Unknown IR protocol
      Serial.print("UNKNOWN");
      break;
    case NEC:
      irproducer = "NEC";
      Serial.print("NEC");
      break;
    case SONY:
      irproducer = "SONY";
      Serial.print("SONY");
      break;
    case RC5:
      irproducer = "RC5";
      Serial.print("RC5");
      break;
    case RC6:
      irproducer = "RC6";
      Serial.print("RC6");
      break;
    case SHARP:
      irproducer = "SHARP";
      Serial.print("SHARP");
      break;
    case JVC:
      irproducer = "JVC";
      Serial.print("JVC");
      break;
    case BOSEWAVE:
      irproducer = "BOSEWAVE";
      Serial.print("BOSEWAVE");
      break;
    case SAMSUNG:
      irproducer = "SAMSUNG";
      Serial.print("SAMSUNG");
      break;
    case LG:
      irproducer = "LG";
      Serial.print("LG");
      break;
    case WHYNTER:
      irproducer = "WHYNTER";
      Serial.print("WHYNTER");
      break;
    case KASEIKYO:
      irproducer = "KASEIKYO";
      Serial.print("KASEIKYO");
      break;
    case PANASONIC:
      irproducer = "PANASONIC";
      Serial.print("PANASONIC");
      break;
    case DENON:
      irproducer = "DENON";
      Serial.print("Denon");
      break;
  }
}

// Function to display information about the decoded IR signal
void dumpInfo() {
  // Check if the buffer overflowed
  if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
    Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWLEN");
    return;
  }

  // Print the encoding type
  Serial.print("Encoding  : ");
  encoding();
  Serial.println("");

  // Print the code and the bit length of the IR signal
  Serial.print("Code      : 0x");
  Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
  Serial.print(" (");
  Serial.print(IrReceiver.decodedIRData.numberOfBits, DEC);
  Serial.println(" bits)");
}

// Function to display the raw IR code in array form for use in source code
void dumpCode() {
  // Start raw data array declaration
  Serial.print("unsigned int  ");
  Serial.print("rawData[");
  Serial.print(IrReceiver.decodedIRData.rawDataPtr->rawlen - 1, DEC);  // Print the size of the array
  Serial.print("] = {");                                               // Start array declaration

  // Print each value in the raw data buffer
  for (int i = 1; i < IrReceiver.decodedIRData.rawDataPtr->rawlen; i++) {
    rawData[i - 1] = IrReceiver.decodedIRData.rawDataPtr->rawbuf[i] * USECPERTICK;  // Convert timing to microseconds
    Serial.print(IrReceiver.decodedIRData.rawDataPtr->rawbuf[i] * USECPERTICK, DEC);
    if (i < IrReceiver.decodedIRData.rawDataPtr->rawlen - 1) Serial.print(",");  // Add comma between values except for the last one
    if (!(i & 1)) Serial.print(" ");                                             // Add space for readability every two values
  }

  // End array declaration
  Serial.print("};");

  // Add comment with protocol and value
  Serial.print("  // ");
  encoding();
  Serial.print(" ");
  Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

  // Add newline
  Serial.println("");

  // Salvataggio del valore decodificato nella variabile data
  char hexString[17];
  sprintf(hexString, "%lX", IrReceiver.decodedIRData.decodedRawData);
  data = String(hexString);

  Serial.println(data);

  // Se il protocollo è Panasonic, salvare anche l'indirizzo
  if (IrReceiver.decodedIRData.protocol == PANASONIC) {
    Serial.print("unsigned int addr = 0x");
    Serial.print(IrReceiver.decodedIRData.address, HEX);
    Serial.println(";");
  }

  Serial.print("unsigned int data = 0x");
  Serial.println(data + ";");
}


// Function to emulate/simulate sending an IR signal
void emulateIr() {
  scanbase();  // Update the UI for sending
  display.setCursor(33, 30);
  display.println("Sending...");           // Display "Sending..." on the screen
  battery();                               // Display battery level
  IrSender.sendRaw(rawData, 67, freq_ir);  // Send the raw IR code
  delay(2000);                             // Wait for 2 seconds
}

// Function to save an IR code to an SD card
void saveIr() {
  scanbase();           // Update the UI for saving
  if (scanning == 0) {  // Ensure scanning is not active
    if (sdbegin) {      // Check if the SD card is ready
      display.setCursor(33, 30);
      display.println("Saving...");  // Display "Saving..." on the screen

      // Loop through 100 files and find an available filename
      for (int i = 0; i < 100; i++) {
        String title;
        // Generate file name based on the current index
        if (i < 10 && i >= 0) {
          title = "/ir/ir_0" + String(i);  // Create a filename like ir_01.txt
          title = title + ".txt";
        } else {
          title = "/ir/ir_" + String(i);  // Create a filename like ir_10.txt
          title = title + ".txt";
        }

        // Check if the file already exists
        if (SD.exists(title)) {
        } else {
          // Open the file for writing
          file = SD.open(title, FILE_WRITE);
          // Write IR producer and data
          file.println(irproducer);  // Write the encoding type to the file
          file.println(data);        // Write the data to the file

          // Write the raw data values to the file
          for (int i = 0; i < 67; i++) {
            file.print(rawData[i]);  // Print each raw data value
            if (i != 66) {
              file.print(" ");  // Add space between values
            }
          }
          file.print(" ");  // Print an additional space
          file.println();   // Move to the next line
          file.close();     // Close the file after writing
          break;            // Exit the loop after saving the file
        }
      }
    } else {
      // If the SD card is not initialized or has an error
      display.setCursor(33, 30);
      display.println("SD Error...");  // Display an error message
    }
  } else {
    // If scanning is still active, indicate that there's nothing to send
    display.setCursor(30, 30);
    display.println("Nothing to send");  // Display the message
  }
  battery();    // Display battery level
}
