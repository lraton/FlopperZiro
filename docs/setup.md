# Setup

## Prerequisites
Before starting, ensure you have the following:
- STM32-L432KC (Microcontroller) 
- FS1000a (Radio freq Transmitter) 
- RXB12 (Radio freq Receiver) 
- PN532 (RFID/NFC)
    - PN7150 (need to test)
- IR LED & Reciver
- 2N222A PBFREE
- SSD1306(Display 128x64 OLED) 
- TF CardMemory Shield 
- TP4056 (Battery charger) 
- DC-DC 5V BOOST (voltage booster) 
- 6x 6x6x8mm push button 
- Micro usb smd 
- Battery 3.7 lipo

## PCB
The pcb may change during updates, I try to keep this page updated.
To send them to print you can use the  [gerber](https://github.com/lraton/FlopperZiro/blob/main/PCB%20and%20Schematic/PCB/Gerber_FlopperZiro_PCB_FlopperZiro_2024.zip) files in the repo, or you can upload them to [EasyEda](https://u.easyeda.com/) using the [.json](https://github.com/lraton/FlopperZiro/blob/main/PCB%20and%20Schematic/PCB/PCB_PCB_FlopperZiro_2024.json) files always present in the repo.

The same thing you can do for upload the scheme on EasyEda.

I reccoment to use EasyEda (or any other software) only for edit the schematic and pcb, i you want only to print just use the gerber.

For the print i had used [JLCPCB](https://jlcpcb.com/), and if you use the same on the print setting don't touch anything except the Mark on PCB where you need to put it **Order Number(Specify Position)**

### Hardware
1. Assemble the components according to the schematic provided in the project repository.
2. Connect the STM32-L432KC board to your PC via USB. Use the microcontroller usb not the pcb one for programming

### Software
1. Download the **Arduino IDE** from [here](https://www.arduino.cc/en/software).
2. Install the required libraries (see `libraries.txt` in the repo).
3. Clone the FlopperZiro repository:
   ```bash
   git clone https://github.com/lraton/FlopperZiro.git
4. Take an sdcard and copy the [file system](https://github.com/lraton/FlopperZiro/blob/main/SD%20Root%20Folder/) inside it 

