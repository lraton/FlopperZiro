void checkSubMenuButton() {
  if (analogRead(buttonUp) == 0) {
    handleSubMenuUpButton();
  }
  if (digitalRead(buttonDown) == LOW) {
    handleSubMenuDownButton();
  }
  if (digitalRead(buttonLeft) == LOW) {
    handleSubMenuLeftButton();
  }
  if (digitalRead(buttonSelect) == LOW) {
    handleSubMenuSelectButton();
  }
  delay(150);
}

void handleSubMenuUpButton() {
  if (currentPageSubMenu > 0) {
    currentPageSubMenu--;
  }
  Serial.println(currentPageSubMenu);
}

void handleSubMenuDownButton() {
  if (currentPageSubMenu < numPagesSubMenu - 1) {
    currentPageSubMenu++;
  }
  Serial.println(currentPageSubMenu);
}

void handleSubMenuLeftButton() {
  currentPage = scelta;
  scelta = 0;
}

void handleSubMenuSelectButton() {
  sceltaSubMenu = currentPageSubMenu;
}
