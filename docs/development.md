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
| `scelta` | `int` | Currently active module (0=menu, 1=badusb, 2=rfid, 3=ir, 4=rf) |
| `sceltaSubMenu` | `int` | Current submenu option (0=submenu, 1=scan, 2=SD) |
| `type` | `int` | Active module type (mirrors `scelta`, used by SD browser) |
| `scanning` | `int` | `1` = actively scanning, `0` = result ready/loaded |
| `detectionStarted` | `bool` | Non-blocking RFID detection flag |
| `uid[]` | `uint8_t[8]` | Last read/loaded RFID UID bytes |
| `uidLength` | `uint8_t` | Length of `uid[]` (4 or 7) |
| `cardType` | `String` | Last read/loaded card type string |
| `selectedFile` | `char[50]` | Currently selected SD file name |
| `sceltaSd` | `int` | `0` = browsing, `≥1` = file selected |
| `sdbegin` | `bool` | `true` if SD card initialized successfully |

### Main Loop Flow

```
loop()
  └─ switch(scelta)
       ├─ 0 → displayMenu()     ← main menu / RFID lock
       ├─ 1 → badusb()
       │        └─ switch(sceltaSubMenu): subMenuDisplay | selectedbadusb | sdMenuDisplay
       ├─ 2 → rfid()
       │        └─ switch(sceltaSubMenu): subMenuDisplay | scanRfid | sdMenuDisplay
       ├─ 3 → ir()
       │        └─ switch(sceltaSubMenu): subMenuDisplay | scanIr | sdMenuDisplay
       └─ 4 → rf()
                └─ switch(sceltaSubMenu): subMenuDisplay | scanRf | sdMenuDisplay
```

Every module screen ends with:
1. `battery()` — refreshes battery and SD status on the display
2. `checkModuleButton(type)` — reads buttons, dispatches save/emulate/back

---

## Adding a New Module

1. Add a new `.ino` file to `main/`.
2. Define `void myModule()` that sets `type = N` and switches on `sceltaSubMenu`.
3. Add `case N: myModule(); break;` to `loop()` in `main.ino`.
4. Add `case N: dir = SD.open("/mymodule/"); break;` in `handleSubMenuSelectButton()` and `checkSdButton()` in `buttonMenu.ino`.
5. Add `case N: saveMyModule(); break;` and `case N: emulateMyModule(); break;` in `checkModuleButton()`.
6. Add `case N: /* load */ break;` in `selectedSd()` in `sdList.ino`.
7. Add the submenu and menu graphic functions in `graphics.ino`.

---

## Libraries

| Library | Version | Used for |
|---|---|---|
| Adafruit PN532 | 1.3.3 | RFID read, emulate (tgInitAsTarget) |
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
