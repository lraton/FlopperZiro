# Flopper Ziro
<p align="center">
<img alt="Github top language" src="https://img.shields.io/github/languages/top/lraton/FlopperZiro?color=56BEB8">
<img alt="Github language count" src="https://img.shields.io/github/languages/count/lraton/FlopperZiro?color=56BEB8">
<img alt="Repository size" src="https://img.shields.io/github/repo-size/lraton/FlopperZiro?color=56BEB8">
<img alt="License" src="https://img.shields.io/github/license/lraton/FlopperZiro?color=56BEB8">
<img alt="Github issues" src="https://img.shields.io/github/issues/lraton/FlopperZiro?color=56BEB8">
<img alt="Github forks" src="https://img.shields.io/github/forks/lraton/FlopperZiro?color=56BEB8">
<img alt="Github stars" src="https://img.shields.io/github/stars/lraton/FlopperZiro?color=56BEB8">

An orribol copi of FlipperZero, uit worst inglish<br>
A Flipper Zero clone, but cheapest, DIY and simply Open Source, made with Arduino IDE<br>
<br>
> ⚠️ **Important Notice**: This is just a fun project, it was started to pass the time, it is not at all professional and is not intended to be. Is not an alternative for professional device.
<br>

![side pic](/img/img-profile.jpg)

## Star History
[![Star History Chart](https://api.star-history.com/svg?repos=lraton/FlopperZiro&type=Date)](https://star-history.com/#lraton/FlopperZiro&Date)

## Component:
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


## Features

- RubberDucky
- RFID/NFC (Work in progress)
- IR 
    - Read ir signal and emulate it or save it
- RF
    - Read rf signal and emulate it or save it
- Save/Load from SD
- Battery percentage
- Sd memory left percentage
- All programmable via Arduino Ide

- Add 3d printed shell (https://www.printables.com/model/1142768-flopper-ziro-shell), not finished yet

## To Do
### Software
- [X]    Resovle SD problem with SSD1306
    - [X]    Do the sd menu
    - [X]    save/load from sd
    - [X]    Make sd listing
        - [X]    Fix bugs on the sd listing 
- [X]    Finish the rf scanner and sender
- [X]   Menu'
- [X]   BadUsb
- [X]   Ir
    - [X] Fix bug when show ir data
- [ ]   Rfid
    - [X]    Read UID
    - [ ]    Read ISO14443B
    - [ ]    Emulating
    - [ ]    Finish save rfid

### Hardware
- [X]   First pcb
- [ ]   New version of pcb
- [ ]   Try PN7150 intead of PN532

### Extra
- [ ]   Documentation

![front pic](/img/img-front.jpg)
![back pic](/img/img-behind.jpg)

![Alt](https://repobeats.axiom.co/api/embed/aec99a18c4d9023826a57995bd009c068bbb8197.svg "Repobeats analytics image")
