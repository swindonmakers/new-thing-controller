//SWINDON MAKERSPACE THING CONTROLLER
// DOOR V0.1

//change board version to match what is printed on the thing controller board get most GPIO correct automatically
// needs to be a string
#define ThingControllerBoardRev 1

//DEFINE IF IN WIRED OR WIFI MODE (ONLY 1 define at a time!!!)
#define WIFIMODE  //UNCOMMENT TO ENABLE WIFI MODE - BOARD SELECTION MUST BE Pi PICO W
//#define WIREDMODE   //UNCOMMENT TO ENABLE WIRED MODE - BOARD SELECTION MUST BE Pi PICO or WIZnet W5500-EVB-Pico


//local files
//#define ENABLE_BUILTIN_BADGES
#ifdef ENABLE_BUILTIN_BADGES
#include "builtinBadges.h"
#endif

// #ifdef WIFIMODE
// #include "ThingControllerWifi.h"
// ThingControllerWifi controller;
// #endif
// #ifdef WIREDMODE
// #include "ThingControllerWired.h"
// ThingControllerWired controller;
// #endif
#include "ThingControllerBase.h"
ThingControllerBase controller;

//Door connection defines
#define EXIT_BUTTON 12  //INPUT digital GPIO expansion connector pin
#define DOOR_SENSOR 13  //INPUT digital GPIO expansion connector pin
#define LCD_ON 11       //use BTN0 as a toggle to turn on the LCD screen
#define VERBOSITY 10    //use BTN1 as a toggle for verbosity

enum statemach {
  SM_IDLE,         //waiting for someone swiping to the door
  SM_UNLOCK,       //door access granted or door unlock pressed
  SM_RESPONSE,     //door access denied
  SM_REMAINS_OPEN  //door has been left open 
  };

statemach thingState = SM_IDLE;


void setup() {

  
  int msg_pos = 0;

  Serial.begin(9600);

  //GPIO setup
  pinMode(EXIT_BUTTON, INPUT_PULLUP);
  pinMode(DOOR_SENSOR, INPUT_PULLUP);
  pinMode(VERBOSITY, INPUT_PULLUP);
  pinMode(LCD_ON, INPUT_PULLUP);

  controller.initLcd();

  // for(size_t i = 0; i < UniqueIDsize; i++){
  //   Serial.println(UniqueID[i], HEX);
  //   int tmp = UniqueID[i];
  //   controller.printMsgln(String(tmp), VERB_ALL);
  // }
  // printMsgln(msg, VERB_ALL);
  controller.printMsgln("Setting up", VERB_ALL);

  //start up little fs to get the config file
  controller.configDevice();

  delay(1000);  //delay so can be read



  // printMsgln("", VERB_ALL);
  //setup wifi
  controller.setupNetwork();



  controller.printMsgln("Setting up PN532", VERB_ALL);
  //connecting to PN532 rfid reader
  nfc.begin();
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
  //msg = String(nfc.getFirmwareVersion());
  //printMsg(msg, VERB_ALL);
  uint32_t versiondata = nfc.getFirmwareVersion();
  controller.printMsgln("Firmware ver. ", VERB_ALL);
  controller.printMsg(String((versiondata >> 16) & 0xFF), VERB_ALL);
  controller.printMsg(".", VERB_ALL);
  controller.printMsg(String((versiondata >> 8) & 0xFF), VERB_ALL);
  controller.printMsgln("PN532 Set up", VERB_ALL);
  controller.colorPattern = PATTERN_ORANGE;
  delay(5000);//delay needed to allow wifi to finish its own thing
  controller.sendLogMsg(controller.Thing_Name + " device started up sucessfully");
  verbosity = VERB_LOW;
  rp2040.wdt_begin(8000);
  controller.showLogo();
}

void loop() {
  rp2040.wdt_reset();
  TOKEN_CACHE_ITEM* tag = controller.lookForCard();  //check if there is a card present
  
  //unlock door if tag is valid
  if (tag != NULL){//if something got scanned
    if (tag->flags && TOKEN_ACCESS){
      thingState = SM_UNLOCK;
      actionTimer = millis();  
      controller.colorWipe(tag->colour);
      controller.printMsgln("Unlocking by Tag", VERB_MED);
      isUnlocked = true;
      controller.sendLogMsg("Access granted to " + String(controller.tokenStr));
      controller.unlockDevice();
      //unlock door
      //set door color
      //set door timeout
    }else{ //access denied but there was still data so got an error in response
      thingState = SM_RESPONSE;
      actionTimer = millis(); 
      controller.colorWipe(tag->colour);
      controller.printMsgln("Denied", VERB_ALL);
      isUnlocked = false;
      actionTimer = millis();  
      controller.sendLogMsg("Access denied to " +String( controller.tokenStr));
      //set colour
      //lock door
    }
  } else {
    switch (thingState){
      case SM_IDLE:
        if (!digitalRead(DOOR_SENSOR)){//if door is open for no reason
          thingState = SM_REMAINS_OPEN;
          controller.colorWipe(PATTERN_PINK);
        }
        if (!digitalRead(EXIT_BUTTON)){
          thingState = SM_UNLOCK;
          controller.colorWipe(PATTERN_PINK);
          controller.printMsgln("Unlocking by Button", VERB_MED);
          controller.unlockDevice();
          actionTimer = millis();  
        }
        break;
      case SM_UNLOCK:
        if(actionTimer + (controller.Unlock_Seconds*1000) < millis()){//if door has stayed open long enough
          if (!digitalRead(DOOR_SENSOR)){ //door still open
            thingState = SM_REMAINS_OPEN;
            controller.printMsgln("Door kept open", VERB_HIGH);
            controller.colorWipe(PATTERN_PINK);
          }else{ //door closed go to idle
            thingState = SM_IDLE;
            controller.colorWipe(PATTERN_ORANGE);
            controller.printMsgln("setting idle", VERB_HIGH);
            isUnlocked = false;
            controller.lockDevice();
          }
        }      
        break;
      case SM_RESPONSE:        
        if(actionTimer + (controller.Unlock_Seconds*1000) < millis()){//if door has stayed open long enough
          thingState = SM_IDLE;
          controller.colorWipe(PATTERN_ORANGE);
          controller.printMsgln("setting idle", VERB_HIGH);
          controller.lockDevice();
          //lock door
          //set colour
        }  
        break;
      case SM_REMAINS_OPEN:
        if (digitalRead(DOOR_SENSOR)){
          thingState = SM_IDLE;
          controller.colorWipe(PATTERN_ORANGE);
          controller.lockDevice();//lock door only once closed
        }
        break;
    }
  }   
  delay(25);
  
    //check if verbostiy should change
  if (!digitalRead(VERBOSITY)){
    controller.changeVerbosity();
    delay (200);
    while (!digitalRead(VERBOSITY)) {
      delay(1);   
      rp2040.wdt_reset(); //if dont put this here then presing the button for 8 seconds resets the device
      }
    delay (200);
  }
  //toggle LCD screen on/off
  if (!digitalRead(LCD_ON))   {
    digitalWrite(LCD_BACKLIGHT, !digitalRead(LCD_BACKLIGHT));
    delay (200);
    while (!digitalRead(LCD_ON)) {
      delay(1);   
      rp2040.wdt_reset(); //if dont put this here then presing the button for 8 seconds resets the device
    }
    delay (200);
  }
  controller.animation(); //update the LED pattern thats currently showing
  //checkDoorStatus(); //check door statis
}


