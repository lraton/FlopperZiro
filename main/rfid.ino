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

//The rfid function
void rfid() {
  type = 2;
  switch (sceltaSubMenu) {
    case 0:
      subMenuDisplay();
      break;
    case 1:
      scanRfid();
      break;
    case 2:
      sdMenuDisplay(2);
      break;
  }
}

//+=============================================================================
// Display IR code
//
void scanRfid() {
  if (scanning == 1) {
    graficascan();
    battery();

    // Keyb on NDEF and Mifare Classic should be the same
    uint8_t keyuniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t success;
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
      scanbase();
      // Display some basic information about the card
      Serial.println("Found an ISO14443A card");
      Serial.print("  UID Length: ");
      Serial.print(uidLength, DEC);
      Serial.println(" bytes");
      Serial.print("  UID Value: ");
      nfc.PrintHex(uid, uidLength);
      Serial.println("");


      display.setCursor(20, 25);
      display.print("UID: ");
      for (int i = 0; i < uidLength; i++) {
        if (i + 1 != uidLength) {
          display.print(uid[i]);
        } else {
          display.println(uid[i]);
        }
      }
      display.setCursor(15, 35);
      display.print("Lenght: " + String(uidLength) + " Bytes");
      scanning = 0;
    }
  } else {
    scanbase();
    display.setCursor(20, 25);
    display.print("UID: ");
    for (int i = 0; i < uidLength; i++) {
      if (i + 1 != uidLength) {
        display.print(uid[i]);
      } else {
        display.println(uid[i]);
      }
    }
    display.setCursor(15, 35);
    display.print("Lenght: " + String(uidLength) + " Bytes");
  }
  battery();
  checkModuleButton(2);
}

void emulateRfid() {
  uint8_t apdubuffer[255] = {}, apdulen = 0;
  uint8_t ppse[] = { 0x8E, 0x6F, 0x23, 0x84, 0x0E, 0x32, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31, 0xA5, 0x11, 0xBF, 0x0C, 0x0E, 0x61, 0x0C, 0x4F, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10, 0x87, 0x01, 0x01, 0x90, 0x00 };
  nfc.AsTarget();
  (void)nfc.getDataTarget(apdubuffer, &apdulen);  //Read initial APDU
  if (apdulen > 0) {
    for (uint8_t i = 0; i < apdulen; i++) {
      Serial.print(" 0x");
      Serial.print(apdubuffer[i], HEX);
    }
    Serial.println("");
  }
  nfc.setDataTarget(ppse, sizeof(ppse));    //Mimic a smart card response with a PPSE APDU
  nfc.getDataTarget(apdubuffer, &apdulen);  //Read respond from the PoS or Terminal
  if (apdulen > 0) {
    for (uint8_t i = 0; i < apdulen; i++) {
      Serial.print(" 0x");
      Serial.print(apdubuffer[i], HEX);
    }
    Serial.println("");
  }
  delay(1000);
}

void saveRfid() {
  scanbase();
  if (scanning == 0) {
    if (sdbegin) {
      display.setCursor(33, 30);
      display.println("Saving...");
      if (SD.exists("ir/prova.txt")) {
        Serial.println("gia esistente");
      } else {
        file = SD.open("ir/prova.txt", FILE_WRITE);
        for (int i = 0; i < 67; i++) {
          file.write("ciao");
        }
        file.close();
      }
    } else {
      display.setCursor(33, 30);
      display.println("SD Error...");
    }
  } else {
    display.setCursor(30, 30);
    display.println("Nothing to send");
  }
  battery();
  delay(2000);
}
