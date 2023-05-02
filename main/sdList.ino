void sdMenuDisplay(int wichType) {
  if (sceltaSd == 0) {

    checkSdButton();
  } else {
    if (sceltaSd > 0) {
      selectedSd(wichType);
    }
  }
}

void sdDisplay(File dir, int wichType) {
  display.clearDisplay();
  display.setCursor(0, 5);
  int i = 0;
  while (file.openNext(&dir, O_RDONLY)) {
    if (i == selectedFileNumber) {
      file.getName(fileName, sizeof(fileName));
      //Serial.print("> ");
      //Serial.println(fileName);
      display.print("> ");
      display.println(String(fileName));
      strcpy(selectedFile, fileName);
    } else {
      file.getName(fileName, sizeof(fileName));
      //Serial.println(fileName);
      display.print("  ");
      display.println(String(fileName));
    }
    file.close();
    i++;
  }
  display.display();
}

void selectedSd(int wichType) {
  switch (wichType) {
    sceltaSubMenu = 1;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
  }
}

///////////////////////////////
int countfile(File dir) {
  int fileCount = 0;
  while (file.openNext(&dir, O_RDONLY)) {
    if (!file.isHidden()) {
      fileCount++;
    }
    file.close();
  }
  return fileCount;
}