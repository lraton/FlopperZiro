# Development

## Code Architecture

The firmware is a **multi-file Arduino sketch** in the `main/` folder. Arduino IDE compiles all `.ino` files in the same directory together — each file is a logical module.

### File Map

| File | Responsibility |
|---|---|
| `main.ino` | Global variables, `setup()`, `loop()`, all `#include` directives |
| `menu.ino` | Main menu display and RFID lock gate |
| `buttonMenu.ino` | All button handlers (main menu, submenu, module screen, SD browser) |
| `graphics.ino` | All OLED drawing functions and bitmap assets |
| `badusb.ino` | BadUSB / DuckyScript execution |
| `rfid.ino` | RFID scan, save, load, and emulation |
| `ir.ino` | IR receive, decode, transmit, and save |
| `rf.ino` | RF 433 MHz receive, transmit, and save |
| `battery.ino` | ADC battery read and display |
| `sdList.ino` | SD file browser, file counter, and file loader |
| `sdFreeSpace.ino` | SD free-space calculation |

### Global State (defined in `main.ino`)

| Variable | Type | Purpose |
|---|---|---|
| `selectedModule` | `int` | Currently active module (0=menu, 1=badusb, 2=rfid, 3=ir, 4=rf) |
| `selectedSubMenu` | `int` | Current submenu option (0=submenu, 1=scan, 2=SD) |
| `currentModuleType` | `int` | Active module type (mirrors `selectedModule`, used by SD browser) |
| `isScanning` | `int` | `1` = actively scanning, `0` = result ready/loaded |
| `rfidDetectionStarted` | `bool` | Non-blocking RFID detection flag |
| `rfidUid[]` | `uint8_t[8]` | Last read/loaded RFID UID bytes |
| `rfidUidLen` | `uint8_t` | Length of `rfidUid[]` (4 or 7) |
| `rfidCardType` | `String` | Last read/loaded card type string |
| `sdSelectedFile[]` | `char[50]` | Name of the file the user has chosen |
| `sdFileSelected` | `int` | `0` = browsing, `≥1` = file selected |
| `sdReady` | `bool` | `true` if SD card initialized successfully |
| `sdFreePercent` | `float` | Percentage of SD space remaining |
| `sdFileName[]` | `char[50]` | Buffer for a file name |
| `sdSelectedFileNum` | `int` | 1-based index of the highlighted SD file |
| `sdFileCount` | `int` | Total number of files in the open directory |
| `sdReadBuffer` | `String` | Temporary buffer for SD file reads |

### Main Loop Flow

```
loop()
  └─ switch(selectedModule)
       ├─ 0 → handleMainMenu()     ← main menu / RFID lock
       ├─ 1 → badUsb()
       │        └─ switch(selectedSubMenu): drawSubMenu() | showSelectedBadUsb() | drawSdMenu(1)
       ├─ 2 → rfid()
       │        └─ switch(selectedSubMenu): drawSubMenu() | scanRfid() | drawSdMenu(2)
       ├─ 3 → ir()
       │        └─ switch(selectedSubMenu): drawSubMenu() | scanIr() | drawSdMenu(3)
       └─ 4 → rf()
                └─ switch(selectedSubMenu): drawSubMenu() | scanRf() | drawSdMenu(4)
```

Every module screen ends with:
1. `displayBattery()` — refreshes battery and SD status on the display
2. `handleModuleButtons(moduleType)` — reads buttons, dispatches save/emulate/back

---

## Adding a New Module

1. Add a new `.ino` file to `main/`.
2. Define `void myModule()` that sets `currentModuleType = N` and switches on `selectedSubMenu`.
3. Add `case N: myModule(); break;` to `loop()` in `main.ino`.
4. Add `case N: dir = SD.open("/mymodule/"); break;` in `onSubMenuSelect()` and `handleSdButtons()` in `buttonMenu.ino`.
5. Add `case N: saveMyModule(); break;` and `case N: emulateMyModule(); break;` in `handleModuleButtons()`.
6. Add `case N: /* load */ break;` in `loadSelectedFile()` in `sdList.ino`.
7. Add the submenu and menu graphic functions in `graphics.ino`.

---

## Libraries

| Library | Version | Used for |
|---|---|---|
| Adafruit PN532 | 1.3.3 | RFID read, emulate (raw I2C command & Wire drain) |
| Adafruit SSD1306 | 2.5.12 | OLED display driver |
| Adafruit GFX | 1.11.11 | Display graphics primitives |
| SdFat | 2.2.3 | SD card (SPI, FAT32) |
| IRremote | 4.4.1 | IR receive and send |
| RCSwitch | 2.6.4 | 433 MHz RF receive and send |
| Keyboard | 1.0.6 | USB HID keyboard emulation |

---

## Roadmap

### Software

- [x] Menu system
- [x] BadUSB / DuckyScript
- [x] IR scan, save, replay
- [x] RF scan, save, replay
- [x] SD card file browser
- [x] RFID read (Mifare Classic, Ultralight, NTAG2xx)
- [x] RFID save to SD
- [x] RFID load from SD
- [x] RFID emulation (4-byte and 7-byte UIDs)
- [ ] ISO14443B support
- [ ] NDEF record read/write
- [ ] Improved RF protocol replay (respect captured protocol index)

### Hardware

- [x] First PCB version
- [ ] New PCB revision
- [ ] PN7150 evaluation (replacement for PN532)
- [ ] 3D-printed shell — [Printables](https://www.printables.com/model/1142768-flopper-ziro-shell) *(in progress)*

---

## Contributing

1. Fork the repository on GitHub.
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Commit your changes with clear messages.
4. Open a pull request — describe what you changed and why.

Bug reports and feature suggestions are welcome via [GitHub Issues](https://github.com/lraton/FlopperZiro/issues).

---

## License

FlopperZiro is released under the **GNU General Public License v3.0**.  
See [LICENSE](https://github.com/lraton/FlopperZiro/blob/main/LICENSE) for the full text.
