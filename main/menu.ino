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

//The menu function
void displayMenu() {
  if (tag == true) {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    if (success) {
      Serial.print("UID Value: ");
      nfc.PrintHex(uid, uidLength);
      for (int i = 0; i < 4; i++) {
        if (uid[i] == buf[i]) {
          tag = true;
        } else {
          tag = false;
          break;
        }
      }
    }
  } else {
    checkMenuButton();
    switch (currentPage) {
      case 0:
        menuprincipale();  //tamaguino
        break;
      case 1:
        menuusb();  //immagine usb
        break;
      case 2:
        menurfid();  //immagine rfid
        break;
      case 3:
        menuir();  //immagine ir
        break;
      case 4:
        menurf();  //immagine rf
        break;
    }
  }
}
