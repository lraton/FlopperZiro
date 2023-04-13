void rfid() {
  switch (sceltaSubMenu) {
    case 0:
      graficair();
      break;
    case 1:
      scanRfid();
      break;
    case 2:
      break;
  }
}

//+=============================================================================
// Display IR code
//
void scanRfid() {
  if (scanning == 1) {
    Serial.println("ciao");
    graficascan();
    //battery();
    uint8_t success;
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
    if (success) {
      scanbase();
      Serial.print("UID Value: ");
      nfc.PrintHex(uid, uidLength);
      display.setCursor(20, 25);
      display.print("UID: ");
      for (int i = 0; i < uidLength; i++) {
        if (i + 1 != uidLength) {
          display.print(uid[i]);
        } else {
          display.println(uid[i]);
        }
      }
      display.setCursor(20, 35);
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
  scanbase();
  display.setCursor(33, 30);
  uint8_t apdubuffer[255] = {}, apdulen = 0;
  nfc.AsTarget();
  nfc.setDataTarget(uid, uidLength);        //Mimic a smart card response with a PPSE APDU
  display.println("Sending...");
  battery();
  delay(2000);
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
