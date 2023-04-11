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
    graficairscan();
    battery();
    uint8_t success;
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
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
      display.print("Lenght: " + uidLength);
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
    display.setCursor(20, 35);
    display.print("Lenght: " + uidLength);
  }
  battery();
  checkModuleButton(2);
}

void emulateRfid() {
  scanbase();
  display.setCursor(33, 30);
  display.println("Sending...");
  battery();
  //IrSender.sendRaw(rawData, 67, freq_ir);
  delay(2000);
}

void saveRfid() {
  scanbase();
  if (sdbegin) {
    display.setCursor(33, 30);
    display.println("Saving...");
  } else {
    display.setCursor(33, 30);
    display.println("SD Error...");
  }
  if (SD.exists("ir/prova.txt")) {
    Serial.println("gia esistente");
  } else {
    file = SD.open("ir/prova.txt", FILE_WRITE);
    for (int i = 0; i < 67; i++) {
      file.write("ciao");
    }
    file.close();
  }
  battery();
  delay(2000);
}
