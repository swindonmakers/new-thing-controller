void updateTFTCB() {
  canvas.fillScreen(ILI9341_BLACK);
  canvas.setCursor(0, 0);
  canvas.setTextColor(ILI9341_WHITE);
  canvas.setTextSize(1);
  canvas.println("Hello World!");
  canvas.setTextColor(ILI9341_YELLOW);
  canvas.setTextSize(2);
  canvas.println(dispVal);
  canvas.setTextColor(ILI9341_RED);
  canvas.setTextSize(3);
  canvas.println(0xDEADBEEF, HEX);
  canvas.println();
  canvas.setTextColor(ILI9341_GREEN);
  canvas.setTextSize(5);
  canvas.println("Groop");
  canvas.setTextSize(2);
  canvas.println("I implore thee,");
  canvas.setTextSize(1);
  canvas.println("my foonting turlingdromes.");
  canvas.println("And hooptiously drangle me");
  canvas.println("with crinkly bindlewurdles,");
  canvas.println("Or I will rend thee");
  canvas.println("in the gobberwarts");
  canvas.println("with my blurglecruncheon,");
  canvas.println("see if I don't!");
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());   //write framebuffer to device
  dispVal--;
}