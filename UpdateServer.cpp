/*
** 
** This is the ESP8266HTTPUpdateServer.h file 
** modified by PA4WD ("https://github.com/PA4WD/Arduino/tree/master/libraries/ESP8266HTTPUpdateServer")
**
** and then modified by Willem Aandewiel
**
*/


#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "StreamString.h"
#include "UpdateServer.h"


static const char serverIndex[] PROGMEM =
  R"(<html charset="UTF-8"><style type='text/css'>
     body {background-color: lightblue;}
     </style>
     <body>
     <h1>DSMR-logger Flash utility</h1>
     <form method='POST' action='?cmd=0' enctype='multipart/form-data'>
        Selecteer een "<b>.ino.bin</b>" bestand<br/>
        <input type='hidden' name='cmd' value='0'>
                  <input type='file' name='update'>
                  <input type='submit' value='Flash Firmware'>
      </form>
      <form method='POST' action='?cmd=100' enctype='multipart/form-data'> 
        Selecteer een "<b>.spiffs.bin</b>" bestand<br/>
        <input type='hidden' name='cmd' value='100'>
                  <input type='file' name='update'>
                  <input type='submit' value='Flash Spiffs'>
      </form>
      <br/>Als het lijkt of er niets gebeurd, wacht dan ongeveer drie minuten
           en klik daarna <b><a href="/">hier</a></b>!
      </body></html>)";
static const char successResponse[] PROGMEM = 
  R"(<html>
       <head>
        <meta charset="UTF-8">
        <meta http-equiv='refresh' content='35';URL='/'>
       </head> 
       <style type='text/css'>
             body {background-color: lightblue;}
       </style>
       <body>     
         <h1>DSMR-logger Flash utility</h1>
         <br/>Update geslaagd! DSMR-logger zal nu opnieuw opstarten...<br/>
         <br/>Wacht <span id="waitSeconds">45</span> seconden...<br/>
       </body>
       <script>
         var seconds = document.getElementById("waitSeconds").textContent;
         var countdown = setInterval(function() {
           seconds--;
           document.getElementById('waitSeconds').textContent = seconds;
           if (seconds <= 0) {
              clearInterval(countdown);
              window.location.pathname = "/";
           }
         }, 1000);
       </script>
     </html>)";

ESP8266HTTPUpdateServer::ESP8266HTTPUpdateServer(bool serial_debug)
{
  _serial_output = serial_debug;
  _server = NULL;
  _username = emptyString;
  _password = emptyString;
  _authenticated = false;
}

void ESP8266HTTPUpdateServer::setup(ESP8266WebServer *server, const String& path, const String& username, const String& password)
{
    _server = server;
    _username = username;
    _password = password;

    // handler for the /update form page
    _server->on(path.c_str(), HTTP_GET, [&](){
      if(_username != emptyString && _password != emptyString && !_server->authenticate(_username.c_str(), _password.c_str()))
        return _server->requestAuthentication();
      _server->send_P(200, PSTR("text/html"), serverIndex);
    });

    // handler for the /update form POST (once file upload finishes)
    _server->on(path.c_str(), HTTP_POST, [&](){
      if(!_authenticated)
        return _server->requestAuthentication();
      if (Update.hasError()) {
        _server->send(200, F("text/html"), String(F("Update error: ")) + _updaterError);
      } else {      
    _command = _server->arg("cmd").toInt();   
        _server->client().setNoDelay(true);
        _server->send_P(200, PSTR("text/html"), successResponse);
        delay(100);
        _server->client().stop();
        ESP.restart();
      }
    },[&](){
      // handler for the file upload, get's the sketch bytes, and writes
      // them through the Update object
      HTTPUpload& upload = _server->upload();

      if(upload.status == UPLOAD_FILE_START){
        _updaterError = String();
        if (_serial_output)
          Serial.setDebugOutput(true);

        _authenticated = (_username == emptyString || _password == emptyString || _server->authenticate(_username.c_str(), _password.c_str()));
        if(!_authenticated){
          if (_serial_output)
            Serial.printf("Unauthenticated Update\n");
          return;
        }

        WiFiUDP::stopAll();
        if (_serial_output)
          Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;   
    _command = _server->arg("cmd").toInt();   
        if(!Update.begin(maxSketchSpace, _command)){//start with max available size
          _setUpdaterError();
        }
      } else if(_authenticated && upload.status == UPLOAD_FILE_WRITE && !_updaterError.length()){
        if (_serial_output) Serial.printf(".");
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          _setUpdaterError();
        }
      } else if(_authenticated && upload.status == UPLOAD_FILE_END && !_updaterError.length()){
        if(Update.end(true)){ //true to set the size to the current progress
          if (_serial_output) Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          _setUpdaterError();
        }
        if (_serial_output) Serial.setDebugOutput(false);
      } else if(_authenticated && upload.status == UPLOAD_FILE_ABORTED){
        Update.end();
        if (_serial_output) Serial.println("Update was aborted");
      }
      delay(0);
    });
}

void ESP8266HTTPUpdateServer::_setUpdaterError()
{
  if (_serial_output) Update.printError(Serial);
  StreamString str;
  Update.printError(str);
  _updaterError = str.c_str();
}
