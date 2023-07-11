//Base file for ThingController
//This includes things such as
//  ) default pinouts of the board
//  ) default includes
//  ) supporting defines
//  ) base functions for RFID and LCD & NEOPIXEL


//Hardware Specific Defines
// Fixed pinout of Thing controller by Joe
// ILI9341 pinout defines
#define LCD_CS 0
#define LCD_DC 2
#define LCD_MOSI 19
#define LCD_CLK 18
#define LCD_RST 1
#define LCD_MISO 16
#define LCD_BACKLIGHT 3
#define BTN0 11
#define BTN1 10
#define BTN2 13
#define BTN3 12
#define NEOPIXEL 9

#if ThingControllerBoardRev == 1
#define I2C_IRQ 14
#define RFID_RESET 15  //not connected
#else
#define I2C_IRQ 14     //Might change in rev 2 of board placeholder if this happens
#define RFID_RESET 15  //not connected
#endif

//default imports
#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_PN532.h"
#include <Wire.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
//WIFI/WIRED specific tools
#ifdef WIFIMODE

#endif
#ifdef WIREDMODE
  #include <Ethernet_Generic.h>
  #include <ArduinoUniqueID.h>
#endif

#define CARD_DEBOUNCE_DELAY 2000  // milliseconds between successful card reads
#define PN532_READ_TIMEOUT 50     // milliseconds

#ifndef NUM_LEDS
#define NUM_LEDS 24  //Number of LEDs in neopixel strip (built in one should be bypassed)
#endif

//NeoPixel Colour references
#define COLOR_RED strip.Color(200, 0, 0)
#define COLOR_GREEN strip.Color(0, 200, 0)
#define COLOR_BLUE strip.Color(0, 0, 200)
#define COLOR_WHITE strip.Color(200, 200, 200)
#define COLOR_PURPLE strip.Color(200, 0, 200)
#define COLOR_YELLOW strip.Color(200, 200, 0)
#define COLOR_PINK strip.Color(200, 20, 130)
#define COLOR_ORANGE strip.Color(200, 80, 8)
#define COLOR_OFF strip.Color(0, 0, 0)
//colour patterns
#define PATTERN_WHITE 0
#define PATTERN_GREEN 1
#define PATTERN_PURPLE 2
#define PATTERN_BLUE 3
#define PATTERN_PINK 4
#define PATTERN_ORANGE 5
#define PATTERN_RAINBOW 17
#define PATTERN_PYB 18
#define PATTERN_FWHITE 24
#define PATTERN_FGREEN 25
#define PATTERN_FPURPLE 26
#define PATTERN_FBLUE 27
#define PATTERN_FPINK 28
#define PATTERN_FORANGE 29
#define PATTERN_RED 32
#define PATTERN_REDBLUE 33
#define PATTERN_REDORANGE 34
#define PATTERN_REDGREEN 35
#define PATTERN_REDPURPLE 36
#define PATTERN_REDPINK 37
#define PATTERN_REDWHITE 38


long actionTimer;


//setup devices
Adafruit_ILI9341 lcd = Adafruit_ILI9341(&SPI, LCD_DC, LCD_CS, LCD_RST);
// neopixel
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NEOPIXEL, NEO_GRB + NEO_KHZ800);
//#define WIRE wire
Adafruit_PN532 nfc(I2C_IRQ, RFID_RESET);

//verbosity msg system
// each msg printed will go through a function called printMsg where it will be checked if it should print based on msg priority
// and if the current level allows for it be shown, this helps see only important messages without showing everything
uint8_t verbosity = 0;  //0 = low, 1 = medium, 2 = high
#define VERB_LOW 0      //show few messages
#define VERB_MED 1      //show a bit more
#define VERB_HIGH 2     //show all messages
#define VERB_ALL 255    //only to be used if you always want the message to appear when calling the printMsg function \
                        // reserved for startup & verbosity settings only
#ifndef LCD_NUM_LINES
#define LCD_NUM_LINES 12  //number of lines of text that can fit on screen
#endif
String lcdMsg[LCD_NUM_LINES];
bool newline = false;
bool isUnlocked = false;
bool deviceOn = false;



// tokens are 4 or 7-byte values, held in a fixed 7-byte array
typedef uint8_t TOKEN[7];

//flags for TOKEN_CACHE_ITEM
#define TOKEN_ACCESS 0x01    //bit0 in flags
#define TOKEN_CACHE 0x02     //bit1 in flags
#define TOKEN_INDUCTOR 0x04  //bit2 in flags
#define TOKEN_DIRECTOR 0x08  //bit3 in flags (not yet used)
#define TOKEN_READ 0x10      //bit4 in flags (card seen but no access)

// struct for items in the token cache
struct TOKEN_CACHE_ITEM {
  TOKEN token;                   // the token uid
  uint8_t length = 0;            // length of token in bytes
  uint8_t flags = 0;             // permission bits
  uint8_t colour = PATTERN_RED;  // colour pattern to display //default red
  uint8_t sync = 0;              // countdown to resync with cache with server
  uint16_t count = 0;            // scan count
};


#define CACHE_SIZE 32   // number of tokens held in cache
#define CACHE_SYNC 240  // resync cache after <value> x 10 minutes

inline int clamp(int v, int minV, int maxV) {
  return min(maxV, max(v, minV));
}





class ThingControllerBase {
public:
  int thingPin = 7;  //default output pin
  int thingOnState = HIGH;
  // the cache
  // number of items in cache
  uint8_t cacheSize = 0;
  TOKEN_CACHE_ITEM cache[CACHE_SIZE];
  TOKEN_CACHE_ITEM serverResult;



  // token as hex string
  char tokenStr[15];                            ///14 + string terminator
  uint8_t colorPattern = 0;                     //current pattern its displaying
  IPAddress ip{ IPAddress(192, 168, 1, 252) };  //default, gets set later
  String Server_Host = "";
  int Server_Port = 0;
  String Server_URLPrefix = "";
  String Thing_ID = "";
  String Thing_Name = "";
  int Unlock_Seconds = 5;  //time staying unlocked
  int wifi_error_count = 0;
  #ifdef WIFIMODE
    //Wifi specific IDs
    String Network_SSID = "";
    String Network_Password = "";
    
  #endif
  byte macAddr[6] = { 0, 0, 0, 0, 0, 0 }; //Wired only? gets set later
  


  void initLcd() {
    lcd.begin();
    lcd.setRotation(3);                 //set screen to right way around
    pinMode(LCD_BACKLIGHT, OUTPUT);     //backlight
    digitalWrite(LCD_BACKLIGHT, HIGH);  //turn backlight on (otherwise you cant see anything)
    //clear screen probably not needed
    lcd.fillScreen(ILI9341_BLACK);
    lcd.setTextColor(ILI9341_WHITE);
    lcd.setTextSize(2);
  }

  void unlockDevice() {
    digitalWrite(thingPin, thingOnState);
    printMsgln("Device unlocked", VERB_HIGH);
  }

  void lockDevice() {
    digitalWrite(thingPin, !thingOnState);
    printMsgln("Device locked", VERB_HIGH);
  }

  int configNetwork(){
     printMsgln("Reading config", VERB_ALL);
    if (!LittleFS.begin()) {
      printMsgln("Error mounting LittleFS", VERB_ALL);
      return 0;
    }
    //Configure Network

    //read wifi configuration from Network.json
    File networkFile = LittleFS.open("/Network.json", "r");
    if (!networkFile) {
      printMsgln("Failed to open network file", VERB_ALL);
      return 0;
    }
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, networkFile);
    if (error) {
      printMsgln("Failed to parse network file", VERB_ALL);
      return 0;
    }
    //convert to json
    JsonObject root = doc.as<JsonObject>();
    networkFile.close();  //close little fs file

    //read in Network settings   
    
    #ifdef WIFIMODE
      //Network SSID (Wifi name)
      if (!root.containsKey("NETWORK_SSID")) {
        printMsgln("Error: No NETWORK_SSID in Network file", VERB_ALL);
        return 0;
      }
      this->Network_SSID = root["NETWORK_SSID"].as<String>();

      //Network password
      if (!root.containsKey("NETWORK_PASSWORD")) {
        printMsgln("Error: No NETWORK_PASSWORD in Network file", VERB_ALL);
        return 0;
      }
      this->Network_Password = root["NETWORK_PASSWORD"].as<String>();
    #endif
    #ifdef WIREDMODE
      printMsgln("Mac Address:", VERB_LOW);
      for (size_t i = 0; i < 6; i++) {
        char hexes[3] = "";
        macAddr[i] = UniqueID[i];
        sprintf(hexes, "%02X:", UniqueID[i]);
        printMsg(String(hexes), VERB_LOW);
      }
    #endif
    
    //settings across wifi and wired
    //IP address that this device should appear as
    if (!root.containsKey("DeviceAddress")) {
      printMsgln("Error: No DeviceAddress in config file", VERB_ALL);
      return 0;
    }
    const char* ipaddress = root["DeviceAddress"];
    this->ip.fromString(ipaddress);
    //IP address of server to connect to
    if (!root.containsKey("SERVER_HOST")) {
      printMsgln("Error: No SERVER_HOST in Network file", VERB_ALL);
      return 0;
    }
    Server_Host = root["SERVER_HOST"].as<String>();

    //port of server
    if (!root.containsKey("SERVER_PORT")) {
      printMsgln("Error: No SERVER_PORT in Network file", VERB_ALL);
      return 0;
    }
    this->Server_Port = root["SERVER_PORT"].as<int>();

    //Prefix of URL to ping server
    if (!root.containsKey("SERVER_URLPREFIX")) {
      printMsgln("Error: No SERVER_URLPREFIX in Network file", VERB_ALL);
      return 0;
    }
    this->Server_URLPrefix = root["SERVER_URLPREFIX"].as<String>();
    
    printMsgln("Device Configured", VERB_ALL);
    printMsg(Thing_Name, VERB_ALL);
    printMsg(" Controller", VERB_ALL);
    return 1;
    
  }

  int configDevice() {
    //read device configuration from Config.json
    //start up little fs to get the config file
    if (!LittleFS.begin()) {
      printMsgln("Error mounting LittleFS", VERB_ALL);
      return 0;
    }
    // Open the config file
    File configFile = LittleFS.open("/Config.json", "r");
    if (!configFile) {
      printMsgln("Failed to open config file", VERB_ALL);
      return 0;
    }
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      printMsgln("Failed to parse config file", VERB_ALL);
      return 0;
    }
    JsonObject root = doc.as<JsonObject>();
    configFile.close();
    //UUID used for accessing the server with correct ID
    //get UUID from json file
    if (!root.containsKey("UUID")) {
      printMsgln("Error: No UUID in config file", VERB_ALL);
      return 0;
    }
    this->Thing_ID = root["UUID"].as<String>();
    //get thing name from json file
    if (!root.containsKey("Thing_Name")) {
      printMsgln("Error: No Thing_Name in config file", VERB_ALL);
      return 0;
    }
    this->Thing_Name = root["Thing_Name"].as<String>();

    //get unlock time from json file
    if (!root.containsKey("UnlockSeconds")) {
      printMsgln("Error: No UnlockSeconds in config file", VERB_ALL);
      return 0;
    }
    this->Unlock_Seconds = root["UnlockSeconds"].as<int>();

    //get output pin from json file
    if (!root.containsKey("ThingPin")) {
      printMsgln("Error: No ThingPin in config file", VERB_ALL);
      return 0;
    }
    this->thingPin = root["ThingPin"].as<int>();

    //get output pin from json file
    if (!root.containsKey("ThingOnState")) {
      printMsgln("Error: No ThingOnState in config file", VERB_ALL);
      return 0;
    }
    this->thingOnState = root["ThingOnState"].as<int>();

    pinMode(thingPin, OUTPUT);
    lockDevice();
    configNetwork();
    return 1;  //return 1 if passed, 0 if failed
  }



  //function to display msg on both Serial (usb debug), and ILI9341 LCD display
  //print msg on new line
  void printMsgln(String newMsg = "", uint8_t verb = 2) {
    newline = true;
    //Always print on serial as that is quick todo
    Serial.println(newMsg);

    //print as scrolling text onto the LCD
    if ((verb == VERB_ALL) || verb <= verbosity) {
      //clear screen
      // lcd.fillScreen(ILI9341_BLACK);
      lcd.fillRect(0, 0, 320, 15 * LCD_NUM_LINES + 4, ILI9341_BLACK);
      //scroll all the text up one line
      for (int i = 1; i < LCD_NUM_LINES; i++) {
        lcdMsg[i - 1] = lcdMsg[i];
      }
      lcdMsg[LCD_NUM_LINES - 1] = newMsg;
    }
    //print the msg out
    //lcd.fillScreen(ILI9341_BLACK);
    for (int i = 0; i < LCD_NUM_LINES; i++) {
      lcd.setCursor(0, i * 15);
      lcd.println(lcdMsg[i]);
    }
  }

  //print msg on same line
  void printMsg(String newMsg = "", uint8_t verb = 2) {
    //Always print on serial as that is quick todo
    Serial.print(newMsg);

    if (newline) {  //previous msg ended with a new line so shift all txt down and then declare not a new line anymore
      printMsgln(newMsg, verb);
      newline = false;
      return;
    }

    if ((verb == VERB_ALL) || verb <= verbosity) {
      //clear screen
      // lcd.fillScreen(ILI9341_BLACK);
      lcd.fillRect(0, 0, 320, 15 * LCD_NUM_LINES + 4, ILI9341_BLACK);
      lcdMsg[LCD_NUM_LINES - 1] += newMsg;
      //print the msg out
      //lcd.fillScreen(ILI9341_BLACK);
      for (int i = 0; i < LCD_NUM_LINES; i++) {
        lcd.setCursor(0, i * 15);
        lcd.println(lcdMsg[i]);
      }
    }
  }

  //print long message over several lines
  void printMsgLarge(String newMsg = "", uint8_t verb = 2) {
    int startIndex = 0;
    int endIndex = 25;
    while (endIndex <= newMsg.length()) {
      rp2040.wdt_reset();  //reset watchdog, lots of text will take time
      String chunk = newMsg.substring(startIndex, endIndex);
      printMsgln(chunk);
      startIndex = endIndex;
      endIndex += 25;
      delay(100);  // Delay for readability
    }

    // Print any remaining characters in the string
    if (startIndex < newMsg.length()) {
      String chunk = newMsg.substring(startIndex);
      printMsgln(chunk);
    }
    //printMsgln(String(newMsg.length()), VERB_ALL);
  }

  void initCache() {

#ifdef ENABLE_BUILTIN_BADGES
    printMsg(F("READING IN BUILTINBADGES"), VERB_HIGH);
    TOKEN* token;
    for (int i = 0; i < BUILTIN_BADGE_COUNT; i++) {
      token = (TOKEN*)&builtinBadges[i];
      addTokenToCache(token, 4, 3, PATTERN_GREEN);
    }
#endif
  }

  void clearLcd() {
    lcd.setTextColor(ILI9341_WHITE);
    for (int i = 0; i < LCD_NUM_LINES; i++)
      lcdMsg[i] = "";
    //wipe screen
    lcd.fillScreen(ILI9341_BLACK);
  }

  // Get cache item by token, returns null if not in cache
  TOKEN_CACHE_ITEM* getTokenFromCache(TOKEN* token, uint8_t length) {
    // search through items to find a match to token
    TOKEN_CACHE_ITEM* item = NULL;
    uint8_t i;
    for (i = 0; i < cacheSize; i++) {
      if ((length == cache[i].length) && (memcmp(token, &cache[i], length) == 0)) {
        item = &cache[i];
        break;
      }
    }
    return item;
  }

  void addTokenToCache(TOKEN* token, uint8_t length, uint8_t flags, uint8_t colour) {
    // if cache not full, then add a new item to end of array
    uint8_t pos = cacheSize;
    uint8_t i;
    // check for existing
    TOKEN_CACHE_ITEM* t = getTokenFromCache(token, length);
    if (t != NULL) {
      // update flags
      t->flags = flags;
      return;
    }

    // if cache is full, find item with least scans to replace
    if (cacheSize == CACHE_SIZE) {
      cacheSize = 0;  // start at the beginning
      for (i = 0; i < cacheSize; i++) {
        if (cache[i].count < cache[pos].count) {
          pos = i;
        }
      }
    } else {  //if cache is not full just go to next location which is empty
      cacheSize++;
    }

    memcpy(&cache[pos].token, token, length);
    cache[pos].length = length;
    cache[pos].flags = flags;
    cache[pos].count = 1;
    cache[pos].sync = CACHE_SYNC;
    cache[pos].colour = colour;

    printMsgln("TOKEN ADDED TO CACHE", VERB_HIGH);
    printMsgln("CACHE SIZE: ", VERB_HIGH);
    printMsg(String(int(cacheSize)), VERB_HIGH);

    return;
  }

  // pass item to remove
  void removeTokenFromCache(TOKEN_CACHE_ITEM* item) {
    item->length = 0;
    item->flags = 0;
    item->count = 0;
    item->colour = PATTERN_RED;
    item->sync = CACHE_SYNC;
  }

  void updateTokenStr(const uint8_t* data, const uint32_t numBytes) {
    const char* hex = "0123456789abcdef";
    uint8_t b = 0;
    for (uint8_t i = 0; i < numBytes; i++) {
      tokenStr[b] = hex[(data[i] >> 4) & 0xF];
      b++;
      tokenStr[b] = hex[(data[i]) & 0xF];
      b++;
    }
    // null remaining bytes in string
    for (uint8_t i = numBytes; i < 7; i++) {
      tokenStr[b] = 0;
      b++;
      tokenStr[b] = 0;
      b++;
    }
    tokenStr[14] = '\0';  //string terminator
  }


  TOKEN_CACHE_ITEM* lookForCard() {
    uint8_t success;
    TOKEN uid;          // Buffer to store the returned UID
    uint8_t uidLength;  // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    static TOKEN luid;  // last scanned uid, for debounce
    static unsigned long lastChecked;

    TOKEN_CACHE_ITEM* item = NULL;

    //printMsg("Looking for RFID", VERB_ALL);
    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, PN532_READ_TIMEOUT);

    //if card was found
    if (success && (millis() > lastChecked + CARD_DEBOUNCE_DELAY)) {
      String partialRFID = "";
      printMsgln("Found RFID TAG", VERB_ALL);
      memcpy(luid, uid, uidLength);

      lastChecked = millis();
      updateTokenStr(uid, uidLength);
      partialRFID += tokenStr;  //[0] + tokenStr[1] + tokenStr[2] + tokenStr[3]+ tokenStr[4] + tokenStr[5] + tokenStr[6] + tokenStr[7] + tokenStr[8] + tokenStr[9] + tokenStr[10] + tokenStr[11];
      //partialRFID+="XXXXXXXXXX";
      printMsgln(partialRFID, VERB_MED);

      //got all the card information
      // check against cache first
      item = getTokenFromCache(&uid, uidLength);
      if (item == NULL) {
        printMsgln("Token not in cache", VERB_MED);
        // bright orange - found card, querying server
        colorWipe(PATTERN_ORANGE);
        //TODO QUERY SERVER
        item = queryServer(uid, uidLength);
        if (item->flags & TOKEN_CACHE) {
          addTokenToCache(&uid, uidLength, item->flags, item->colour);
        }
      }
      //build up properties that should be obtained from JSON
    }
    return item;
  }
  void spinner(uint8_t pattern, uint8_t from, uint8_t reduce) {
    // stating at LED from, fades from c to black, by reduce each LED

    uint32_t c1 = 0;
    int r;
    for (uint8_t i = 0; i < strip.numPixels(); i++) {
      uint32_t c = flag2color(pattern, i);
      r = (23 - i);
      c1 = strip.Color(
        clamp(((c >> 16) & 0xff) * r / 23, 0, 255),
        clamp(((c >> 8) & 0xff) * r / 23, 0, 255),
        clamp((c & 0xff) * r / 23, 0, 255));
      int pix = from - i;
      if (pix < 0) pix += 24;
      strip.setPixelColor(pix % strip.numPixels(), c1);
    }
    strip.show();
  }

  void animation() {
    static int pos = 0;

    // animation changes based on lock and doorbell states

    if (isUnlocked) {
      // countdown animation

      //if (millis() < actionTimer) {
      pos = 23 * (actionTimer - millis()) / 4000;  //5 second animation loop
                                                   //} else {
                                                   //  pos = 0;
                                                   //}
      switch (colorPattern) {
        case 0 ... 7:
          spinner(colorPattern, pos, 5);
          break;
        case PATTERN_RAINBOW:
        case PATTERN_PYB:                 //pyb or rainbow
          spinner(colorPattern, pos, 0);  //spinner works with no fade reduction
          break;
          //  case 24 ... 31:
          //    flash(pos);
          //    break;
        case 32 ... 40:  //half colours
          spinner(colorPattern, pos, 0);
          break;
        default:  //shouldnt happen
          spinner(colorPattern, pos, 5);
          break;
      }
      //segment(colorPattern, 0, 4, pos + 4);

      //reset pos to top postion, so that spinner always resumes from right position
      pos = 4;

    } else {

      // normal spinner animation
      pos++;
      if (pos > 23) pos = 0;


      //colorPattern = PATTERN_ORANGE;
      if (deviceOn) {
        colorPattern = PATTERN_RED;  // red if doorOpen
      }
      //do appropriate animation
      spinner(colorPattern, pos, 5);
    }
  }

  void colorWipe(uint8_t pattern) {
    colorPattern = pattern;
    Serial.print(F("Setting Pattern to "));
    Serial.println(colorPattern);
    for (uint16_t i = 0; i < strip.numPixels(); i++) {

      strip.setPixelColor(i, flag2color(pattern, i));
    }
    strip.show();
  }

  uint32_t flag2color(uint8_t pattern, int pos) {
    //patterns from the 6 bit code
    uint8_t val = pos % 24;
    switch (pattern) {
      case PATTERN_WHITE:  //solid white
        return (COLOR_WHITE);
      case PATTERN_GREEN:  //solid green
        return (COLOR_GREEN);
      case PATTERN_PURPLE:  //solid purple
        return (COLOR_PURPLE);
      case PATTERN_BLUE:  //solid blue
        return (COLOR_BLUE);
      case PATTERN_PINK:  //solid pink
        return (COLOR_PINK);
      case PATTERN_ORANGE:  //solid orange
        return (COLOR_ORANGE);
      case PATTERN_RAINBOW:  //rainbow spiral
        switch (val >> 2) {
          case 1: return (COLOR_RED);
          case 2: return (COLOR_ORANGE);
          case 3: return (COLOR_YELLOW);
          case 4: return (COLOR_GREEN);
          case 5: return (COLOR_BLUE);
          default: return (COLOR_PURPLE);
        }
      case PATTERN_PYB:  //PYB spiral
        switch (val / 8) {
          case 0: return (COLOR_PINK);
          case 1: return (COLOR_ORANGE);
          default: return (COLOR_BLUE);
        }
      case PATTERN_FWHITE:  //flash white
        return (COLOR_WHITE);
      case PATTERN_FGREEN:  //flash green
        return (COLOR_GREEN);
      case PATTERN_FPURPLE:  //flash purple
        return (COLOR_PURPLE);
      case PATTERN_FBLUE:  //flash blue
        return (COLOR_BLUE);
      case PATTERN_FPINK:  //flash pink
        return (COLOR_PINK);
      case PATTERN_FORANGE:  //flash orange
        return (COLOR_ORANGE);
      case PATTERN_RED:  //error solid red
        return (COLOR_RED);
      case PATTERN_REDBLUE:  //error half red/blue
        if (pos < NUM_LEDS / 2)
          return (COLOR_RED);
        else
          return (COLOR_BLUE);
      case PATTERN_REDORANGE:  //error half red/yellow
        if (pos < NUM_LEDS / 2)
          return (COLOR_RED);
        else
          return (COLOR_ORANGE);
      case PATTERN_REDGREEN:  //error half reg/green
        if (pos < NUM_LEDS / 2)
          return (COLOR_RED);
        else
          return (COLOR_GREEN);
      case PATTERN_REDPURPLE:  //error half reg/purple
        if (pos < NUM_LEDS / 2)
          return (COLOR_RED);
        else
          return (COLOR_PURPLE);
      case PATTERN_REDPINK:  //error half red/pink
        if (pos < NUM_LEDS / 2)
          return (COLOR_RED);
        else
          return (COLOR_PINK);
	  case PATTERN_REDWHITE:
		if (pos < NUM_LEDS / 2)
          return (COLOR_RED);
        else
          return (COLOR_WHITE);
	  
      default:  //invalid color code
        if (pos < NUM_LEDS / 2)
          return (COLOR_RED);
        else
          return (COLOR_WHITE);
    }
  }

  void showLogo() {
    File image = LittleFS.open("/MS5050.bmp", "r");
    if (!image) {
      printMsgln("Failed to open image file", VERB_ALL);
      return;
    }

    uint32_t fileSize = image.size();
    uint32_t headerSize = 54;  // Bitmap header size
    uint32_t imageSize = fileSize - headerSize;
    for (int i = 0; i < 18; i++) {  //skip non important data
      image.read();
    }
    uint32_t bmpWidth = image.read();  // + (image.read() << 8) + (image.read() << 16) + (image.read() << 24);
    image.read();
    image.read();
    image.read();
    uint32_t bmpHeight = image.read();  // + (image.read() << 8) + (image.read() << 16) + (image.read() << 24);
    image.read();
    image.read();
    image.read();
    // printMsg("Image size: ", VERB_ALL);
    // printMsg(String(fileSize), VERB_ALL);
    // printMsgln(" bytes", VERB_ALL);
    // printMsgln("Image dimensions: ", VERB_ALL);
    // printMsgln(String(bmpWidth), VERB_ALL);
    // printMsgln(" x ", VERB_ALL);
    // printMsgln(String(bmpHeight), VERB_ALL);

    // Read past unused bytes in BMP header
    for (int i = 0; i < 28; i++) {
      image.read();
    }

    // Set starting position for display to btm right
    uint16_t xPos = (lcd.width() - bmpWidth);
    uint16_t yPos = (lcd.height() - bmpHeight);

    // Read bitmap data
    for (int y = bmpHeight - 1; y >= 0; y--) {
      for (int x = 0; x < bmpWidth; x++) {
        uint16_t blue = image.read();
        uint16_t green = image.read();
        uint16_t red = image.read();
        uint16_t color = lcd.color565(red, green, blue);
        lcd.drawPixel(xPos + x, yPos + y, color);
      }
      image.read();
      image.read();
    }
    delay(1000);
    image.close();
  }

  void changeVerbosity() {  //on button press cycle through verbosity levels
    String verbmsg = "Verbosity set to ";
    if (verbosity == 2) {  //verbosity is at highest cycle to low (0)
      verbosity = 0;
    } else {
      verbosity++;
    }
    switch (verbosity) {
      case 0: verbmsg += "LOW"; break;
      case 1: verbmsg += "MEDIUM"; break;
      case 2: verbmsg += "HIGH"; break;
    }

    printMsgln(verbmsg, VERB_ALL);  //print new verbosity level
  }
  
  // send a log msg to the server, fire and forget
  void sendLogMsg(String msg) {//msg to server
    msg.replace(" ", "%20"); //msg cannot have spaces in it or any other character that might mess up URL
      
    #ifdef WIFIMODE
      if (WiFi.status() != WL_CONNECTED) {
        printMsgln("Error: WiFi Not Connected", VERB_LOW);
        return;
      }    
      WiFiClient client;
    #endif
    #ifdef WIREDMODE
      if (!(Ethernet.linkStatus() == LinkON)) {
        printMsgln("ETHERNET NOT CONNECTED", VERB_ALL);
        return;
      }    
      EthernetClient client;
    #endif
    
    printMsgln("Messaging server", VERB_HIGH);
    if (!client.connect(Server_Host.c_str(), Server_Port)) {
      printMsgln("Error: Connection failed", VERB_LOW);
      return;
    }    
    String url = Server_URLPrefix;
    url += "msglog?thing=";
    url += Thing_ID.c_str();
    url += "&msg=";
    url += msg;

    //printMsgLarge(url, VERB_LOW);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + Server_Host.c_str() + "\r\n" + "Connection: close\r\n\r\n");

    // don't care if it succeeds, so close connection and return
    client.flush();
    client.stop();
    return;        
    
    
  }
  
  
  void setupNetwork() {
    #ifdef WIFIMODE
      printMsgln("Connecting to Wifi", VERB_ALL);
      WiFi.disconnect(true);
      WiFi.mode(WIFI_STA);
      WiFi.begin(Network_SSID.c_str(), Network_Password.c_str());
      WiFi.config(ip);
      // Print periods on monitor while establishing connection
      rp2040.wdt_reset();
      while (WiFi.status() != WL_CONNECTED) {
        printMsg(".", VERB_ALL);
        delay(1000);
      }

      // Connection established
      printMsgln("Wifi Connected", VERB_ALL);
      printMsgln(WiFi.SSID(), VERB_ALL);    
    #endif
    #ifdef WIREDMODE
	  printMsgln("Enabling Ethernet", VERB_ALL);
      Ethernet.init(17);
      Ethernet.begin(macAddr, ip);    
    #endif
  }
  
  TOKEN_CACHE_ITEM* queryServer(const uint8_t* uid, uint8_t uidLength) {
    //clear results from pervious query
    serverResult.flags = 0;
    serverResult.colour = PATTERN_RED;
    serverResult.count = 0;
	
	memset(serverResult.token, 0, sizeof(serverResult.token)); //clear all of token
    memcpy(serverResult.token, uid, uidLength);
    serverResult.length = uidLength;
    serverResult.sync = 0;
	
	
	#ifdef WIFIMODE
	if ( WiFi.status() != WL_CONNECTED ) {
      char countMsg[20] = "";
      printMsgln("Error: WiFi Not Connected", VERB_ALL);
      serverResult.colour = PATTERN_WHITE;
      wifi_error_count++;
      sprintf(countMsg, "Wifi error Count: %d",wifi_error_count );
      printMsgln(String(countMsg), VERB_MED);
      setupNetwork();
      if (wifi_error_count > 4){
        while (1){ //if failed to connect 3 times use watchdog to reset
          delay(100);
        }
      }
      return &serverResult;
    } 
	// Use WiFiClient class to create TCP connections
	WiFiClient client;
	#endif

	#ifdef WIREDMODE
    printMsgln("Check Ethernet", VERB_HIGH);
    if (!(Ethernet.linkStatus() == LinkON)) {
      printMsgln("ETHERNET NOT CONNECTED", VERB_ALL);
      serverResult.colour = PATTERN_REDWHITE;
      return &serverResult;
    }
	EthernetClient client;
	printMsgln("Check server", VERB_LOW);
    if (!client.connect(Server_Host.c_str(), Server_Port)) {
      printMsgln("Error: Connection failed", VERB_ALL);
      serverResult.colour = PATTERN_REDWHITE;
      client.flush();
      client.stop();
      return &serverResult;
    }
	#endif

    #ifdef WIREDMODE
    if (!client.connect(Server_Host.c_str(), Server_Port)) {
    #endif	
    #ifdef WIFIMODE	
    if (!client.connect(Server_Host, Server_Port)) {
    #endif
      char countMsg[20] = "";
      printMsgln("Error: Connection failed", VERB_ALL);
      serverResult.colour = PATTERN_WHITE;
      client.flush();
      client.stop();  
      wifi_error_count++;
      sprintf(countMsg, "Wifi error Count: %d",wifi_error_count );
      printMsgln(String(countMsg), VERB_MED);
      setupNetwork();
      if (wifi_error_count > 4){
        while (1){ //if failed to connect 3 times use watchdog to reset
          delay(100);
        }
      }
    return &serverResult;
    }
    wifi_error_count = 0;

	//rest of code is independant of wifi and wired mode
    printMsgln("Sending Msg to server", VERB_HIGH);
    // We now create a URI for the request
    String url = Server_URLPrefix;
    url += "verify";
    url += "?token=";
    url += tokenStr;
    url += "&thing=";
    url += Thing_ID.c_str();    
    //printMsgln(url, VERB_ALL);

    int server_response_time =0;
    server_response_time = millis();
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                  "Host: " + Server_Host.c_str() + "\r\n" +
                  "Connection: close\r\n\r\n");  
    int checkCounter = 0;

    while (!client.available()) {
      delay(10);
      checkCounter++;
      if (checkCounter > 3000){
        printMsgln ("Server Did not Respond", VERB_LOW); 
        serverResult.colour = PATTERN_REDWHITE;
        return &serverResult;
      }
    }   
    //printMsgln ("Server Responded", VERB_MED); 
    printMsg ("Response time ms: ", VERB_HIGH); 
    printMsgln(String(millis() - server_response_time), VERB_HIGH);  
    String json;
    int loop_counter = 0;
    bool endOfHeaders = false;
    while(client.available() && (loop_counter < 512) && !endOfHeaders){
      loop_counter++;    
      json = client.readStringUntil('\n');
      if (json == "") endOfHeaders = true;    
      if (json.length() >=500) break; //way too much text
        //printMsgLarge(json, VERB_HIGH);
    
    }
    //clean up and close connection 
    client.flush();
    client.stop();  

    //printMsgLarge(json, VERB_HIGH);

    //convert to json
    DynamicJsonDocument jsonBuffer(200);
    deserializeJson(jsonBuffer,json);
    JsonObject root = jsonBuffer.as<JsonObject>();
    if (root.isNull()) {
      printMsgln("Error: Couldn't parse JSON", VERB_LOW);
	  serverResult.colour = PATTERN_REDWHITE;
      return &serverResult;
    }
    if (!root.containsKey("access")) {
      printMsgln("Error: No access info");
      //TODO add error color
      return &serverResult;
    }else{
      if (root["access"] == 1) { //if access allowed
        serverResult.flags |= TOKEN_ACCESS;
      } 
    }
    if (root.containsKey("cache")){
      if (root["cache"] == 1) { //dont  cache if weekend member
        serverResult.flags |= TOKEN_CACHE; 
        serverResult.sync = CACHE_SYNC;//setup how long staying in cache before checking again  
      }
    }
    if (root.containsKey("inductor")){
      if (root["inductor"] == 1) { //check if an inductor
        serverResult.flags |= TOKEN_INDUCTOR; //or assign
      }
    } 
    if (root.containsKey("director")){
      if (root["director"] == 1) { //TBD director do anything special?
        serverResult.flags |= TOKEN_DIRECTOR;
      }
    } 
    if (root.containsKey("colour")){
      serverResult.colour  = root["colour"].as<int>(); //value between 0-63
    } 
    if (root.containsKey("error")){//print out error messages
      printMsgLarge(root["error"], VERB_MED);
    }   
  
    return &serverResult;
  }  
  
};