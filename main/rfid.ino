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
  uint8_t target[] = {
    0x8C,              // INIT AS TARGET
    0x00,              // MODE -> BITFIELD
    0x08, 0x00,        // SENS_RES - MIFARE PARAMS
    0xdc, 0x44, 0x20,  // NFCID1T
    0x60,              // SEL_RES
    0x01, 0xfe,        // NFCID2T MUST START WITH 01fe - FELICA PARAMS - POL_RES
    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xc0,
    0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,  // PAD
    0xff, 0xff,                                // SYSTEM CODE
    0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
    0x33, 0x22, 0x11, 0x01, 0x00,  // NFCID3t MAX 47 BYTES ATR_RES
    0x0d, 0x52, 0x46, 0x49, 0x44, 0x49, 0x4f,
    0x74, 0x20, 0x50, 0x4e, 0x35, 0x33, 0x32  // HISTORICAL BYTES
  };
  /*  uint8_t target[] = {
      PN532_COMMAND_TGINITASTARGET,
      0x05,                  // MODE: 0x04 = PICC only, 0x01 = Passive only (0x02 = DEP only)

      // MIFARE PARAMS
      0x04, 0x00,         // SENS_RES (seeeds studio set it to 0x04, nxp to 0x08)
      0x00, 0x00, 0x00,   // NFCID1t	(is set over sketch with setUID())
      0x20,               // SEL_RES    (0x20=Mifare DelFire, 0x60=custom)

      // FELICA PARAMS
      0x01, 0xFE,         // NFCID2t (8 bytes) FeliCa NEEDS TO BEGIN WITH 0x01 0xFE!
      0x05, 0x01, 0x86,
      0x04, 0x02, 0x02,
      0x03, 0x00,         // PAD (8 bytes)
      0x4B, 0x02, 0x4F, 
      0x49, 0x8A, 0x00,   
      0xFF, 0xFF,         // System code (2 bytes)
      
      0x01, 0x01, 0x66,   // NFCID3t (10 bytes)
      0x6D, 0x01, 0x01, 0x10,
      0x02, 0x00, 0x00,

	  0x00, // length of general bytes
      0x00  // length of historical bytes
  };
  */
  scanbase();
  display.setCursor(33, 30);
  uint8_t apdubuffer[255] = {}, apdulen = 0;
  //n fc.AsTarget();
  nfc.setDataTarget(target, sizeof(target));  //Mimic a smart card response with a PPSE APDU
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
