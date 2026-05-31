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

// Displays the main menu and enforces the optional RFID lock gate.
void handleMainMenu() {
  if (!deviceUnlocked) {
    // ── RFID lock: wait for the authorised card ──────────────────────────────
    uint8_t uid[7]   = { 0 };
    uint8_t uidLen   = 0;
    bool    success  = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);

    if (success) {
      Serial.print("UID Value: ");
      nfc.PrintHex(uid, uidLen);

      // Compare the first 4 bytes against the stored authorised UID.
      bool matched = true;
      for (int i = 0; i < 4; i++) {
        if (uid[i] != (uint8_t)authorizedUid[i]) {
          matched = false;
          break;
        }
      }
      deviceUnlocked = matched;
    }
    return;  // Stay on the lock screen until the card matches.
  }

  // ── Normal menu navigation ────────────────────────────────────────────────
  handleMenuButtons();

  switch (menuCurrentPage) {
    case 0: drawHomeScreen();    break;
    case 1: drawUsbMenuPage();   break;
    case 2: drawRfidMenuPage();  break;
    case 3: drawIrMenuPage();    break;
    case 4: drawRfMenuPage();    break;
  }
}
