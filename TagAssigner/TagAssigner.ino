//SWINDON MAKERSPACE TAG ASSIGNER
// The aim of this device is to:
//   Assign new tags to members accounts (tag assigner inductor required to prevent bad actors)
//   View current status of a members membership (paid uptil when, membership type)
//   View inductions of a member


//Uses Thing Controller Base for bulk of code


#define ThingControllerBoardRev 1
//DEFINE IF IN WIRED OR WIFI MODE (ONLY 1 define at a time!!!)
#define WIFIMODE  //UNCOMMENT TO ENABLE WIFI MODE - BOARD SELECTION MUST BE Pi PICO W
//#define WIREDMODE   //UNCOMMENT TO ENABLE WIRED MODE - BOARD SELECTION MUST BE Pi PICO or WIZnet W5500-EVB-Pico

#include "ThingController.h"

#include "Wire.h" //used for keypad
#include "I2CKeyPad.h" //used for keypad
const uint8_t KEYPAD_ADDRESS = 0x20;
I2CKeyPad keyPad(KEYPAD_ADDRESS);

SM_ACCOUNT account;
SM_ACCOUNT assignee;
RFID_TAG assignee_tag;
RFID_TAG tag;

enum menu_state {
  main_menu,          //main 
  tag_assign_auth,    //tag assign, need authenticator to login
  tag_assign_code,    //input members account code before
  tag_assign_new_tag, //account code valid, scan tag to join to it
  tag_assign_desc_name,  //description for tag name in database - pt 1
  tag_assign_desc_color1, //description for tag name in database - pt 2 option 1
  tag_assign_desc_color2, //description for tag name in database - pt 2 option 2
  member_status,      //scan tag to show members status
  induction_list      //scan tag to show induction list

};

menu_state menu = main_menu;
uint32_t lastKeyPressed = 0;
char old_key = 'X';
int ID_number[4] = {-1,-1,-1,-1};
String tag_desc = "";
String display_code = "";
// int ID_number_count = 0;

//functions
void clear_body();

void setup() {

  Serial.begin(9600);
  

  printHeadline("Booting Up");
  thing_setup();
  showLogo();

  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    printBody("ERROR: cannot communicate to keypad.\nPlease reboot.");
    while(1);
  }
  printTitle("AdminBox");
  TFTLcd_setIconColour(ILI9341_PINK);
  printHeadline("Booted up");
  rp2040.wdt_begin(8000);

  goto_main_menu(); //setup display menu system
  sendServerLogMsg(Thing_Name + " Booted Sucessfully!");
}

void loop() {
  rp2040.wdt_reset(); //reset watchdog
  char key_pressed = get_key();

  //menu system
  switch(menu){
    case main_menu:
      //main menu //button press to go to certain activity
      switch(key_pressed){
        case 'A':
          goto_tag_assign_auth();
          break;
        case 'B':
          // goto_member_status();
          break;
      }
      break;
    case tag_assign_auth:
      getRFID_tag(&tag); //see if there is a RFID tag present
      if (tag.valid == true){
        account.tag = tag;
        getSMAccountFromServer(&account);
        if (account.flags & TOKEN_ACCESS){
          delay(500);
          goto_tag_assign_code();
        }
        else{
          printBody("Tag found");
          printBody("Not Valid Tag Assigner!");
        }
      }
      switch(key_pressed){
        case 'C': //cancel and go back  to menu
          goto_main_menu();
          break;
      }
      break;
    case tag_assign_code:
      
      switch(key_pressed){
        case 'C':
          goto_main_menu();
          break;
      }
      if (isDigit(key_pressed)){
        display_code= "";
        for (int i=0; i< 4; i++){
          
          if (ID_number[i] == -1){
            ID_number[i] = int(key_pressed-48);//ascii convert -48
            display_code = display_code + String(ID_number[i]);
            break;
          }
          display_code = display_code + String(ID_number[i]);//build up msg
        }
        clear_body();
        printBody_updated("Enter Membership ID",false);
        printBody_updated("All 4 Digits",false);
        printBody("SM" + display_code);
      }
      if (ID_number[3] != -1){
        //goto_tag_assign_new_tag(display_code);
        goto_tag_assign_desc_name();
      }
      
      break;

    case tag_assign_desc_name:
      switch(key_pressed){
        case 'C':
          goto_main_menu();
          break;
        case '1':
          tag_desc = "Keyring";
          goto_tag_assign_desc_color1();
          break;
        case '2':
          tag_desc = "Makercoin";
          goto_tag_assign_desc_color1();
          break;
        case '3':
          tag_desc = "Card";
          goto_tag_assign_desc_color1();
          break;
        case '4':
          tag_desc = "Other";
          goto_tag_assign_desc_color1();
          break;
      }
      break;

    case tag_assign_desc_color1:
      switch(key_pressed){
        case 'C':
          goto_main_menu();
          break;
        case '1':
          tag_desc = "Black " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '2':
          tag_desc = "White " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '3':
          tag_desc = "Red " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '4':
          tag_desc = "Orange " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '5':
          tag_desc = "Yellow " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '6':
          tag_desc = "Green " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '7':
          tag_desc = "Blue " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '8':
          goto_tag_assign_new_tag();
          break;
        case '#':
          goto_tag_assign_desc_color2();
          break;

      }
      break;

    case tag_assign_desc_color2:
      switch(key_pressed){
        case 'C':
          goto_main_menu();
          break;
        case '1':
          tag_desc = "Pink " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '2':
          tag_desc = "Purple " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '3':
          tag_desc = "Violet " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '4':
          tag_desc = "Cyan " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '5':
          tag_desc = "Magenta " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '6':
          tag_desc = "Rainbow " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '7':
          tag_desc = "Transparent " + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '8':
          tag_desc = "Multicolour" + tag_desc;
          goto_tag_assign_new_tag();
          break;
        case '#':
          goto_tag_assign_desc_color1();
          break;

      }
      break;
    case tag_assign_new_tag:

      uint8_t success;
      uint8_t uidLength;  // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
      static unsigned long lastChecked;
      getRFID_tag(&assignee_tag);
      if (assignee_tag.valid && (millis() > lastChecked + CARD_DEBOUNCE_DELAY)) {
        printBody("Found RFID TAG");

        lastChecked = millis();
        add_tag_to_member();
        delay(3000);
        printBody_updated("Returning to",false);
        printBody("Main Menu");
        delay(3000);
        goto_main_menu();
      }



      switch(key_pressed){
        case 'C':
          goto_main_menu();
          
      }
      break;
    case member_status:
      switch(key_pressed){
        case 'C':
          goto_main_menu();
      }
      break;
  }


}

char get_key(){ //get key from keypad if pressed and return the character

  uint32_t now = millis();
  char keys[]   = "147*2580369#ABCDNF"; // N = NoKey, F = Fail //yes dumb order but thats how its wired right now :P 
  char current_key = 0;

  if (now - lastKeyPressed >= 50)
  {
    lastKeyPressed = now;
    current_key = keys[keyPad.getKey()]; //get key  from I2CKeypad library
    if ((current_key != 'N') && (current_key != 'F')){ //N is no key press
      if (current_key != old_key) { 
        //controller.printMsgln(String(current_key), VERB_ALL);
        old_key = current_key;
        return current_key;
      }
    }
    old_key = current_key;
  }

  return 0;
}

void clear_body(){
  for (int i = 0; i < LCD_NUM_LINES; i++)
  {
      bodyMsg[i] = "";
  }
  printBody("");//update screen(s)
}


void goto_main_menu(){
  menu = main_menu;
  clear_body();
  printHeadline_updated("MAIN MENU",false);
  printBody_updated("A = Tag Assignment",false);
  printBody_updated("",false);
  printBody_updated("B = Member Status",false);
  printBody_updated("     TODO",false);
  printBody_updated("C = Member",false);
  printBody("     Inductions TODO");
}
void goto_tag_assign_auth(){
  menu = tag_assign_auth;
  clear_body();
  printHeadline_updated("Assign TAG",false);
  printBody_updated("C = Cancel",false);
  printBody_updated("Admin to login",false);
  printBody("Scan Admin Tag");
}
void goto_tag_assign_code(){
  menu = tag_assign_code;  
  clear_body();
  printHeadline_updated("Assign TAG",false);
  printBody_updated("C = Cancel",false);
  printBody_updated("Enter Membership ID",false);
  printBody_updated("All 4 Digits",false);
  printBody("SM_");
  //controller.printMsg("_", VERB_ALL);
  for (int i=0; i< 4; i++){
    ID_number[i] = -1;
  }
  
  display_code = "";
}
void goto_tag_assign_new_tag(){
  //String result = "SM" + String(ID_number[0]) + String(ID_number[1]) + String(ID_number[2]) + String(ID_number[3]);
  clear_body();
  printBody_updated("Assign Tag",false);
  printBody_updated("Assigning tag to",false);
  printBody_updated("SM" + display_code,false);
  printBody_updated("Called", false);
  printBody_updated(tag_desc, false);
  printBody("Scan New Tag");
  // for (int i=0; i< 4; i++){
  //   controller.printMsg(String(ID_number[i]), VERB_ALL);
  // }   
  menu = tag_assign_new_tag;
}

void goto_tag_assign_desc_name(){
  tag_desc="";
  menu = tag_assign_desc_name;
  clear_body();
  printHeadline_updated("SM"+ display_code);
  printBody_updated("Select Tag desc",false);
  printBody_updated("1 = Keyring",false);
  printBody_updated("2 = Makercoin",false);
  printBody_updated("3 = Card",false);
  printBody_updated("4 = Other",true);
}
void goto_tag_assign_desc_color1(){
  menu = tag_assign_desc_color1;
  clear_body();
  printHeadline_updated("SM"+ display_code);
  printBody_updated("Select Tag desc",false);
  printBody_updated("1 = Black",false);
  printBody_updated("2 = White",false);
  printBody_updated("3 = Red",false);
  printBody_updated("4 = Orange",false);
  printBody_updated("5 = Yellow",false);
  printBody_updated("6 = Green",false);
  printBody_updated("7 = Blue",false);
  printBody_updated("8 = None",false);
  printBody_updated("# for more",true);
}
void goto_tag_assign_desc_color2(){
  menu = tag_assign_desc_color2;
  clear_body();
  printHeadline_updated("SM"+ display_code);
  printBody_updated("Select Tag desc",false);
  printBody_updated("1 = Pink",false);
  printBody_updated("2 = Purple",false);
  printBody_updated("3 = Violet",false);
  printBody_updated("4 = Rainbow",false);
  printBody_updated("5 = Magenta",false);
  printBody_updated("6 = Cyan",false);
  printBody_updated("7 = Transparent",false);
  printBody_updated("8 = Multicolor",false);
  printBody_updated("# go back",true);
}

// void goto_member_status(){
//   controller.clearLcd();
//   controller.printMsgln("MEMBERSHIP STATUS", VERB_ALL);
//   controller.printMsgln("Scan tag to see ", VERB_ALL);
//   controller.printMsgln("membership status", VERB_ALL);
//   menu = member_status;
// }
// void goto_induction_list(){
//   menu = induction_list;
// }


void add_tag_to_member(){

  String custom_msg = "";
  // url += "assign";
  custom_msg += "&admin_token=";
  custom_msg += uid2String(tag.uid, tag.uid_length);
  custom_msg += "&person_id=";
  for (int i=0; i< 4; i++){
    custom_msg += String(ID_number[i]);
  } 
  custom_msg += "&token_id=";
  custom_msg += uid2String(assignee_tag.uid, assignee_tag.uid_length);//to replace with actual token;
  custom_msg += "&desc=" + tag_desc;
  
  printBodyLong(ParseMsgErrorJson(sendServerCustomMsg("assign","","",custom_msg, 1)));

}
