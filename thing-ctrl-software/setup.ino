void setupTFT() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);  //turn backlight on
  tft.begin();                 //begin at 48MHzz
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
}

void setupTasks() {
  ts.addTask(updateTFT);
  updateTFT.enable();
}