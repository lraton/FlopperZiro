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

// ─── Main menu button handler ─────────────────────────────────────────────────

void handleMenuButtons() {
  if (analogRead(BTN_UP) == 0)         onMenuUp();
  if (digitalRead(BTN_DOWN)   == LOW)  onMenuDown();
  if (digitalRead(BTN_LEFT)   == LOW)  onMenuLeft();
  if (digitalRead(BTN_SELECT) == LOW)  onMenuSelect();
  delay(150);  // Debounce delay
}

// Scroll to the previous main-menu page (minimum page 1).
void onMenuUp() {
  if (menuCurrentPage > 1) menuCurrentPage--;
}

// Scroll to the next main-menu page (maximum MENU_PAGE_COUNT - 1).
void onMenuDown() {
  if (menuCurrentPage < MENU_PAGE_COUNT - 1) menuCurrentPage++;
}

// Return from any module back to the main menu.
void onMenuLeft() {
  selectedSubMenu  = 0;
  menuCurrentPage  = selectedModule;
  selectedModule   = 0;
}

// Enter the module shown on the current main-menu page.
void onMenuSelect() {
  selectedModule = menuCurrentPage;
}

// ─── Sub-menu button handler ──────────────────────────────────────────────────

void handleSubMenuButtons() {
  if (analogRead(BTN_UP) == 0)         onSubMenuUp();
  if (digitalRead(BTN_DOWN)   == LOW)  onSubMenuDown();
  if (digitalRead(BTN_LEFT)   == LOW)  onSubMenuLeft();
  if (digitalRead(BTN_SELECT) == LOW)  onSubMenuSelect();
  delay(150);
}

// Scroll to the previous sub-menu page (wraps around).
void onSubMenuUp() {
  if (subMenuCurrentPage > 0) {
    subMenuCurrentPage--;
  }
  if (subMenuCurrentPage == 0) {
    subMenuCurrentPage = SUB_MENU_PAGE_COUNT - 1;  // Wrap to last page
  }
}

// Scroll to the next sub-menu page (wraps around).
void onSubMenuDown() {
  if (subMenuCurrentPage < SUB_MENU_PAGE_COUNT - 1) {
    subMenuCurrentPage++;
  } else {
    subMenuCurrentPage = 0;  // Wrap to first page
  }
}

// Return from the sub-menu back to the main menu.
void onSubMenuLeft() {
  selectedSubMenu  = 0;
  menuCurrentPage  = selectedModule;
  selectedModule   = 0;
}

// Confirm the highlighted sub-menu option.
// If option 2 (SD browser) is selected, open the matching directory and render the file list.
void onSubMenuSelect() {
  selectedSubMenu = subMenuCurrentPage;

  if (selectedSubMenu == 2) {
    File dir;
    switch (currentModuleType) {
      case 1: dir = SD.open("/badusb/"); break;
      case 2: dir = SD.open("/rfid/");   break;
      case 3: dir = SD.open("/ir/");     break;
      case 4: dir = SD.open("/rf/");     break;
    }
    sdFileCount = countFiles(dir);
    drawSdFileList(dir, currentModuleType);
  }
}

// ─── Module (scan / emulate / save) button handler ───────────────────────────

void handleModuleButtons(int moduleType) {
  // UP — start a new scan (resets any previous result)
  if (analogRead(BTN_UP) == 0) {
    isScanning           = 1;
    rfidDetectionStarted = false;  // Reset the non-blocking RFID state
  }

  // DOWN — save the current signal to SD
  if (digitalRead(BTN_DOWN) == LOW) {
    switch (moduleType) {
      case 1: /* BadUSB has no save step */   break;
      case 2: saveRfid();  break;
      case 3: saveIr();    break;
      case 4: saveRf();    break;
    }
    sdFreePercent = getSdFreePercent();  // Refresh the SD free-space indicator
  }

  // LEFT — exit back to the sub-menu
  if (digitalRead(BTN_LEFT) == LOW) {
    sdFileSelected       = 0;
    sdSelectedFileNum    = 1;
    isScanning           = 1;
    selectedSubMenu      = 0;
    rfidDetectionStarted = false;
  }

  // RIGHT — emulate / replay the current signal
  if (digitalRead(BTN_RIGHT) == LOW) {
    switch (moduleType) {
      case 1: executeBadUsb();  break;
      case 2: emulateRfid();    break;
      case 3: emulateIr();      break;
      case 4: emulateRf();      break;
    }
  }

  // SELECT — reserved for future use
  if (digitalRead(BTN_SELECT) == LOW) { }

  delay(150);  // Debounce delay
}

// ─── SD file browser button handler ──────────────────────────────────────────

void handleSdButtons() {
  // UP — move selection one file up (wraps around)
  if (analogRead(BTN_UP) == 0) {
    File dir;
    switch (currentModuleType) {
      case 1: dir = SD.open("/badusb/"); break;
      case 2: dir = SD.open("/rfid/");   break;
      case 3: dir = SD.open("/ir/");     break;
      case 4: dir = SD.open("/rf/");     break;
    }
    if (sdSelectedFileNum > 1) {
      sdSelectedFileNum--;
      sdFileCount = countFiles(dir);
      drawSdFileList(dir, currentModuleType);
    } else {
      sdSelectedFileNum = sdFileCount;
      drawSdFileList(dir, currentModuleType);
    }
  }

  // DOWN — move selection one file down (wraps around)
  if (digitalRead(BTN_DOWN) == LOW) {
    File dir;
    switch (currentModuleType) {
      case 1: dir = SD.open("/badusb/"); break;
      case 2: dir = SD.open("/rfid/");   break;
      case 3: dir = SD.open("/ir/");     break;
      case 4: dir = SD.open("/rf/");     break;
    }
    if (sdSelectedFileNum < sdFileCount) {
      sdSelectedFileNum++;
      sdFileCount = countFiles(dir);
      drawSdFileList(dir, currentModuleType);
    } else {
      sdSelectedFileNum = 1;
      drawSdFileList(dir, currentModuleType);
    }
  }

  // RIGHT — reserved for future use
  if (digitalRead(BTN_RIGHT) == LOW) { }

  // SELECT — confirm the highlighted file
  if (digitalRead(BTN_SELECT) == LOW) {
    sdFileSelected = sdSelectedFileNum;
    if (currentModuleType == 1) {
      selectedSubMenu = 1;  // BadUSB jumps straight to the execute screen
    }
  }

  // LEFT — cancel and exit the SD browser
  if (digitalRead(BTN_LEFT) == LOW) {
    selectedSubMenu   = 0;
    sdSelectedFileNum = 1;
    sdFileSelected    = 0;
  }

  delay(150);
}
