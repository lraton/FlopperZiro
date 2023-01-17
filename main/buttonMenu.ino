void checkMenuButton(){
  if(digitalRead(buttonUp) == LOW) {
    handleUpButton();
  }
  if(digitalRead(buttonDown) == LOW) {
    handleDownButton();
    }
  if(digitalRead(buttonLeft) == LOW) {
    handleLeftButton();
  }
  if(digitalRead(buttonSelect) == LOW) {
    handleSelectButton();
  }
}

void handleUpButton() {
  if(currentPage > 1) {
    currentPage--;
  }
  Serial.println("next");
}

void handleDownButton() {
  if(currentPage < numPages-1) {
    currentPage++;
  }
  Serial.println("Prev");
}

void handleLeftButton() {
  if (scelta==0){
    currentPage=0;
  }else{
      currentPage=scelta;
      scelta=0;
  }
}

void handleSelectButton() {
  scelta=currentPage;
}
