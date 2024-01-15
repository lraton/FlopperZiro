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

void sdMenuDisplay(int wichType) {
  if (sceltaSd == 0) {
    checkSdButton();
  } else {
    if (sceltaSd >= 1) {
      selectedSd(wichType);
    }
  }
}

void sdDisplay(File dir, int wichType) {
  int i = 1;

  int pages = fileCount / 6;
  int add = fileCount % 6;
  if (add > 0) {
    pages = pages + 1;
  }
  int currentPage = selectedFileNumber / 6;
  int addCurrentPage = selectedFileNumber % 6;
  if (addCurrentPage > 0) {
    currentPage = currentPage + 1;
  }

  Serial.print("Pages:");
  Serial.println(pages);
  Serial.print("Current page:");
  Serial.println(currentPage);

  display.clearDisplay();
  display.drawBitmap(0, 0, frame, 128, 64, WHITE);
  int positionText = 4;
  display.setCursor(10, positionText);
  while (true) {
    file.openNext(&dir, O_RDONLY);
    if (!file || i > currentPage * 6) {
      break;
    }
    if (i > currentPage * 6 - 6) {
      file.getName(fileName, sizeof(fileName));
      if (i == selectedFileNumber) {
        display.print("> ");
        display.println(String(fileName));
        strcpy(selectedFile, fileName);
      } else {
        display.print("  ");
        display.println(String(fileName));
      }
      file.close();
      display.setCursor(10, positionText = positionText + 10);
    }
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
      file = SD.open(String("/ir/") + String(selectedFile));
      if (file) {
        buffer = file.readStringUntil('\n');
        irproducer = buffer;
        buffer = file.readStringUntil('\n');
        data = buffer;
        buffer = file.readStringUntil('\n');
        memset(rawData, 0, sizeof rawData);
        int n = 0;
        for (int k = 0; k < 67; k++) {
          String tempData = "";
          while (buffer[n] != ' ') {
            tempData = tempData + buffer[n];
            rawData[k] = tempData.toInt();
            n++;
          }
          n++;
        }
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