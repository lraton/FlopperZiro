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

// ─── PN532 raw-frame helpers (I²C) ───────────────────────────────────────────
//
// The Adafruit_PN532 library exposes sendCommandCheckAck() publicly, but its
// readResponse() is private.  We communicate with the PN532 over I²C directly
// using the helpers below, which replicate the PN532 frame protocol from
// UM10232 (NXP PN532 User Manual).
//
// Frame layout (host → PN532):
//   PREAMBLE  0x00
//   STARTCODE 0x00 0xFF
//   LEN       N+1        (data bytes + TFI byte)
//   LCS       (~LEN)+1   (length checksum)
//   TFI       0xD4       (host-to-PN532 direction)
//   CMD[0]    command code
//   CMD[1..N] command parameters
//   DCS       checksum of TFI+CMD bytes
//   POSTAMBLE 0x00
//
// Frame layout (PN532 → host, prepended by a "ready" byte 0x01):
//   READY     0x01
//   PREAMBLE  0x00
//   STARTCODE 0x00 0xFF
//   LEN       …
//   LCS       …
//   TFI       0xD5        (PN532-to-host direction)
//   RSP[0]    command code + 1
//   RSP[1..N] response data
//   DCS       …
//   POSTAMBLE 0x00

// Maximum payload we ever read back from the PN532 in emulation mode.
#define PN532_EMUL_BUFSIZE 64

// How long (ms) to wait for the PN532 to signal it is ready.
#define PN532_READY_TIMEOUT_MS 3000

// PN532 I²C address (defined in Adafruit_PN532.h as a public #define).
// Value: 0x24
#define PN532_ADDR  PN532_I2C_ADDRESS

// ── Low-level write ───────────────────────────────────────────────────────────
// Sends a raw PN532 command frame over I²C.
// `data`    = command bytes starting with the command code (e.g. 0x8C).
// `dataLen` = number of bytes in `data`.
static void pn532_sendFrame(const uint8_t *data, uint8_t dataLen) {
  uint8_t len = dataLen + 1;  // +1 for TFI byte
  uint8_t lcs = (~len) + 1;

  // Checksum: sum of TFI (0xD4) + all data bytes, then (~sum)+1
  uint8_t dcs = 0xD4;
  for (uint8_t i = 0; i < dataLen; i++) dcs += data[i];
  dcs = (~dcs) + 1;

  Wire.beginTransmission(PN532_ADDR);
  Wire.write(0x00);   // PREAMBLE
  Wire.write(0x00);   // STARTCODE[0]
  Wire.write(0xFF);   // STARTCODE[1]
  Wire.write(len);    // LEN
  Wire.write(lcs);    // LCS
  Wire.write(0xD4);   // TFI (host→PN532)
  for (uint8_t i = 0; i < dataLen; i++) Wire.write(data[i]);
  Wire.write(dcs);    // DCS
  Wire.write(0x00);   // POSTAMBLE
  Wire.endTransmission();
}

// ── Wait for PN532 ready ──────────────────────────────────────────────────────
// The PN532 signals readiness over I²C by returning 0x01 as the first byte of
// a read.  Returns true when ready within `timeoutMs`, false on timeout or abort.
static bool pn532_waitReady(uint16_t timeoutMs) {
  uint32_t start = millis();
  while ((millis() - start) < timeoutMs) {
    Wire.requestFrom((uint8_t)PN532_ADDR, (uint8_t)1);
    if (Wire.available() && Wire.read() == 0x01) return true;

    // Check for user interruption (any menu button pressed)
    if (analogRead(BTN_UP) < 100 || digitalRead(BTN_DOWN) == LOW || 
        digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW || 
        digitalRead(BTN_SELECT) == LOW) {
      
      // Send an ACK frame to abort the pending PN532 command
      Wire.beginTransmission(PN532_ADDR);
      Wire.write(0x00);
      Wire.write(0x00);
      Wire.write(0xFF);
      Wire.write(0x00);
      Wire.write(0xFF);
      Wire.write(0x00);
      Wire.endTransmission();
      
      // Short delay so the buttonMenu handler registers the active press
      delay(50);
      return false;
    }

    delay(5);
  }
  return false;
}

// ── Drain ACK ─────────────────────────────────────────────────────────────────
// After a command, PN532 sends a 6-byte ACK frame.  We discard it.
static void pn532_drainAck() {
  if (!pn532_waitReady(500)) return;
  Wire.requestFrom((uint8_t)PN532_ADDR, (uint8_t)7);  // RDY + 6 ACK bytes
  while (Wire.available()) Wire.read();
}

// ── Read response frame ───────────────────────────────────────────────────────
// Reads the PN532 information response into `buf`.
// Returns the number of payload bytes (after TFI + CMD+1), or 0 on error.
// Payload starts at buf[0].
//
// NOTE: Arduino Wire buffer = 32 bytes.  We read in two I²C transactions:
//   1. The 8-byte fixed header to learn LEN.
//   2. The remaining payload (up to 24 bytes per transaction due to Wire limit).
static uint8_t pn532_readResponse(uint8_t *buf, uint8_t bufSize, uint16_t timeoutMs) {
  if (!pn532_waitReady(timeoutMs)) return 0;

  // ── Transaction 1: read the fixed 8-byte frame header ──────────────────────
  //   RDY(1) PREAMBLE(1) STARTCODE(2) LEN(1) LCS(1) TFI(1) CMD+1(1)
  Wire.requestFrom((uint8_t)PN532_ADDR, (uint8_t)8);

  // RDY byte
  if (!Wire.available()) return 0;
  Wire.read();  // 0x01

  // PREAMBLE + STARTCODE (3 bytes)
  for (uint8_t i = 0; i < 3 && Wire.available(); i++) Wire.read();

  // LEN
  if (!Wire.available()) return 0;
  uint8_t len = Wire.read();

  // LCS
  if (Wire.available()) Wire.read();

  // TFI (0xD5)
  if (!Wire.available()) return 0;
  Wire.read();

  // CMD+1
  if (!Wire.available()) return 0;
  Wire.read();

  // payloadLen = LEN - 2  (LEN field counts TFI + CMD+1 + payload)
  uint8_t payloadLen = (len >= 2) ? (len - 2) : 0;
  if (payloadLen > bufSize) payloadLen = bufSize;

  if (payloadLen == 0) return 0;

  // ── Transaction 2: read payload + DCS + postamble ──────────────────────────
  // Wire buffer is 32 bytes; payload <= 64 bytes.  Read in chunks.
  uint8_t got = 0;
  uint8_t remaining = payloadLen + 2;  // +2 for DCS and POSTAMBLE

  while (got < payloadLen && remaining > 0) {
    uint8_t chunk = (remaining > 30) ? 30 : remaining;  // keep under Wire limit
    Wire.requestFrom((uint8_t)PN532_ADDR, chunk);
    while (Wire.available() && got < payloadLen) {
      buf[got++] = Wire.read();
    }
    // Drain any trailing bytes (DCS/POSTAMBLE excess)
    while (Wire.available()) Wire.read();
    remaining -= chunk;
  }

  return got;
}

// ─── TgGetData ────────────────────────────────────────────────────────────────
// Asks the PN532 (in target mode) for the next command sent by the initiator.
// Returns number of bytes placed in `buf`, or 0 on error / timeout.
static uint8_t pn532_tgGetData(uint8_t *buf, uint8_t bufSize) {
  uint8_t cmd[1] = { 0x86 };  // TgGetData
  pn532_sendFrame(cmd, 1);
  pn532_drainAck();
  return pn532_readResponse(buf, bufSize, PN532_READY_TIMEOUT_MS);
}

// ─── TgSetData ────────────────────────────────────────────────────────────────
// Sends a response from our emulated card back to the initiator.
// `data` / `dataLen` = payload to send (APDU response or similar).
// Returns true if the PN532 accepted the frame.
static bool pn532_tgSetData(const uint8_t *data, uint8_t dataLen) {
  uint8_t cmd[1 + PN532_EMUL_BUFSIZE];
  if (dataLen > PN532_EMUL_BUFSIZE - 1) dataLen = PN532_EMUL_BUFSIZE - 1;
  cmd[0] = 0x8E;  // TgSetData
  memcpy(cmd + 1, data, dataLen);
  pn532_sendFrame(cmd, 1 + dataLen);
  pn532_drainAck();
  uint8_t resp[4];
  uint8_t n = pn532_readResponse(resp, sizeof(resp), 1000);
  return (n >= 1 && resp[0] == 0x00);  // 0x00 = success
}

// ─── TgInitAsTarget ───────────────────────────────────────────────────────────
// Puts the PN532 into ISO14443A card-emulation mode.
//
// UM10232 § 7.3.9  TgInitAsTarget payload layout:
//
//  Byte  Field
//  0     Mode       (0x05 = PICC-only | Passive-only)
//  1-2   SENS_RES   (ATQA, 2 bytes: byte0 is "high" byte sent first by ISO14443A)
//  3-5   NFCID1t    (3 bytes — only the first 3 bytes of the UID)
//  6     SEL_RES    (SAK)
//  7-24  FeliCa params (18 bytes, all 0 if not using FeliCa)
//  25-34 NFCID3t    (10 bytes, all 0 if unused)
//  35    LenGt      (0 = no General bytes)
//  36    LenTk      (0 = no Historical bytes)
//
// IMPORTANT: SENS_RES byte order in this command is [byte1, byte0] relative
// to what the ISO14443A standard calls ATQA.  For example:
//   Mifare Classic 1K  → ATQA 0x0004 → SENS_RES[0]=0x00, SENS_RES[1]=0x04
//   NTAG/Ultralight    → ATQA 0x0044 → SENS_RES[0]=0x00, SENS_RES[1]=0x44
//
// The SEL_RES / SAK byte controls what the reader "thinks" the card is:
//   0x08 → Mifare Classic 1K (4-byte UID)
//   0x00 → Mifare Ultralight / NTAG (7-byte UID, no ISO14443-4)
//   0x20 → ISO14443-4 tag (phone shows NDEF prompt)
//
// Returns true if the PN532 ACKed the command and became ready
// (i.e. a reader's RF field was detected).
static bool pn532_tgInitAsTarget(uint8_t sens_res_0, uint8_t sens_res_1,
                                  const uint8_t *nfcid1,  // 3 bytes
                                  uint8_t sel_res,
                                  uint16_t timeoutMs) {
  // Build the full 38-byte command payload:
  //   [0]     cmd code 0x8C
  //   [1]     Mode
  //   [2-3]   SENS_RES (2)
  //   [4-6]   NFCID1t  (3)
  //   [7]     SEL_RES
  //   [8-25]  FeliCa params (18, all 0)
  //   [26-35] NFCID3t (10, all 0)
  //   [36]    LenGt (0)
  //   [37]    LenTk (0)
  uint8_t cmd[38];
  memset(cmd, 0x00, sizeof(cmd));

  cmd[0] = 0x8C;        // TgInitAsTarget command code
  cmd[1] = 0x05;        // Mode: PICC-only | Passive-only
  cmd[2] = sens_res_0;  // SENS_RES[0]
  cmd[3] = sens_res_1;  // SENS_RES[1]
  cmd[4] = nfcid1[0];   // NFCID1[0]
  cmd[5] = nfcid1[1];   // NFCID1[1]
  cmd[6] = nfcid1[2];   // NFCID1[2]
  cmd[7] = sel_res;     // SEL_RES
  // bytes 8-37 remain 0x00 (FeliCa params, NFCID3, LenGt, LenTk)

  pn532_sendFrame(cmd, sizeof(cmd));
  pn532_drainAck();

  // Wait for a reader — this blocks until an RF field is detected or timeout.
  return pn532_waitReady(timeoutMs);
}

// ─── NDEF / ISO14443-4 APDU emulation ────────────────────────────────────────
//
// When SAK = 0x20 the reader will try ISO14443-4.  We must respond to three
// APDU commands in sequence to be recognised as an NDEF Type-4 tag:
//
//  1. SELECT (by name)  D2 76 00 00 85 01 01  → NDEF Application
//  2. SELECT (by ID)    E1 03                 → Capability Container (CC) file
//  3. READ BINARY       read CC data
//  4. SELECT (by ID)    E1 04                 → NDEF file
//  5. READ BINARY       read NDEF content
//
// For a plain UID-only emulation (like Flipper Zero), we use SAK=0x08 /
// SAK=0x00 so the reader only gets the UID and stops.  No APDU loop needed.
//
// rfidEmulMode controls behaviour:
//   0 = UID-only (SAK 0x08 / 0x00, no data exchange — readable by NFC Tools
//       as a raw card — fastest, most compatible with simple readers)
//   1 = NDEF Type 4 (SAK 0x20, full APDU loop — phone opens NDEF payload)
//
// We store the stored NDEF payload in rfidNdefPayload[] so the user can
// put a URL or text that the phone will open.  If no payload is set, we serve
// a minimal empty NDEF message.

#define RFID_EMUL_MODE_UID   0
#define RFID_EMUL_MODE_NDEF  1

// Tiny hardcoded NDEF payload (Text record "FlopperZiro").
// Replace with your own NDEF if needed.
static const uint8_t RFID_NDEF_DEFAULT[] = {
  0xD1,        // MB=1 ME=1 TNF=0x01 (NFC Forum Well-Known)
  0x01,        // Type length = 1
  0x0D,        // Payload length = 13
  'T',         // Type: 'T' (Text record)
  0x02,        // Status: UTF-8, lang length = 2
  'e', 'n',   // Language: "en"
  'F','l','o','p','p','e','r','Z','i','r','o'
};

// ─── Routes the RFID sub-menu pages ──────────────────────────────────────────
void rfid() {
  currentModuleType = 2;
  switch (selectedSubMenu) {
    case 0: drawSubMenu();    break;  // Sub-menu selection screen
    case 1: scanRfid();       break;  // Active scan / result display
    case 2: drawSdMenu(2);    break;  // SD file browser for /rfid/
  }
}

// ─── Scan ─────────────────────────────────────────────────────────────────────
// Non-blocking passive target detection.
// Starts a detection round on the first call, then polls for a result on
// subsequent calls without blocking the display update loop.
void scanRfid() {
  if (isScanning == 1) {
    drawScanScreen();

    uint8_t keyUniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    // Kick off detection if not already started.
    if (!rfidDetectionStarted) {
      nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);
      rfidDetectionStarted = true;
    }

    bool tagFound = nfc.readDetectedPassiveTargetID(rfidUid, &rfidUidLen);

    if (tagFound) {
      drawScanBase();

      // ── Serial debug output ────────────────────────────────────────────────
      Serial.println("Found an ISO14443A card");
      Serial.print("  UID Length: ");
      Serial.print(rfidUidLen, DEC);
      Serial.println(" bytes");
      Serial.print("  UID Value: ");
      nfc.PrintHex(rfidUid, rfidUidLen);
      Serial.println();

      // ── Display UID ────────────────────────────────────────────────────────
      display.setCursor(20, 25);
      display.print("UID: ");
      for (int i = 0; i < rfidUidLen; i++) {
        if (rfidUid[i] < 0x10) display.print("0");
        display.print(rfidUid[i], HEX);
        if (i + 1 < rfidUidLen) display.print(" ");
        else                    display.println();
      }

      // ── Card-type-specific handling ────────────────────────────────────────
      switch (rfidUidLen) {
        case 4: {
          // Mifare Classic (4-byte UID)
          Serial.println("Mifare Classic card (4-byte UID)");
          rfidCardType = "Mif. Classic";

          bool authOk = nfc.mifareclassic_AuthenticateBlock(
              rfidUid, rfidUidLen, 4, 0, keyUniversal);

          if (authOk) {
            uint8_t blockData[16];
            if (nfc.mifareclassic_ReadDataBlock(4, blockData)) {
              Serial.print("Block 4: ");
              nfc.PrintHexChar(blockData, 16);
            }
          }
          break;
        }

        case 7: {
          // NTAG2xx or Mifare Ultralight (7-byte UID)
          Serial.println("Mifare Ultralight / NTAG2xx (7-byte UID)");
          rfidCardType = "NTAG2xx";

          // Dump all readable pages (pages 0-63, 4 bytes each)
          uint8_t pageData[4];
          // Clear ntagData before reading
          memset(ntagData, 0x00, sizeof(ntagData));
          for (uint8_t pg = 0; pg < 64; pg++) {
            if (nfc.ntag2xx_ReadPage(pg, pageData)) {
              memcpy(&ntagData[pg * 4], pageData, 4);
              Serial.print("PAGE ");
              if (pg < 10) Serial.print("0");
              Serial.print(pg);
              Serial.print(": ");
              nfc.PrintHexChar(pageData, 4);
            } else {
              Serial.print("PAGE ");
              if (pg < 10) Serial.print("0");
              Serial.print(pg);
              Serial.println(": (unreadable)");
              break;
            }
          }
          break;
        }

        default: {
          rfidCardType = "Unknown";
          break;
        }
      }

      // ── Display card type ──────────────────────────────────────────────────
      String cardLabel = rfidCardType + " " + String(rfidUidLen) + " B";
      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(cardLabel, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH - w) / 2, 35);
      display.print(cardLabel);

      isScanning = 0;  // Stop scanning — result is now on screen
    }

  } else {
    // ── Result display (not scanning) ─────────────────────────────────────────
    drawScanBase();
    display.setCursor(20, 25);
    display.print("UID: ");
    for (int i = 0; i < rfidUidLen; i++) {
      if (rfidUid[i] < 0x10) display.print("0");
      display.print(rfidUid[i], HEX);
      if (i + 1 < rfidUidLen) display.print(" ");
      else                    display.println();
    }

    String cardLabel = rfidCardType + " " + String(rfidUidLen) + " B";
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(cardLabel, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 35);
    display.print(cardLabel);
  }

  displayBattery();
  handleModuleButtons(2);
}

// ─── Emulate ──────────────────────────────────────────────────────────────────
//
// Puts the PN532 into ISO14443A target (card emulation) mode and, once a
// reader is detected, handles the APDU exchange so that readers like NFC Tools
// can interact with the emulated card.
//
// Strategy:
//   • 4-byte UID → Mifare Classic  (ATQA=0x0004, SAK=0x08)
//     Readers see it as a Classic 1K.  No APDU loop needed because Classic
//     uses its own crypto handshake after anticollision — our PN532 can't
//     replay that, but the UID is announced correctly and NFC Tools will
//     at least show the UID.
//
//   • 7-byte UID → NTAG / Ultralight (ATQA=0x0044, SAK=0x00)
//     No ISO14443-4 layer.  Readers see the UID, some attempt page reads
//     which will time out (PN532 doesn't respond to those in target mode).
//     NFC Tools shows the UID.
//
// For both cases the APDU loop is run so that:
//   – The PN532 stays in target mode (doesn't reset after first exchange).
//   – If a reader sends higher-layer commands they get an error SW (6A81)
//     instead of silence, which is more ISO-compliant.
//
// The emulation times out after ~5 seconds of no reader activity.
void emulateRfid() {
  if (rfidUidLen == 0) {
    drawScanBase();
    display.setCursor(20, 30);
    display.print("Scan first!");
    display.display();
    displayBattery();
    delay(1200);
    return;
  }

  // Wait for the user to release all buttons before entering emulation,
  // otherwise the button they just pressed to enter emulation will 
  // immediately trigger the abort sequence!
  while (analogRead(BTN_UP) < 100 || digitalRead(BTN_DOWN) == LOW || 
         digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW || 
         digitalRead(BTN_SELECT) == LOW) {
    delay(10);
  }

  // ── Build display ──────────────────────────────────────────────────────────
  drawScanBase();
  display.setCursor(4, 22);
  display.print("UID: ");
  for (int i = 0; i < rfidUidLen; i++) {
    if (rfidUid[i] < 0x10) display.print("0");
    display.print(rfidUid[i], HEX);
    if (i < rfidUidLen - 1) display.print(":");
  }
  display.setCursor(4, 32);
  display.println("Emulating...");
  display.display();

  // ── Choose emulation parameters ───────────────────────────────────────────
  // SENS_RES (ATQA) — byte order in TgInitAsTarget: [byte0=high, byte1=low]
  uint8_t sens_res_0, sens_res_1, sel_res;

  if (rfidUidLen == 4) {
    // Mifare Classic 1K: ATQA = 0x0004, SAK = 0x08
    sens_res_0 = 0x00;
    sens_res_1 = 0x04;
    sel_res    = 0x08;
  } else {
    // NTAG / Ultralight: ATQA = 0x0044, SAK = 0x00
    sens_res_0 = 0x00;
    sens_res_1 = 0x44;
    sel_res    = 0x00;
  }

  // NFCID1t: only 3 bytes go into TgInitAsTarget.
  // For 4-byte UIDs: bytes 0-2.
  // For 7-byte UIDs: bytes 0-2 (the PN532 handles cascade internally).
  uint8_t nfcid1[3] = { rfidUid[0], rfidUid[1], rfidUid[2] };

  Serial.println("[RFID] Starting emulation...");
  Serial.print("[RFID] SENS_RES: "); Serial.print(sens_res_0, HEX); Serial.print(" "); Serial.println(sens_res_1, HEX);
  Serial.print("[RFID] SEL_RES: "); Serial.println(sel_res, HEX);
  Serial.print("[RFID] NFCID1: ");
  for (int i = 0; i < 3; i++) { Serial.print(nfcid1[i], HEX); Serial.print(" "); }
  Serial.println();

  // ── Wait for a reader ─────────────────────────────────────────────────────
  // 20-second timeout to give the user time to place the phone/reader.
  bool readerFound = pn532_tgInitAsTarget(sens_res_0, sens_res_1,
                                           nfcid1, sel_res,
                                           20000);

  if (!readerFound) {
    // Timeout — no reader came close
    drawScanBase();
    display.setCursor(14, 28);
    display.print("No reader found");
    display.display();
    displayBattery();
    delay(1200);
    return;
  }

  // ── Read the TgInitAsTarget response ──────────────────────────────────────
  // The response tells us what the initiator is doing (activation data).
  uint8_t initResp[32];
  uint8_t initRespLen = pn532_readResponse(initResp, sizeof(initResp), 500);
  Serial.print("[RFID] TgInitAsTarget response len=");
  Serial.println(initRespLen);

  // Update display
  display.setCursor(4, 42);
  display.print("Reader detected!");
  display.display();

  // ── APDU exchange loop ────────────────────────────────────────────────────
  // Respond to whatever commands the initiator sends us.
  // We implement a minimal ISO7816-4 responder so readers get proper errors
  // rather than silence (which causes them to reset / give up).
  //
  // Commands we handle:
  //   SELECT (by name) AID → 90 00
  //   SELECT (by ID)   FID → 90 00
  //   READ BINARY          → 6A 81 (function not supported)
  //   Anything else        → 6A 81 (function not supported)
  //
  // A proper NDEF emulation would serve the CC file and NDEF file contents.
  // That requires significantly more memory than an Arduino typically has,
  // but you can extend rfidNdefData[] below if your board has the RAM.

  uint8_t rxBuf[PN532_EMUL_BUFSIZE];
  uint8_t txBuf[PN532_EMUL_BUFSIZE];

  const uint8_t SW_OK[]   = { 0x90, 0x00 };
  const uint8_t SW_ERR[]  = { 0x6A, 0x81 };  // Function not supported

  // Loop for up to ~10 APDU exchanges or until the exchange fails.
  for (uint8_t round = 0; round < 20; round++) {
    // Get the next command from the reader
    uint8_t rxLen = pn532_tgGetData(rxBuf, sizeof(rxBuf));

    if (rxLen == 0) {
      Serial.println("[RFID] TgGetData returned 0 — reader gone / timeout");
      break;
    }

    Serial.print("[RFID] RX ("); Serial.print(rxLen); Serial.print("): ");
    for (uint8_t i = 0; i < rxLen; i++) {
      Serial.print(rxBuf[i], HEX); Serial.print(" ");
    }
    Serial.println();

    // Identify and respond to the APDU
    uint8_t txLen = 0;

    if (rxLen >= 2 && rxBuf[0] == 0x00 && rxBuf[1] == 0xA4) {
      // SELECT FILE command (CLA=00, INS=A4)
      txBuf[0] = 0x90; txBuf[1] = 0x00;
      txLen = 2;
      Serial.println("[RFID] -> SELECT → 90 00");

    } else if (rxLen >= 2 && rxBuf[0] == 0x00 && rxBuf[1] == 0xB0) {
      // READ BINARY — we don't have actual file data to serve
      // Return error (function not supported)
      txBuf[0] = 0x6A; txBuf[1] = 0x81;
      txLen = 2;
      Serial.println("[RFID] -> READ BINARY → 6A 81");

    } else if (rxLen >= 2 && rxBuf[0] == 0x00 && rxBuf[1] == 0xD6) {
      // UPDATE BINARY — reject writes
      txBuf[0] = 0x69; txBuf[1] = 0x82;  // Security status not satisfied
      txLen = 2;
      Serial.println("[RFID] -> UPDATE BINARY → 69 82");

    } else if (rxLen == 1 && rxBuf[0] == 0x60) {
      // RATS (Request for Answer to Select) — ISO14443-4 layer activation
      // ATS response: length=5, T0=0x78 (FSCI=8), TA1=0x77, TB1=0x77, TC1=0x02
      uint8_t ats[] = { 0x05, 0x78, 0x77, 0x77, 0x02 };
      memcpy(txBuf, ats, sizeof(ats));
      txLen = sizeof(ats);
      Serial.println("[RFID] -> RATS → ATS");

    } else if (rxLen == 2 && rxBuf[0] == 0x30) {
      // NTAG READ command (ISO14443-3)
      uint8_t startPage = rxBuf[1];
      if (startPage < 61) { // 64 total pages, need to return 4 pages (16 bytes)
        memcpy(txBuf, &ntagData[startPage * 4], 16);
        txLen = 16;
        Serial.print("[RFID] -> NTAG READ page "); Serial.print(startPage);
        Serial.println(" → 16 bytes");
      } else {
        txBuf[0] = 0x00; // NAK
        txLen = 1;
        Serial.println("[RFID] -> NTAG READ out of bounds → NAK");
      }

    } else if (rxLen >= 6 && rxBuf[0] == 0xA2) {
      // NTAG WRITE command (ISO14443-3)
      uint8_t pageAddr = rxBuf[1];
      if (pageAddr < 64) {
        memcpy(&ntagData[pageAddr * 4], &rxBuf[2], 4);
        txBuf[0] = 0x0A; // ACK
        txLen = 1;
        Serial.print("[RFID] -> NTAG WRITE page "); Serial.println(pageAddr);
      } else {
        txBuf[0] = 0x00; // NAK
        txLen = 1;
        Serial.println("[RFID] -> NTAG WRITE out of bounds → NAK");
      }

    } else {
      // Unknown command — return function-not-supported
      txBuf[0] = 0x6A; txBuf[1] = 0x81;
      txLen = 2;
      Serial.print("[RFID] -> Unknown cmd → 6A 81  (INS=0x");
      if (rxLen >= 2) Serial.print(rxBuf[1], HEX);
      Serial.println(")");
    }

    // Send our response
    if (txLen > 0) {
      bool ok = pn532_tgSetData(txBuf, txLen);
      if (!ok) {
        Serial.println("[RFID] TgSetData failed — reader may have left");
        break;
      }
    }
  }

  Serial.println("[RFID] Emulation done.");
  displayBattery();
}

// ─── Save ─────────────────────────────────────────────────────────────────────
// Saves the current tag data to the SD card.
//
// File format (one value per line):
//   <rfidCardType>
//   <rfidUidLen>
//   <rfidUid[0]>
//   …
//   <rfidUid[rfidUidLen-1]>
//
// Files are auto-numbered: /rfid/rfid_00.txt … /rfid/rfid_99.txt
void saveRfid() {
  drawScanBase();

  if (isScanning != 0) {
    display.setCursor(30, 30);
    display.println("Nothing to save");
    displayBattery();
    return;
  }

  if (!sdReady) {
    display.setCursor(33, 30);
    display.println("SD Error...");
    displayBattery();
    return;
  }

  display.setCursor(33, 30);
  display.println("Saving...");

  for (int i = 0; i < 100; i++) {
    String path;
    if (i < 10) {
      path = "/rfid/rfid_0" + String(i) + ".txt";
    } else {
      path = "/rfid/rfid_"  + String(i) + ".txt";
    }

    if (!SD.exists(path)) {
      file = SD.open(path, FILE_WRITE);
      file.println(rfidCardType);  // Line 1: card type
      file.println(rfidUidLen);    // Line 2: UID length

      // Lines 3+: one UID byte per line as a decimal integer.
      for (int j = 0; j < rfidUidLen; j++) {
        file.println(rfidUid[j]);
      }

      // If it's an NTAG (7-byte UID), save the 256 bytes of memory data
      if (rfidUidLen == 7) {
        for (int j = 0; j < 256; j++) {
          file.println(ntagData[j]);
        }
      }

      file.close();
      break;
    }
  }

  displayBattery();
}
