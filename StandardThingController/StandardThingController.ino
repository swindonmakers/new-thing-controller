//SWINDON MAKERSPACE THING CONTROLLER
// DOOR V0.1
#define SOFTWARE_VER "1.1"
#define SOFTWARE_TYPE "Standard"

//DEFINE IF IN WIRED OR WIFI MODE (ONLY 1 define at a time!!!)
#define WIFIMODE  //UNCOMMENT TO ENABLE WIFI MODE - BOARD SELECTION MUST BE Pi PICO W
//#define WIREDMODE   //UNCOMMENT TO ENABLE WIRED MODE - BOARD SELECTION MUST BE Pi PICO or WIZnet W5500-EVB-Pico

// #define OVERRIDE_TOKENS

#define ACTIVATE_BUTTON 11
#define SIGNOUT_BUTTON 10
#define INDUCT_BUTTON 12
#define ACTIVATE_LED 22
#define SIGNOUT_LED 17
#define INDUCT_LED 15
#define SCREEN_ROTATION 1

#include "ThingContoller.h"
#ifdef OVERRIDE_TOKENS
    #include "override_tokens.h"
#endif 

long actionTimer = 0; //time between actions to automate actions in statemachine
long logout_timer = 0;
long tool_on_timer = 0; //start time value of when device is turned on 
long continued_on_timer = 0; //helps track time intervals to see if device is still on
int  idle_timeout = 0;
long debug_timer = 0; //count how long the debug button has been pressed for
bool debug_btn_state = false;

enum statemach {
    SM_SIGNED_OUT,        //waiting for someone swiping to sign in
    SM_SIGNED_IN,         //unlocked but machine not turned on yet
    SM_THING_ACTIVE,      //machine active
    SM_INDUCTING,         //inductor has signed in and inducting someone
    SM_OTA_DEBUG          //Hidden menu, enter to see debug info + enable OTA mode
};

void gotoSM_SIGNED_OUT();
void gotoSM_SIGNED_IN();
void gotoSM_THING_ACTIVE();
void gotoSM_INDUCTING();
void gotoSM_OTA_DEBUG();

SM_ACCOUNT account;
statemach thingState = SM_SIGNED_OUT;

void setup() {
  printHeadline("Booting Up");
  thing_setup();
  showLogo();
  delay(500);
  printHeadline("Booted up");
  rp2040.wdt_begin(8000);

  gotoSM_SIGNED_OUT();
  sendServerLogMsg(Thing_Name + " Booted Sucessfully!");

  pinMode(ACTIVATE_BUTTON, INPUT_PULLUP);
  pinMode(SIGNOUT_BUTTON, INPUT_PULLUP);
  pinMode(INDUCT_BUTTON, INPUT_PULLUP);
  pinMode(ACTIVATE_LED, OUTPUT);
  pinMode(SIGNOUT_LED, OUTPUT);
  pinMode(INDUCT_LED, OUTPUT);
  actionTimer = millis();
}

void loop() {
  rp2040.wdt_reset(); //reset watchdog
  RFID_TAG tag;
  switch (thingState){
    case SM_SIGNED_OUT:
      getRFID_tag(&tag); //see if there is a RFID tag present
      if (tag.valid == true){
        actionTimer = millis();
        account.tag = tag;
        if (checkIfInStoredAccounts(&account))
        {
          printBody("got from cache");
        }else{
          getSMAccountFromServer(&account);
          if (account.flags & TOKEN_CACHE){
              addToStoredAccounts(account);
          }
        }

        colourPattern = account.colour;
        if (account.flags & TOKEN_ACCESS)
        {
          gotoSM_SIGNED_IN();
          printBody_update("Welcome",false);
          printBody(account.Name);
          sendServerLogMsg("Access Granted to :" + String(account.Name) , + "to " + Thing_Name);
        }
      }
      if(actionTimer + (Unlock_Seconds*1000) < millis()){
        colourPattern = PATTERN_ORANGE;
      };

      //debug mode check
      if(!digitalRead(INDUCT_BUTTON)){
        if(debug_btn_state == false){
          debug_btn_state = true;
          debug_timer = millis();
        }else{
          if(debug_timer + 5000 < millis()){
            gotoSM_OTA_DEBUG();
          }
        }
      }

      break;//SM_SIGNED_OUT
      
    case SM_SIGNED_IN:
      if (!digitalRead(SIGNOUT_BUTTON)){
        gotoSM_SIGNED_OUT();
        break;
      }
      if (!digitalRead(ACTIVATE_BUTTON)){
        gotoSM_THING_ACTIVE();
        while(!digitalRead(ACTIVATE_BUTTON))
          {delay(50);}
        delay(50);
        break;
      }
      if (account.flags & TOKEN_INDUCTOR){
        if (!digitalRead(INDUCT_BUTTON)){
          gotoSM_INDUCTING();
          break;
        }
      } 
      
      if (continued_on_timer + 60000 < millis()){
        continued_on_timer += 60000;
        idle_timeout--;
        actionTimer = millis();//prevent screen clearing
        switch (idle_timeout){
          case 10: 
            printBody("Auto Logout in 10 Mins");
            break;
          case 5: 
            printBody("Auto Logout in 5 Mins");
            break;
          case 3: 
            printBody("Auto Logout in 3 Mins");
            break;
          case 2: 
            printBody("Auto Logout in 2 Mins");
            break;
          case 1: 
            printBody("Auto Logout in 1 Min");
            break;
          case 0: 
            printBody("Logout due to inactivity");
            gotoSM_SIGNED_OUT();
            break;
        }        
      }
      break; //SM_SIGNED_IN

    case SM_THING_ACTIVE:
      if (!digitalRead(ACTIVATE_BUTTON)){
        gotoSM_SIGNED_IN();
        break;
      }
      if (!digitalRead(SIGNOUT_BUTTON)){
        gotoSM_SIGNED_OUT();
        break;
      }
      if (continued_on_timer + 300000 < millis()){
        continued_on_timer += 300000; //not millis as this will keep it in time better
        long active_time = millis() - tool_on_timer;
        printBody("Still on for " + String(active_time/1000) + " Seconds");    
        sendServerUptimer(uid2String(account.tag.uid, account.tag.uid_length), "1", Thing_Name + " still being used by " + account.Name, String(active_time/1000));
        actionTimer = millis();
      }      
      break;
    case SM_INDUCTING:
      getRFID_tag(&tag); //see if there is a RFID tag present
      if (!digitalRead(INDUCT_BUTTON)){
        gotoSM_SIGNED_IN();
        break;
      }
      if (!digitalRead(SIGNOUT_BUTTON)){
        gotoSM_SIGNED_OUT();
        break;
      }      
      if (tag.valid == true){
        sendServerInduction(uid2String(account.tag.uid, account.tag.uid_length), uid2String(tag.uid, tag.uid_length));
        printBody("tag inducted");
      }
      break;//SM_THING_ACTIVE

    case SM_OTA_DEBUG:
      ArduinoOTA.handle(); //check for OTA updates
      if(!digitalRead(INDUCT_BUTTON)){
        if(debug_btn_state == false){
           gotoSM_SIGNED_OUT();
        }
      }else{
        debug_btn_state = false;
      }
      delay(10);
      actionTimer = millis(); //dont clear the screen in this state
      break;//SM_OTA_DEBUG

  }

  animate_leds(); //animate the leds    
  if (millis() > actionTimer + 30000)//if 30 seconds of inactivity
    {
        if(bodyMsg[LCD_NUM_LINES-1] != "")//if screen isnt already blank 
        {
            //printBody("Clearing Screen");
            delay(1000);
            for (int i = 0; i < LCD_NUM_LINES; i++)
            {
                //    printBody(String(i));
                bodyMsg[i] = "";
            }
            printBody("");//update screen(s)
            if (text_lcd_enabled) //turn of led backlight
                Textlcd.noBacklight();
        }
    }
}

void gotoSM_SIGNED_OUT(){
  thingState = SM_SIGNED_OUT;
  colourPattern = PATTERN_ORANGE;
  lockDevice("Tool Off",ILI9341_RED);
  printHeadline_update("Scan a Tag", false);
  printBody("Signed out");
  actionTimer = millis(); 
  tool_on_timer = 0;
  //set LEDS on buttons
  digitalWrite(ACTIVATE_LED, 0);
  digitalWrite(SIGNOUT_LED, 0);
  digitalWrite(INDUCT_LED, 0);
  ArduinoOTA.end();
  
}

void gotoSM_SIGNED_IN(){
  thingState = SM_SIGNED_IN;
  actionTimer = millis();
  titleColour = ILI9341_GREEN;
  colourPattern = account.colour;
  lockDevice("Tool Off",ILI9341_YELLOW);
  printHeadline("Signed In");
  continued_on_timer = millis();
  idle_timeout = 15; //15 min time out
  if (tool_on_timer != 0){
    long active_time = millis() - tool_on_timer;
    printBody_update("Device was on for ", false);
    printBody(String(active_time/1000) + " Seconds");
    sendServerUptimer(uid2String(account.tag.uid, account.tag.uid_length), "0", Thing_Name + " turned off by " + account.Name, String(active_time/1000));
  }
  
  //set LEDS on buttons
  digitalWrite(ACTIVATE_LED, 1);
  digitalWrite(SIGNOUT_LED, 1);
  if (account.flags & TOKEN_INDUCTOR){
    digitalWrite(INDUCT_LED, 1);
  }else{
    digitalWrite(INDUCT_LED, 0);
  }
}

void gotoSM_THING_ACTIVE(){
  unlockDevice("Tool ON!");
  printHeadline("Machine active");
  thingState = SM_THING_ACTIVE;
  actionTimer = millis();
  colourPattern = PATTERN_GREEN;
  continued_on_timer = millis();
  tool_on_timer = millis();
  sendServerUptimer(uid2String(account.tag.uid, account.tag.uid_length), "1", Thing_Name + " turned on by " + account.Name, "0");
  //set LEDS on buttons
  digitalWrite(ACTIVATE_LED, 1);
  digitalWrite(SIGNOUT_LED, 1);
  digitalWrite(INDUCT_LED, 0);
}

void gotoSM_INDUCTING(){
  thingState = SM_INDUCTING;
  printHeadline("Scan inductee Tag");
  
  lockDevice("Tool Off",ILI9341_BLUE);
  actionTimer = millis();
  colourPattern = PATTERN_BLUE;
  tool_on_timer = 0;
  //set LEDS on buttons
  digitalWrite(ACTIVATE_LED, 0);
  digitalWrite(SIGNOUT_LED, 1);
  digitalWrite(INDUCT_LED, 1);
}

void gotoSM_OTA_DEBUG(){
  thingState = SM_OTA_DEBUG;
  lockDevice("Tool Off",ILI9341_PINK);  
  for (int i = 0; i < LCD_NUM_LINES; i++)  {
      bodyMsg[i] = "";
  }
  printTitle_update("DEBUG", false);
  printHeadline_update("OTA ENABLED", false);
  printBody_update(Thing_Name, false); 
  #ifdef WIFIMODE
    printBody("WIFI: " + Network_SSID);
    if (WiFi.status() != WL_CONNECTED) 
      printBody_update("Disconnected", false); 
    else 
      printBody_update("Connected", false); 
  #endif
  #ifdef WIREDMODE
    printBody("ETHERNET " + Network_SSID);
    if (!(Ethernet.linkStatus() == LinkON))
      printBody_update("Disconnected", false); 
    else 
      printBody_update("Connected", false); 
  #endif
  printBody_update("SW = " +String(SOFTWARE_TYPE) + " V" + String(SOFTWARE_VER), true);
  setupOTA();  
  digitalWrite(INDUCT_LED, 1);

  
}
