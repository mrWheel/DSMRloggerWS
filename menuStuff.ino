/*
***************************************************************************  
**  Program  : menuStuff, part of DSMRloggerWS
**  Version  : v0.4.3
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//===========================================================================================
void displayDaysHist(bool Telnet=true) {
//===========================================================================================
  char EDT1[20], EDT2[20], ERT1[20], ERT2[20], GDT[20];
  uint16_t YY, MM, DD;
  int32_t  Label;
  dataStruct tmpRec;

  _dThis = false;
  if (Telnet) Debugln("\n======== WeekDay History ==========\r\n");

  fileWriteData(DAYS, dayData);

  for (int i=1; i<=DAYS_RECS; i++) {
    tmpRec = fileReadData(DAYS, i);
    Label = tmpRec.Label;
    YY    = Label / 10000;
    MM    = (Label - (YY * 10000)) / 100;
    DD    = Label % 100;
    dtostrf(tmpRec.EDT1, 12, 3, EDT1);
    dtostrf(tmpRec.EDT2, 12, 3, EDT2);
    dtostrf(tmpRec.ERT1, 12, 3, ERT1);
    dtostrf(tmpRec.ERT2, 12, 3, ERT2);
    dtostrf(tmpRec.GDT,  10, 2, GDT);
    sprintf(cMsg, "[%02d][20%02d-%02d-%02d] EDT1[%s], EDT2[%s], ERT1[%s], ERT2[%s], GDT[%s]\r", i
                                                                            , YY, MM, DD
                                                                            , EDT1, EDT2, ERT1, ERT2, GDT);
    _dThis = false;
    if (Telnet) Debugln(cMsg);
  }
  
} // displayDaysHist()


//===========================================================================================
void displayHoursHist(bool Telnet=true) {
//===========================================================================================
  char EDT1[20], EDT2[20], ERT1[20], ERT2[20], GDT[20], cHour;
  int thisHourKey = HoursKeyTimestamp(pTimestamp);
  uint32_t Label;
  dataStruct tmpRec;

  _dThis = false;
  if (Telnet) Debugln("\n======== Hours History ==========\r\n");
  fileWriteData(HOURS, hourData);  
  for (int i=1; i < HOURS_RECS; i++) {
    tmpRec = fileReadData(HOURS, i);
    Label = tmpRec.Label;
    dtostrf(tmpRec.EDT1, 12, 3, EDT1);
    dtostrf(tmpRec.ERT1, 12, 3, ERT1);
    dtostrf(tmpRec.EDT2, 12, 3, EDT2);
    dtostrf(tmpRec.ERT2, 12, 3, ERT2);
    dtostrf(tmpRec.GDT, 10, 2, GDT);
    sprintf(cMsg, "[%02d][%08d] EDT1[%s], EDT2[%s], ERT1[%s], ERT2[%s], GDT[%s]\r", i, Label
                                                                          , EDT1, EDT2, ERT1, ERT2, GDT);
    _dThis = false;
    if (Telnet) Debugln(cMsg);

  }

} // displayHoursHist()


//===========================================================================================
void displayMonthsHist(bool Telnet=true) {
//===========================================================================================
  char EDT1[20], EDT2[20], ERT1[20], ERT2[20], GDT[20];
  dataStruct tmpRec;

  _dThis = false;
  if (Telnet) Debugln("\n======== Months History ==========\r\n");
  fileWriteData(MONTHS, monthData);
  
  for (int i=1; i <= MONTHS_RECS; i++) {
    tmpRec = fileReadData(MONTHS, i);
    dtostrf(tmpRec.EDT1, 12, 3, EDT1);
    dtostrf(tmpRec.ERT1, 12, 3, ERT1);
    dtostrf(tmpRec.EDT2, 12, 3, EDT2);
    dtostrf(tmpRec.ERT2, 12, 3, ERT2);
    dtostrf(tmpRec.GDT,  10, 2, GDT);
    sprintf(cMsg, "[%02d][%04d] EDT1[%s], EDT2[%s], ERT1[%s], ERT2[%s], GDT[%s]\r", i
                                                                          , tmpRec.Label
                                                                          , EDT1, EDT2, ERT1, ERT2, GDT);
    _dThis = false;
    if (Telnet) Debugln(cMsg);

  }

} // displayMonthsHist()


//===========================================================================================
void displayBoardInfo() {
//===========================================================================================
  Debugln(F("\r\n==================================================================\r"));
  Debug(F(" \r\n               (c)2019 by [Willem Aandewiel"));
  Debug(F("]\r\n         Firmware Version ["));  Debug( _FW_VERSION );
  Debug(F("]\r\n                 Compiled ["));  Debug( __DATE__ ); 
                                                           Debug( "  " );
                                                           Debug( __TIME__ );
  Debug(F("]\r\n      Telegrams Processed ["));  Debug( telegramCount );
  Debug(F("]\r\n              With Errors ["));  Debug( telegramErrors );
  Debug(F("]\r\n                 FreeHeap ["));  Debug( ESP.getFreeHeap() );
  Debug(F("]\r\n                  Chip ID ["));  Debug( ESP.getChipId(), HEX );
  Debug(F("]\r\n             Core Version ["));  Debug( ESP.getCoreVersion() );
  Debug(F("]\r\n              SDK Version ["));  Debug( ESP.getSdkVersion() );
  Debug(F("]\r\n           CPU Freq (MHz) ["));  Debug( ESP.getCpuFreqMHz() );
  Debug(F("]\r\n         Sketch Size (kB) ["));  Debug( ESP.getSketchSize() / 1024.0 );
  Debug(F("]\r\n   Free Sketch Space (kB) ["));  Debug( ESP.getFreeSketchSpace() / 1024.0 );

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) 
        sprintf(cMsg, "%08X (PUYA)", ESP.getFlashChipId());
  else  sprintf(cMsg, "%08X", ESP.getFlashChipId());
  Debug("]\r\n            Flash Chip ID [");  Debug( cMsg );
  Debug("]\r\n     Flash Chip Size (kB) [");  Debug( ESP.getFlashChipSize() / 1024 );
  Debug("]\r\nFlash Chip Real Size (kB) [");  Debug( ESP.getFlashChipRealSize() / 1024 );
  Debug("]\r\n         Flash Chip Speed [");  Debug( ESP.getFlashChipSpeed() / 1000 / 1000 );
  FlashMode_t ideMode = ESP.getFlashChipMode();
  Debug("]\r\n          Flash Chip Mode [");  Debug( flashMode[ideMode] );

  Debugln("]\r");

  Debugln("==================================================================");
  Debug(" \r\n               Board type [");
#ifdef ARDUINO_ESP8266_NODEMCU
    Debug("ESP8266_NODEMCU");
#endif
#ifdef ARDUINO_ESP8266_GENERIC
    Debug("ESP8266_GENERIC");
#endif
#ifdef ESP8266_ESP01
    Debug("ESP8266_ESP01");
#endif
#ifdef ESP8266_ESP12
    Debug("ESP8266_ESP12");
#endif
  Debug("]\r\n                     SSID [");  Debug( WiFi.SSID() );
#ifdef SHOW_PSK_KEY
  Debug("]\r\n                  PSK key [");  Debug( WiFi.psk() );
#else
  Debug("]\r\n                  PSK key [**********");
#endif
  Debug("]\r\n               IP Address [");  Debug( WiFi.localIP() );
  Debug("]\r\n                 Hostname [");  Debug( _HOSTNAME );
  Debug("]\r\n       Last reset reason: [");  Debug( ESP.getResetReason() );
  Debug("]\r\n                   upTime [");  Debug( upTime() );
  Debugln("]\r");
  Debugln("==================================================================\r\n");

} // displayBoardInfo()


//===========================================================================================
void waitForOTAupload() {
//===========================================================================================
  uint32_t maxWait = millis() + 33000;

  Debugln("Wait 30 seconds for OTAupload to start....\r");

  OTAinProgress = true;
  while(maxWait > millis()) { // hm.. does not seem to work ..
    //---ArduinoOTA.handle();
    delay(10);
  }
  Debugln("now Rebooting.\r");
  ESP.reset();
    
} // waitForOTAupload()


//===========================================================================================
void handleKeyInput() {
//===========================================================================================
  char    inChar;

  while (TelnetStream.available() > 0) {
    yield();
    inChar = (char)TelnetStream.read();
    
    switch(inChar) {
      case 'b':
      case 'B':     displayBoardInfo();
                    readSettings();
                    break;
      case 'd':
      case 'D':     displayDaysHist(true);
                    break;
      case 'h':
      case 'H':     displayHoursHist(true);
                    break;
      case 'm':
      case 'M':     displayMonthsHist(true);
                    break;
                    
      case 'F':     Debugf("\r\nConnect to AP [%s] and go to ip address shown in the AP-name\r\n", _HOSTNAME);
                    delay(1000);
                    WiFi.disconnect(true);  // deletes credentials !
                    //setupWiFi(true);
                    delay(2000);
                    ESP.reset();
                    delay(2000);
                    break;
      case 'i':
      case 'I':     for(int I=0; I<10; I++) {
                      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
                      delay(1000);
                    }
                    break;
#ifdef HAS_NO_METER
      case 'Z':     createDummyData();
                    break;
      case 'n':     forceDay++;
                    break;
      case 'N':     forceMonth++;
                    break;
#else
      case 'p':
      case 'P':     showRaw = !showRaw;
                    break;
#endif
      case 'R':     Debug("Reboot in 3 seconds ... \r");
                    delay(3000);
                    Debugln("now Rebooting.                      \r");
                    ESP.reset();
                    break;
      case 's':
      case 'S':     listSPIFFS();
                    break;
      case 'v':
      case 'V':     if (Verbose2) {
                      Debugln("Verbose is OFF\r");
                      Verbose1 = false;
                      Verbose2 = false;
                    } else if (Verbose1) {
                      Debugln("Verbose Level 2 is ON\r");
                      Verbose2 = true;
                    } else {
                      Debugln("Verbose Level 1 is ON\r");
                      Verbose1 = true;
                      Verbose2 = false;
                    }
                    break;
      default:      _dThis = false;
                    Debugln("\nCommands are:\r\n");
                    Debugln("   B - Board Type\r");
                    Debugln("   D - Display Day table from SPIFFS\r");
                    Debugln("   H - Display Hour table from SPIFFS\r");
                    Debugln("   M - Display Month table from SPIFFS\r");
                    Debugln("  *F - Force Re-Config WiFi\r");
                    Debugf ("   I - Identify by blinking LED on GPIO[%02d]\r\n", LED_BUILTIN);
#ifdef HAS_NO_METER
                    Debugln("  *Z - create Dummy Data\r");
                    Debugln("  *n - force next Day\r");
                    Debugln("  *N - force next Month\r");
#endif
                    if (showRaw) {
                      Debugln("   P - Start Parsing again\r");
                    } else {
                      Debugln("   P - No Parsing (show RAW data from Smart Meter)\r");
                      showRawCount = 0;
                    }
                    Debugln("  *R - Reboot\r");
                    Debugln("   S - SPIFFS space available\r");
                    Debugln("  *U - Update SPIFFS (save Data-files)\r");
                    if (Verbose1 & Verbose2)  Debugln("   V - Toggle Verbose Off\r");
                    else if (Verbose1)        Debugln("   V - Toggle Verbose 2\r");
                    else                      Debugln("   V - Toggle Verbose 1\r");

    } // switch()
    while (TelnetStream.available() > 0) {
       yield();
       (char)TelnetStream.read();
    }
  }
  
} // handleKeyInput()


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
