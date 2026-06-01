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

// ─── SD menu entry point ──────────────────────────────────────────────────────

// Dispatches between the file browser and the file-action screen depending on
// whether the user has already selected a file (sdFileSelected > 0).
void drawSdMenu(int moduleType) {
  if (sdFileSelected == 0) {
    handleSdButtons();  // Navigate the file list
  } else {
    loadSelectedFile(moduleType);  // Act on the chosen file
  }
}

// ─── File list renderer ───────────────────────────────────────────────────────

// Draws up to 6 files per page from the given SD directory.
// The selected file is highlighted with a ">" prefix.
void drawSdFileList(File dir, int moduleType) {
  int fileIndex = 1;

  int totalPages = sdFileCount / 6 + (sdFileCount % 6 > 0 ? 1 : 0);
  int currentPage = (sdSelectedFileNum / 6) + (sdSelectedFileNum % 6 > 0 ? 1 : 0);

  Serial.print("Pages: ");        Serial.println(totalPages);
  Serial.print("Current page: "); Serial.println(currentPage);

  display.clearDisplay();
  display.drawBitmap(0, 0, frame, 128, 64, WHITE);
  int textY = 4;
  display.setCursor(10, textY);

  while (true) {
    file.openNext(&dir, O_RDONLY);
    if (!file || fileIndex > currentPage * 6) break;

    if (fileIndex > currentPage * 6 - 6) {
      file.getName(sdFileName, sizeof(sdFileName));

      if (fileIndex == sdSelectedFileNum) {
        display.print("> ");
        display.println(String(sdFileName));
        strcpy(sdSelectedFile, sdFileName);  // Remember the chosen filename
      } else {
        display.print("  ");
        display.println(String(sdFileName));
      }

      file.close();
      textY += 10;
      display.setCursor(10, textY);
    }
    fileIndex++;
  }

  display.display();
}

// ─── File action dispatcher ───────────────────────────────────────────────────

// Called when the user has confirmed a file selection.
// Reads the file and loads its contents into the appropriate global variables,
// then returns to the scan screen (selectedSubMenu = 1, isScanning = 0).
void loadSelectedFile(int moduleType) {
  switch (moduleType) {

    case 1:
      // BadUSB — nothing to load; the filename is already in sdSelectedFile.
      break;

    case 2: {
      // RFID — parse card type, UID length, and UID bytes.
      // Expected format (one value per line):
      //   <rfidCardType>
      //   <rfidUidLen>
      //   <rfidUid[0]> … <rfidUid[rfidUidLen-1]>
      file = SD.open(String("/rfid/") + String(sdSelectedFile));
      if (file) {
        sdReadBuffer = file.readStringUntil('\n');
        sdReadBuffer.trim();
        rfidCardType = sdReadBuffer;

        sdReadBuffer = file.readStringUntil('\n');
        sdReadBuffer.trim();
        rfidUidLen = (uint8_t)sdReadBuffer.toInt();
        if (rfidUidLen > 8) rfidUidLen = 8;  // Guard against buffer overflow

        for (uint8_t k = 0; k < rfidUidLen; k++) {
          sdReadBuffer = file.readStringUntil('\n');
          sdReadBuffer.trim();
          rfidUid[k] = (uint8_t)sdReadBuffer.toInt();
        }

        memset(ntagData, 0x00, sizeof(ntagData));
        if (rfidUidLen == 7) {
          for (int k = 0; k < 256; k++) {
            if (!file.available()) break;
            sdReadBuffer = file.readStringUntil('\n');
            sdReadBuffer.trim();
            ntagData[k] = (uint8_t)sdReadBuffer.toInt();
          }
        }

        file.close();
        isScanning      = 0;
        selectedSubMenu = 1;
      }
      break;
    }

    case 3: {
      // IR — parse protocol, hex value, and raw timing array.
      file = SD.open(String("/ir/") + String(sdSelectedFile));
      if (file) {
        sdReadBuffer = file.readStringUntil('\n');
        irProtocol   = sdReadBuffer;

        sdReadBuffer = file.readStringUntil('\n');
        irHexValue   = sdReadBuffer;

        // Raw timings: 67 space-separated values on one line.
        sdReadBuffer = file.readStringUntil('\n');
        memset(irRawData, 0, sizeof(irRawData));
        int pos = 0;
        for (int k = 0; k < 67; k++) {
          String token = "";
          while (sdReadBuffer[pos] != ' ' && pos < (int)sdReadBuffer.length()) {
            token += sdReadBuffer[pos++];
          }
          irRawData[k] = token.toInt();
          pos++;  // Skip the space separator
        }

        file.close();
        isScanning      = 0;
        selectedSubMenu = 1;
      }
      break;
    }

    case 4: {
      // RF — parse value, bit length, and protocol index.
      file = SD.open(String("/rf/") + String(sdSelectedFile));
      if (file) {
        sdReadBuffer       = file.readStringUntil('\n');
        rfReceivedValue    = sdReadBuffer.toInt();

        sdReadBuffer       = file.readStringUntil('\n');
        rfBitLength        = sdReadBuffer.toInt();

        sdReadBuffer       = file.readStringUntil('\n');
        rfReceivedProtocol = sdReadBuffer.toInt();

        file.close();
        isScanning      = 0;
        selectedSubMenu = 1;
      }
      break;
    }
  }
}

// ─── File counter ─────────────────────────────────────────────────────────────

// Returns the number of non-hidden files in the given directory.
int countFiles(File dir) {
  int count = 0;
  while (file.openNext(&dir, O_RDONLY)) {
    if (!file.isHidden()) count++;
    file.close();
  }
  return count;
}
