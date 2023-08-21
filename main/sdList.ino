void sdMenuDisplay(int wichType) {
  if (sceltaSd == 0) {
    checkSdButton();
  } else {
    if (sceltaSd > 1) {
      selectedSd(wichType);
    }
  }
}

void sdDisplay(File dir, int wichType) {
  display.clearDisplay();
  display.setCursor(0, 5);
  int i = 1;
  while (file.openNext(&dir, O_RDONLY)) {
    if (i == selectedFileNumber) {
      file.getName(fileName, sizeof(fileName));
      //Serial.print("> ");
      //Serial.println(fileName);
      display.print("> ");
      display.println(String(fileName));
      strcpy(selectedFile, fileName);
      Serial.println(selectedFile);
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
    case 2:
      break;
    case 3:
      file = SD.open(String("/ir/") + String(selectedFile));
      if (file) {
        buffer = file.readStringUntil('\n');
        irproducer = buffer;
        buffer = file.readStringUntil('\n');
        data = buffer;
        buffer = file.readStringUntil('\n');
        //buffer.toCharArray(rawdataChar,67);
        Serial.println(buffer);
        //rawData = buffer.toInt();
        file.close();
        scanning = 0;
        sceltaSubMenu = 1;
      }
      break;
    case 4:
      file = SD.open(String("/rf/") + String(selectedFile));
      if (file) {
        buffer = file.readStringUntil('\n');
        rfvalue = buffer.toInt();
        buffer = file.readStringUntil('\n');
        rfbit = buffer.toInt();
        buffer = file.readStringUntil('\n');
        rfprotocol = buffer.toInt();
        file.close();
        scanning = 0;
        sceltaSubMenu = 1;
      }
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