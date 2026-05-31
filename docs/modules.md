# Modules

Detailed technical reference for every FlopperZiro module.

---

## 🦆 BadUSB

**Source file:** `main/badusb.ino`

FlopperZiro enumerates as a **USB HID keyboard** (using the Arduino `Keyboard` library) and replays DuckyScript-style text files stored on the SD card.

### Script Syntax

Scripts are plain `.txt` files placed in `/badusb/` on the SD card. Each line is one command:

| Command | Syntax | Description |
|---|---|---|
| `STRING` | `STRING Hello World` | Types the text as keyboard input |
| `DELAY` | `DELAY 1000` | Waits the specified number of milliseconds |
| `REM` | `REM This is a comment` | Comment line — ignored |
| Key names | `CTRL ALT DELETE` | Presses modifier + key combinations |
| Single char | `a` | Types that single character |

**Supported key names:**

`ENTER` `CTRL` `SHIFT` `ALT` `GUI` `UP` `UPARROW` `DOWN` `DOWNARROW` `LEFT` `LEFTARROW` `RIGHT` `RIGHTARROW` `DELETE` `PAGEUP` `PAGEDOWN` `HOME` `END` `ESC` `INSERT` `TAB` `CAPSLOCK` `SPACE` `F1`–`F12`

### Example Script

```
REM Open notepad on Windows
GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
STRING Hello from FlopperZiro!
ENTER
```

### How It Works

1. The selected file is opened from `/badusb/<filename>`.
2. Each line is parsed: the command before the first space determines the action.
3. For multi-key combos (e.g., `CTRL ALT DELETE`), each token is pressed in sequence with a 5 ms delay, then all keys are released together via `Keyboard.releaseAll()`.

---

## 🔖 RFID / NFC

**Source file:** `main/rfid.ino`

Uses the **Adafruit PN532** library in **I²C mode** (`IRQ = D1`, `RESET = D0`). The PN532 is shared with the display on the same I²C bus.

### Reading Tags

Scanning uses **non-blocking** detection:

1. `nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)` is called once when a scan starts.
2. On each loop iteration, `nfc.readDetectedPassiveTargetID(rfidUid, &rfidUidLen)` is polled — it returns immediately if no card is present, keeping the UI responsive.

**Card type detection:**

| UID Length | Assumed type | Extra read attempted |
|---|---|---|
| 4 bytes | Mifare Classic | Block 4 (with default key `FF FF FF FF FF FF`) |
| 7 bytes | Mifare Ultralight | Page 4; falls back to NTAG2xx (pages 0–41) if Ultralight read fails |

### Saving Tags

Tags are saved with `saveRfid()` when **DOWN** is pressed. Files are auto-numbered `/rfid/rfid_00.txt` … `/rfid/rfid_99.txt` — the first available slot is used.

**File format — see [SD Card File Format](sd-format.md).**

### Loading Tags from SD

Selecting a file in the RFID SD browser calls `loadSelectedFile(2)`, which reads the file back into the global `rfidUid[]`, `rfidUidLen`, and `rfidCardType` variables. The scan screen then shows the loaded tag, ready to emulate.

### Emulating Tags

`emulateRfid()` manually builds the raw **PN532 TgInitAsTarget** command buffer (command code `0x8C`) according to the PN532 User Manual §7.3.9 (since the standard Adafruit library doesn't expose a public `tgInitAsTarget` API). 

| Card type | ATQA | SAK | UID bytes |
|---|---|---|---|
| Mifare Classic | `0x04 0x00` | `0x08` | 4 |
| NTAG / Ultralight | `0x44 0x00` | `0x00` | 7 |

It transmits the command frame using `nfc.sendCommandCheckAck()` and blocks for up to **3 seconds** waiting for a reader to make contact. Once the reader contacts the tag, the raw response is drained from the PN532's output FIFO directly over the I²C bus using `Wire.requestFrom()`. Finally, it resets the PN532 to a clean active state by calling `nfc.SAMConfig()`.

> **Note:** Emulation quality depends on the reader's tolerance. Most access-control readers and NFC phones work well for 4-byte UIDs. 7-byte UID emulation works on phones and lenient readers; strict readers may reject it.

---

## 🔴 IR

**Source file:** `main/ir.ino`

Uses the **IRremote** library version 4.x.

- **Receive pin:** `D6` — `IrReceiver`
- **Send pin:** `D9` — `IrSender`
- **Carrier frequency:** 38 kHz

### Receiving

`IrReceiver.decode()` is called each loop iteration. On success:

- `irProtocol` is set to the protocol name (NEC, SONY, RC5, RC6, SAMSUNG, LG, PANASONIC, etc. — `UNKNOWN` if unrecognized).
- `irHexValue` is set to the decoded hex value as a string.
- `irRawData[67]` stores raw timing values in microseconds for lossless retransmit.

### Transmitting

`emulateIr()` calls `IrSender.sendRaw(irRawData, 67, 38)`. Raw replay ensures compatibility even with proprietary or unrecognized protocols.

### File format

Each IR file contains 3 lines: protocol name, hex value, and space-separated raw timing array. **See [SD Card File Format](sd-format.md).**

---

## 📡 RF (433 MHz)

**Source file:** `main/rf.ino`

Uses the **RCSwitch** library.

- **Receive pin:** `D3` — `rfSwitch.enableReceive(3)`
- **Transmit pin:** `A6` — `rfSwitch.enableTransmit(A6)`

### Receiving

`rfSwitch.available()` is polled each loop. On signal:

- `rfReceivedValue` — the decimal value of the received code
- `rfBitLength` — number of bits
- `rfReceivedProtocol` — protocol index (1–12, per RCSwitch docs)

### Transmitting

`emulateRf()` calls `rfSwitch.send(rfReceivedValue, rfBitLength)`. The protocol is not re-sent (RCSwitch defaults to protocol 1 for send); for advanced replay, the protocol would need to be saved and set before `send()` using `rfSwitch.setProtocol(rfReceivedProtocol)`.

---

## 🔋 Battery

**Source file:** `main/battery.ino`

Battery level is read from pin `A4` via a **100 kΩ / 100 kΩ voltage divider** on the battery line:

```
voltage = ((analogRead(A4) × 3.3) / 1024) × 2 − calibration
```

`calibration = 2.25` compensates for component tolerances. The voltage is then mapped from the range **2.8 V → 4.2 V** to **0% → 100%**.

The percentage is clamped to `[1, 100]` and displayed via `displayBattery()`:
- **Home screen:** bottom-right corner, alongside an SD icon
- **All other screens:** top-right corner as `XX%`

---

## 💾 SD Card

**Source files:** `main/sdList.ino`, `main/sdFreeSpace.ino`

Uses **SdFat** (v2.2.3) at `SD_SCK_MHZ(4)` on pin `A0` (CS).

### File Browser

`drawSdFileList()` shows up to **6 files per page** from a directory. The selected file is highlighted with `>`. Navigation: **UP / DOWN** scrolls, **SELECT** confirms, **LEFT** exits.

### Free Space

`getSdFreePercent()` reads cluster counts from the FAT volume and computes `(freeBytes / totalBytes) × 100`. This is refreshed after every save operation.

### Directory Structure

```
/
├── badusb/   ← DuckyScript .txt files
├── ir/       ← ir_00.txt … ir_99.txt
├── rf/       ← rf_00.txt … rf_99.txt
└── rfid/     ← rfid_00.txt … rfid_99.txt
```

Files are auto-numbered; the first available slot (0–99) is used on save.
