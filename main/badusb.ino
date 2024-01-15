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

//the badusb function
void badusb() {
  type = 1;
  switch (sceltaSubMenu) {
    case 0:
      subMenuDisplay();
      break;
    case 1:
      selectedbadusb();
      break;
    case 2:
      sdMenuDisplay(1);
      break;
  }
}
void selectedbadusb() {
  graficausb();
  if (sdbegin) {
    display.setCursor(33, 30);
    display.println(String(selectedFile));  //da cambiare in base a quale scegli
  } else {
    display.setCursor(33, 30);
    display.println("SD Error...");
  }
  battery();
  checkModuleButton(1);
}

void emulateUsb() {
  graficausb();
  String DEFAULT_FILE_NAME = "/badusb/" + String(selectedFile);
  if (sdbegin) {
    display.setCursor(33, 30);
    display.println("Sending...");
  } else {
    display.setCursor(33, 30);
    display.println("SD Error...");
  }
  battery();
  file = SD.open(DEFAULT_FILE_NAME);
  if (file) {
    Keyboard.begin();

    String line = "";
    while (file.available()) {
      char m = file.read();
      if (m == '\n') {
        Line(line);
        line = "";
      } else if ((int)m != 13) {
        line += m;
      }
    }
    Line(line);

    file.close();
  }
  Keyboard.end();
}

void Line(String l) {
  int space_1 = l.indexOf(" ");
  if (space_1 == -1) {
    Press(l);
  } else if (l.substring(0, space_1) == "STRING") {
    Keyboard.print(l.substring(space_1 + 1));
  } else if (l.substring(0, space_1) == "DELAY") {
    int delaytime = l.substring(space_1 + 1).toInt();
    delay(delaytime);
  } else if (l.substring(0, space_1) == "REM") {
  } else {
    String remain = l;

    while (remain.length() > 0) {
      int latest_space = remain.indexOf(" ");
      if (latest_space == -1) {
        Press(remain);
        remain = "";
      } else {
        Press(remain.substring(0, latest_space));
        remain = remain.substring(latest_space + 1);
      }
      delay(5);
    }
  }
  battery();
  Keyboard.releaseAll();
}


void Press(String b) {
  if (b.length() == 1) {
    char c = b[0];
    Keyboard.press(c);
  } else if (b.equals("ENTER")) {
    Keyboard.press(KEY_RETURN);
  } else if (b.equals("CTRL")) {
    Keyboard.press(KEY_LEFT_CTRL);
  } else if (b.equals("SHIFT")) {
    Keyboard.press(KEY_LEFT_SHIFT);
  } else if (b.equals("ALT")) {
    Keyboard.press(KEY_LEFT_ALT);
  } else if (b.equals("GUI")) {
    Keyboard.press(KEY_LEFT_GUI);
  } else if (b.equals("UP") || b.equals("UPARROW")) {
    Keyboard.press(KEY_UP_ARROW);
  } else if (b.equals("DOWN") || b.equals("DOWNARROW")) {
    Keyboard.press(KEY_DOWN_ARROW);
  } else if (b.equals("LEFT") || b.equals("LEFTARROW")) {
    Keyboard.press(KEY_LEFT_ARROW);
  } else if (b.equals("RIGHT") || b.equals("RIGHTARROW")) {
    Keyboard.press(KEY_RIGHT_ARROW);
  } else if (b.equals("DELETE")) {
    Keyboard.press(KEY_DELETE);
  } else if (b.equals("PAGEUP")) {
    Keyboard.press(KEY_PAGE_UP);
  } else if (b.equals("PAGEDOWN")) {
    Keyboard.press(KEY_PAGE_DOWN);
  } else if (b.equals("HOME")) {
    Keyboard.press(KEY_HOME);
  } else if (b.equals("ESC")) {
    Keyboard.press(KEY_ESC);
  } else if (b.equals("INSERT")) {
    Keyboard.press(KEY_INSERT);
  } else if (b.equals("TAB")) {
    Keyboard.press(KEY_TAB);
  } else if (b.equals("END")) {
    Keyboard.press(KEY_END);
  } else if (b.equals("CAPSLOCK")) {
    Keyboard.press(KEY_CAPS_LOCK);
  } else if (b.equals("F1")) {
    Keyboard.press(KEY_F1);
  } else if (b.equals("F2")) {
    Keyboard.press(KEY_F2);
  } else if (b.equals("F3")) {
    Keyboard.press(KEY_F3);
  } else if (b.equals("F4")) {
    Keyboard.press(KEY_F4);
  } else if (b.equals("F5")) {
    Keyboard.press(KEY_F5);
  } else if (b.equals("F6")) {
    Keyboard.press(KEY_F6);
  } else if (b.equals("F7")) {
    Keyboard.press(KEY_F7);
  } else if (b.equals("F8")) {
    Keyboard.press(KEY_F8);
  } else if (b.equals("F9")) {
    Keyboard.press(KEY_F9);
  } else if (b.equals("F10")) {
    Keyboard.press(KEY_F10);
  } else if (b.equals("F11")) {
    Keyboard.press(KEY_F11);
  } else if (b.equals("F12")) {
    Keyboard.press(KEY_F12);
  } else if (b.equals("SPACE")) {
    Keyboard.press(' ');
  }
}
