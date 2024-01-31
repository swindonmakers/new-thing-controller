//SWINDON MAKERSPACE THING CONTROLLER
// DOOR V0.1

//DEFINE IF IN WIRED OR WIFI MODE (ONLY 1 define at a time!!!)
//#define WIFIMODE  //UNCOMMENT TO ENABLE WIFI MODE - BOARD SELECTION MUST BE Pi PICO W
#define WIREDMODE   //UNCOMMENT TO ENABLE WIRED MODE - BOARD SELECTION MUST BE Pi PICO or WIZnet W5500-EVB-Pico

#define OVERRIDE_TOKENS

#include "ThingContoller.h"
#ifdef OVERRIDE_TOKENS
    #include "override_tokens.h"
#endif 
//Door IO defines
#define EXIT_BUTTON 12  //INPUT digital GPIO expansion connector pin
#define DOOR_SENSOR 13  //INPUT digital GPIO expansion connector pin

long actionTimer = 0; //time between actions to automate actions in statemachine
long cacheTimer = 0;  //time counter between checking cache (hourly)


enum statemach {
    SM_IDLE,         //waiting for someone swiping to the door
    SM_UNLOCK,       //door access granted or door unlock pressed
    SM_RESPONSE,     //door access denied
    SM_REMAINS_OPEN  //door has been left open 
};


void gotoSM_IDLE();
void gotoSM_UNLOCK();
void gotoSM_RESPONSE();
void gotoSM_REMAINS_OPEN();

statemach thingState = SM_IDLE;
int i=0;
void setup() {
    pinMode(EXIT_BUTTON, INPUT_PULLUP);
    pinMode(DOOR_SENSOR, INPUT_PULLUP);
    printHeadline("Booting Up");
    populate_override_tags();//do before setup so tags can be used during setup
    thing_setup();
    showLogo();
    delay(2000);
    printHeadline("Booted up");
    rp2040.wdt_begin(8000);
    

    gotoSM_IDLE();
    sendServerLogMsg(Thing_Name + " Booted Sucessfully!");
};

void loop(){
    rp2040.wdt_reset(); //reset watchdog
    RFID_TAG tag;
    getRFID_tag(&tag); //see if there is a RFID tag present
    if (tag.valid == true){ //if tag found
        SM_ACCOUNT account;
        account.tag = tag;
        #ifdef OVERRIDE_TOKENS
        if (is_override_tag(tag)){
            gotoSM_UNLOCK();
            colourPattern = PATTERN_PYB;
            TFTLcd_setIconColour(ILI9341_WHITE);
            printTitle("OVERRIDE");
            sendServerLogMsg("Override Key Used", uid2String(tag.uid, tag.uid_length));
        }else{
        #endif
            if (checkIfInStoredAccounts(&account))
            {
                printBody("got from cache");
            }else{
                //not in cache check server
                getSMAccountFromServer(&account);
                if (account.flags & TOKEN_CACHE){
                    addToStoredAccounts(account);
                }
            }
            colourPattern = account.colour;
            if (account.flags & TOKEN_ACCESS)
            {
                gotoSM_UNLOCK();
                printHeadline("Valid Tag");
                printBody("Welcome");
                printBody(account.Name);
                TFTLcd_setIconColour(ILI9341_GREEN);
                sendServerLogMsg("Access Granted to :" + String(account.Name) , uid2String(account.tag.uid, account.tag.uid_length));

            }
            else
            {
                gotoSM_RESPONSE();
                printHeadline("Access Denied");
                if (account.Name[0] == 0 )//if no name
                {
                    sendServerLogMsg("Access Denied to" , uid2String(account.tag.uid, account.tag.uid_length));
                } else{
                    sendServerLogMsg("Access Denied to :" + String(account.Name) ,uid2String(account.tag.uid, account.tag.uid_length));
                }
            }
        #ifdef OVERRIDE_TOKENS
        }
        #endif
    }
    
    //statemachine logic

    switch (thingState){
        case SM_IDLE:
            if (!digitalRead(DOOR_SENSOR)){//if door is open for no reason
                gotoSM_REMAINS_OPEN();
            }
            if (!digitalRead(EXIT_BUTTON)){//if someone wants to leave
                gotoSM_UNLOCK();
                colourPattern = PATTERN_PINK;
                printBody("Unlocking by Button");
            }
            break;
        case SM_UNLOCK:
            if(actionTimer + (Unlock_Seconds*1000) < millis()){//if door has stayed open long enough
                if (!digitalRead(DOOR_SENSOR)){ //door still open
                    gotoSM_REMAINS_OPEN();
                }else{ //door closed go to idle
                    gotoSM_IDLE();
                }
            }
            break;
        case SM_RESPONSE:        
            if(actionTimer + (Unlock_Seconds*1000) < millis()){//if door has stayed open long enough
                gotoSM_IDLE();
            }  
            break;                  
        case SM_REMAINS_OPEN:
            if (digitalRead(DOOR_SENSOR)){
                gotoSM_IDLE();
            }
            break;

    }


    if (millis() > (cacheTimer + 3600000)){
        //once every hour check membership cache 
        updateStoredAccounts();
        cacheTimer = millis();
    }
    animate_leds();//animate the spiral effect

 
    if (millis() > actionTimer + 30000)//if 30 seconds of inactivity
    {
        if(bodyMsg[LCD_NUM_LINES-1] != "")//if screen isnt already blank 
        {
            printBody("Clearing Screen");
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

void gotoSM_IDLE(){
    thingState = SM_IDLE;
    colourPattern = PATTERN_ORANGE;
    printHeadline("Scan a Tag");
    lockDevice();//lock door only once closed
    actionTimer = millis(); 
}
void gotoSM_UNLOCK(){
    unlockDevice();
    thingState = SM_UNLOCK;
    actionTimer = millis();
}
void gotoSM_RESPONSE(){
    thingState = SM_RESPONSE;
    actionTimer = millis();
}
void gotoSM_REMAINS_OPEN(){
    thingState = SM_REMAINS_OPEN;
    colourPattern = PATTERN_PINK;
    if (headlineMsg != "Door is open"){//update display to say its open if it isnt right now
        printHeadline("Door is open");
    }
}
//todo 
//update master tokens
//allow override tag to work during wifi boot
//4x20 lcd stuff