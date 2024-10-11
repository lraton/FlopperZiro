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

// Function to handle button inputs when in the main menu
void checkMenuButton() {
  // Checks if the "Up" button is pressed (via analogRead)
  if (analogRead(buttonUp) == 0) {
    handleUpButton();  // Calls function to handle "Up" action
  }
  // Checks if the "Down" button is pressed (via digitalRead)
  if (digitalRead(buttonDown) == LOW) {
    handleDownButton();  // Calls function to handle "Down" action
  }
  // Checks if the "Left" button is pressed
  if (digitalRead(buttonLeft) == LOW) {
    handleLeftButton();  // Calls function to handle "Left" action
  }
  // Checks if the "Select" button is pressed
  if (digitalRead(buttonSelect) == LOW) {
    handleSelectButton();  // Calls function to handle "Select" action
  }
  // Adds a delay to debounce the buttons
  delay(150);
}

// Function to handle the "Up" button behavior in the main menu
void handleUpButton() {
  // Decrease the current page number if it's greater than the first page
  if (currentPage > 1) {
    currentPage--;
  }
}

// Function to handle the "Down" button behavior in the main menu
void handleDownButton() {
  // Increase the current page number if it's less than the last page
  if (currentPage < numPages - 1) {
    currentPage++;
  }
}

// Function to handle the "Left" button behavior in the main menu
void handleLeftButton() {
  // Resets submenu choice and returns to the main menu
  sceltaSubMenu = 0;
  currentPage = scelta;
  scelta = 0;
}

// Function to handle the "Select" button behavior in the main menu
void handleSelectButton() {
  // Stores the current page as the selected option
  scelta = currentPage;
}

////////////////// Button handling for the second menu ///////////////////

void checkSubMenuButton() {
  // Same logic as the main menu buttons but for the submenu
  if (analogRead(buttonUp) == 0) {
    handleSubMenuUpButton();  // Calls "Up" action for the submenu
  }
  if (digitalRead(buttonDown) == LOW) {
    handleSubMenuDownButton();  // Calls "Down" action for the submenu
  }
  if (digitalRead(buttonLeft) == LOW) {
    handleSubMenuLeftButton();  // Calls "Left" action for the submenu
  }
  if (digitalRead(buttonSelect) == LOW) {
    handleSubMenuSelectButton();  // Calls "Select" action for the submenu
  }
  delay(150);  // Adds delay for button debounce
}

// Function to handle the "Up" button behavior in the submenu
void handleSubMenuUpButton() {
  // Navigate through submenu pages in a circular way
  if (currentPageSubMenu > 0) {
    currentPageSubMenu--;  // Decrease page
  }
  if (currentPageSubMenu == 0) {
    currentPageSubMenu = numPagesSubMenu - 1;  // Wrap around to the last page
  }
}

// Function to handle the "Down" button behavior in the submenu
void handleSubMenuDownButton() {
  // Navigate through submenu pages in a circular way
  if (currentPageSubMenu < numPagesSubMenu - 1) {
    currentPageSubMenu++;  // Increase page
  } else {
    currentPageSubMenu = 0;  // Wrap around to the first page
  }
}

// Function to handle the "Left" button behavior in the submenu
void handleSubMenuLeftButton() {
  // Reset submenu and return to main menu
  sceltaSubMenu = 0;
  currentPage = scelta;
  scelta = 0;
}

// Function to handle the "Select" button behavior in the submenu
void handleSubMenuSelectButton() {
  // Store the current submenu page as the selection
  sceltaSubMenu = currentPageSubMenu;

  // If the selected submenu option is 2, open a directory based on the type
  if (sceltaSubMenu == 2) {
    File dir;  // Declare a file object
    switch (type) {
      case 1:
        dir = SD.open("/badusb/");  // Open the "badusb" directory if type is 1
        break;
      case 2:
        dir = SD.open("/rfid/");  // Open the "rfid" directory if type is 2
        break;
      case 3:
        dir = SD.open("/ir/");  // Open the "ir" directory if type is 3
        break;
      case 4:
        dir = SD.open("/rf/");  // Open the "rf" directory if type is 4
        break;
    }
    // Count files in the opened directory and display them
    fileCount = countfile(dir);
    sdDisplay(dir, type);
  }
}

//////////////////// Button handling for the last module menu //////////////////////////////

void checkModuleButton(int wichMenu) {
  // Start scanning if the "Up" button is pressed
  if (analogRead(buttonUp) == 0) {
    scanning = 1;
  }
  // Execute different save functions depending on the active module (whichMenu)
  if (digitalRead(buttonDown) == LOW) {
    switch (wichMenu) {
      case 1:
        break;  // No action for menu 1
      case 2:
        saveRfid();  // Save RFID data for menu 2
        break;
      case 3:
        saveIr();  // Save IR data for menu 3
        break;
      case 4:
        saveRf();  // Save RF data for menu 4
        break;
    }
  }
  // Handle "Left" button behavior to reset scanning and submenu for each module
  if (digitalRead(buttonLeft) == LOW) {
    sceltaSd = 0;
    selectedFileNumber = 1;
    scanning = 1;
    sceltaSubMenu = 0;
  }
  // Execute emulation functions depending on the module
  if (digitalRead(buttonRight) == LOW) {
    switch (wichMenu) {
      case 1:
        emulateUsb();  // Emulate USB for menu 1
        break;
      case 2:
        emulateRfid();  // Emulate RFID for menu 2
        break;
      case 3:
        emulateIr();  // Emulate IR for menu 3
        break;
      case 4:
        emulateRf();  // Emulate RF for menu 4
        break;
    }
  }
  if (digitalRead(buttonSelect) == LOW) {
    // No action currently set for "Select" in this menu
  }
  delay(150);  // Delay to debounce buttons
}

/////////////////// Button handling for the SD card menu ///////////////////////

void checkSdButton() {
  // Navigate up through files on the SD card when "Up" button is pressed
  if (analogRead(buttonUp) == 0) {
    File dir;  // Declare a file object
    switch (type) {
      case 1:
        dir = SD.open("/badusb/");  // Open the "badusb" directory if type is 1
        break;
      case 2:
        dir = SD.open("/rfid/");  // Open the "rfid" directory if type is 2
        break;
      case 3:
        dir = SD.open("/ir/");  // Open the "ir" directory if type is 3
        break;
      case 4:
        dir = SD.open("/rf/");  // Open the "rf" directory if type is 4
        break;
    }
    // Decrease file selection number or wrap around to the last file
    if (selectedFileNumber > 1) {
      selectedFileNumber--;
      fileCount = countfile(dir);
      sdDisplay(dir, type);
    } else {
      selectedFileNumber = fileCount;
      sdDisplay(dir, type);
    }
  }
  // Navigate down through files on the SD card when "Down" button is pressed
  if (digitalRead(buttonDown) == LOW) {
    File dir;
    switch (type) {
      case 1:
        dir = SD.open("/badusb/");
        break;
      case 2:
        dir = SD.open("/rfid/");
        break;
      case 3:
        dir = SD.open("/ir/");
        break;
      case 4:
        dir = SD.open("/rf/");
        break;
    }
    // Increase file selection number or wrap around to the first file
    if (selectedFileNumber < fileCount) {
      selectedFileNumber++;
      fileCount = countfile(dir);
      sdDisplay(dir, type);
    } else {
      selectedFileNumber = 1;
      sdDisplay(dir, type);
    }
  }

  // Right button doesn't seem to have a function defined in this section
  if (digitalRead(buttonRight) == LOW) {
    // Add any desired function here
  }

  // When "Select" is pressed, set the selected file as the chosen one
  if (digitalRead(buttonSelect) == LOW) {
    sceltaSd = selectedFileNumber;
    if (type == 1) {
      sceltaSubMenu = 1;  // Specific action for type 1 in SD menu
    }
  }

  // When "Left" button is pressed, reset the file selection and exit the SD menu
  if (digitalRead(buttonLeft) == LOW) {
    sceltaSubMenu = 0;
    selectedFileNumber = 1;
    sceltaSd = 0;
  }

  // Delay for button debouncing
  delay(150);
}
