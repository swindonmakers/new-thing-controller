GUIDE TO NEW DOOR CONTROLLER

NOTE CURRENTLY REQURIES Adafruit PN532 library to be on version 1.2.3 (1.3.0 doesnt work for some reason and cant tell why right now)

DOOR COLOUR CODES REFERENCE
Normal colours
YELLOW - Idle waiting for tag, door is locked
RED - Tag not recognised as anyone
PINK - Door open or unlocked by exit button
GREEN - Access granted to member


HALF RED/YELLOW - Member found but hasnt paid
HALF RED/BLUE - Weekend member trying to access on week day
WHITE - Network Issue - attempt multple times reset device and it might regain connection after reset
HALF RED/PURPLE - Member doesnt have permission (shouldnt happen on door but may on tools)
HALF RED/PINK - Tag is associated to more than 1 account - please talk to directors as there is a mistake in database
HALF RED/GREEN - Unknown error - please let directors know as this shouldnt happen (if repeatable please take video of what how its caused)



PINOUT
NFC reader IRQ Pin to Pin 17 (defined in code)
LCD ON/OFF toggle SW0 (defined in code)
verbostiy toggle SW1 (defined in code)
Door sensor SW2 (defined in code) - Check that active low is correct behaviour
Exit button SW3 (defined in code) - Check that active low is correct behaviour
Door unlock Pin8 (defined in config json) 

Device configuration
The device configuration is defined by the LitteFS file stored on the Pi Pico called config.json
This includes settings which should vary per THING such as :
  How long the device is unlocked for (5 seconds for door)
  Thing name - door
  Thing UUID - UUID needed for server ping

Network configuration 
The network configuration is defined by the LittleFS file stored on the pi Pico called network.json
If you want to swap between WIFI and WIRED then you will need to comment/uncomment the define in the main code 
#define WIFIMODE //uncomment for WIFI MODE
#define WIREDMODE //uncomment for WIRED MODE (NOT YET IMPLEMENTED!)

To change the config files you need the older Arduino software with the littleFS plug in to upload to the Pi Pico 
  Set Pi Pico flash size to 2Mb with FS 64Kb


Verbosity System
The display is used only for debug messages right now
To prevent too much text being displayed at times and missing the useful stuff or wanting more info there are 3 verbosity levels to the messages
Low/Med/High
SW1 switches between these settings and each message has an assigned verbosity level
High shows all messages (low, med and high)
Medium shows some messages (low and med)
Low shows limited messages


Code implementation
The bulk of the code is done by ThingControllerBase.h 
  It has an class that is instaniated by ThingControllerWifi/Wired.h depending on defines
  ThingControllerBase.h has no network functions as they are library specific based on hardware config, wifi/wired fills this gap with override functions
  Most actions are in this class and you just call its functions to save code rewrite
The Top file should create an object from this, set it up and have a statemachine that defines how the thing unlocks/locks the thing
  As each tool or door might have different behaviour then each one requires its own logic for when to unlock

