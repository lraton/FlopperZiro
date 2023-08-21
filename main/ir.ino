void ir() {
  type = 3;
  switch (sceltaSubMenu) {
    case 0:
      subMenuDisplay();
      break;
    case 1:
      scanIr();
      break;
    case 2:
      sdMenuDisplay(3);
      break;
  }
}

//+=============================================================================
// Display IR code
//
void scanIr() {
  if (scanning == 1) {
    graficascan();
    decode_results results;             // Somewhere to store the results
    if (IrReceiver.decode(&results)) {  // Grab an IR code
      dumpInfo(&results);               // Output the results
      dumpCode(&results);               // Output the results as source code
      Serial.println("");               // Blank line between entries
      scanbase();
      display.setCursor(20, 25);
      display.println("Encoding: " + irproducer);
      display.setCursor(20, 35);
      display.println("Data: 0x" + data);
      Serial.println(irproducer);
      IrReceiver.resume();  // Prepare for the next value
      scanning = 0;
    }
  } else {
    scanbase();
    display.setCursor(20, 25);
    display.println("Encoding: " + irproducer);
    display.setCursor(20, 35);
    display.println("Data: 0x" + data);
  }
  battery();
  checkModuleButton(3);
}
void ircode(decode_results *results) {
  // Panasonic has an Address
  if (results->decode_type == PANASONIC) {
    Serial.print(results->address, HEX);
    Serial.print(":");
  }

  // Print Code
  Serial.print(results->value, HEX);
}

//+=============================================================================
// Display encoding type
//
void encoding(decode_results *results) {
  switch (results->decode_type) {
    default:
    case UNKNOWN:
      irproducer = "UNKNOWN";
      Serial.print("UNKNOWN");
      break;
    case NEC:
      irproducer = "NEC";
      Serial.print("NEC");
      break;
    case SONY:
      irproducer = "SONY";
      Serial.print("SONY");
      break;
    case RC5:
      irproducer = "RC5";
      Serial.print("RC5");
      break;
    case RC6:
      irproducer = "RC6";
      Serial.print("RC6");
      break;
    case SHARP:
      irproducer = "SHARP";
      Serial.print("SHARP");
      break;
    case JVC:
      irproducer = "JVC";
      Serial.print("JVC");
      break;
    case BOSEWAVE:
      irproducer = "BOSEWAVE";
      Serial.print("BOSEWAVE");
      break;
    case SAMSUNG:
      irproducer = "SAMSUNG";
      Serial.print("SAMSUNG");
      break;
    case LG:
      irproducer = "LG";
      Serial.print("LG");
      break;
    case WHYNTER:
      irproducer = "WHYNTER";
      Serial.print("WHYNTER");
      break;
    case KASEIKYO:
      irproducer = "KASEIKYO";
      Serial.print("KASEIKYO");
      break;
    case PANASONIC:
      irproducer = "PANASONIC";
      Serial.print("PANASONIC");
      break;
    case DENON:
      irproducer = "DENON";
      Serial.print("Denon");
      break;
  }
}

//+=============================================================================
// Dump out the decode_results structure.
//
void dumpInfo(decode_results *results) {
  // Check if the buffer overflowed
  if (results->overflow) {
    Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWLEN");
    return;
  }

  // Show Encoding standard
  Serial.print("Encoding  : ");
  encoding(results);
  Serial.println("");

  // Show Code & length
  Serial.print("Code      : ");
  ircode(results);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
}

//+=============================================================================
// Dump out the decode_results structure.
//
void dumpCode(decode_results *results) {
  // Start declaration
  Serial.print("unsigned int  ");          // variable type
  Serial.print("rawData[");                // array name
  Serial.print(results->rawlen - 1, DEC);  // array size
  Serial.print("] = {");                   // Start declaration

  // Dump data
  for (int i = 1; i < results->rawlen; i++) {
    rawData[i - 1] = results->rawbuf[i] * USECPERTICK;
    Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
    if (i < results->rawlen - 1) Serial.print(",");  // ',' not needed on last one
    if (!(i & 1)) Serial.print(" ");
  }

  // End declaration
  Serial.print("};");  //

  // Comment
  Serial.print("  // ");
  encoding(results);
  Serial.print(" ");
  ircode(results);

  // Newline
  Serial.println("");

  // Now dump "known" codes
  if (results->decode_type != UNKNOWN) {

    // Some protocols have an address
    if (results->decode_type == PANASONIC) {
      Serial.print("unsigned int  addr = 0x");
      data = String(results->address, HEX);
      Serial.print(results->address, HEX);
      Serial.println(";");
    }

    // All protocols have data
    Serial.print("unsigned int  data = 0x");
    data = String(results->value, HEX);
    Serial.print(results->value, HEX);
    Serial.println(";");
  }
}

void emulateIr() {
  scanbase();
  display.setCursor(33, 30);
  display.println("Sending...");
  battery();
  IrSender.sendRaw(rawData, 67, freq_ir);
  delay(2000);
}

void saveIr() {
  scanbase();
  if (scanning == 0) {
    if (sdbegin) {
      display.setCursor(33, 30);
      display.println("Saving...");
      if (SD.exists("/ir/prova.txt")) {
        display.setCursor(33, 30);
        display.println("Già esistente");
      } else {
        file = SD.open("/ir/prova.txt", FILE_WRITE);
        file.println(irproducer);
        file.println(data);
        for (int i = 0; i < 67; i++) {
          file.print(rawData[i]);
          if (i != 66) {
            file.print(" ");
          }
        }
        file.println();
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
