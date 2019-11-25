/* 
***************************************************************************  
**  Program  : webSocketEvent, part of DSMRloggerWS
**  Version  : v1.0.4
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/


static String   prevTimestamp, thisTime;
static bool     graphActual = false;
static int8_t   savMin = 0;
static uint32_t updateClock = millis() + 1000;
static String   wOut[10], wParm[30], wPair[4];


//===========================================================================================
void webSocketEvent(uint8_t wsClient, WStype_t type, uint8_t * payload, size_t lenght) {
//===========================================================================================
    String  wsPayload = String((char *) &payload[0]);
    //v1.0.3b char *  wsPayloadC = (char *) &payload[0];
    String  wsString;

    switch(type) {
        case WStype_DISCONNECTED:
            DebugTf("[%u] Disconnected!\r\n", wsClient);
            isConnected = false;
            break;
            
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(wsClient);
                if (!isConnected) {
                 DebugTf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", wsClient, ip[0], ip[1], ip[2], ip[3], payload);
                 isConnected = true;
                 webSocket.sendTXT(wsClient, "{\"msgType\":\"ConnectState\",\"Value\":\"Connected\"}");
                }
        
            }
            break;
            
        case WStype_TEXT:
            DebugTf("[%u] Got message: [%s]\r\n", wsClient, payload);
  //v1.0.3c String FWversion = String(_FW_VERSION);

            updateClock = millis();
            
            if (wsPayload.indexOf("getDevInfo") > -1) {
              String DT  = buildDateTimeString(pTimestamp);
              wsString  = ", devName=" + String(_HOSTNAME) +
            //            ", devIPaddress=" + WiFi.localIP().toString() 
  //v1.0.3c               ", devVersion=[" + FWversion.substring(0, (FWversion.indexOf(' ')+1)) + "]"
                          ", devVersion=[" + String(_FW_VERSION).substring(0, String(_FW_VERSION).indexOf(' ')) + "]"
                          ", settingBgColor=" + String(settingBgColor) +
                          ", settingFontColor=" + String(settingFontColor) +
                          ", theTime=" + DT.substring(0, 16);

              if (Verbose1) DebugTln(wsString);
              webSocket.sendTXT(wsClient, "msgType=devInfo" + wsString);
            } 
            if (wsPayload.indexOf("graphActual") <= 0) {
              graphActual = false;
            }
            if (wsPayload.indexOf("tabActual") > -1) {
              actTab = TAB_ACTUEEL;
              updateActual(wsClient);
              
            } else if (wsPayload.indexOf("tabLastHours") > -1) {
              actTab = TAB_LAST24HOURS;
              fileWriteData(HOURS, hourData);
              updateLastHours(wsClient, "lastHoursHeaders", 25);
              
            } else if (wsPayload.indexOf("lastHoursRow") > -1) {
              actTab = TAB_LAST24HOURS;
              doLastHoursRow(wsClient, wsPayload);
                            
            } else if (wsPayload.indexOf("tabLastDays") > -1) {
              actTab = TAB_LAST7DAYS;
              fileWriteData(DAYS, dayData);
              updateLastDays(wsClient, "lastDaysHeaders", 0);
 
            } else if (wsPayload.indexOf("lastDaysRow") > -1) {
              actTab = TAB_LAST7DAYS;
              doLastDaysRow(wsClient, wsPayload);
                            
            } else if (wsPayload.indexOf("tabLastMonths") > -1) {
              actTab = TAB_LAST24MONTHS;
              fileWriteData(MONTHS, monthData);
              updateLastMonths(wsClient, "lastMonthsHeaders", 0);
              
            } else if (wsPayload.indexOf("lastMonthsRow") > -1) {
              actTab = TAB_LAST24MONTHS;
              doLastMonthsRow(wsClient, wsPayload);
                            
            } else if (wsPayload.indexOf("tabGraphics") > -1) {
              if (Verbose1) DebugTln("now plot Grafiek()!");
              actTab = TAB_GRAPHICS;
              webSocket.sendTXT(wsClient, "msgType=graphStart");

            } else if (wsPayload.indexOf("graphYearRow") > -1) {
              actTab = TAB_GRAPHICS;
              doGraphMonthRow(wsClient, wsPayload);

            } else if (wsPayload.indexOf("graphWeekRow") > -1) {
              actTab = TAB_GRAPHICS;
              doGraphDayRow(wsClient, wsPayload);
              
            } else if (wsPayload.indexOf("graphDayRow") > -1) {
              actTab = TAB_GRAPHICS;
              doGraphHourRow(wsClient, wsPayload);

            } else if (wsPayload.indexOf("graphFinancialRow") > -1) {
              actTab = TAB_GRAPHICS;
              doGraphFinancialRow(wsClient, wsPayload);

            } else if (wsPayload.indexOf("graphActualNext") > -1) {
              actTab = TAB_GRAPHICS;
              graphActual = true;
              updateGraphActual(wsClient);
              
            } else if (wsPayload.indexOf("tabSysInfo") > -1) {
              if (Verbose1) DebugTf("now updateSysInfo(%d)\r\n", wsClient);
              actTab = TAB_SYSINFO;
              updateSysInfo(wsClient);
              
            } else if (wsPayload.indexOf("sendMonths") > -1) {
              actTab = TAB_EDITOR;
              doSendMonths(wsClient, wsPayload);
              
            } else if (wsPayload.indexOf("editMonthsRow") > -1) {
              actTab = TAB_EDITOR;
              doEditMonthsRow(wsClient, wsPayload);
                            
            } else if (wsPayload.indexOf("updateMonth") > -1) {
              actTab = TAB_EDITOR;
              doUpdateMonth(wsClient, wsPayload);  
              
            } else if (wsPayload.indexOf("sendSettings") > -1) {
              actTab = TAB_EDITOR;
              doSendSettings(wsClient, wsPayload);
              
            } else if (wsPayload.indexOf("saveSettings") > -1) {
              actTab = TAB_EDITOR;
              doSaveSettings(wsClient, wsPayload);
              
            } else if (wsPayload.indexOf("sendColors") > -1) {
              actTab = TAB_EDITOR;
              doSendColors(wsClient, wsPayload);
              
            } else if (wsPayload.indexOf("saveColors") > -1) {
              DebugTln("message: saveColors");
              actTab = TAB_EDITOR;
              doSaveColors(wsClient, wsPayload);
            }
            break;
                        
    } // switch(type)
  
} // webSocketEvent()


//===========================================================================================
void handleRefresh() {
//===========================================================================================
  
  //if (millis() > updateClock && MinuteFromTimestamp(pTimestamp) != savMin) {
    if (millis() > updateClock) {
      updateClock = millis() + 5000;
      savMin      = MinuteFromTimestamp(pTimestamp);
      String DT   = buildDateTimeString(pTimestamp);
      webSocket.broadcastTXT("msgType=clock,DT=" + DT.substring(0, 16));
    }

} // handleRefresh()


//=======================================================================
void updateSysInfo(uint8_t wsClient) {
//=======================================================================
  char wsChars[900]; 
  
  wsChars[0] = '\0';
  strcat(wsChars, "msgType=sysInfo");

//-Slimme Meter Info----------------------------------------------------------
  strcat(wsChars, ",SysID=");                strcat(wsChars, Identification);
#ifdef USE_PRE40_PROTOCOL               //PRE40
  strcat(wsChars, ",SysP1=DSMR 3.0");   //PRE40
#else                                   //else
  strcat(wsChars, ",SysP1=DSMR ");           strcat(wsChars, P1_Version.c_str());
#endif
  strcat(wsChars, ",SysEqID=");              strcat(wsChars, Equipment_Id.c_str());
  strcat(wsChars, ",SysET=");                strcat(wsChars, ElectricityTariff.c_str());
  strcat(wsChars, ",GDT=");                  strcat(wsChars, intToStr(GasDeviceType));
  strcat(wsChars, ",GEID=");                 strcat(wsChars, GasEquipment_Id.c_str());
  
//-Device Info-----------------------------------------------------------------
  strcat(wsChars, ",SysAuth=Willem Aandewiel");
  strcat(wsChars, ",SysFwV=");               strcat(wsChars, _FW_VERSION );
  strcat(wsChars, ",Compiled=");             strcat(wsChars, __DATE__ );
                                              strcat(wsChars, "  " );
                                              strcat(wsChars, __TIME__ );
  strcat(wsChars, ",FreeHeap=");             strcat(wsChars, intToStr(ESP.getFreeHeap()));
                                              strcat(wsChars, " / max.Blck ");
                                              strcat(wsChars, intToStr(ESP.getMaxFreeBlockSize()));

  strcat(wsChars, ",ChipID=");               strcat(wsChars, String(ESP.getChipId(), HEX ).c_str());
  strcat(wsChars, ",CoreVersion=");          strcat(wsChars, String(ESP.getCoreVersion()).c_str() );
  strcat(wsChars, ",SdkVersion=");           strcat(wsChars, String(ESP.getSdkVersion()).c_str() );
  strcat(wsChars, ",CpuFreqMHz=");           strcat(wsChars, intToStr(ESP.getCpuFreqMHz()));
  strcat(wsChars, ",SketchSize=");           strcat(wsChars, floatToStr((ESP.getSketchSize() / 1024.0), 3));
                                              strcat(wsChars, "kB");
  strcat(wsChars, ",FreeSketchSpace=");      strcat(wsChars, floatToStr((ESP.getFreeSketchSpace() / 1024.0), 3 ));
                                              strcat(wsChars, "kB");;

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) 
        sprintf(cMsg, "%08X (PUYA)", ESP.getFlashChipId());
  else  sprintf(cMsg, "%08X"       , ESP.getFlashChipId());
  strcat(wsChars, ",FlashChipID=");          strcat(wsChars, cMsg);   // flashChipId
  strcat(wsChars, ",FlashChipSize=");        strcat(wsChars, floatToStr((ESP.getFlashChipSize() / 1024.0 / 1024.0), 3 ));
                                              strcat(wsChars, "MB");
  strcat(wsChars, ",FlashChipRealSize=");    strcat(wsChars, floatToStr((ESP.getFlashChipRealSize() / 1024.0 / 1024.0), 3 ));
                                              strcat(wsChars, "MB");
  strcat(wsChars, ",FlashChipSpeed=");       strcat(wsChars, floatToStr((ESP.getFlashChipSpeed() / 1000.0 / 1000.0),0));
                                              strcat(wsChars, "MHz");

  FlashMode_t ideMode = ESP.getFlashChipMode();
  strcat(wsChars, ",FlashChipMode=");        strcat(wsChars, flashMode[ideMode]); 
  strcat(wsChars, ",BoardType=");
#ifdef ARDUINO_ESP8266_NODEMCU
   strcat(wsChars, "ESP8266_NODEMCU");
#endif
#ifdef ARDUINO_ESP8266_GENERIC
   strcat(wsChars, "ESP8266_GENERIC");
#endif
#ifdef ESP8266_ESP01
  strcat(wsChars, "ESP8266_ESP01");
#endif
#ifdef ESP8266_ESP12
   strcat(wsChars, "ESP8266_ESP12");
#endif
#ifdef ARDUINO_ESP8266_WEMOS_D1R1
   strcat(wsChars, "Wemos D1 R1");
#endif

   strcat(wsChars, ",SSID=");                strcat(wsChars, WiFi.SSID().c_str() );
#ifdef SHOW_PASSWRDS
   strcat(wsChars, ",PskKey=");              strcat(wsChars, WiFi.psk().c_str() ); 
#else
   strcat(wsChars, ",PskKey=*********");
#endif
   strcat(wsChars, ",IpAddress=");           strcat(wsChars, WiFi.localIP().toString().c_str());
   strcat(wsChars, ",WiFiRSSI=");            strcat(wsChars, intToStr(WiFi.RSSI()));
   strcat(wsChars, ",Hostname=");            strcat(wsChars, _HOSTNAME );
   strcat(wsChars, ",upTime=");              strcat(wsChars, String(upTime()).c_str() );
   strcat(wsChars, ",TelegramCount=");       strcat(wsChars, intToStr(telegramCount)); 
   strcat(wsChars, ",TelegramErrors=");      strcat(wsChars, intToStr(telegramErrors));
   strcat(wsChars, ",lastReset=");           strcat(wsChars, lastReset.c_str());

  //DebugTf("===>> wsChars is [%d] chars, used [%d] chars\r\n", sizeof(wsChars), strlen(wsChars));
  webSocket.sendTXT(wsClient, wsChars);

} // updateSysInfo()


//=======================================================================
void updateLastMonths(uint8_t wsClient, String callBack, int8_t slot) {
//=======================================================================
  char    wsChars[128];
  char    cYear1[10], cYear2[10];
  int8_t  iMonth, nextSlot, nextSlot12, slot12;
  float   ED1, ED2, ER1, ER2, GD1, GD2;
  dataStruct wrkDat, wrkDat12, nxtDat, nxtDat12;

  if (slot == 0) {
    if (Verbose1) DebugTf("webSocket.sendTXT(%d, msgType=%s,MaxRows=12)\r\n", wsClient, callBack.c_str());
    webSocket.sendTXT(wsClient, "msgType=" + callBack + ",MaxRows=12");
    return;
  }
  
  wsChars[0]    = '\0';

//--- slot must have a value 1 .. 12
    slot12      = slot + 12;
    nextSlot    = slot + 1;
    //if (nextSlot > 24) nextSlot = 24; 
    nextSlot12  = nextSlot + 12;
    if (nextSlot12 > 25) nextSlot12 = 25; // not very likely ;-)

    wrkDat    = fileReadData(MONTHS,slot);
    nxtDat    = fileReadData(MONTHS, nextSlot);
    wrkDat12  = fileReadData(MONTHS, slot12);
    nxtDat12  = fileReadData(MONTHS, nextSlot12);
    
    if (Verbose1) DebugTf("slot[%02d], slotLabel[%04d], nextSlot[%02d], slot12[%02d], slot12Label[%04d], nextSlot12[%02d] \r\n"
                                                       , slot, wrkDat.Label, nextSlot, slot12, wrkDat12.Label, nextSlot12);


    iMonth = (wrkDat.Label % 100);
    sprintf(cYear1, "20%02d", (wrkDat.Label / 100));
    sprintf(cYear2, "20%02d", (wrkDat12.Label / 100));
    
    sprintf(cMsg, ",R=%d,M=%s,Y1=%s,Y2=%s", slot, monthName[iMonth], String(cYear1).c_str(), String(cYear2).c_str()); 
    if (Verbose2) DebugTln(cMsg);
    strcat(wsChars, cMsg);
    if (Verbose2) DebugTf("ED[%04d]=[%.3f], nxtED[%04d=[%.3f]\r\n", wrkDat.Label, (wrkDat.EDT1 + wrkDat.EDT2)
                                                                  , nxtDat.Label, (nxtDat.EDT1 + nxtDat.EDT2));
    ED1 = (wrkDat.EDT1 - nxtDat.EDT1) + (wrkDat.EDT2 - nxtDat.EDT2);
    if (ED1 < 0) ED1 = 0;
    strcat(wsChars, ",ED1=");  strcat(wsChars, floatToStr(ED1, 3));

    ED2 = (wrkDat12.EDT1 - nxtDat12.EDT1) + (wrkDat12.EDT2 - nxtDat12.EDT2);
    if (ED2 < 0) ED2 = 0;
    strcat(wsChars, ",ED2=");  strcat(wsChars, floatToStr(ED2, 3));
    
    ER1 = (wrkDat.ERT1  - nxtDat.ERT1) + (wrkDat.ERT2  - nxtDat.ERT2);
    if (ER1 < 0) ER1 = 0;
    strcat(wsChars, ",ER1=");  strcat(wsChars, floatToStr(ER1, 3));
    
    ER2 = (wrkDat12.ERT1  - nxtDat12.ERT1) + (wrkDat12.ERT2  - nxtDat12.ERT2);
    if (ER2 < 0) ER2 = 0;
    strcat(wsChars, ",ER2=");  strcat(wsChars, floatToStr(ER2, 3));

    GD1 = wrkDat.GDT    - nxtDat.GDT;
    if (GD1 < 0) GD1 = 0;
    strcat(wsChars, ",GD1=");  strcat(wsChars, floatToStr(GD1, 3));

    GD2 = wrkDat12.GDT    - nxtDat12.GDT;
    if (GD2 < 0) GD2 = 0;
    strcat(wsChars, ",GD2=");  strcat(wsChars, floatToStr(GD2, 3));
    
    if (Verbose2) DebugTf("webSocket.sendTXT(%d, msgType=%s - %s)\r\n", wsClient, callBack.c_str(), wsChars);
    //DebugTf("===>> wsChars is [%d] chars, used [%d] chars\r\n", sizeof(wsChars), strlen(wsChars));
    webSocket.sendTXT(wsClient, "msgType="+ callBack + wsChars);

    wsChars[0]    = '\0';

} // updateLastMonths()


//=======================================================================
void updateLastDays(uint8_t wsClient, String callBack, int8_t r) {
//=======================================================================
  char    wsChars[128];
  int8_t  YY, MM, DD, thisDD, prevDD;
  float   ED, ER, GD, COSTS;
  dataStruct daySlot, dayPrev;
  
  if (r == 0) {
    fileWriteData(DAYS, dayData);
    webSocket.sendTXT(wsClient, "msgType=" + callBack + ",MaxRows=" + DAYS_RECS);
    return;
  }

  thisDD = r;
  prevDD = r + 1;  
  wsChars[0] = '\0';
  
  if (Verbose1) DebugTf("thisMonth[%d], thisDD[%d] => prevDD[%d] (actLabel[%d])\r\n"
                                      , thisMonth, thisDD,  prevDD, dayData.Label);

  dayPrev = fileReadData(DAYS, prevDD);
  daySlot = fileReadData(DAYS, thisDD);

  label2Fields(daySlot.Label, YY, MM, DD);

  sprintf(cMsg, "%06d010101", daySlot.Label);
  time_t tmpTimestamp = epoch(cMsg);  // tmpTimestamp is not used but we need this call!!!!
  int weekDay = weekday();
  setTime(ntpTimeSav);  // set back time from NTP after epoch()

  if (actTab == TAB_GRAPHICS)
        sprintf(cMsg, ",R=%d,D=%9.9s %02d",r, weekDayName[weekDay], DD); 
  else  sprintf(cMsg, ",R=%d,D=%9.9s 20%02d-%02d-%02d",r, weekDayName[weekDay], YY, MM, DD); 
  strcat(wsChars, cMsg);

  ED = (daySlot.EDT1 - dayPrev.EDT1) + (daySlot.EDT2 - dayPrev.EDT2);
  COSTS  = (daySlot.EDT1 - dayPrev.EDT1) * settingEDT1;
  COSTS += (daySlot.EDT2 - dayPrev.EDT2) * settingEDT2;
  if ((ED < 0) || (dayPrev.EDT1 == 0 && dayPrev.EDT2 == 0)) ED = 0;
  strcat(wsChars, ",ED=");      strcat(wsChars, floatToStr(ED, 3));

  ER = (daySlot.ERT1  - dayPrev.ERT1) + (daySlot.ERT2  - dayPrev.ERT2);
  COSTS -= (daySlot.ERT1 - dayPrev.ERT1) * settingERT1;
  COSTS -= (daySlot.ERT2 - dayPrev.ERT2) * settingERT2;
  if ((ER < 0) || (dayPrev.ERT1 == 0 && dayPrev.ERT2 == 0)) ER = 0;
  strcat(wsChars, ",ER=");      strcat(wsChars, floatToStr(ER, 3));

  GD = daySlot.GDT    - dayPrev.GDT;
  COSTS += (daySlot.GDT - dayPrev.GDT) * settingGDT;
  if ((GD < 0) || (dayPrev.GDT == 0)) GD = 0;
  strcat(wsChars, ",GD=");      strcat(wsChars, floatToStr(GD, 2));

  if (COSTS < 0) COSTS = 0;
  COSTS += settingGNBK / 30;
  COSTS += settingENBK / 30;
  strcat(wsChars, ",COSTS=");   strcat(wsChars, floatToStr(COSTS, 2));

  if (Verbose1) DebugTf("wsChars has room for [%d] cahrs -> uses [%d] chars\r\n", sizeof(wsChars), strlen(wsChars));
  webSocket.sendTXT(wsClient, "msgType=" + callBack + wsChars);

} // updateLastDays()


//=======================================================================
void updateLastHours(uint8_t wsClient, String callBack, int8_t r) {
//=======================================================================
  char    wsChars[128];
  char    cHour[20], cDH[10];
  //v1.0.3b int8_t  n;
  int8_t  thisHH, prevHH, YY, MM, DD, HH;
  float   ER, ED, GD, COSTS;
  dataStruct hourThis, hourPrev;

  if (r == 0) {
    webSocket.sendTXT(wsClient, "msgType=" + callBack + ",MaxRows=" + HOURS_RECS);
    return;
  }

  thisHH = r;
  prevHH = r + 1;
  
  wsChars[0] = '\0';
  if (Verbose1) DebugTf("thisHH[%d] => prevHH[%d] (actLabel[%d])\r\n"
                                     , thisHH,  prevHH, hourData.Label);

  wsChars[0] = '\0';

  hourPrev = fileReadData(HOURS, prevHH);
  hourThis = fileReadData(HOURS, thisHH);

  label2Fields(hourThis.Label, YY, MM, DD, HH);

  sprintf(cHour, "(%02d) %02d:00 - %02d:59", DD, HH, HH);
  sprintf(cDH, "(%02d) %02d", DD, HH);
    
  if (Verbose2) DebugTf("=> r[%02d] (DH=[%s]), thisHour[%s], thisHH[%02d], prevHH[%02d] \r\n"
                                                  , r, cDH, cHour, thisHH, prevHH);

  sprintf(cMsg, ",R=%d,DH=%s,H=%s", r, cDH, cHour); 
  strcat(wsChars, cMsg);
  ED = ((hourThis.EDT1 - hourPrev.EDT1) + (hourThis.EDT2 - hourPrev.EDT2)) * 1000.0;    // kWh *1000 => Wh
  if ((ED < 0) || (hourPrev.EDT1 == 0 && hourPrev.EDT2 == 0)) ED = 0;
  //sprintf(cMsg, ",ED=%.0f", ED); 
  strcat(wsChars, ",ED=");        strcat(wsChars, floatToStr(ED, 0));
  ER = ((hourThis.ERT1  - hourPrev.ERT1) + (hourThis.ERT2  - hourPrev.ERT2)) * 1000.0;  // kWh *1000 => Wh
  if ((ER < 0) || (hourPrev.ERT1 == 0 && hourPrev.ERT2 == 0)) ER = 0;
  //sprintf(cMsg, ",ER=%.0f", ER); 
  strcat(wsChars, ",ER=");        strcat(wsChars, floatToStr(ER, 0));
  GD = hourThis.GDT    - hourPrev.GDT;
  if ((GD < 0) || (hourPrev.GDT == 0)) GD = 0;
  sprintf(cMsg, ",GD=%.2f", GD); 
  strcat(wsChars, ",GD=");        strcat(wsChars, floatToStr(GD, 3));

  COSTS  = (hourThis.EDT1 - hourPrev.EDT1) * settingEDT1;
  COSTS += (hourThis.EDT2 - hourPrev.EDT2) * settingEDT2;
  COSTS += (hourThis.ERT1 - hourPrev.ERT1) * settingERT1 * -1.0;
  COSTS += (hourThis.ERT2 - hourPrev.ERT2) * settingERT2 * -1.0;
  COSTS += (hourThis.GDT  - hourPrev.GDT)  * settingGDT;
  if (COSTS < 0) COSTS = 0.01;
  //sprintf(cMsg, ",COSTS=%.2f", COSTS); 
  strcat(wsChars, ",COSTS=");     strcat(wsChars, floatToStr(COSTS, 2));

  if (Verbose1) DebugTln(wsChars);
  //DebugTf("===>> wsChars is [%d] chars, used [%d] chars\r\n", sizeof(wsChars), strlen(wsChars));
  webSocket.sendTXT(wsClient, "msgType=" + callBack + wsChars);

} // updateLastHours()


//===========================================================================================
void updateActual(uint8_t wsClient) { // version with c-strings
//===========================================================================================
  char    wsChars[500];
  float   PD, PR;
  String  tmpStrng;
  
  String DT   = buildDateTimeString(pTimestamp);

  wsChars[0]  = '\0';
  strcat(wsChars, "msgType=Actual");

  strcat(wsChars,",TS=");      strcat(wsChars, pTimestamp.c_str());
  strcat(wsChars,",ED=");      strcat(wsChars, floatToStr(EnergyDelivered, 3));
  strcat(wsChars,",EDT1=");    strcat(wsChars, floatToStr(EnergyDeliveredTariff1, 3));
  strcat(wsChars,",EDT2=");    strcat(wsChars, floatToStr(EnergyDeliveredTariff2, 3));
  strcat(wsChars,",ER=");      strcat(wsChars, floatToStr(EnergyReturned, 3));
  strcat(wsChars,",ERT1=");    strcat(wsChars, floatToStr(EnergyReturnedTariff1, 3));
  strcat(wsChars,",ERT2=");    strcat(wsChars, floatToStr(EnergyReturnedTariff2, 3));
  strcat(wsChars,",GD=");      strcat(wsChars, floatToStr(GasDelivered, 3));
  strcat(wsChars,",ET=");      strcat(wsChars, ElectricityTariff.c_str() );
  PD = (float)(PowerDelivered_l1 + PowerDelivered_l2 + PowerDelivered_l3) / 1000.0;
  strcat(wsChars,",PD=");      strcat(wsChars, floatToStr(PD, 3));
  strcat(wsChars,",PD_l1=");   strcat(wsChars, intToStr(PowerDelivered_l1 ));
  strcat(wsChars,",PD_l2=");   strcat(wsChars, intToStr(PowerDelivered_l2 ));
  strcat(wsChars,",PD_l3=");   strcat(wsChars, intToStr(PowerDelivered_l3));
  PR = (float)(PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3) / 1000.0;
  strcat(wsChars,",PR=");      strcat(wsChars, floatToStr(PR, 3));
  strcat(wsChars,",PR_l1=");   strcat(wsChars, intToStr(PowerReturned_l1 ));
  strcat(wsChars,",PR_l2=");   strcat(wsChars, intToStr(PowerReturned_l2 ));
  strcat(wsChars,",PR_l3=");   strcat(wsChars, intToStr(PowerReturned_l3 ));
  strcat(wsChars,",V_l1=");    strcat(wsChars, floatToStr(Voltage_l1, 1));
  strcat(wsChars,",V_l2=");    strcat(wsChars, floatToStr(Voltage_l2, 1));
  strcat(wsChars,",V_l3=");    strcat(wsChars, floatToStr(Voltage_l3, 1));
  strcat(wsChars,",C_l1=");    strcat(wsChars, intToStr(Current_l1 ));
  strcat(wsChars,",C_l2=");    strcat(wsChars, intToStr(Current_l2 ));
  strcat(wsChars,",C_l3=");    strcat(wsChars, intToStr(Current_l3 ));
  tmpStrng = String((maxPowerDelivered / 1000.0), 3) + "  @" + maxTimePD;
  strcat(wsChars,",MPD=");     strcat(wsChars, tmpStrng.c_str());
  tmpStrng = String((maxPowerReturned / 1000.0), 3) + "  @" + maxTimePR;
  strcat(wsChars,",MPR=");     strcat(wsChars, tmpStrng.c_str());
  strcat(wsChars,",theTime="); strcat(wsChars, DT.substring(0, 16 ).c_str());
              
  //DebugTf("===>> wsChars is [%d] chars, used [%d] chars\r\n", sizeof(wsChars), strlen(wsChars));
  webSocket.sendTXT(wsClient, wsChars);
  
} // updateActual()


//===========================================================================================
void updateGraphActual(uint8_t wsClient) {
//===========================================================================================
  char  wsChars[128];
  
  wsChars[0] = '\0';
  
  if (graphActual) {
      wsChars[0] = '\0';
      strcat(wsChars, "msgType=graphRow,R=0");
      prevTimestamp = pTimestamp;
      sprintf(cMsg, ",A=1,T=%02d:%02d:%02d", HourFromTimestamp(pTimestamp)
                                           , MinuteFromTimestamp(pTimestamp)
                                           , SecondFromTimestamp(pTimestamp)); 
      strcat(wsChars, cMsg);
      sprintf(cMsg, ",ER=%.1f", (float)((PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3) / 1.0));     // Watt
      strcat(wsChars, cMsg);
      sprintf(cMsg, ",EDL1=%.1f", (float)(PowerDelivered_l1 / 1.0));  // Watt
      strcat(wsChars, cMsg);
      sprintf(cMsg, ",EDL2=%.1f", (float)(PowerDelivered_l2 / 1.0));  // Watt 
      strcat(wsChars, cMsg);
      sprintf(cMsg, ",EDL3=%.1f", (float)(PowerDelivered_l3 / 1.0));  // Watt 
      strcat(wsChars, cMsg);

      if (Verbose2) DebugTf("webSocket.sendTXT(%d, msgType=graphRow,R=0%s)\r\n", wsClient, wsChars);
      //DebugTf("===>> wsChars is [%d] chars, used [%d] chars\r\n", sizeof(wsChars), strlen(wsChars));
      webSocket.sendTXT(wsClient, wsChars);

    }

} // updateGraphActual()


//=======================================================================
void updateGraphFinancial(uint8_t wsClient, String callBack, int8_t slot) {
//=======================================================================
  char    wsChars[128];
  char    cYear1[10], cYear2[10];
  int8_t  iMonth, nextSlot, nextSlot12, slot12;
  float   ED1C, ED2C, ER1C, ER2C, GD1C, GD2C;
  dataStruct wrkDat, wrkDat12, nxtDat, nxtDat12;

  if (slot == 0) {
    if (Verbose1) DebugTf("webSocket.sendTXT(%d, msgType=%s,MaxRows=12)\r\n", wsClient, callBack.c_str());
    webSocket.sendTXT(wsClient, "msgType=" + callBack + ",MaxRows=12");
    return;
  }
  
  wsChars[0] = '\0';

//--- slot must have a value 1 .. 12
    slot12      = slot + 12;
    nextSlot    = slot + 1;
    //if (nextSlot > 24) nextSlot = 24; 
    nextSlot12  = nextSlot + 12;
    if (nextSlot12 > 25) nextSlot12 = 25; // not very likely ;-)

    wrkDat    = fileReadData(MONTHS,slot);
    nxtDat    = fileReadData(MONTHS, nextSlot);
    wrkDat12  = fileReadData(MONTHS, slot12);
    nxtDat12  = fileReadData(MONTHS, nextSlot12);
    
    if (Verbose1) DebugTf("slot[%02d], slotLabel[%04d], nextSlot[%02d], slot12[%02d], slot12Label[%04d], nextSlot12[%02d] \r\n"
                                                       , slot, wrkDat.Label, nextSlot, slot12, wrkDat12.Label, nextSlot12);


    iMonth = (wrkDat.Label % 100);
    sprintf(cYear1, "20%02d", (wrkDat.Label / 100));
    sprintf(cYear2, "20%02d", (wrkDat12.Label / 100));
    
    sprintf(cMsg, ",R=%d,M=%s,Y1=%s,Y2=%s", slot, monthName[iMonth], String(cYear1).c_str(), String(cYear2).c_str()); 
    if (Verbose2) DebugTln(cMsg);
    strcat(wsChars, cMsg);
    if (Verbose2) DebugTf("ED[%04d]=[%.3f], nxtED[%04d=[%.3f]\r\n", wrkDat.Label, (wrkDat.EDT1 + wrkDat.EDT2)
                                                                 , nxtDat.Label, (nxtDat.EDT1 + nxtDat.EDT2));
    ED1C  = (wrkDat.EDT1 - nxtDat.EDT1) * settingEDT1;
    ED1C += (wrkDat.EDT2 - nxtDat.EDT2) * settingEDT2;
    if (ED1C < 0) ED1C = 0;
    //sprintf(cMsg, ",ED1C=%s", String(ED1C, 2).c_str()); 
    strcat(wsChars, ",ED1C=");      strcat(wsChars, floatToStr(ED1C, 2));

    ED2C  = (wrkDat12.EDT1 - nxtDat12.EDT1) * settingEDT1;
    ED2C += (wrkDat12.EDT2 - nxtDat12.EDT2) * settingEDT2;
    if (ED2C < 0) ED2C = 0;
    //sprintf(cMsg, ",ED2C=%s", String(ED2C, 2).c_str()); 
    strcat(wsChars, ",ED2C=");      strcat(wsChars, floatToStr(ED2C, 2));

    ER1C  = (wrkDat.ERT1  - nxtDat.ERT1) * settingERT1;
    ER1C += (wrkDat.ERT2  - nxtDat.ERT2) * settingERT2;
    if (ER1C < 0) ER1C = 0;
    //sprintf(cMsg, ",ER1C=%s",String(ER1C, 2).c_str()); 
    strcat(wsChars, ",ER1C=");      strcat(wsChars, floatToStr(ER1C, 2));

    ER2C  = (wrkDat12.ERT1  - nxtDat12.ERT1) * settingERT1;
    ER2C += (wrkDat12.ERT2  - nxtDat12.ERT2) * settingERT2;
    if (ER2C < 0) ER2C = 0;
    //sprintf(cMsg, ",ER2C=%s", String(ER2C, 2).c_str()); 
    strcat(wsChars, ",ER2C=");      strcat(wsChars, floatToStr(ER2C, 2));

    GD1C = (wrkDat.GDT - nxtDat.GDT) * settingGDT;

    DebugTf("[%04d]: actGDT[%.3f] - nxtGDT[%.3f] => GD1C[%.3f] * [%.5f] = [%.3f]\r\n", wrkDat.Label
                                                                          , wrkDat.GDT, nxtDat.GDT
                                                                          , GD1C 
                                                                          , settingGDT, (GD1C * settingGDT)); 
    if (GD1C < 0) GD1C = 0;
    //sprintf(cMsg, ",GD1C=%s",String(GD1C, 3).c_str()); 
    strcat(wsChars, ",GD1C=");      strcat(wsChars, floatToStr(GD1C, 3));

    GD2C = (wrkDat12.GDT - nxtDat12.GDT) * settingGDT;
    if (GD2C < 0) GD2C = 0;
    //sprintf(cMsg, ",GD2C=%s",String(GD2C, 3).c_str()); 
    strcat(wsChars, ",GD2C");       strcat(wsChars, floatToStr(GD2C, 3));

    if (Verbose2) DebugTf("webSocket.sendTXT(%d, msgType=%s,%s)\r\n", wsClient, callBack.c_str(), wsChars);
    //DebugTf("===>> wsChars is [%d] chars, used [%d] chars\r\n", sizeof(wsChars), strlen(wsChars));
    webSocket.sendTXT(wsClient, "msgType="+ callBack + wsChars);
    wsChars[0] = '\0';

} // updateGraphFinancial()


//=======================================================================
void editMonths(uint8_t wsClient, String callBack, int8_t slot) {
//=======================================================================
  char    wsChars[128];
  char    cYear1[10];
  int8_t  iMonth;
  dataStruct wrkDat;

  if (slot == 0) {
    if (Verbose2) DebugTf("webSocket.sendTXT(%d, msgType=%s)\r\n", wsClient, callBack.c_str());
    webSocket.sendTXT(wsClient, "msgType=" + callBack);
    return;
  }
  
  wsChars[0]  = '\0';

  if (slot < 1 || slot > MONTHS_RECS) {
    DebugTf("Error slot must be >= 1 and <= 25 but is [%02d]\r\n", slot);
    return;
  }

    wrkDat    = fileReadData(MONTHS, slot);
    
    if (Verbose1) DebugTf("sendTableMonths(): slotLabel[%04d], slot[%02d]\r\n", wrkDat.Label, slot);


    iMonth = (wrkDat.Label % 100);
    sprintf(cYear1, "20%02d", (wrkDat.Label / 100));
    
    sprintf(cMsg, ",R=%d,M=%d,Y=%s", slot, iMonth, cYear1); 
    strcat(wsChars, cMsg);
    sprintf(cMsg, ",EDT1=%s", floatToStr(wrkDat.EDT1, 3)); 
    strcat(wsChars, cMsg);
    sprintf(cMsg, ",EDT2=%s", floatToStr(wrkDat.EDT2, 3)); 
    strcat(wsChars, cMsg);
    sprintf(cMsg, ",ERT1=%s", floatToStr(wrkDat.ERT1, 3)); 
    strcat(wsChars, cMsg);
    sprintf(cMsg, ",ERT2=%s", floatToStr(wrkDat.ERT2, 3)); 
    strcat(wsChars, cMsg);
    sprintf(cMsg, ",GAS=%s",  floatToStr(wrkDat.GDT, 3)); 
    strcat(wsChars, cMsg);

    //DebugTf("===>> wsChars is [%d] chars, used [%d] chars\r\n", sizeof(wsChars), strlen(wsChars));
    webSocket.sendTXT(wsClient, "msgType="+ callBack + wsChars);
    wsChars[0] = '\0';

} // editMonths()

              
//=======================================================================
void doLastHoursRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) DebugTf("now update updateLastHours(%d, lastHoursRow, %ld)!\r\n", wsClient, wParm[1].toInt());
  actTab = TAB_LAST24HOURS;
  if (HOURS_RECS > 25) {
    if (wParm[1].toInt() > 0 && wParm[1].toInt() < 25) {
        updateLastHours(wsClient,"lastHoursRow", wParm[1].toInt());
    } 
  } else {
    if (wParm[1].toInt() > 0 && wParm[1].toInt() < HOURS_RECS) {  // no need to show all rows in a table
      updateLastHours(wsClient,"lastHoursRow", wParm[1].toInt());
    }
  }
  
} // doLastHoursRow()


//=======================================================================
void doLastDaysRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) DebugTf("now update updateLastDays(%d, LastDaysRow, %ld)!\r\n", wsClient, wParm[1].toInt());
  actTab = TAB_LAST7DAYS;
  if (wParm[1].toInt() > 0 && wParm[1].toInt() < DAYS_RECS) {
    updateLastDays(wsClient, "lastDaysRow", wParm[1].toInt());
  }

} // doLastDaysRow()


//=======================================================================
void doLastMonthsRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) DebugTf("now update updateLastMonths(%d, %ld)!\r\n", wsClient, wParm[1].toInt());

  if (wParm[1].toInt() > 0 && wParm[1].toInt() <= 12) {
    updateLastMonths(wsClient, "lastMonthsRow", wParm[1].toInt());
  }

} // doLastMonthsRow()


//=======================================================================
void doGraphMonthRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (wParm[1].toInt() == 1) {
    fileWriteData(MONTHS, monthData);
  }
  if (Verbose1) DebugTf("now update graphRow(%d, %ld)!\r\n", wsClient, wParm[1].toInt());
  if (wParm[1].toInt() > 0 && wParm[1].toInt() <= 12) {
    updateLastMonths(wsClient, "graphRow", wParm[1].toInt());
  }

} // doGraphMonthRow()


//=======================================================================
void doGraphDayRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) DebugTf("now update graphRow(%d, %ld)!\r\n", wsClient, wParm[1].toInt());
  if (wParm[1].toInt() == 1) {
     fileWriteData(DAYS, dayData);
  }
  if (wParm[1].toInt() > 0 && wParm[1].toInt() < DAYS_RECS) {
    updateLastDays(wsClient, "graphRow", wParm[1].toInt());
  }

} // doGraphDayRow()


//=======================================================================
void doGraphHourRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) DebugTf("now update graphRow(%d, %ld)!\r\n", wsClient, wParm[1].toInt());
  if (wParm[1].toInt() == 1) {
    fileWriteData(HOURS, hourData);
  }
  if (wParm[1].toInt() > 0 && wParm[1].toInt() < (HOURS_RECS - 1)) {  // we cannot calculate values for last row!!
     updateLastHours(wsClient, "graphRow", wParm[1].toInt());
  }

} // doGraphHourRow()


//=======================================================================
void doGraphFinancialRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (wParm[1].toInt() == 1) {
    fileWriteData(MONTHS, monthData);
  }
  if (Verbose1) DebugTf("now update graphRow(%d, %ld)!\r\n", wsClient, wParm[1].toInt());
  if (wParm[1].toInt() > 0 && wParm[1].toInt() <= 12) {
    updateGraphFinancial(wsClient, "graphRow", wParm[1].toInt());
  }

} // doGraphFinancialRow()


//=======================================================================
void doSendMonths(uint8_t wsClient, String wsPayload) {
//=======================================================================
  if (Verbose1) DebugTf("now editMonths(%d, 'editMonthsHeaders', 0)!\r\n", wsClient);
  editMonths(wsClient, "editMonthsHeaders", 0);

} // doSendMonths()

              
//=======================================================================
void doEditMonthsRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) DebugTf("now editMonthsRow(%d, %ld)!\r\n", wsClient, wParm[1].toInt());
  if (wParm[1].toInt() == 1) {
     fileWriteData(MONTHS, monthData, 1);
  }
  if (wParm[1].toInt() > 0 && wParm[1].toInt() <= MONTHS_RECS) {  // we need to be able to also edit record 25!
    if (Verbose1) DebugTf("next: editMonths(%d, %s, %ld)!\r\n", wsClient, "editMonthsRow", wParm[1].toInt());
    editMonths(wsClient, "editMonthsRow", wParm[1].toInt());
  }

} // doEditMonthsRow()


//=======================================================================
void doUpdateMonth(uint8_t wsClient, String wsPayload) {
//=======================================================================
  int16_t YY=2012, MM=1;
  dataStruct updDat;

  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  for (int x=0; x<wc; x++) {
    if (Verbose1) DebugTf("wOut[%d] => [%s]\r\n", x, wOut[x].c_str());
  }
  wc = splitString(wOut[1].c_str(), ',', wParm, 10);
  int8_t wp = splitString(wParm[0].c_str(), '=', wPair, 3);
  int16_t recNo = wPair[1].toInt();
  if (Verbose1) DebugTf("now updateMonth(%d, %d)!\r\n", wsClient, recNo);
  updDat = fileReadData(MONTHS, recNo);

  for (int x=1; x<wc; x++) {
    int8_t wp = splitString(wParm[x].c_str(), '=', wPair, 3);
    if (Verbose1) DebugTf("wPair[0] (%s)-> [%s] \r\n", wPair[0].c_str(), wPair[1].c_str());
    if (wPair[0] == "Y") {
      YY = wPair[1].toInt();
      if (Verbose1) DebugTf("Y set to [%02d]\r\n", YY);
    } else if (wPair[0] == "M") {
      MM = wPair[1].toInt();
      if (Verbose1) DebugTf("M set to [%02d]\r\n", MM);
      updDat.Label = ((YY - 2000)*100) + MM;
      if (Verbose1) DebugTf("Label is now [%04d]\r\n", updDat.Label);
    } else if (wPair[0] == "EDT1") {
      updDat.EDT1 = wPair[1].toFloat();
      if (Verbose1) DebugTf("EDT1 set to [%.3f]\r\n", updDat.EDT1);
    } else if (wPair[0] == "EDT2") {
      updDat.EDT2 = wPair[1].toFloat();
      if (Verbose1) DebugTf("EDT2 set to [%.3f]\r\n", updDat.EDT2);
    } else if (wPair[0] == "ERT1") {
      updDat.ERT1 = wPair[1].toFloat();
      if (Verbose1) DebugTf("ERT1 set to [%.3f]\r\n", updDat.ERT1);
    } else if (wPair[0] == "ERT2") {
      updDat.ERT2 = wPair[1].toFloat();
      if (Verbose1) DebugTf("ERT2 set to [%.3f]\r\n", updDat.ERT2);
    } else if (wPair[0] == "GAS") {
      updDat.GDT  = wPair[1].toFloat();
      if (Verbose1) DebugTf("GDT set to [%.3f]\r\n", updDat.GDT);
    }
  } // for ...
     
  fileWriteData(MONTHS, updDat, recNo);

} // doUpdateMonth()


//=======================================================================
void doSendSettings(uint8_t wsClient, String wsPayload) {
//=======================================================================
  char  wsChars[500];
  
  if (Verbose1) DebugTf("now sendSettings(%d)!\r\n", wsClient);

  readSettings(false);

  wsChars[0] = '\0';
  strcat(wsChars, "msgType=settings");
  
  strcat(wsChars, ",DT1=");             strcat(wsChars, floatToStr(settingEDT1, 5));
  strcat(wsChars, ",DT2=");             strcat(wsChars, floatToStr(settingEDT2, 5));
  strcat(wsChars, ",RT1=");             strcat(wsChars, floatToStr(settingERT1, 5));
  strcat(wsChars, ",RT2=");             strcat(wsChars, floatToStr(settingERT2, 5));
  strcat(wsChars,  ",GAST=");           strcat(wsChars, floatToStr(settingGDT,  5));
  strcat(wsChars, ",ENBK=");            strcat(wsChars, floatToStr(settingENBK, 2));
  strcat(wsChars, ",GNBK=");            strcat(wsChars, floatToStr(settingGNBK, 2));
  strcat(wsChars, ",BgColor=");         strcat(wsChars, settingBgColor);
  strcat(wsChars, ",FontColor=");       strcat(wsChars, settingFontColor);
  strcat(wsChars, ",Interval=");        strcat(wsChars, intToStr(settingInterval));
  strcat(wsChars, ",SleepTime=");       strcat(wsChars, intToStr(settingSleepTime));
#ifdef USE_MQTT
  strcat(wsChars, ",MQTTbroker=");      strcat(wsChars, MQTTbrokerURL);
                                        strcat(wsChars, ":");
                                        strcat(wsChars, intToStr(MQTTbrokerPort));
  strcat(wsChars, ",MQTTuser=");        strcat(wsChars, settingMQTTuser);
  strcat(wsChars, ",MQTTpasswd=");      strcat(wsChars, settingMQTTpasswd);
  strcat(wsChars, ",MQTTinterval=");    strcat(wsChars, intToStr(settingMQTTinterval));
  strcat(wsChars, ",MQTTtopTopic=");    strcat(wsChars, settingMQTTtopTopic);
#endif
  strcat(wsChars, ",MindergasAuthtoken="); strcat(wsChars, settingMindergasAuthtoken);

  //DebugTf("===>> wsChars is [%d] chars, used [%d] chars\r\n", sizeof(wsChars), strlen(wsChars));
  webSocket.sendTXT(wsClient, wsChars);

} // doSendSettings()


//=======================================================================
void doSaveSettings(uint8_t wsClient, String wsPayload) {
//=======================================================================
  String            nColor, oldMQTTbroker = settingMQTTbroker;
 
  if (Verbose1) DebugTf("now saveSettings(%d) with [%s]!\r\n", wsClient, wsPayload.c_str());
  uint8_t wc = splitString(wsPayload.c_str(), ',', wParm, 29);
  if (Verbose2) DebugTf("-> found [%d] pairs!\r\n", wc);
  for(int p=1; p<wc; p++) {
    yield();
    int wp = splitString(wParm[p].c_str(), '=', wPair, 3);
    nColor = wPair[1].substring(0, (MAXCOLORNAME - 1));
    wPair[1].trim();
    if (Verbose2) DebugTf("wParm[%d] => [%s]=[%s]\r\n", p, wPair[0].c_str(), wPair[1].c_str());
    if (wPair[0] == "DT1") {
      settingEDT1 = wPair[1].toFloat();
    } else if (wPair[0] == "DT2") {
      settingEDT2 = wPair[1].toFloat();
    } else if (wPair[0] == "RT1") {
      settingERT1 = wPair[1].toFloat();
    } else if (wPair[0] == "RT2") {
      settingERT2 = wPair[1].toFloat();
    } else if (wPair[0] == "GAST") {
      settingGDT = wPair[1].toFloat();
    } else if (wPair[0] == "ENBK") {
      settingENBK = wPair[1].toFloat();
    } else if (wPair[0] == "GNBK") {
      settingGNBK = wPair[1].toFloat();
    } else if (wPair[0] == "BgColor") {
      strcpy(settingBgColor, nColor.c_str());
    } else if (wPair[0] == "FontColor") {
      strcpy(settingFontColor, nColor.c_str());
    } else if (wPair[0] == "Interval") {
      settingInterval  = wPair[1].toInt();
      if (settingInterval <  2) settingInterval =  2;
      if (settingInterval > 60) settingInterval = 60;
    } else if (wPair[0] == "SleepTime") {
      settingSleepTime = wPair[1].toInt();
    } else if (wPair[0] == "MQTTbroker") {
      strcpy(settingMQTTbroker, wPair[1].c_str());
      Debugf(" => settingMQTTbroker [%s]\n", settingMQTTbroker);
    } else if (wPair[0] == "MQTTuser") {
      strcpy(settingMQTTuser, wPair[1].c_str());
    } else if (wPair[0] == "MQTTpasswd") {
      strcpy(settingMQTTpasswd, wPair[1].c_str());
    } else if (wPair[0] == "MQTTinterval") {
      settingMQTTinterval  = wPair[1].toInt();
      if (settingMQTTinterval < 10)  settingMQTTinterval = 10;
      if (settingMQTTinterval > 600) settingMQTTinterval = 600;
    } else if (wPair[0] == "MQTTtopTopic") {
      strcpy(settingMQTTtopTopic, wPair[1].c_str());
      if (String(settingMQTTtopTopic).length() < 1) {
        strcpy(settingMQTTtopTopic, "DSMR-WS");
      }
    } else if (wPair[0] == "MindergasAuthtoken") {
        strcpy(settingMindergasAuthtoken, wPair[1].c_str());
    }
  }
  yield();
  writeSettings();
#ifdef USE_MQTT
  if (oldMQTTbroker != settingMQTTbroker) {
    MQTTclient.disconnect();
    MQTTisConnected = false;
    startMQTT();
    if (MQTTreconnect()) {
      DebugTf("Connected to [%s]:[%d]\r\n", MQTTbrokerURL, MQTTbrokerPort);
    }
  }
#endif

  
} // doSaveSettings()


//=======================================================================
void doSendColors(uint8_t wsClient, String wsPayload) {
//=======================================================================
  char wsChars[500];
  
  if (Verbose1) DebugTf("now sendColors(%d)!\r\n", wsClient);

  wsChars[0] = '\0';
  strcat(wsChars, "msgType=colors");
  
  readColors(false);
  
  strcat(wsChars, ",LEDC=");    strcat(wsChars, iniBordEDC);
  strcat(wsChars, ",BEDC=");    strcat(wsChars, iniFillEDC);
  strcat(wsChars, ",LERC=");    strcat(wsChars, iniBordERC);
  strcat(wsChars, ",BERC=");    strcat(wsChars, iniFillERC);
  strcat(wsChars, ",LGDC=");    strcat(wsChars, iniBordGDC);
  strcat(wsChars, ",BGDC=");    strcat(wsChars, iniFillGDC);
  strcat(wsChars, ",LED2C=");   strcat(wsChars, iniBordED2C);
  strcat(wsChars, ",BED2C=");   strcat(wsChars, iniFillED2C);
  strcat(wsChars, ",LER2C=");   strcat(wsChars, iniBordER2C);
  strcat(wsChars, ",BER2C=");   strcat(wsChars, iniFillER2C);
  strcat(wsChars, ",LGD2C=");   strcat(wsChars, iniBordGD2C);
  strcat(wsChars, ",BGD2C=");   strcat(wsChars, iniFillGD2C);
  strcat(wsChars, ",LPR123C="); strcat(wsChars, iniBordPR123C);
  strcat(wsChars, ",BPR123C="); strcat(wsChars, iniFillPR123C);
  strcat(wsChars, ",LPD1C=");   strcat(wsChars, iniBordPD1C);
  strcat(wsChars, ",BPD1C=");   strcat(wsChars, iniFillPD1C);
  strcat(wsChars, ",LPD2C=");   strcat(wsChars, iniBordPD2C);
  strcat(wsChars, ",BPD2C=");   strcat(wsChars, iniFillPD2C);
  strcat(wsChars, ",LPD3C=");   strcat(wsChars, iniBordPD3C);
  strcat(wsChars, ",BPD3C=");   strcat(wsChars, iniFillPD3C);

  //DebugTf("===>> wsChars is [%d] chars, used [%d] chars\r\n", sizeof(wsChars), strlen(wsChars));
  webSocket.sendTXT(wsClient, wsChars);

} // doSendColors()


//=======================================================================
void doSaveColors(uint8_t wsClient, String wsPayload) {
//=======================================================================
  String            nColor;

  if (Verbose1) DebugTf("now saveColors(%d) with [%s]!\r\n", wsClient, wsPayload.c_str());
  uint8_t wc = splitString(wsPayload.c_str(), ',', wParm, 24);
  if (Verbose2) DebugTf("-> found [%d] pairs!\r\n", wc);
  for(int p=1; p<wc; p++) {
    delay(10);
    int wp = splitString(wParm[p].c_str(), '=', wPair, 3);
    nColor = wPair[1].substring(0, (MAXCOLORNAME - 1));
    wPair[1].trim();
    if (Verbose2) DebugTf("wParm[%d] => [%s]=[%s]\r\n", p, wPair[0].c_str(), wPair[1].c_str());
    if (wPair[0] == "LEDC") {
      strcpy(iniBordEDC , nColor.c_str());
    } else if (wPair[0] == "BEDC") {
      strcpy(iniFillEDC, nColor.c_str());
    } else if (wPair[0] == "LERC") {
      strcpy(iniBordERC, nColor.c_str());
    } else if (wPair[0] == "BERC") {
      strcpy(iniFillERC, nColor.c_str());
    } else if (wPair[0] == "LGDC") {
      strcpy(iniBordGDC, nColor.c_str());
    } else if (wPair[0] == "BGDC") {
      strcpy(iniFillGDC, nColor.c_str());
    } else if (wPair[0] == "LED2C") {
      strcpy(iniBordED2C, nColor.c_str());
    } else if (wPair[0] == "BED2C") {
      strcpy(iniFillED2C, nColor.c_str());
    } else if (wPair[0] == "LER2C") {
      strcpy(iniBordER2C, nColor.c_str());
    } else if (wPair[0] == "BER2C") {
      strcpy(iniFillER2C, nColor.c_str());
    } else if (wPair[0] == "LGD2C") {
      strcpy(iniBordGD2C, nColor.c_str());
    } else if (wPair[0] == "BGD2C") {
      strcpy(iniFillGD2C, nColor.c_str());
    } else if (wPair[0] == "LPR123C") {
      strcpy(iniBordPR123C, nColor.c_str());
    } else if (wPair[0] == "BPR123C") {
      strcpy(iniFillPR123C, nColor.c_str());
    } else if (wPair[0] == "LPD1C") {
      strcpy(iniBordPD1C, nColor.c_str());
    } else if (wPair[0] == "BPD1C") {
      strcpy(iniFillPD1C, nColor.c_str());
    } else if (wPair[0] == "LPD2C") {
      strcpy(iniBordPD2C, nColor.c_str());
    } else if (wPair[0] == "BPD2C") {
      strcpy(iniFillPD2C, nColor.c_str());
    } else if (wPair[0] == "LPD3C") {
      strcpy(iniBordPD3C, nColor.c_str());
    } else if (wPair[0] == "BPD3C") {
      strcpy(iniFillPD3C, nColor.c_str());
    }
  } // for(int p=1 ...
  
  yield();
  writeColors();
  
} // doSaveColors()


//=======================================================================
char *intToStr(int32_t v)
{
  static char buff[25];
  sprintf(buff,"%d", v);
  return buff;
} // intToStr()

//=======================================================================
char *floatToStr(float v, int dec)
{
  static char buff[25];
  if (dec == 0)       sprintf(buff,"%.0f", v);
  else if (dec == 1)  sprintf(buff,"%.1f", v);
  else if (dec == 2)  sprintf(buff,"%.2f", v);
  else if (dec == 3)  sprintf(buff,"%.3f", v);
  else if (dec == 4)  sprintf(buff,"%.4f", v);
  else if (dec == 5)  sprintf(buff,"%.5f", v);
  else                sprintf(buff,"%f",   v);
  return buff;
} // floattToStr()

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
