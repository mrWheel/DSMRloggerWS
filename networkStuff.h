/*
***************************************************************************  
**  Program  : networkStuff.h, part of DSMRloggerWS
**  Version  : v0.4.1
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

#include <ESP8266WiFi.h>        // version 1.0.0 - part of ESP8266 Core https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>   // Version 1.0.0 - part of ESP8266 Core https://github.com/esp8266/Arduino
//#include <ESP8266HTTPUpdateServer.h>
//#include <DNSServer.h>        // part of ESP8266 Core https://github.com/esp8266/Arduino
#include <WiFiUdp.h>            // part of ESP8266 Core https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>        // part of ESP8266 Core https://github.com/esp8266/Arduino
#include <WiFiManager.h>        // version 0.14.0 - https://github.com/tzapu/WiFiManager
#include <TelnetStream.h>       // Version 0.0.1 - https://github.com/jandrassy/TelnetStream
#include <WebSocketsServer.h>   // Version 20.05.2015 - https://github.com/Links2004/arduinoWebSockets
//#include <Hash.h>
#include <FS.h>                 // part of ESP8266 Core https://github.com/esp8266/Arduino

ESP8266WebServer        HttpServer ( 80 );
// serverIndex[] and successResponse[] changed in source ESP8266HTTPUpdateServer.cpp!!!!!!!!!!!!
//ESP8266HTTPUpdateServer HttpUpdater(true);

WebSocketsServer webSocket = WebSocketsServer(81);

bool        OtaInProgress = false;
static      FSInfo SPIFFSinfo;
bool        SPIFFSmounted; 
bool        isConnected = false;

//gets called when WiFiManager enters configuration mode
//===========================================================================================
void configModeCallback (WiFiManager *myWiFiManager) {
//===========================================================================================
  _dThis = true;
  Debugln("Entered config mode");
  _dThis = true;
  Debugln(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  _dThis = true;
  Debugln(myWiFiManager->getConfigPortalSSID());

} // configModeCallback()


//===========================================================================================
void startWiFi() {
//===========================================================================================
  WiFiManager manageWiFi;

  String thisAP = String(_HOSTNAME) + "-" + WiFi.macAddress();
  
  manageWiFi.setDebugOutput(true);
  
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  manageWiFi.setAPCallback(configModeCallback);

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep in seconds
  manageWiFi.setTimeout(180);  // 3 minuten
  
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!manageWiFi.autoConnect(thisAP.c_str())) {
    _dThis = true;
    Debugln("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  _dThis = true;
  Debugf("Connected with IP-address [%s]\n\n", WiFi.localIP().toString().c_str());
  
} // startWiFi()


//===========================================================================================
void startTelnet() {
//===========================================================================================
        
  TelnetStream.begin();
  _dThis = true;
  Debugln("\nTelnet server started ..");
  TelnetStream.flush();

} // startTelnet()


//===========================================================================================
#ifdef USE_ARDUINO_OTA
void startArduinoOTA() {
//===========================================================================================

  //ArduinoOTA.setHostname();
  //ArduinoOTA.setPassword("");
  
  ArduinoOTA.onStart([]() {
    String type;
    
    OtaInProgress = true;

    Serial.end();
    webSocket.disconnect();
    webSocket = 0;    
    
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
      if (SPIFFSmounted) {
        SPIFFS.end();
        SPIFFSmounted = false;
      }
    }
    _dThis = true;
    Debugln("Start updating " + type);
    DebugFlush();
  });
  ArduinoOTA.onEnd([]() {
    Debugln("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    _dThis = true;
    Debugf("Progress: %u%%\r", (progress / (total / 100)));
    DebugFlush();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Debugf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Debugln("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Debugln("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Debugln("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Debugln("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Debugln("End Failed");
    }
    DebugFlush();
  });
  ArduinoOTA.begin();

} // startArduinoOTA()
#endif


//=======================================================================
void startMDNS(const char *Hostname) {
//=======================================================================
  _dThis = true;
  Debugf("[1] mDNS setup as [%s.local]\n\n", Hostname);
  _dThis = true;
  if (MDNS.begin(Hostname)) {              // Start the mDNS responder for Hostname.local
    Debugf("[2] mDNS responder started as [%s.local]\n\n", Hostname);
  } else {
    Debugln("[3] mDNS Error setting up MDNS responder!\n");
  }
  MDNS.addService(Hostname, "tcp", 80);
  
} // startMDNS()

/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
