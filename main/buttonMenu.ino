//all button function when im in the menu
void checkMenuButton() {
  if (analogRead(buttonUp) == 0) {
    handleUpButton();
  }
  if (digitalRead(buttonDown) == LOW) {
    handleDownButton();
  }
  if (digitalRead(buttonLeft) == LOW) {
    handleLeftButton();
  }
  if (digitalRead(buttonSelect) == LOW) {
    handleSelectButton();
  }
  delay(150);
}

void handleUpButton() {
  if (currentPage > 1) {
    currentPage--;
  }
}

void handleDownButton() {
  if (currentPage < numPages - 1) {
    currentPage++;
  }
}

void handleLeftButton() {
  sceltaSubMenu = 0;
  currentPage = scelta;
  scelta = 0;
}

void handleSelectButton() {
  scelta = currentPage;
}

//////////////////button for the second menu///////////////////
void checkSubMenuButton() {
  if (analogRead(buttonUp) == 0) {
    handleSubMenuUpButton();
  }
  if (digitalRead(buttonDown) == LOW) {
    handleSubMenuDownButton();
  }
  if (digitalRead(buttonLeft) == LOW) {
    handleSubMenuLeftButton();
  }
  if (digitalRead(buttonSelect) == LOW) {
    handleSubMenuSelectButton();
  }
  delay(150);
}

void handleSubMenuUpButton() {
  if (currentPageSubMenu > 0) {
    currentPageSubMenu--;
  }
  Serial.println(currentPageSubMenu);
}

void handleSubMenuDownButton() {
  if (currentPageSubMenu < numPagesSubMenu - 1) {
    currentPageSubMenu++;
  }
  Serial.println(currentPageSubMenu);
}

void handleSubMenuLeftButton() {
  sceltaSubMenu = 0;
  currentPage = scelta;
  scelta = 0;
}

void handleSubMenuSelectButton() {
  sceltaSubMenu = currentPageSubMenu;
  if (sceltaSubMenu == 2) {
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
    fileCount = countfile(dir);
    sdDisplay(dir, type);
  }
}
////////////////////button on the last menu////////////////////////////
void checkModuleButton(int wichMenu) {
  if (analogRead(buttonUp) == 0) {
    scanning = 1;
  }
  if (digitalRead(buttonDown) == LOW) {
    switch (wichMenu) {
      case 1:
        break;
      case 2:
        saveRfid();
        break;
      case 3:
        saveIr();
        break;
      case 4:
        saveRf();
        break;
    }
  }
  if (digitalRead(buttonLeft) == LOW) {
    sceltaSd = 0;
    switch (wichMenu) {
      case 1:
        scanning = 1;
        sceltaSubMenu = 0;
        break;
      case 2:
        scanning = 1;
        sceltaSubMenu = 0;
        break;
      case 3:
        scanning = 1;
        sceltaSubMenu = 0;
        break;
      case 4:
        scanning = 1;
        sceltaSubMenu = 0;
        break;
    }
  }
  if (digitalRead(buttonRight) == LOW) {
    switch (wichMenu) {
      case 1:
        emulateUsb();
        break;
      case 2:
        emulateRfid();
        break;
      case 3:
        emulateIr();
        break;
      case 4:
        emulateRf();
        break;
    }
  }
  if (digitalRead(buttonSelect) == LOW) {
  }
  delay(150);
}
///////////////////Button for the sd menu///////////////////////
void checkSdButton() {
  if (analogRead(buttonUp) == 0) {
    if (selectedFileNumber > 0) {
      selectedFileNumber--;
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
      fileCount = countfile(dir);
      sdDisplay(dir, type);
    }
  }
  if (digitalRead(buttonDown) == LOW) {
    if (selectedFileNumber < fileCount) {
      selectedFileNumber++;
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
      fileCount = countfile(dir);
      sdDisplay(dir, type);
    }
  }
  if (digitalRead(buttonRight) == LOW) {
  }
  if (digitalRead(buttonSelect) == LOW) {
    sceltaSd = selectedFileNumber;
  }
  if (digitalRead(buttonLeft) == LOW) {
    sceltaSubMenu = 0;
    sceltaSd = 0;
  }
  delay(150);
}