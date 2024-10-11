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

// Function to handle the SD menu display
// It checks if a file is selected, if not, it calls the checkSdButton() function to handle navigation
void sdMenuDisplay(int wichType) {
  if (sceltaSd == 0) {
    checkSdButton();  // Check for button presses in the SD menu
  } else {
    if (sceltaSd >= 1) {
      selectedSd(wichType);  // If a file is selected, proceed to handle that file
    }
  }
}

// Function to display files from the SD card on the screen
// It calculates the number of pages, and current page based on the file count and selected file
void sdDisplay(File dir, int wichType) {
  int i = 1;  // Counter to keep track of files

  int pages = fileCount / 6;  // Calculate the number of pages (6 files per page)
  int add = fileCount % 6;    // Remainder to handle cases where files don't fit perfectly on pages
  if (add > 0) {
    pages = pages + 1;  // Add an extra page if there are leftover files
  }
  int currentPage = selectedFileNumber / 6;     // Calculate the current page based on the selected file number
  int addCurrentPage = selectedFileNumber % 6;  // Check if the selected file fits within a complete page
  if (addCurrentPage > 0) {
    currentPage = currentPage + 1;  // Adjust current page if there's an incomplete page
  }

  // Display the current page and total number of pages in the serial monitor (for debugging purposes)
  Serial.print("Pages:");
  Serial.println(pages);
  Serial.print("Current page:");
  Serial.println(currentPage);

  // Clear the screen and prepare for new content
  display.clearDisplay();
  display.drawBitmap(0, 0, frame, 128, 64, WHITE);  // Draw a frame or border on the screen
  int positionText = 4;                             // Initial Y position for the text
  display.setCursor(10, positionText);              // Set cursor for the display

  // Loop through files and display them on the screen
  while (true) {
    file.openNext(&dir, O_RDONLY);  // Open the next file in the directory
    if (!file || i > currentPage * 6) {
      break;  // Break the loop if there are no more files or if we've reached the last file on the current page
    }
    if (i > currentPage * 6 - 6) {
      file.getName(fileName, sizeof(fileName));  // Get the file name
      if (i == selectedFileNumber) {
        display.print("> ");                // Highlight the selected file with a ">"
        display.println(String(fileName));  // Display the file name
        strcpy(selectedFile, fileName);     // Store the selected file name
      } else {
        display.print("  ");                // Indent for non-selected files
        display.println(String(fileName));  // Display the file name
      }
      file.close();                                             // Close the file after reading its name
      display.setCursor(10, positionText = positionText + 10);  // Move the cursor down for the next file
    }
    i++;  // Increment file counter
  }

  display.display();  // Update the display with the newly rendered content
}

// Function to handle selected file from SD card
// Depending on the "wichType" parameter, different actions are performed based on the file type
void selectedSd(int wichType) {
  switch (wichType) {
    case 1:
      // Action for type 1 can be added here
      break;
    case 2:
      // Action for type 2 can be added here
      break;
    case 3:
      // Action for type 3: Read IR-related data from the selected file
      file = SD.open(String("/ir/") + String(selectedFile));
      if (file) {
        buffer = file.readStringUntil('\n');  // Read the first line from the file
        irproducer = buffer;                  // Store IR producer data
        buffer = file.readStringUntil('\n');  // Read the second line
        data = buffer;                        // Store other data
        buffer = file.readStringUntil('\n');  // Read raw data (as space-separated values)
        memset(rawData, 0, sizeof rawData);   // Clear the rawData array
        int n = 0;                            // Index for parsing the buffer
        for (int k = 0; k < 67; k++) {        // Loop to parse the raw data from the buffer
          String tempData = "";
          while (buffer[n] != ' ') {  // Read until the next space character
            tempData = tempData + buffer[n];
            rawData[k] = tempData.toInt();  // Convert the string to an integer and store it in rawData
            n++;
          }
          n++;  // Move past the space character
        }
        file.close();       // Close the file after reading
        scanning = 0;       // Stop scanning
        sceltaSubMenu = 1;  // Return to the submenu
      }
      break;
    case 4:
      // Action for type 4: Read RF-related data from the selected file
      file = SD.open(String("/rf/") + String(selectedFile));
      if (file) {
        buffer = file.readStringUntil('\n');  // Read the first line
        rfvalue = buffer.toInt();             // Store the RF value
        buffer = file.readStringUntil('\n');  // Read the second line
        rfbit = buffer.toInt();               // Store RF bit data
        buffer = file.readStringUntil('\n');  // Read the third line
        rfprotocol = buffer.toInt();          // Store RF protocol
        file.close();                         // Close the file after reading
        scanning = 0;                         // Stop scanning
        sceltaSubMenu = 1;                    // Return to the submenu
      }
      break;
  }
}

// Function to count the number of files in a directory on the SD card
int countfile(File dir) {
  int fileCount = 0;                       // Initialize file count
  while (file.openNext(&dir, O_RDONLY)) {  // Loop through all files in the directory
    if (!file.isHidden()) {                // Check if the file is not hidden
      fileCount++;                         // Increment file count
    }
    file.close();  // Close the file after counting
  }
  return fileCount;  // Return the total file count
}
