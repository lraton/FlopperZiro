//all button function when im in the menu

void checkMenuButton() {
  if (analogRead(buttonUp) == 0) {
    handleUpButton();
  }
  if (digitalRead(buttonDown) == LOW) {
    handleDownButton();
  }
  if (digitalRead(buttonLeft) == LOW) {
    handleLeftButton();
  }
  if (digitalRead(buttonSelect) == LOW) {
    handleSelectButton();
  }
  delay(150);
}

void handleUpButton() {
  if (currentPage > 1) {
    currentPage--;
  }
}

void handleDownButton() {
  if (currentPage < numPages - 1) {
    currentPage++;
  }
}

void handleLeftButton() {
  sceltaSubMenu = 0;
  currentPage = scelta;
  scelta = 0;
}

void handleSelectButton() {
  scelta = currentPage;
}

/////////////////////////////////////
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
  sceltaSubMenu = 0;
  currentPage = scelta;
  scelta = 0;
}

void handleSubMenuSelectButton() {
  sceltaSubMenu = currentPageSubMenu;
}
////////////////////////////////////////////////
void checkModuleButton() {
  if (analogRead(buttonUp) == 0) {
    scanning = 1;
  }
  if (digitalRead(buttonDown) == LOW) {
    save();
  }
  if (digitalRead(buttonLeft) == LOW) {
    scanning = 1;
    sceltaSubMenu = 0;
  }
  if (digitalRead(buttonRight) == LOW) {
    emulate();
  }
  if (digitalRead(buttonSelect) == LOW) {

  }
  delay(150);
}
