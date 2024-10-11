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

// The menu function to control which menu is displayed
void displayMenu() {

  // If the tag is detected (tag == true), try to read the RFID/NFC tag
  if (tag == true) {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    // Attempt to read the passive NFC tag's UID using PN532 reader
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    // If the tag is successfully read
    if (success) {
      Serial.print("UID Value: ");
      nfc.PrintHex(uid, uidLength);  // Print the UID value in hexadecimal format

      // Check if the UID matches a predefined buffer (buf) for verification
      for (int i = 0; i < 4; i++) {
        // Compare the first 4 bytes of the UID with the buf array
        if (uid[i] == buf[i]) {
          tag = true;  // Set tag as true if there's a match
        } else {
          tag = false;  // If any byte doesn't match, set tag to false
          break;        // Exit the loop if there's no match
        }
      }
    }

    // If no tag is detected (tag == false), check for menu button input
  } else {
    checkMenuButton();  // Function to handle menu button interactions

    // Switch case to determine which page/menu to display
    switch (currentPage) {
      case 0:
        menuprincipale();  // Show the main menu (Tamagotchi-like menu)
        break;
      case 1:
        menuusb();  // Display USB-related menu
        break;
      case 2:
        menurfid();  // Display RFID-related menu
        break;
      case 3:
        menuir();  // Display IR (Infrared) related menu
        break;
      case 4:
        menurf();  // Display RF (Radio Frequency) related menu
        break;
    }
  }
}
