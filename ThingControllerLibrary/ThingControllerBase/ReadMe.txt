Readme for the thingcontroller Library

//How to setup
) Copy this folder "ThingControllerBase" into your arduino library
) For windows this in in Documents\Arduino\libraries

//Useage
) This library contains an object to instansiate called ThingControllerBase
) It is designed to run specifically on the Pi Pico 2040 Wifi or with Ethernet Controller
) To swap between Wifi and Wired you need to #define WIFIMODE or WIREDMODE in your program otherwise you will get compiling errors
) This Library is designed to cover the base functions that all controllers might need. 
)   Not all controllers will need every function but the majority like contacting the server and getting the response back is there
) The actual locking/unlocking rules of each "thing" is left to the user with the main script that calls this library

Supporting files
) This library relies on LittleFS to read in critical files regarding each thingcontrollers properties
) These must be stored in a folder called data in the project directory and use the old arduino software (1.8.X) with the littleFS plug in to flash to the pi pico
) This requires "flash size" under tools set to "2MB (Sketch: 1984KB, FS:64KB)"
) Required files - should be in github repo
)   Config.json - properties like thing name, ID and pinout - thing specific
)   Network.json - network settings (both wifi and wired) - should be common across things on the same network, should contain all settings for wifi and wired to make swapping easier
)   MS5050.bmp - makerspace logo bitmap shown in corner

Required Libraries - newer version may work but listed version has been tested and known to work ok
) Adafruit GFX Library - 1.11.5
) Adafruit IL9341 - 1.5.12
) Wifi - installed when installing Pi Pico board manager
) Adafruit Neopixel - 1.11.0
) Adafruit PN532 - 1.2.3 - newer versions have issues right now - need to look into more
) ArduinoJson - 6.20.1
) littleFs - 2.6.0 @ https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases