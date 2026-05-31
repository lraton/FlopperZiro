# SD Card File Format

FlopperZiro uses a simple, human-readable line-by-line format for all saved files. Every value is on its own line, making files easy to inspect and edit in any text editor.

---

## RFID / NFC — `/rfid/rfid_NN.txt`

```
<cardType>
<uidLength>
<uid[0]>
<uid[1]>
...
<uid[uidLength-1]>
```

**Field descriptions:**

| Line | Content | Example |
|---|---|---|
| 1 | Card type string | `Mifare Classic` |
| 2 | UID length in bytes (4 or 7) | `4` |
| 3 + | UID bytes, one decimal integer per line | `115` |

**Example — Mifare Classic (4-byte UID `73 E8 0F BA`):**

```
Mifare Classic
4
115
232
15
186
```

**Example — NTAG213 (7-byte UID `04 A1 B2 C3 D4 E5 F6`):**

```
NTAG2xx
7
4
161
178
195
212
229
246
```

> UID bytes are stored as **decimal integers** (0–255), one per line. When emulating, each byte is loaded back as `uint8_t`.

---

## IR — `/ir/ir_NN.txt`

```
<protocol>
<hex_value>
<raw_timing_0> <raw_timing_1> ... <raw_timing_66> 
```

**Field descriptions:**

| Line | Content | Example |
|---|---|---|
| 1 | Protocol name string | `NEC` |
| 2 | Decoded value as hex string | `FF00FF` |
| 3 | 67 raw timing values in microseconds, space-separated | `9000 4500 560 560 ...` |

**Example:**

```
NEC
20DF10EF
9024 4512 564 564 564 1692 564 564 ...
```

> The raw timing line is always 67 values (matching `uint16_t rawData[67]`). When replaying, `IrSender.sendRaw(rawData, 67, 38)` uses this array directly.

---

## RF — `/rf/rf_NN.txt`

```
<value>
<bitlength>
<protocol>
```

**Field descriptions:**

| Line | Content | Example |
|---|---|---|
| 1 | Received value (decimal) | `5592405` |
| 2 | Bit length | `24` |
| 3 | RCSwitch protocol index | `1` |

**Example:**

```
5592405
24
1
```

> On replay, `mySwitch.send(rfvalue, rfbit)` is called. The protocol index is stored for reference but RCSwitch defaults to protocol 1 when sending — advanced users can add `mySwitch.setProtocol(rfprotocol)` before the send call.

---

## BadUSB — `/badusb/*.txt`

DuckyScript-style plain text files. No special format imposed by FlopperZiro — write any filename you like. See [Modules → BadUSB](modules.md) for the full syntax reference.

**Example (`hello.txt`):**

```
REM Open run dialog
GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
STRING Hello from FlopperZiro!
ENTER
```
