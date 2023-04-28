//This should only be imported and compiled when using a pi pico with Ethernet
//If using a wifi connection then this file will not be compiled

//thing controller base includes lots of default settings and useful functions, import after all defines
#include "ThingControllerBase.h"

//TO BE WORK ON, ONLY WIRELESS WORKS RIGHT NOW!


class ThingControllerWired : public ThingControllerBase {
  
  void configureDevice(){
    //Configure Network
    
    congigDevice(); //function in Thing controller base that sets up all generic settings
  }
}