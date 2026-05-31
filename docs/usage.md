# Usage

## Button Layout

FlopperZiro uses **5 buttons** for navigation:

```
        [ UP ]
[ LEFT ] [ SELECT ] [ RIGHT ]
        [ DOWN ]
```

| Button | Main Menu | Module Screen |
|---|---|---|
| **UP** | Previous menu page | Start scanning / reset scan |
| **DOWN** | Next menu page | Save current signal to SD |
| **LEFT** | — | Back to submenu |
| **RIGHT** | — | Emulate / replay signal |
| **SELECT** | Enter selected module | — |

---

## Main Menu

On power-up, FlopperZiro boots to the **home screen** showing the FlopperZiro logo, battery percentage, and SD free space.

Navigate with **UP / DOWN** through the module pages, then press **SELECT** to enter.

| Page | Module |
|---|---|
| 0 | Home screen |
| 1 | 🦆 BadUSB |
| 2 | 🔖 RFID/NFC |
| 3 | 🔴 IR |
| 4 | 📡 RF |

---

## Submenu

Every module has a **submenu** with three options, navigated with **UP / DOWN**, selected with **SELECT**:

| Option | Action |
|---|---|
| 0 | *(submenu display — navigate here)* |
| 1 | **Scan / Use** — enter the active module screen |
| 2 | **SD** — browse saved files on the SD card |

Press **LEFT** from inside any module to return to the submenu, and again to return to the main menu.

---

## Module Screens

### 🦆 BadUSB

1. Navigate to **BadUSB → SD** to browse `/badusb/` on the SD card.
2. Select a DuckyScript `.txt` file using **UP / DOWN** → **SELECT**.
3. Plug the device into a target computer via its USB-C/micro port.
4. Press **RIGHT** to execute the script. The device types the script as a keyboard.

> The script engine supports: `STRING`, `DELAY`, `REM`, modifier keys (`CTRL`, `ALT`, `SHIFT`, `GUI`), arrows, function keys F1–F12, and all printable characters.

---

### 🔖 RFID / NFC

1. Navigate to **RFID → Scan**.
2. Hold a card near the PN532 — the OLED shows the **UID** and **card type** when detected.
3. Press **DOWN** to save the tag to `/rfid/rfid_NN.txt` on the SD card.
4. Press **RIGHT** to **emulate** the tag immediately (holds the PN532 in target mode for 3 s).

To **replay a saved tag**:
1. Go to **RFID → SD**, select a file.
2. The tag is loaded into memory and shown on screen.
3. Press **RIGHT** to emulate it.

**Supported card types:**

| Card Type | UID Length | ATQA | SAK |
|---|---|---|---|
| Mifare Classic 1K | 4 bytes | `0x00 0x04` | `0x08` |
| Mifare Ultralight | 7 bytes | `0x00 0x44` | `0x00` |
| NTAG213/215/216 | 7 bytes | `0x00 0x44` | `0x00` |

---

### 🔴 IR

1. Navigate to **IR → Scan**.
2. Point a remote at the IR receiver — encoding type and data value appear on screen.
3. Press **DOWN** to save to `/ir/ir_NN.txt`.
4. Press **RIGHT** to re-transmit the captured signal (raw timing replay at 38 kHz).

To **replay a saved IR code**:
1. Go to **IR → SD**, select a file.
2. Press **RIGHT** to send.

---

### 📡 RF (433 MHz)

1. Navigate to **RF → Scan**.
2. Point a transmitter (e.g., remote, key fob) at the RXB12 — value, bit length, and protocol are displayed.
3. Press **DOWN** to save to `/rf/rf_NN.txt`.
4. Press **RIGHT** to re-transmit using the FS1000A.

To **replay a saved RF code**:
1. Go to **RF → SD**, select a file.
2. Press **RIGHT** to send.

---

## Status Bar

Every module screen shows a **persistent status bar**:

- **Top left:** SD free space percentage (or "No SD" if no card)
- **Top right:** Battery percentage

On the home screen these appear at the bottom right instead.

---

## Lock Mode

FlopperZiro supports an **optional RFID lock**: if the `deviceUnlocked` variable is set to `false` in `main.ino`, the device will wait for a specific authorized card (defined by the `authorizedUid[]` array) before showing the main menu. Set `deviceUnlocked = true` to disable this feature.