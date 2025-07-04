// Base code for the thing controller, contains useful code required by all types of thing controllers in the makerspace from door to tool, etc

// TODO TEXTLCD support

// required libraries
#include "SPI.h"              //hardware SPI for display
#include <Adafruit_GFX.h>     //draw things on the display
#include <Adafruit_ILI9341.h> //display
#include <LittleFS.h>         //File system
#include "Adafruit_PN532.h"   //RFID reader USE VERSION 1.2.2!!!!!!!
#include <FastLED.h>          //addressable RGB led control
#include <ArduinoJson.h>      //json parser
#include <LiquidCrystal_I2C.h>//4x20 lcd display
#include <ArduinoOTA.h>       //Over the air updating
#ifdef WIFIMODE               // wifi libraries
#include <WiFi.h>
#endif
#ifdef WIREDMODE // wired libraries
#include <Ethernet_Generic.h>
#include <ArduinoUniqueID.h>
#endif

#ifndef SCREEN_ROTATION
#define SCREEN_ROTATION 3
#endif

#define STRINGIFY(x) #x
//text lcd display dimensions
#define LCD_COLS 20
#define LCD_ROWS 4
const uint8_t i2cAddr = 0x27;
LiquidCrystal_I2C Textlcd = LiquidCrystal_I2C(i2cAddr, 20, 4); // Change to (0x27,20,4) for 20x4 LCD.

// ILI9341 pinout defines
#define TFTLCD_CS 0
#define TFTLCD_DC 2
#define TFTLCD_MOSI 19
#define TFTLCD_CLK 18
#define TFTLCD_RST 1
#define TFTLCD_MISO 16
#define TFTLCD_BACKLIGHT 3
#define I2C_IRQ 14    // Might change in rev 2 of board placeholder if this happens
#define RFID_RESET 27 // not connected but needed in code
#define ETHERNET_RESET 20

// NFC reader control properties
#define CARD_DEBOUNCE_DELAY 2000 // milliseconds between successful card reads
#define PN532_READ_TIMEOUT 50    // milliseconds

// rgb addressable led
#define BRIGHTNESS 160
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define LED_PINOUT 9
int NUM_LEDS = 0; // number of RGB leds, if 0 then disable
CRGB *leds;       // led colour array

// colour patterns - matching value arriving in JSON from server for what colur to display
#define PATTERN_WHITE 0
#define PATTERN_GREEN 1
#define PATTERN_PURPLE 2
#define PATTERN_BLUE 3
#define PATTERN_PINK 4
#define PATTERN_ORANGE 5
#define PATTERN_RAINBOW 17
#define PATTERN_PYB 18
#define PATTERN_RED 32     // 32 and higher are error colours
#define PATTERN_REDBLUE 33 // half red half blue
#define PATTERN_REDORANGE 34
#define PATTERN_REDGREEN 35
#define PATTERN_REDPURPLE 36
#define PATTERN_REDPINK 37
#define PATTERN_REDWHITE 38
int colourPattern = 0; // led state

//flags for swindonmakerspace account information
#define TOKEN_ACCESS 0x01    //bit0 in flags
#define TOKEN_CACHE 0x02     //bit1 in flags
#define TOKEN_INDUCTOR 0x04  //bit2 in flags
#define TOKEN_DIRECTOR 0x08  //bit3 in flags (not yet used)
#define TOKEN_READ 0x10      //bit4 in flags (card seen but no access)

// setup devices
Adafruit_ILI9341 TFT_lcd = Adafruit_ILI9341(&SPI, TFTLCD_DC, TFTLCD_CS, TFTLCD_RST); // Colour LCD
Adafruit_PN532 nfc(I2C_IRQ, RFID_RESET);                                             // rfid tag reader

bool TFT_lcd_enabled = false;  // if this thing has a colour lcd
bool serial_enabled = false;   // if this thing should output information on the serial port
bool text_lcd_enabled = false; // if this thing has a 4x20 LCD text display

// output pin signal of the thing that is being contolled
int thingPin = 7;              // output pin of the thing being controller (default 7)
PinStatus thingOnState = HIGH; // default output value to enable the machine

// symbols to show on the 4x20 LCD
byte LOCKED_SYMBOL[] = {0b01110, 0b10001, 0b10001, 0b11111, 0b11011, 0b11011, 0b11111, 0b00000};
byte UNLOCKED_SYMBOL[] = {0b01110, 0b10000, 0b10000, 0b11111, 0b11011, 0b11011, 0b11111, 0b00000};

// Msg information. Needed to show text reliably on ILI9341
#define LCD_NUM_LINES 10
String titleMsg;
String headlineMsg;
String bodyMsg[LCD_NUM_LINES];
uint16_t titleColour = ILI9341_WHITE;
uint16_t headlineColour = ILI9341_WHITE;
uint16_t bodyColour = ILI9341_WHITE;

// network settings (wired or wireless)
//  configured later from json on littleFS
IPAddress ip{IPAddress(192, 168, 1, 252)}; // default, gets set later
String Server_Host = "";
int Server_Port = 0;
String Server_URLPrefix = "";
int wifi_error_count = 0; // if gets to 3 then reset device as its probably a wifi issue
#ifdef WIFIMODE
                          // Wifi specific IDs
String Network_SSID = "";
String Network_Password = "";
#endif
byte macAddr[6] = {0, 0, 0, 0, 0, 0}; // Wired only? gets set later

// device settings
String Thing_ID = "";
String Thing_Name = "";
String AdminPW = "";
int Unlock_Seconds = 5; // time staying unlocked
int old_ota_prog = 1; //ATO update progress tracker

typedef uint8_t RFID_token[7]; // value in an rfid tag

// RFID tag information
struct RFID_TAG
{
    char uid[14];
    uint8_t uid_length = 0;
    uint8_t valid = 0;
};

// Swindon Makerspace Account information
struct SM_ACCOUNT
{
    char Name[20] = "NULL";     // name
    uint8_t flags = 0;          // action flags
    uint8_t colour = 0;         // access colour pattern
    uint8_t store_counter = 24; // decrement every hour and once 0 remove from cache
    RFID_TAG tag;
};
#define STORAGE_TIME 24 //number of hours to store an account

#define STOREDACCOUNTSNUM 64
SM_ACCOUNT stored_accounts[STOREDACCOUNTSNUM];

#define  VERB_NORMAL        0  //Standard amound of messages appear
#define  VERB_HIGH          1  //Display alot more info on the screen for debug
#define  VERB_HIGH_SERVER   2  //Display alot more info on the screen for debug and send more logs to server
#define  VERB_MAX           2  //Highest value verbosity listed

int Verbosity = VERB_NORMAL;

// function declarations - not all are listed here
void printTitle(String);                  // prints message at top of colour LCD in big font (1 line) (8 char max)
void printTitle_update(String, bool);     // prints message just below top of colour lcd in medium font (1 line) (12 char max) - bool for updating screen (off when multiple msgs to reduce lag)
void printHeadline(String);               // prints message just below top of colour lcd in medium font (1 line) (12 char max)
void printHeadline_update(String, bool); // prints message just below top of colour lcd in medium font (1 line) (12 char max) - bool for updating screen (off when multiple msgs to reduce lag)
void printBody(String);                   // prints message in smallest text in a scrolling text format (10 lines) (20 char max)
void printBody_update(String, bool);     // prints message in smallest text in a scrolling text format (10 lines) (20 char max) - bool for updating screen (off when multiple msgs to reduce lag
void printBodyLong(String);
void toggle_TFTlcd();               // turns colour LCD on/off
void unlockDevice(String msg = "" ); // Unlocks the device with the message being sent to printTitle
void lockDevice(String msg = "", uint16_t color = ILI9341_RED);   // locks the device with the message being sent to printTitle
void initialise_TFT_lcd(bool);      // Initialise the colour lcd
void updateTFTlcd();                // draws information on the color lcd
int configNetwork();       // confiugures network settings based of network.json in littleFS
int configDevice();        // confiugures device settings based of config.json in littleFS
void setupNetwork();       // initialises network conection WIFI or wired based on #define in main code
void setColorPattern(int); // set colour pattern on rgb leds
void initialise_leds();    // setup for the rgb addressable leds
void animate_leds();
bool is_override_tag(RFID_TAG tag); //override code from separate file
void getRFID_tag(RFID_TAG *item);
void updateTextLcd();
void setupOTA();

#ifndef OVERRIDE_TOKENS
bool is_override_tag(RFID_TAG tag){return 0;};
#endif
// setup called by the main code
void thing_setup(bool TFT_lcd_init = true, bool TFT_lcd_backlight_on = true, bool textlcd_init = true,bool serial_init = false, int thing_pin = 7, PinStatus thing_on_state = HIGH, bool rfid_init = true, bool network_config = true, bool thing_config = true)
{
    thingPin = thing_pin;
    thingOnState = thing_on_state;
    lockDevice();
    if (TFT_lcd_init)
    {
        initialise_TFT_lcd(TFT_lcd_backlight_on);
        TFT_lcd_enabled = true;
    }
    if (serial_init)
    {
        Serial.begin(9600);
        serial_enabled = true;
    }
    if (textlcd_init){
        Textlcd.init();
        Textlcd.backlight();
        text_lcd_enabled = true;
    }
    // boot LittleFS
    if (!LittleFS.begin())
    {
        printBody("Failed to start LittleFS");
        while (1)
        {
            delay(1000);
        } // failed to start correctly so dont finish booting
    }

    if (rfid_init)
    {
        // connecting to PN532 rfid reader
        printBody("Setting up PN532");
        nfc.begin();
        nfc.setPassiveActivationRetries(0xFF);
        nfc.SAMConfig();
        uint32_t versiondata = nfc.getFirmwareVersion();
        printBody("Firmware ver: " + String((versiondata >> 16) & 0xFF) + "." + String((versiondata >> 8) & 0xFF));
        if (versiondata == 0)
        { // if failed to talk to NFC
            printBody("FAILED TO REACH NFC");
            printTitle("ERROR!");
            while (true)
                delay(1000); // stay here
        }
        // printBody();
        // printBody(".");
        // printBody();
        printBody("PN532 Set up");
    }

    // configure netork from littleFS - if disabled the setup function must do this manually
    if (network_config)
    {
        if (configNetwork())
        {
            printTitle("ERROR!");
            printBody("Network Config Error");
            while (true)
                delay(1000); // stay here
        }
    }
    // configure thing from littleFS - if disabled the setup function must do this manually
    if (thing_config)
    {
        if (configDevice())
        {
            printTitle("ERROR!");
            printBody("Device Config Error");
            while (true)
                delay(1000); // stay here
        }
    }
    if (NUM_LEDS) // if any amount of LEDs are selected then setup leds
    {
        initialise_leds();
        colourPattern = PATTERN_ORANGE;
        animate_leds();
    }

    // run again now that display info is setup so it can show status on display
    lockDevice();
    printBody(Thing_Name);
    setupNetwork();
    //setupOTA();
}

void setupOTA(){
    String h_name = "ThingController-" + Thing_Name + rp2040.getChipID();//String(id_str, PICO_UNIQUE_BOARD_ID_SIZE_BYTES*2+1); 
    ArduinoOTA.setHostname(h_name.c_str());
    ArduinoOTA.setPassword(AdminPW.c_str());

    ArduinoOTA.begin();

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
        } else {  // U_FS
        type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        printBody("OTA Started " + type);
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        int progress_per = progress / (total / 100);
        if (((progress_per % 10) == 0) && (progress_per != old_ota_prog)){
            old_ota_prog = progress_per;
            String prog = "Progress " + String(progress_per);
            printBody(prog);
            rp2040.wdt_reset(); //reset watchdog
        }
    });

    ArduinoOTA.onError([](ota_error_t error) {
        if (error == OTA_AUTH_ERROR) {
        printBody("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
        printBody("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
        printBody("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
        printBody("Receive Failed");
        } else if (error == OTA_END_ERROR) {
        printBody("End Failed");
        }
    });

}

int configNetwork()
{
    printBody("Reading Network config");
    // Configure Network

    // read wifi configuration from Network.json
    File networkFile = LittleFS.open("/Network.json", "r");
    if (!networkFile)
    {
        printBody("Failed to open network file");
        return 1;
    }
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, networkFile);
    if (error)
    {
        printBody("Failed to parse network file");
        return 1;
    }
    // convert to json
    JsonObject root = doc.as<JsonObject>();
    networkFile.close(); // close little fs file

    // read in Network settings

#ifdef WIFIMODE
                         // Network SSID (Wifi name)
    if (!root.containsKey("NETWORK_SSID"))
    {
        printBody("Error: No NETWORK_SSID in Network file");
        return 1;
    }
    Network_SSID = root["NETWORK_SSID"].as<String>();

    // Network password
    if (!root.containsKey("NETWORK_PASSWORD"))
    {
        printBody("Error: No NETWORK_PASSWORD in Network file");
        return 1;
    }
    Network_Password = root["NETWORK_PASSWORD"].as<String>();
#endif
#ifdef WIREDMODE
    printBody("Mac Address:");
    String mac_addr_comb = "";
    for (size_t i = 0; i < 6; i++)
    {
        char hexes[3] = "";
        macAddr[i] = UniqueID[i];
        sprintf(hexes, "%02X:", UniqueID[i]);
        mac_addr_comb += String(hexes);
        // printBody(String(hexes));
    }
    printBody(mac_addr_comb);
#endif

    // settings across wifi and wired
    // IP address that this device should appear as
    if (!root.containsKey("DeviceAddress"))
    {
        printBody("Error: No DeviceAddress in config file");
        return 1;
    }
    const char *ipaddress = root["DeviceAddress"];
    ip.fromString(ipaddress);
    // IP address of server to connect to
    if (!root.containsKey("SERVER_HOST"))
    {
        printBody("Error: No SERVER_HOST in Network file");
        return 1;
    }
    Server_Host = root["SERVER_HOST"].as<String>();

    // port of server
    if (!root.containsKey("SERVER_PORT"))
    {
        printBody("Error: No SERVER_PORT in Network file");
        return 1;
    }
    Server_Port = root["SERVER_PORT"].as<int>();

    // Prefix of URL to ping server
    if (!root.containsKey("SERVER_URLPREFIX"))
    {
        printBody("Error: No SERVER_URLPREFIX in Network file");
        return 1;
    }
    Server_URLPrefix = root["SERVER_URLPREFIX"].as<String>();

    printBody("Network Configured");
    return 0;
}

int configDevice()
{
    printBody("Reading Device config");
    // Open the config file
    File configFile = LittleFS.open("/Config.json", "r");
    if (!configFile)
    {
        printBody("Failed to open config file");
        return 1;
    }
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, configFile);
    if (error)
    {
        printBody("Failed to parse config file");
        return 1;
    }
    JsonObject root = doc.as<JsonObject>();
    configFile.close();
    // UUID used for accessing the server with correct ID
    // get UUID from json file
    if (!root.containsKey("UUID"))
    {
        printBody("Error: No UUID in config file");
        return 1;
    }
    Thing_ID = root["UUID"].as<String>();
    // get thing name from json file
    if (!root.containsKey("Thing_Name"))
    {
        printBody("Error: No Thing_Name in config file");
        return 1;
    }
    Thing_Name = root["Thing_Name"].as<String>();

    // get unlock time from json file
    if (!root.containsKey("UnlockSeconds"))
    {
        printBody("Error: No UnlockSeconds in config file");
        return 1;
    }
    Unlock_Seconds = root["UnlockSeconds"].as<int>();

    // get output pin from json file
    if (!root.containsKey("NumRGBleds"))
    {
        printBody("Error: No NumRGBleds in config file");
        return 1;
    }
    NUM_LEDS = root["NumRGBleds"].as<int>();

    // get output pin from json file
    if (!root.containsKey("ThingPin"))
    {
        printBody("Error: No ThingPin in config file");
        return 1;
    }
    thingPin = root["ThingPin"].as<int>();
    // get output pin from json file
    if (!root.containsKey("AdminPW"))
    {
        printBody("Error: No AdminPW in config file");
        return 1;
    }
    AdminPW = root["AdminPW"].as<String>();
    // get output pin from json file
    if (!root.containsKey("ThingOnState"))
    {
        printBody("Error: No ThingOnState in config file");
        return 1;
    }
    if (root["ThingOnState"].as<int>())
    {
        thingOnState = HIGH;
    }
    else
    {
        thingOnState = LOW;
    }
    printBody("Device Configured");
    pinMode(thingPin, OUTPUT);
    return 0; // return 0 if passed, 1 if failed
}

void initialise_leds()
{
    printBody("Setting up leds");
    leds = new CRGB[NUM_LEDS];
    FastLED.addLeds<LED_TYPE, LED_PINOUT, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
    FastLED.setBrightness(BRIGHTNESS);
}

void initialise_TFT_lcd(bool lcd_on)
{

    // setup LCD display
    TFT_lcd.begin();
    TFT_lcd.setRotation(SCREEN_ROTATION);            // set screen to right way around
    pinMode(TFTLCD_BACKLIGHT, OUTPUT); // backlight
    if (lcd_on)
        digitalWrite(TFTLCD_BACKLIGHT, HIGH); // turn backlight on
    else
        digitalWrite(TFTLCD_BACKLIGHT, HIGH); // turn backlight off
    TFT_lcd.fillScreen(ILI9341_BLACK);
    TFT_lcd.setTextColor(ILI9341_WHITE);
    TFT_lcd.setTextSize(2);
}

// display functions. Vary depending on the screen used
//  Title should display the status of thing (locked, unlocked etc). 1 Line
//  Headline should display more granular info such as querying server, last tag rejected. 1 Line
//  body should display extra details, i.e response from server, current user. upto 4 Lines (scrolling)
// Serial: all msgs are sent the same
// ILI9341: Title in largest font at top, headline in a smaller font, body in smallest
// LCD 4x20: 1st row Title, 2nd row Headline, 3rd & 4th row Body
void printTitle(String newMsg = ""){printTitle_update(newMsg,true);}
void printTitle_update(String newMsg = "", bool update = true)
{
    titleMsg = newMsg;
    if (serial_enabled)
        Serial.println(newMsg);
    if (update){
      if (TFT_lcd_enabled)
      {
          updateTFTlcd();
      }
      if (text_lcd_enabled){
          updateTextLcd();
      }
    }
}
void printHeadline(String newMsg = ""){printHeadline_update(newMsg,true);}

void printHeadline_update(String newMsg = "", bool update = true)
{
    headlineMsg = newMsg;
    if (serial_enabled)
        Serial.println(newMsg);
    if (update){
      if (TFT_lcd_enabled)
      {
          updateTFTlcd();
      }
      if (text_lcd_enabled){
          updateTextLcd();
      }
    }
}
void printBody(String newMsg = ""){printBody_update(newMsg,true);}

void printBody_update(String newMsg = "", bool update = true)
{
    // scroll all the text up one line
    for (int i = 1; i < LCD_NUM_LINES; i++)
    {
        bodyMsg[i - 1] = bodyMsg[i];
    }
    bodyMsg[LCD_NUM_LINES - 1] = newMsg;
    if (serial_enabled)
        Serial.println(newMsg);
    if (update){
      if (TFT_lcd_enabled)
      {
          updateTFTlcd();
      }
      if (text_lcd_enabled){
          updateTextLcd();
      }
    }
}
void printBodyLong(String newMsg = "")
{
    int line_count = 0;
    String msg = newMsg;
    while  (msg.length() > 20){ //if long msg then print over many lines
        printBody_update(msg.substring(0,20), true);
        msg = msg.substring(20);
        line_count++;
        if (line_count > 8) { //slow down text scrolling to be able to read whats going on
          rp2040.wdt_reset(); //reset watchdog
          delay(100);
        }
    }
    printBody(msg);
}

void updateTextLcd(){
    Textlcd.backlight();
    Textlcd.clear();
    Textlcd.setCursor(0, 0);
    Textlcd.print(titleMsg); 
    Textlcd.setCursor(20 - headlineMsg.length(), 0);
    Textlcd.print(headlineMsg);
    Textlcd.setCursor(0, 1);
    Textlcd.print(bodyMsg[LCD_NUM_LINES-3]);
    Textlcd.setCursor(0, 2);
    Textlcd.print(bodyMsg[LCD_NUM_LINES-2]);
    Textlcd.setCursor(0, 3);
    Textlcd.print(bodyMsg[LCD_NUM_LINES-1]);
}

String sendServerCustomMsg(String query = "", String msg ="", String token = "", String custom = "", bool response = 0){ //custom msg to server such as induct?XXX or tool_access?XXX
    msg.replace(" ", "%20");
    query.replace(" ", "%20");
    custom.replace(" ", "%20");
    #ifdef WIFIMODE
      if (WiFi.status() != WL_CONNECTED) {
        printBody("Error: WiFi Not Connected");
        return "";
      }    
      WiFiClient client;
    #endif
    #ifdef WIREDMODE
      if (!(Ethernet.linkStatus() == LinkON)) {
        printBody("ETHERNET NOT CONNECTED");
        return "";
      }    
      EthernetClient client;
    #endif
    if (!client.connect(Server_Host.c_str(), Server_Port)) {
            printBody("Error: server connection failed");
            return "";
    }    
    String url = Server_URLPrefix;
    url += query;
    url += "?thing=";
    url += Thing_ID.c_str();
    if (msg != ""){
        url += "&msg=";
        url += msg;
    }
    if (token != "") //token input is optional
    {
        url += "&token=";
        url += token;
    }
    if (custom != ""){
        url += custom;
    }

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + Server_Host.c_str() + "\r\n" + "Connection: close\r\n\r\n");

    if (response == 0){
    // don't care if it succeeds, so close connection and return
    client.flush();
    client.stop();
    return "";       
    } else {
        //response wanted
        int checkCounter = 0;
        while (!client.available())
        {
            delay(10);
            checkCounter++;
            if (checkCounter > 3000)
            {
                printBody("Server Didnt Respond");
                return "";
            }
        }

        //read json response
        String json;
        int loop_counter = 0;
        bool endOfHeaders = false;
        while (client.available() && (loop_counter < 512) && !endOfHeaders)
        {
            loop_counter++;
            json = client.readStringUntil('\n');
            if (json == "")
                endOfHeaders = true;
            if (json.length() >= 500)
                break; // way too much text

        }
        //clean up and close connection 
        client.flush();
        client.stop();  
        return json;       
    } 
}


void sendServerLogMsg(String msg = "", String token = "")
{
    msg.replace(" ", "%20");

    sendServerCustomMsg("msglog", msg, token, "");
    return;        
}

void sendServerUptimer(String token = "", String state ="", String msg = "", String active_time = ""){
    String custom = "&state=" + state;
    custom += "&active_time=" + active_time;
    sendServerCustomMsg("tool_access", msg, token, custom);
}

void sendServerInduction(String inductor_token = "", String inductee_token = ""){
    String custom = "&token_t="; //token of trainer
    custom += inductor_token;
    custom += "&token_s=";       //token of student
    custom += inductee_token;
    String json = sendServerCustomMsg("induct", "", "", custom, 1);
    //printBodyLong(json);
    if (json == "") return;
    DynamicJsonDocument jsonBuffer(384);
    deserializeJson(jsonBuffer,json);
    JsonObject root = jsonBuffer.as<JsonObject>();


    if (root.isNull()) {
      printBody_update("Error: Couldn't ", false);
      printBody_update("parse JSON", false);
      printBody_update("is server down?", true);
      if (Verbosity = VERB_HIGH)
        printBodyLong(json);
      if (Verbosity = VERB_HIGH_SERVER)
        sendServerLogMsg(Thing_Name + " DEBUG: " + json);
      return;
    }
    String result = "";
    if (root.containsKey("allowed")) {
        if (root["allowed"] == 1) {
            result += "Induction email sent to: ";
        }else{
            result += "Induction denied to: ";
        }
    }
    if (root.containsKey("person")){
        result += String(root["person"]["name"]);
    }
    printBodyLong(result);
    if (root.containsKey("error")){
        printBodyLong(root["error"]);
    }
}

void setupNetwork()
{
#ifdef WIFIMODE
    String hostname = "TC_" + Thing_Name;
    printBody("Connecting to Wifi");
    printBody(Network_SSID);
    //cyw43_wifi_pm(&cyw43_state, CYW43_PERFORMANCE_PM);
    WiFi.setHostname(hostname.c_str());
    WiFi.disconnect(true);
    //WiFi.mode(WIFI_STA);
    //const uint8_t WIFI_BSSID[] = {0x74, 0x83, 0xc2, 0x21, 0x74, 0xb9};
    WiFi.begin(Network_SSID.c_str(), Network_Password.c_str());
    //WiFi.config(ip);
    // Print periods on monitor while establishing connection
    rp2040.wdt_reset();
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        printBody(".");
        delay(1000);
        retry++;
        if (retry == 20){
            retry = 0;
            rp2040.wdt_begin(5000);//reset in 5 seconds to give wifi a chance to recover
            WiFi.begin(Network_SSID.c_str(), Network_Password.c_str());
        }
        #ifdef OVERRIDE_TOKENS
        //allow for override tags to unlock door regardless of wifi status
        RFID_TAG tag;
        getRFID_tag(&tag); //see if there is a RFID tag present
        if (tag.valid == true){ //if tag found
            printBody("Tag found");
            if (is_override_tag(tag)){
                unlockDevice();
            }
        }
        #endif

    }

    // Connection established
    printBody("Wifi Connected");
#endif
#ifdef WIREDMODE
    delay(250);
    printBody("Reseting Ethernet");
    Ethernet.setRstPin(ETHERNET_RESET);
    Ethernet.hardreset();
    delay(250);
    printBody("Initialising Ethernet");
    Ethernet.init(17);
    Ethernet.begin(macAddr, ip);
#endif
}

void updateTFTlcd()
{
    TFT_lcd.fillRect(0, 0, 239, 240, ILI9341_BLACK); // black out left side all the say to the status Icon
    // draw title message
    TFT_lcd.setCursor(0, 0);
    TFT_lcd.setTextSize(5);
    TFT_lcd.setTextColor(titleColour);
    TFT_lcd.println(titleMsg.substring(0, 8)); // can only fit 8 characters on top line
    // draw headline message
    TFT_lcd.setCursor(0, 50);
    TFT_lcd.setTextSize(3);
    TFT_lcd.setTextColor(headlineColour);
    TFT_lcd.println(headlineMsg.substring(0, 13)); // can only fit 13 characters on this line
    // draw body message
    TFT_lcd.setCursor(0, 80);
    TFT_lcd.setTextSize(2);
    TFT_lcd.setTextColor(bodyColour);
    for (int i = 0; i < LCD_NUM_LINES; i++)
    {
        TFT_lcd.setCursor(0, 80 + i * 15);
        TFT_lcd.println(bodyMsg[i].substring(0, 20)); // capped at 20 characters per line
    }
}

void toggle_TFTlcd()
{
    digitalWrite(TFTLCD_BACKLIGHT, !digitalRead(TFTLCD_BACKLIGHT));
}

// the Icon is a circle in the top right of the TFT lcd which at a quick glance shows the status of the device
//  its just a small circle which colour represents whats happening
void TFTLcd_setIconColour(uint16_t color = ILI9341_RED)
{
    TFT_lcd.fillCircle(279, 40, 40, color); // put circle in top right of display
    titleColour = color;
    if (TFT_lcd_enabled)
    {
        updateTFTlcd();
    }
}

void unlockDevice(String msg)
{
    digitalWrite(thingPin, thingOnState);
    if (TFT_lcd_enabled)
    {
        TFTLcd_setIconColour(ILI9341_GREEN);
    }
    printTitle(msg);
}

void lockDevice(String msg, uint16_t color)
{
    digitalWrite(thingPin, !thingOnState);
    if (TFT_lcd_enabled)
    {
        TFTLcd_setIconColour(color);
        //TFTLcd_setIconColour(ILI9341_RED);
    }
    printTitle(msg);
}

void showLogo()
{
    File image = LittleFS.open("/MS8080.bmp", "r");
    if (!image)
    {
        printBody("Failed to open image file");
        return;
    }

    uint32_t fileSize = image.size();
    uint32_t headerSize = 54; // Bitmap header size
    uint32_t imageSize = fileSize - headerSize;
    for (int i = 0; i < 18; i++)
    { // skip non important data
        image.read();
    }
    uint32_t bmpWidth = image.read(); // + (image.read() << 8) + (image.read() << 16) + (image.read() << 24);
    // bmpWidth = 60;
    image.read(); // skip useless info
    image.read();
    image.read();
    uint32_t bmpHeight = image.read(); // + (image.read() << 8) + (image.read() << 16) + (image.read() << 24);
    // bmpHeight = 60;
    image.read();
    image.read();
    image.read();

    // Read past unused bytes in BMP header
    for (int i = 0; i < 28; i++)
    {
        image.read();
    }

    // Set starting position for display to btm right
    uint16_t xPos = (TFT_lcd.width() - bmpWidth);
    uint16_t yPos = (TFT_lcd.height() - bmpHeight);

    // Read bitmap data
    for (int y = bmpHeight - 1; y >= 0; y--)
    {
        for (int x = 0; x < bmpWidth; x++)
        {
            uint16_t blue = image.read();
            uint16_t green = image.read();
            uint16_t red = image.read();
            uint16_t color = TFT_lcd.color565(red, green, blue);
            TFT_lcd.drawPixel(xPos + x, yPos + y, color);
        }
    }
    delay(1000);
    image.close();
}

// scan for tag and put it in the RFID_TAG that from pointer
void getRFID_tag(RFID_TAG *item)
{
    RFID_token uid;    // Buffer to store the returned UID (unique ID)
    uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    uint8_t success;
    static unsigned long lastChecked; // prevent from being scanned to frequently

    item->valid = 0;
    // read rfid tag
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, PN532_READ_TIMEOUT);
    if (success & (millis() > lastChecked + CARD_DEBOUNCE_DELAY))
    { // if card found and hasnt scanned it twice in quick sucession
        item->valid = 1;
        // memcpy(item->uid, uid, uidLength);
        item->uid_length = uidLength * 2;
        // convert from raw bytes to hex equivalent in chars
        const char *hex = "0123456789abcdef"; // look up table
        for (uint8_t i = 0; i < uidLength; i++)
        {
            item->uid[i * 2] = hex[(uid[i] >> 4) & 0xF]; // upper nibble
            item->uid[i * 2 + 1] = hex[(uid[i]) & 0xF];  // lower nibble
        }
        // null remaining bytes in string
        for (uint8_t i = uidLength; i < 7; i++)
        {
            item->uid[i * 2] = '0';
            item->uid[i * 2 + 1] = '0';
        }
        
        lastChecked = millis();
    }
    return;
}

void getSMAccountFromServer(SM_ACCOUNT *account)
{
    account->flags = 0; // i.e. no access
    account->colour = PATTERN_RED;
    memset(account->Name, 0, 20);

    // check still connected to the network
#ifdef WIFIMODE
    if (WiFi.status() != WL_CONNECTED) // if not connected
    {
        char countMsg[20] = "";
        printBody("Error: WiFi Not Connected");
        account->colour = PATTERN_WHITE;
        wifi_error_count++;
        sprintf(countMsg, "Wifi error Count: %d", wifi_error_count);
        printBody(String(countMsg));
        setupNetwork();
        if (wifi_error_count > 4)
        {
            while (1)
            { // if failed to connect 3 times use watchdog to reset
                delay(100);
            }
        }
        return;
    }
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
#endif
#ifdef WIREDMODE
    if (!(Ethernet.linkStatus() == LinkON))
    {
        printBody("ETHERNET NOT CONNECTED");
        account->colour = PATTERN_WHITE;
        wifi_error_count++;
        if (wifi_error_count > 4){
            while (1){ // if failed to connect 3 times use watchdog to reset
                delay(100);
            }
        }
        return;
    }
    EthernetClient client;
#endif
    // network connection is good now to connect to server
#ifdef WIREDMODE
    if (!client.connect(Server_Host.c_str(), Server_Port))
    {
#endif
#ifdef WIFIMODE
        if (!client.connect(Server_Host, Server_Port))
        {
#endif
        char countMsg[20] = "";
        printBody("Error: server ");
        printBody("Connection failed");
        account->colour = PATTERN_WHITE;
        client.flush();
        client.stop();
        wifi_error_count++;
        sprintf(countMsg, "Server error Count: %d", wifi_error_count);
        printBody(String(countMsg));
        setupNetwork();
        if (wifi_error_count > 4)
        {
            while (1)
            { // if failed to connect 3 times use watchdog to reset
                delay(100);
            }
        }
        return;
    }
    wifi_error_count = 0; // passed ok so reset error counter

    // network is ok, and can see server so on to sending a request
    String url = Server_URLPrefix;
    url += "verify";
    url += "?token=";
    for (int i = 0; i< account->tag.uid_length; i++){
        url += account->tag.uid[i];
    }
    //url += account->tag.uid;
    url += "&thing=";
    url += Thing_ID.c_str();


    // int server_response_time = 0;
    // server_response_time = millis();
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                    "Host: " + Server_Host.c_str() + "\r\n" +
                    "Connection: close\r\n\r\n");
    int checkCounter = 0;
    while (!client.available())
    {
        delay(10);
        checkCounter++;
        if (checkCounter > 3000)
        {
            printBody("Server Didnt Respond");
            account->colour = PATTERN_REDWHITE;
            return;
        }
    }

    //read json response
    String json;
    int loop_counter = 0;
    bool endOfHeaders = false;
    while (client.available() && (loop_counter < 512) && !endOfHeaders)
    {
        loop_counter++;
        json = client.readStringUntil('\n');
        if (json == "")
            endOfHeaders = true;
        if (json.length() >= 500)
            break; // way too much text

    }
    //clean up and close connection 
    client.flush();
    client.stop(); 

    //convert to json
    DynamicJsonDocument jsonBuffer(384);
    deserializeJson(jsonBuffer,json);
    JsonObject root = jsonBuffer.as<JsonObject>();
    account->colour = PATTERN_REDWHITE;//default error colour for network
    if (root.isNull()) {
      printBody("Error: Couldn't ");
      printBody("parse JSON");
      printBody("is server down?");
      if (Verbosity = VERB_HIGH)
        printBodyLong(json);
      if (Verbosity = VERB_HIGH_SERVER)
        sendServerLogMsg(Thing_Name + " DEBUG: " + json);
	  account->colour = PATTERN_REDWHITE;
      return;
    }
    if (!root.containsKey("access")) {
      printBody("Error: No access info");
      return;
    }else{
      if (root["access"] == 1) { //if access allowed
        account->flags |= TOKEN_ACCESS;
      } 
    }
    if (root.containsKey("cache")){
      if (root["cache"] == 1) { //dont  cache if weekend member
        account->flags |= TOKEN_CACHE; 
        account->store_counter = STORAGE_TIME;//setup how long staying in cache before checking again  
      }
    }
    if (root.containsKey("inductor")){
      if (root["inductor"] == 1) { //check if an inductor
        account->flags |= TOKEN_INDUCTOR; //or assign
      }
    } 
    if (root.containsKey("director")){
      if (root["director"] == 1) { //TBD director do anything special?
        account->flags |= TOKEN_DIRECTOR;
      }
    } 
    if (root.containsKey("colour")){
      account->colour  = root["colour"].as<int>(); //value between 0-63
    } 
    if (root.containsKey("message")){//Any message from the server that should be displayed
      printBodyLong(root["message"]);
    }
    if (root.containsKey("person")){
        if (sizeof(root["person"]["name"]) > 20)
            strncpy(account->Name, root["person"]["name"], 20); 
        else 
            strncpy(account->Name, root["person"]["name"], sizeof(root["person"]["name"]));
    } 
    if (root.containsKey("error")){//print out error messages
        printHeadline("Access Denied");
        if (root.containsKey("person")){
            //printHeadline("Access Denied to ");
            printBodyLong(root["person"]["name"]);
        }
        printBodyLong(root["error"]);
    }   
    return;
}

// should only be called by loop
void animate_leds()
    {
        if (NUM_LEDS == 0)
            return; // if no LEDs then just return

        uint8_t spin = (millis() / 300) % (NUM_LEDS); // fade leds around the led wheel based off time
        uint8_t fade_factor = (255 / NUM_LEDS);       // how much the leds should fade by
        switch (colourPattern)
        {
        case PATTERN_WHITE:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB::White;
                leds[i].nscale8((NUM_LEDS - ((i + spin) % NUM_LEDS)) * fade_factor);//fade the leds
            }
            break;    
        case PATTERN_GREEN:  //solid white
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB::Green;
                leds[i].nscale8((NUM_LEDS - ((i + spin) % NUM_LEDS)) * fade_factor);//fade the leds
            }
            break;  
        case PATTERN_PURPLE:  //solid white
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB::Purple;
                leds[i].nscale8((NUM_LEDS - ((i + spin) % NUM_LEDS)) * fade_factor);//fade the leds
            }
            break;   
        case PATTERN_BLUE:  //solid white
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB::Blue;
                leds[i].nscale8((NUM_LEDS - ((i + spin) % NUM_LEDS)) * fade_factor);//fade the leds
            }
            break;
        case PATTERN_PINK:  //solid white
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB::Pink;
                leds[i].nscale8((NUM_LEDS - ((i + spin) % NUM_LEDS)) * fade_factor);//fade the leds
            }
            break;
        case PATTERN_ORANGE:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB::Orange;
                leds[i].nscale8((NUM_LEDS - ((i + spin) % NUM_LEDS)) * fade_factor);//fade the leds
            }
            break;
        case PATTERN_RAINBOW:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CHSV((i+spin)*fade_factor, 255,255);
            }
            break;
        case PATTERN_PYB:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                int pos = ((i+spin) % NUM_LEDS)/8;
                switch (pos)
                {
                    case 1:
                        leds[i] = CRGB::Pink;
                        break;
                    case 2:
                        leds[i] = CRGB::Orange;
                        break;
                    case 0:
                        leds[i] = CRGB::Blue;
                        break;
                }
            }
            break;
        case PATTERN_RED:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB::Red;
                leds[i].nscale8((NUM_LEDS - ((i + spin) % NUM_LEDS)) * fade_factor);//fade the leds
            }
            break;
        
        case PATTERN_REDBLUE:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                if (((i + spin) % NUM_LEDS) > (NUM_LEDS/2) )
                    leds[i] = CRGB::Red;
                else
                    leds[i] = CRGB::Blue;
            }
            break;
        case PATTERN_REDORANGE:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                if (((i + spin) % NUM_LEDS) > (NUM_LEDS/2) )
                    leds[i] = CRGB::Red;
                else
                    leds[i] = CRGB::Orange;
            }
            break;
        case PATTERN_REDGREEN:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                if (((i + spin) % NUM_LEDS) > (NUM_LEDS/2) )
                    leds[i] = CRGB::Red;
                else
                    leds[i] = CRGB::Green;
            }
            break;
        case PATTERN_REDPURPLE:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                if (((i + spin) % NUM_LEDS) > (NUM_LEDS/2) )
                    leds[i] = CRGB::Red;
                else
                    leds[i] = CRGB::Purple;
            }
            break;
        case PATTERN_REDPINK:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                if (((i + spin) % NUM_LEDS) > (NUM_LEDS/2) )
                    leds[i] = CRGB::Red;
                else
                    leds[i] = CRGB::Pink;
            }
            break;
        case PATTERN_REDWHITE:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                if (((i + spin) % NUM_LEDS) > (NUM_LEDS/2) )
                    leds[i] = CRGB::Red;
                else
                    leds[i] = CRGB::White;
            }
            break;



        }
        FastLED.show();
    }

//remove SM account from storage
void removeFromStoredAccounts(int position){
    //clear all of it
    stored_accounts[position].tag.valid = 0;
    stored_accounts[position].tag.uid_length = 0;
    memset(stored_accounts[position].Name, 0, 20);
    memset(stored_accounts[position].tag.uid, 0, 14);
    stored_accounts[position].flags = 0;
    stored_accounts[position].store_counter = 0;
    stored_accounts[position].colour = PATTERN_RED;
}
//add SM account to storage
void addToStoredAccounts(SM_ACCOUNT new_account){
    //try and find a blank space in the list of SM_ACCOUNTs
    for (int i =0; i < STOREDACCOUNTSNUM; i++)
    {
        if (stored_accounts[i].tag.valid == 0){
            //found blank space, copy over data
            stored_accounts[i].tag.valid = new_account.tag.valid;
            stored_accounts[i].tag.uid_length = new_account.tag.uid_length;
            memcpy(stored_accounts[i].Name, new_account.Name, 20);
            memcpy(stored_accounts[i].tag.uid, new_account.tag.uid ,14);
            stored_accounts[i].flags = new_account.flags;
            stored_accounts[i].store_counter = STORAGE_TIME;
            stored_accounts[i].colour = new_account.colour; 
            //printBody("stored at loc: "+ String(i));
            return;
        }
    }
    printBody("out of space");
    //if no space left find the lowest store_counter as that will expire soonest
    int  lowest_counter = 1000;
    int  lowest_pos = 0;
    for (int i =0; i < STOREDACCOUNTSNUM; i++)
    {
        if (stored_accounts[i].store_counter < lowest_counter){
            lowest_counter = stored_accounts[i].store_counter;
            lowest_pos = i;
        }
    }
    removeFromStoredAccounts(lowest_pos);
    stored_accounts[lowest_pos].tag.valid = new_account.tag.valid;
    stored_accounts[lowest_pos].tag.uid_length = new_account.tag.uid_length;
    memcpy(stored_accounts[lowest_pos].Name, new_account.Name, 20);
    memcpy(stored_accounts[lowest_pos].tag.uid, new_account.tag.uid ,new_account.tag.uid_length);;
    stored_accounts[lowest_pos].flags = new_account.flags;
    stored_accounts[lowest_pos].store_counter = STORAGE_TIME;
    stored_accounts[lowest_pos].colour = new_account.colour; 
    printBody("stored at loc: "+ String(lowest_pos));
    return;   

}   


//check if in SM account storage
bool checkIfInStoredAccounts(SM_ACCOUNT *account){
    //check every account stored
    for (int i =0; i < STOREDACCOUNTSNUM; i++)
    {   
        
        //valid tag is used as a "is there a real entry here" flag
        if (stored_accounts[i].tag.valid)
        {

            bool match = true;
            //match uid length
            if (stored_accounts[i].tag.uid_length != account->tag.uid_length){
                match = false;
                continue;
            }
            //match uid
            for (int j = 0; j < stored_accounts[i].tag.uid_length; j++)
            {
                if (stored_accounts[i].tag.uid[j] != account->tag.uid[j])
                {
                    match = false;
                    break;
                }

            }
            if (match)
            {
                memcpy(account->Name, stored_accounts[i].Name, 20);
                account->flags  = stored_accounts[i].flags;
                account->colour = stored_accounts[i].colour;
                return true;
            }

   
        
        }
    }
    return false;
}

//check if SM accounts has been here for too long and remove from storage if needed
void updateStoredAccounts(){

    for (int i =0; i < STOREDACCOUNTSNUM; i++)
    { 
        if (stored_accounts[i].tag.valid)
        {
            if (stored_accounts[i].store_counter == 0){
                // printBody("removing from cache");
                // printBody(String(stored_accounts[i].Name));
                removeFromStoredAccounts(i);

            }
            else {
                stored_accounts[i].store_counter--;
            }
        }
    }

}

String uid2String(char *uid, int length){
    String result = "";
    for (int i = 0; i < length; i++){
        result += uid[i];
    }
    return result;

}
