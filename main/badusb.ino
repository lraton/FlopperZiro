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

// Routes the BadUSB sub-menu pages.
void badUsb() {
  currentModuleType = 1;
  switch (selectedSubMenu) {
    case 0: drawSubMenu();       break;  // Sub-menu selection screen
    case 1: showSelectedBadUsb(); break;  // Show selected script file
    case 2: drawSdMenu(1);       break;  // SD file browser for /badusb/
  }
}

// Displays the name of the selected BadUSB script file and waits for the user
// to press RIGHT to execute it.
void showSelectedBadUsb() {
  drawUsbScreen();
  display.setCursor(33, 30);
  if (sdReady) {
    display.println(String(sdSelectedFile));  // Show the selected filename
  } else {
    display.println("SD Error...");
  }
  displayBattery();
  handleModuleButtons(1);
}

// Reads a DuckyScript-style text file from the SD card and replays it as
// USB HID keyboard events.
void executeBadUsb() {
  drawUsbScreen();
  String filePath = "/badusb/" + String(sdSelectedFile);
  display.setCursor(33, 30);
  if (sdReady) {
    display.println("Sending...");
  } else {
    display.println("SD Error...");
  }
  displayBattery();

  file = SD.open(filePath);
  if (file) {
    Keyboard.begin();

    String lineBuffer = "";
    while (file.available()) {
      char ch = file.read();
      if (ch == '\n') {
        executeScriptLine(lineBuffer);
        lineBuffer = "";
      } else if ((int)ch != 13) {  // Ignore carriage-return characters
        lineBuffer += ch;
      }
    }
    executeScriptLine(lineBuffer);  // Process the final line (no trailing newline)
    file.close();
  }
  Keyboard.end();
}

// Parses and executes one line from a DuckyScript-style script file.
//
// Supported commands:
//   STRING <text>   — types the text as keyboard input
//   DELAY <ms>      — waits for the specified number of milliseconds
//   REM <text>      — comment, ignored
//   <KEY> [<KEY>…]  — presses one or more named keys simultaneously
void executeScriptLine(String line) {
  int firstSpace = line.indexOf(' ');

  if (firstSpace == -1) {
    // Single token — press it as a key
    pressKey(line);
  } else if (line.substring(0, firstSpace) == "STRING") {
    Keyboard.print(line.substring(firstSpace + 1));
  } else if (line.substring(0, firstSpace) == "DELAY") {
    delay(line.substring(firstSpace + 1).toInt());
  } else if (line.substring(0, firstSpace) == "REM") {
    // Comment — do nothing
  } else {
    // Multiple key names separated by spaces: press each in sequence
    String remaining = line;
    while (remaining.length() > 0) {
      int nextSpace = remaining.indexOf(' ');
      if (nextSpace == -1) {
        pressKey(remaining);
        remaining = "";
      } else {
        pressKey(remaining.substring(0, nextSpace));
        remaining = remaining.substring(nextSpace + 1);
      }
      delay(5);
    }
  }

  displayBattery();
  Keyboard.releaseAll();
}

// Maps a DuckyScript key name (or single character) to an Arduino Keyboard press.
void pressKey(String keyName) {
  if (keyName.length() == 1) {
    Keyboard.press(keyName[0]);
  } else if (keyName.equals("ENTER"))                          { Keyboard.press(KEY_RETURN); }
  else if (keyName.equals("CTRL"))                             { Keyboard.press(KEY_LEFT_CTRL); }
  else if (keyName.equals("SHIFT"))                            { Keyboard.press(KEY_LEFT_SHIFT); }
  else if (keyName.equals("ALT"))                              { Keyboard.press(KEY_LEFT_ALT); }
  else if (keyName.equals("GUI"))                              { Keyboard.press(KEY_LEFT_GUI); }
  else if (keyName.equals("UP")    || keyName.equals("UPARROW"))    { Keyboard.press(KEY_UP_ARROW); }
  else if (keyName.equals("DOWN")  || keyName.equals("DOWNARROW"))  { Keyboard.press(KEY_DOWN_ARROW); }
  else if (keyName.equals("LEFT")  || keyName.equals("LEFTARROW"))  { Keyboard.press(KEY_LEFT_ARROW); }
  else if (keyName.equals("RIGHT") || keyName.equals("RIGHTARROW")) { Keyboard.press(KEY_RIGHT_ARROW); }
  else if (keyName.equals("DELETE"))   { Keyboard.press(KEY_DELETE); }
  else if (keyName.equals("PAGEUP"))   { Keyboard.press(KEY_PAGE_UP); }
  else if (keyName.equals("PAGEDOWN")) { Keyboard.press(KEY_PAGE_DOWN); }
  else if (keyName.equals("HOME"))     { Keyboard.press(KEY_HOME); }
  else if (keyName.equals("END"))      { Keyboard.press(KEY_END); }
  else if (keyName.equals("ESC"))      { Keyboard.press(KEY_ESC); }
  else if (keyName.equals("INSERT"))   { Keyboard.press(KEY_INSERT); }
  else if (keyName.equals("TAB"))      { Keyboard.press(KEY_TAB); }
  else if (keyName.equals("CAPSLOCK")) { Keyboard.press(KEY_CAPS_LOCK); }
  else if (keyName.equals("SPACE"))    { Keyboard.press(' '); }
  else if (keyName.equals("F1"))  { Keyboard.press(KEY_F1); }
  else if (keyName.equals("F2"))  { Keyboard.press(KEY_F2); }
  else if (keyName.equals("F3"))  { Keyboard.press(KEY_F3); }
  else if (keyName.equals("F4"))  { Keyboard.press(KEY_F4); }
  else if (keyName.equals("F5"))  { Keyboard.press(KEY_F5); }
  else if (keyName.equals("F6"))  { Keyboard.press(KEY_F6); }
  else if (keyName.equals("F7"))  { Keyboard.press(KEY_F7); }
  else if (keyName.equals("F8"))  { Keyboard.press(KEY_F8); }
  else if (keyName.equals("F9"))  { Keyboard.press(KEY_F9); }
  else if (keyName.equals("F10")) { Keyboard.press(KEY_F10); }
  else if (keyName.equals("F11")) { Keyboard.press(KEY_F11); }
  else if (keyName.equals("F12")) { Keyboard.press(KEY_F12); }
}
