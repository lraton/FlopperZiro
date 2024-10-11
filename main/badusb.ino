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

// Function that handles the 'BadUSB' feature.
// It selects which submenu option to execute based on the variable `sceltaSubMenu`.
void badusb() {
  type = 1;                 // Sets a global variable to indicate 'BadUSB' operation
  switch (sceltaSubMenu) {  // Switch based on the selected submenu option
    case 0:
      subMenuDisplay();  // Display the submenu for selection
      break;
    case 1:
      selectedbadusb();  // Show the selected 'BadUSB' file
      break;
    case 2:
      sdMenuDisplay(1);  // Display the SD card menu for 'BadUSB' files
      break;
  }
}

// Function to display selected 'BadUSB' file and handle its visual layout
void selectedbadusb() {
  graficausb();   // Set up the graphical user interface for the 'BadUSB' screen
  if (sdbegin) {  // Check if the SD card is initialized
    display.setCursor(33, 30);
    display.println(String(selectedFile));  // Display the name of the selected file
  } else {
    display.setCursor(33, 30);
    display.println("SD Error...");  // Show an error if SD card initialization fails
  }
  battery();             // Display battery status
  checkModuleButton(1);  // Check for button press events in module 1
}

// Function to emulate 'BadUSB' behavior by reading commands from a file and sending them as keyboard inputs
void emulateUsb() {
  graficausb();                                                  // Display the 'BadUSB' graphics
  String DEFAULT_FILE_NAME = "/badusb/" + String(selectedFile);  // Path to the selected 'BadUSB' file
  if (sdbegin) {                                                 // Check if SD card is initialized
    display.setCursor(33, 30);
    display.println("Sending...");  // Show "Sending..." while emulating
  } else {
    display.setCursor(33, 30);
    display.println("SD Error...");  // Display error if SD card fails
  }
  battery();  // Display battery status

  file = SD.open(DEFAULT_FILE_NAME);  // Open the selected file from the SD card
  if (file) {                         // If the file is successfully opened
    Keyboard.begin();                 // Begin keyboard emulation

    String line = "";             // Variable to store each line from the file
    while (file.available()) {    // Loop through the file until the end
      char m = file.read();       // Read each character from the file
      if (m == '\n') {            // If newline character is found, process the line
        Line(line);               // Process the current line
        line = "";                // Clear the line variable for the next line
      } else if ((int)m != 13) {  // Ignore carriage return character (13 in ASCII)
        line += m;                // Append character to the current line
      }
    }
    Line(line);  // Process the last line after reading the file

    file.close();  // Close the file after reading
  }
  Keyboard.end();  // End keyboard emulation
}

// Function to process each line of the 'BadUSB' script and execute corresponding keyboard commands
void Line(String l) {
  int space_1 = l.indexOf(" ");                        // Find the first space in the line
  if (space_1 == -1) {                                 // If no space is found, treat the entire line as a single command
    Press(l);                                          // Press the corresponding key for the command
  } else if (l.substring(0, space_1) == "STRING") {    // If command is "STRING", print the remaining text as keyboard input
    Keyboard.print(l.substring(space_1 + 1));          // Type the string after "STRING"
  } else if (l.substring(0, space_1) == "DELAY") {     // If command is "DELAY", pause for the specified time
    int delaytime = l.substring(space_1 + 1).toInt();  // Convert delay value to an integer
    delay(delaytime);                                  // Delay for the specified time
  } else if (l.substring(0, space_1) == "REM") {
    // Ignore "REM" (comment) lines
  } else {
    String remain = l;  // Handle multiple key presses in a line

    // Process the line by breaking it into individual key presses
    while (remain.length() > 0) {
      int latest_space = remain.indexOf(" ");  // Find the next space to split the commands
      if (latest_space == -1) {                // If no space is found, press the remaining part
        Press(remain);                         // Execute the key press
        remain = "";                           // Clear the string
      } else {
        Press(remain.substring(0, latest_space));     // Press the key before the space
        remain = remain.substring(latest_space + 1);  // Update the remaining part of the string
      }
      delay(5);  // Add a small delay between each key press
    }
  }
  battery();              // Display battery status after each line
  Keyboard.releaseAll();  // Release all pressed keys after processing the line
}

// Function to simulate pressing a key or combination of keys based on the string input
void Press(String b) {
  if (b.length() == 1) {  // If the input is a single character
    char c = b[0];        // Get the character
    Keyboard.press(c);    // Press the corresponding key
  } else if (b.equals("ENTER")) {
    Keyboard.press(KEY_RETURN);  // Press the 'Enter' key
  } else if (b.equals("CTRL")) {
    Keyboard.press(KEY_LEFT_CTRL);  // Press the 'Ctrl' key
  } else if (b.equals("SHIFT")) {
    Keyboard.press(KEY_LEFT_SHIFT);  // Press the 'Shift' key
  } else if (b.equals("ALT")) {
    Keyboard.press(KEY_LEFT_ALT);  // Press the 'Alt' key
  } else if (b.equals("GUI")) {
    Keyboard.press(KEY_LEFT_GUI);  // Press the 'GUI' (Windows or Command) key
  } else if (b.equals("UP") || b.equals("UPARROW")) {
    Keyboard.press(KEY_UP_ARROW);  // Press the 'Up Arrow' key
  } else if (b.equals("DOWN") || b.equals("DOWNARROW")) {
    Keyboard.press(KEY_DOWN_ARROW);  // Press the 'Down Arrow' key
  } else if (b.equals("LEFT") || b.equals("LEFTARROW")) {
    Keyboard.press(KEY_LEFT_ARROW);  // Press the 'Left Arrow' key
  } else if (b.equals("RIGHT") || b.equals("RIGHTARROW")) {
    Keyboard.press(KEY_RIGHT_ARROW);  // Press the 'Right Arrow' key
  } else if (b.equals("DELETE")) {
    Keyboard.press(KEY_DELETE);  // Press the 'Delete' key
  } else if (b.equals("PAGEUP")) {
    Keyboard.press(KEY_PAGE_UP);  // Press the 'Page Up' key
  } else if (b.equals("PAGEDOWN")) {
    Keyboard.press(KEY_PAGE_DOWN);  // Press the 'Page Down' key
  } else if (b.equals("HOME")) {
    Keyboard.press(KEY_HOME);  // Press the 'Home' key
  } else if (b.equals("ESC")) {
    Keyboard.press(KEY_ESC);  // Press the 'Escape' key
  } else if (b.equals("INSERT")) {
    Keyboard.press(KEY_INSERT);  // Press the 'Insert' key
  } else if (b.equals("TAB")) {
    Keyboard.press(KEY_TAB);  // Press the 'Tab' key
  } else if (b.equals("END")) {
    Keyboard.press(KEY_END);  // Press the 'End' key
  } else if (b.equals("CAPSLOCK")) {
    Keyboard.press(KEY_CAPS_LOCK);  // Press the 'Caps Lock' key
  } else if (b.equals("F1")) {
    Keyboard.press(KEY_F1);  // Press the 'F1' key
  } else if (b.equals("F2")) {
    Keyboard.press(KEY_F2);  // Press the 'F2' key
  } else if (b.equals("F3")) {
    Keyboard.press(KEY_F3);  // Press the 'F3' key
  } else if (b.equals("F4")) {
    Keyboard.press(KEY_F4);  // Press the 'F4' key
  } else if (b.equals("F5")) {
    Keyboard.press(KEY_F5);  // Press the 'F5' key
  } else if (b.equals("F6")) {
    Keyboard.press(KEY_F6);  // Press the 'F6' key
  } else if (b.equals("F7")) {
    Keyboard.press(KEY_F7);  // Press the 'F7' key
  } else if (b.equals("F8")) {
    Keyboard.press(KEY_F8);  // Press the 'F8' key
  } else if (b.equals("F9")) {
    Keyboard.press(KEY_F9);  // Press the 'F9' key
  } else if (b.equals("F10")) {
    Keyboard.press(KEY_F10);  // Press the 'F10' key
  } else if (b.equals("F11")) {
    Keyboard.press(KEY_F11);  // Press the 'F11' key
  } else if (b.equals("F12")) {
    Keyboard.press(KEY_F12);  // Press the 'F12' key
  } else if (b.equals("SPACE")) {
    Keyboard.press(' ');  // Press the spacebar
  }
}
