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
