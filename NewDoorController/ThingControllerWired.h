//This should only be imported and compiled when using a pi pico with Ethernet
//If using a wifi connection then this file will not be compiled

//thing controller base includes lots of default settings and useful functions, import after all defines
#include "ThingControllerBase.h"
#include <ArduinoUniqueID.h>
#include "Ethernet_Generic.h"
//TO BE WORK ON, ONLY WIRELESS WORKS RIGHT NOW!


class ThingControllerWired : public ThingControllerBase {
public:
  byte macAddr[6] = { 0, 0, 0, 0, 0, 0 };
  void sendLogMsg(String msg) {
    //Serial.print("Sending log to server: ");
    //Serial.println(host);
    msg.replace(" ", "%20");
    // check if connected
    if (!(Ethernet.linkStatus() == LinkON)) {
      printMsgln("ETHERNET NOT CONNECTED", VERB_ALL);
      return;
    }

    EthernetClient client;
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
  int configureDevice() {
    //Configure Network
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

    congigDevice();  //function in Thing controller base that sets up all generic settings

    //setup mac address
    //Wiznet ethernet controller doesnt have a default mac address
    //so randomize based of Pi Picos Unique ID ot prevent address conflicts
    //when there are many of these devices
    printMsgln("Mac Address:", VERB_LOW);
    for (size_t i = 0; i < 6; i++) {
      char hexes[3] = "";
      macAddr[i] = UniqueID[i];
      sprintf(hexes, "%02X:", UniqueID[i]);
      printMsg(String(hexes), VERB_LOW);
    }
    printMsgln("Device Configured", VERB_ALL);
    printMsg(Thing_Name, VERB_ALL);
    printMsg(" Controller", VERB_ALL);

    return 1;
  }

  void setupNetwork() {

    Ethernet.init(17);
    Ethernet.begin(macAddr, ip);
  }

  TOKEN_CACHE_ITEM* queryServer(const uint8_t* uid, uint8_t uidLength) {

    //clear results from pervious query
    serverResult.flags = 0;
    serverResult.colour = PATTERN_RED;
    serverResult.count = 0;
    // serverResult.token = uid;
    memset(serverResult.token, 0, sizeof(serverResult.token));  //clear all of token
    memcpy(serverResult.token, uid, uidLength);
    serverResult.length = uidLength;
    serverResult.sync = 0;
    printMsgln("Check Ethernet", VERB_HIGH);
    if (!(Ethernet.linkStatus() == LinkON)) {
      printMsgln("ETHERNET NOT CONNECTED", VERB_ALL);
      serverResult.colour = PATTERN_WHITE;
      return &serverResult;
    }

    EthernetClient client;
    printMsgln("Check server", VERB_LOW);
    if (!client.connect(Server_Host.c_str(), Server_Port)) {
      printMsgln("Error: Connection failed", VERB_ALL);
      serverResult.colour = PATTERN_WHITE;
      client.flush();
      client.stop();
      return &serverResult;
    }

    printMsgln("Sending Msg to server", VERB_HIGH);
    // We now create a URI for the request
    String url = Server_URLPrefix;
    url += "verify";
    url += "?token=";
    url += tokenStr;
    url += "&thing=";
    url += Thing_ID.c_str();
    //printMsgln(url, VERB_ALL);

    int server_response_time = 0;
    server_response_time = millis();
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + Server_Host.c_str() + "\r\n" + "Connection: close\r\n\r\n");
    int checkCounter = 0;

    while (!client.available()) {
      delay(10);
      checkCounter++;
      if (checkCounter > 3000) {
        printMsgln("Server Did not Respond", VERB_LOW);
        return &serverResult;
      }
    }
    //printMsgln ("Server Responded", VERB_MED);
    printMsg("Response time ms: ", VERB_HIGH);
    printMsgln(String(millis() - server_response_time), VERB_HIGH);
    String json;
    int loop_counter = 0;
    bool endOfHeaders = false;
    while (client.available() && (loop_counter < 512) && !endOfHeaders) {
      loop_counter++;
      json = client.readStringUntil('\n');
      if (json == "") endOfHeaders = true;
      if (json.length() >= 500) break;  //way too much text
      //printMsgLarge(json, VERB_HIGH);
    }
    //clean up and close connection
    client.flush();
    client.stop();

    //printMsgLarge(json, VERB_HIGH);

    //convert to json
    DynamicJsonDocument jsonBuffer(200);
    deserializeJson(jsonBuffer, json);
    JsonObject root = jsonBuffer.as<JsonObject>();
    if (root.isNull()) {
      printMsgln("Error: Couldn't parse JSON", VERB_LOW);
      //TODO add error color
      return &serverResult;
    }
    if (!root.containsKey("access")) {
      printMsgln("Error: No access info");
      //TODO add error color
      return &serverResult;
    } else {
      if (root["access"] == 1) {  //if access allowed
        serverResult.flags |= TOKEN_ACCESS;
      }
    }
    if (root.containsKey("cache")) {
      if (root["cache"] == 1) {  //dont  cache if weekend member
        serverResult.flags |= TOKEN_CACHE;
        serverResult.sync = CACHE_SYNC;  //setup how long staying in cache before checking again
      }
    }
    if (root.containsKey("inductor")) {
      if (root["inductor"] == 1) {             //check if an inductor
        serverResult.flags |= TOKEN_INDUCTOR;  //or assign
      }
    }
    if (root.containsKey("director")) {
      if (root["director"] == 1) {  //TBD director do anything special?
        serverResult.flags |= TOKEN_DIRECTOR;
      }
    }
    if (root.containsKey("colour")) {
      serverResult.colour = root["colour"].as<int>();  //value between 0-63
    }
    if (root.containsKey("error")) {  //print out error messages
      printMsgLarge(root["error"], VERB_MED);
    }

    return &serverResult;


    return NULL;
  }
};