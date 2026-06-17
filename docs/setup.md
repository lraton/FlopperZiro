# Setup

## Hardware Requirements

### Components

| Component | Purpose |
|---|---|
| **STM32-L432KC** | Main microcontroller (ARM Cortex-M4, 80 MHz) |
| **PN532** | RFID/NFC reader & emulator (I²C mode) |
| **FS1000A** | 433 MHz RF transmitter |
| **RXB12** | 433 MHz RF receiver |
| **IR LED + IR receiver** | Infrared transmit and receive |
| **2N2222A PBFREE** | NPN transistor for IR LED drive |
| **SSD1306** | 128×64 OLED display (I²C) |
| **TF Card / MicroSD shield** | SD card storage (SPI) |
| **TP4056** | LiPo battery charger IC |
| **DC-DC 5V Boost** | Voltage step-up from 3.7 V battery to 5 V |
| **6× 6×6×6×8 mm push button** | Navigation buttons |
| **Micro USB SMD** | PCB charging port |
| **3.7 V LiPo battery** | Power source |

> **Note:** PN7150 is listed as a possible RFID alternative but has not been tested yet.

---

## Pin Map

| Signal | STM32 Pin |
|---|---|
| IR Receiver | `D6` (`6` in code) |
| IR Sender | `D9` |
| RF Receiver | `D3` |
| RF Transmitter | `A6` |
| SD Card CS | `A0` |
| Button UP | `A4` |
| Button DOWN | `A0` |
| Button LEFT | `A3` |
| Button SELECT | `A2` |
| Button RIGHT | `A1` |
| Battery ADC | `A4` |
| RFID IRQ | `D1` |
| RFID RESET | `D0` |
| I²C SDA (Display + PN532) | Default I²C SDA |
| I²C SCL (Display + PN532) | Default I²C SCL |

> The PN532 is addressed at I²C address **0x24** and the SSD1306 at **0x3C**. Both share the same I²C bus.

---

## PCB

The PCB is available in two formats inside the repository under `PCB and Schematic/PCB/`:

- **Gerber ZIP** — send directly to a PCB manufacturer (e.g., [JLCPCB](https://jlcpcb.com/))
- **JSON** — import into [EasyEDA](https://easyeda.com/) for editing

When ordering from JLCPCB, leave all settings at their defaults except **"Order Number"** — set that to **"Specify Position"** to place it where indicated on the silkscreen.

> Use the microcontroller's native USB port for **programming**, not the PCB's USB port. The PCB USB is for **charging only**.

---

## Software Setup

### 1 — Install Arduino IDE

Download Arduino IDE 2.x from [arduino.cc](https://www.arduino.cc/en/software).

### 2 — Add STM32 board support

In Arduino IDE → **File → Preferences**, add this URL to *Additional boards manager URLs*:

```
https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
```

Then go to **Tools → Board → Boards Manager** and install **STM32 MCU based boards**.

Select: `Tools → Board → STM32 MCU based boards → Nucleo-32` and set **Board part number** to `Nucleo L432KC`.

### 3 — Install Libraries

Go to **Tools → Manage Libraries** and install the following:

| Library | Version |
|---|---|
| SdFat | 2.2.3 |
| Adafruit SSD1306 | 2.5.12 |
| Adafruit GFX Library | 2.11.11 |
| Adafruit PN532 | 1.3.3 |
| IRremote | 4.4.1 |
| RCSwitch | 2.6.4 |
| Keyboard | 1.0.6 |

`SPI`, `Wire`, and `string.h` are part of the standard Arduino / STM32 core.

### 4 — Clone and open the sketch

```bash
git clone https://github.com/lraton/FlopperZiro.git
```

Open the `main/` folder in Arduino IDE (it will load all `.ino` tab files automatically).

### 5 — Prepare the SD card

Format an SD card as **FAT32**. Copy the folder structure from `SD Root Folder/` in the repository into the root of the card:

```
/
├── badusb/     ← place .txt DuckyScript files here
├── ir/         ← IR signals saved here
├── rf/         ← RF signals saved here
└── rfid/       ← RFID/NFC tags saved here
```

### 6 — Flash

Connect the STM32-L432KC via its native USB, select the correct COM port in Arduino IDE, and click **Upload**.
