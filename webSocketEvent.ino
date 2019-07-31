/*
***************************************************************************  
**  Program  : webSocketEvent, part of DSMRloggerWS
**  Version  : v0.4.5
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
String wOut[10], wParm[10], wPair[4];


//===========================================================================================
void webSocketEvent(uint8_t wsClient, WStype_t type, uint8_t * payload, size_t lenght) {
//===========================================================================================
    String  wsPayload = String((char *) &payload[0]);
    char *  wsPayloadC = (char *) &payload[0];
    String  wsString;

    switch(type) {
        case WStype_DISCONNECTED:
            _dThis = true;
            Debugf("[%u] Disconnected!\n", wsClient);
            isConnected = false;
            break;
            
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(wsClient);
                if (!isConnected) {
                 _dThis = true;
                 Debugf("[%u] Connected from %d.%d.%d.%d url: %s\n", wsClient, ip[0], ip[1], ip[2], ip[3], payload);
                 isConnected = true;
                 webSocket.sendTXT(wsClient, "{\"msgType\":\"ConnectState\",\"Value\":\"Connected\"}");
                }
        
            }
            break;
            
        case WStype_TEXT:
            _dThis = true;
            Debugf("[%u] Got message: [%s]\n", wsClient, payload);
            String FWversion = String(_FW_VERSION);

            updateClock = millis();
            
            if (wsPayload.indexOf("getDevInfo") > -1) {
              String DT  = buildDateTimeString(pTimestamp);
              wsString  = "";
              wsString += ", devName=" + String(_HOSTNAME);
            //wsString += ", devIPaddress=" + WiFi.localIP().toString() ;
              wsString += ", devVersion=[" + FWversion.substring(0, (FWversion.indexOf('(') -1)) + "]";
              wsString += ", settingBgColor=" + String(settingBgColor);
              wsString += ", settingFontColor=" + String(settingFontColor);
              wsString += ", theTime=" + DT.substring(0, 16);

              _dThis = true;
              if (Verbose1) Debugln(wsString);
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
              _dThis = true;
              if (Verbose1) Debugln("now plot Grafiek()!");
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
              _dThis = true;
              if (Verbose1) Debugf("now updateSysInfo(%d)\n", wsClient);
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
String wsString;

//-Slimme Meter Info----------------------------------------------------------
  wsString  = ",SysID=" + String(Identification);
#ifdef USE_PRE40_PROTOCOL                                             //PRE40
  wsString += ",SysP1=DSMR 3.0";                                      //PRE40
#else                                                                 //else
  wsString += ",SysP1=DSMR " + String(P1_Version);
#endif
  wsString += ",SysEqID=" + String(Equipment_Id);
  wsString += ",SysET=" + String(ElectricityTariff);
  wsString += ",GDT=" + String(GasDeviceType);
  wsString += ",GEID=" + String(GasEquipment_Id);
  
//-Device Info-----------------------------------------------------------------
  wsString += ",SysAuth=Willem Aandewiel";
  wsString += ",SysFwV="            + String( _FW_VERSION );
  wsString += ",Compiled="          + String( __DATE__ ) 
                                      + String( "  " )
                                      + String( __TIME__ );
  wsString += ",FreeHeap="          + String( ESP.getFreeHeap() );
  wsString += ",ChipID="            + String( ESP.getChipId(), HEX );
  wsString += ",CoreVersion="       + String( ESP.getCoreVersion() );
  wsString += ",SdkVersion="        + String( ESP.getSdkVersion() );
  wsString += ",CpuFreqMHz="        + String( ESP.getCpuFreqMHz() );
  wsString += ",SketchSize="        + String( (ESP.getSketchSize() / 1024.0), 3) + "kB";
  wsString += ",FreeSketchSpace="   + String( (ESP.getFreeSketchSpace() / 1024.0), 3 ) + "kB";

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) 
        sprintf(cMsg, "%08X (PUYA)", ESP.getFlashChipId());
  else  sprintf(cMsg, "%08X", ESP.getFlashChipId());
  wsString += ",FlashChipID="       + String(cMsg);  // flashChipId
  wsString += ",FlashChipSize="     + String( (float)(ESP.getFlashChipSize() / 1024.0 / 1024.0), 3 ) + "MB";
  wsString += ",FlashChipRealSize=" + String( (float)(ESP.getFlashChipRealSize() / 1024.0 / 1024.0), 3 ) + "MB";
  wsString += ",FlashChipSpeed="    + String( (float)(ESP.getFlashChipSpeed() / 1000.0 / 1000.0) ) + "MHz";

  FlashMode_t ideMode = ESP.getFlashChipMode();
  wsString += ",FlashChipMode="     + String( flashMode[ideMode] );
  wsString += ",BoardType=";
#ifdef ARDUINO_ESP8266_NODEMCU
    wsString += String("ESP8266_NODEMCU");
#endif
#ifdef ARDUINO_ESP8266_GENERIC
    wsString += String("ESP8266_GENERIC");
#endif
#ifdef ESP8266_ESP01
    wsString += String("ESP8266_ESP01");
#endif
#ifdef ESP8266_ESP12
    wsString += String("ESP8266_ESP12");
#endif
#ifdef ARDUINO_ESP8266_WEMOS_D1R1
    wsString += String("Wemos D1 R1");
#endif

  wsString += ",SSID="              + String( WiFi.SSID() );
#ifdef SHOW_PSK_KEY
  wsString += ",PskKey="            + String( WiFi.psk() );    
#else
  wsString += ",PskKey=*********";    
#endif
  wsString += ",IpAddress="         + WiFi.localIP().toString() ;
  wsString += ",WiFiRSSI="          + String( WiFi.RSSI() );
  wsString += ",Hostname="          + String( _HOSTNAME );
  wsString += ",upTime="            + String( upTime() );
  wsString += ",TelegramCount="     + String( telegramCount );
  wsString += ",TelegramErrors="    + String( telegramErrors );
  wsString += ",lastReset=" + lastReset;

  webSocket.sendTXT(wsClient, "msgType=sysInfo" + wsString);

} // updateSysInfo()


//=======================================================================
void updateLastMonths(uint8_t wsClient, String callBack, int8_t slot) {
//=======================================================================
  String wsString;
  char    cYear1[10], cYear2[10];
//int8_t  iMonth, nextSlot, slot, nextSlot12, slot12;
  int8_t  iMonth, nextSlot, nextSlot12, slot12;
  float   ED1, ED2, ER1, ER2, GD1, GD2;
  dataStruct wrkDat, wrkDat12, nxtDat, nxtDat12;

  if (slot == 0) {
    _dThis = true;
    if (Verbose1) Debugf("webSocket.sendTXT(%d, msgType=%s,MaxRows=12)\n\r", wsClient, callBack.c_str());
    webSocket.sendTXT(wsClient, "msgType=" + callBack + ",MaxRows=12");
    return;
  }
  
  wsString    = "";

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
    
    if (Verbose1) Debugf("slot[%02d], slotLabel[%04d], nextSlot[%02d], slot12[%02d], slot12Label[%04d], nextSlot12[%02d] \r\n"
                                                       , slot, wrkDat.Label, nextSlot, slot12, wrkDat12.Label, nextSlot12);


    iMonth = (wrkDat.Label % 100);
    sprintf(cYear1, "20%02d", (wrkDat.Label / 100));
    sprintf(cYear2, "20%02d", (wrkDat12.Label / 100));
    
    sprintf(cMsg, ",R=%d,M=%s,Y1=%s,Y2=%s", slot, monthName[iMonth], String(cYear1).c_str(), String(cYear2).c_str()); 
    _dThis = true;
    if (Verbose2) Debugln(cMsg);
    wsString +=  String(cMsg);
    if (Verbose2) Debugf("ED[%04d]=[%.3f], nxtED[%04d=[%.3f]\n\r", wrkDat.Label, (wrkDat.EDT1 + wrkDat.EDT2)
                                                                 , nxtDat.Label, (nxtDat.EDT1 + nxtDat.EDT2));
    ED1 = (wrkDat.EDT1 - nxtDat.EDT1) + (wrkDat.EDT2 - nxtDat.EDT2);
    if (ED1 < 0) ED1 = 0;
    sprintf(cMsg, ",ED1=%s", String(ED1, 3).c_str()); 
    wsString +=  String(cMsg);
    ED2 = (wrkDat12.EDT1 - nxtDat12.EDT1) + (wrkDat12.EDT2 - nxtDat12.EDT2);
    if (ED2 < 0) ED2 = 0;
    sprintf(cMsg, ",ED2=%s", String(ED2, 3).c_str()); 
    wsString +=  String(cMsg);
    ER1 = (wrkDat.ERT1  - nxtDat.ERT1) + (wrkDat.ERT2  - nxtDat.ERT2);
    if (ER1 < 0) ER1 = 0;
    sprintf(cMsg, ",ER1=%s",String(ER1, 3).c_str()); 
    wsString +=  String(cMsg);
    ER2 = (wrkDat12.ERT1  - nxtDat12.ERT1) + (wrkDat12.ERT2  - nxtDat12.ERT2);
    if (ER2 < 0) ER2 = 0;
    sprintf(cMsg, ",ER2=%s", String(ER2, 3).c_str()); 
    wsString +=  String(cMsg);
    GD1 = wrkDat.GDT    - nxtDat.GDT;
    if (GD1 < 0) GD1 = 0;
    sprintf(cMsg, ",GD1=%s",String(GD1, 2).c_str()); 
    wsString +=  String(cMsg);
    GD2 = wrkDat12.GDT    - nxtDat12.GDT;
    if (GD2 < 0) GD2 = 0;
    sprintf(cMsg, ",GD2=%s",String(GD2, 2).c_str()); 
    wsString +=  String(cMsg);
    _dThis = true;
    if (Verbose2) Debugf("webSocket.sendTXT(%d, msgType=%s - %s)\n\r", wsClient, callBack.c_str(), wsString.c_str());
    webSocket.sendTXT(wsClient, "msgType="+ callBack + wsString);
    wsString = "";


} // updateLastMonths()


//=======================================================================
void updateLastDays(uint8_t wsClient, String callBack, int8_t r) {
//=======================================================================
  String wsString;
  int8_t  YY, MM, DD, thisDD, prevDD;
  float ED, ER, GD, COSTS;
  //time_t  tmpTimestamp;
  dataStruct daySlot, dayPrev;
  
  if (r == 0) {
    fileWriteData(DAYS, dayData);
    webSocket.sendTXT(wsClient, "msgType=" + callBack + ",MaxRows=" + DAYS_RECS);
    return;
  }


  thisDD = r;
  prevDD = r + 1;
  
  wsString    = "";
  _dThis = true;
  if (Verbose1) Debugf("thisMonth[%d], thisDD[%d] => prevDD[%d] (actLabel[%d])\r\n"
                                     , thisMonth, thisDD,  prevDD, dayData.Label);

  dayPrev = fileReadData(DAYS, prevDD);
  daySlot = fileReadData(DAYS, thisDD);

  label2Fields(daySlot.Label, YY, MM, DD);

  sprintf(cMsg, "%06d010101", daySlot.Label);
  time_t tmpTimestamp = epoch(cMsg);
  int weekDay = weekday();
  setTime(ntpTimeSav);  // set back time from NTP after epoch()

  if (actTab == TAB_GRAPHICS)
        sprintf(cMsg, ",R=%d,D=%9.9s %02d",r, weekDayName[weekDay], DD); 
  else  sprintf(cMsg, ",R=%d,D=%9.9s 20%02d-%02d-%02d",r, weekDayName[weekDay], YY, MM, DD); 
  wsString +=  String(cMsg);

  ED = (daySlot.EDT1 - dayPrev.EDT1) + (daySlot.EDT2 - dayPrev.EDT2);
  COSTS  = (daySlot.EDT1 - dayPrev.EDT1) * settingEDT1;
  COSTS += (daySlot.EDT2 - dayPrev.EDT2) * settingEDT2;
  if (ED < 0) ED = 0;
  sprintf(cMsg, ",ED=%.3f", ED); 
  wsString +=  String(cMsg);

  ER = (daySlot.ERT1  - dayPrev.ERT1) + (daySlot.ERT2  - dayPrev.ERT2);
  COSTS -= (daySlot.ERT1 - dayPrev.ERT1) * settingERT1;
  COSTS -= (daySlot.ERT2 - dayPrev.ERT2) * settingERT2;
  if (ER < 0) ER = 0;
  sprintf(cMsg, ",ER=%.3f", ER); 
  wsString +=  String(cMsg);

  GD = daySlot.GDT    - dayPrev.GDT;
  COSTS += (daySlot.GDT - dayPrev.GDT) * settingGDT;
  if (GD < 0) GD = 0;
  sprintf(cMsg, ",GD=%.2f", GD); 
  wsString +=  String(cMsg);
  if (COSTS < 0) COSTS = 0;
  COSTS += settingGNBK / 30;
  COSTS += settingENBK / 30;
  sprintf(cMsg, ",COSTS=%.2f", COSTS); 
  wsString +=  String(cMsg);

  webSocket.sendTXT(wsClient, "msgType=" + callBack + wsString);

} // updateLastDays()


//=======================================================================
void updateLastHours(uint8_t wsClient, String callBack, int8_t r) {
//=======================================================================
  String  wsString;
  char    cHour[20], cDH[10];
  int8_t  n, thisHH, prevHH, YY, MM, DD, HH;
  float   ER, ED, GD, COSTS;
  dataStruct hourThis, hourPrev;

  if (r == 0) {
    webSocket.sendTXT(wsClient, "msgType=" + callBack + ",MaxRows=" + HOURS_RECS);
    return;
  }

  thisHH = r;
  prevHH = r + 1;
  
  wsString    = "";
  _dThis = true;
  if (Verbose1) Debugf("thisHH[%d] => prevHH[%d] (actLabel[%d])\r\n"
                                     , thisHH,  prevHH, hourData.Label);

  wsString    = "";

  hourPrev = fileReadData(HOURS, prevHH);
  hourThis = fileReadData(HOURS, thisHH);

  label2Fields(hourThis.Label, YY, MM, DD, HH);

  sprintf(cHour, "(%02d) %02d:00 - %02d:59", DD, HH, HH);
  sprintf(cDH, "(%02d) %02d", DD, HH);
    
  _dThis = true;
  if (Verbose2) Debugf("=> r[%02d] (DH=[%s]), thisHour[%s], thisHH[%02d], prevHH[%02d] \r\n"
                                                  , r, cDH, cHour, thisHH, prevHH);

  sprintf(cMsg, ",R=%d,DH=%s,H=%s", r, cDH, cHour); 
  wsString +=  String(cMsg);
  ED = ((hourThis.EDT1 - hourPrev.EDT1) + (hourThis.EDT2 - hourPrev.EDT2)) * 1000.0;    // kWh *1000 => Wh
  if (ED < 0) ED = 0;
  sprintf(cMsg, ",ED=%.0f", ED); 
  wsString +=  String(cMsg);
  ER = ((hourThis.ERT1  - hourPrev.ERT1) + (hourThis.ERT2  - hourPrev.ERT2)) * 1000.0;  // kWh *1000 => Wh
  if (ER < 0) ER = 0;
  sprintf(cMsg, ",ER=%.0f", ER); 
  wsString +=  String(cMsg);
  GD = hourThis.GDT    - hourPrev.GDT;
  if (GD < 0) GD = 0;
  sprintf(cMsg, ",GD=%.2f", GD); 
  wsString +=  String(cMsg);

  COSTS  = (hourThis.EDT1 - hourPrev.EDT1) * settingEDT1;
  COSTS += (hourThis.EDT2 - hourPrev.EDT2) * settingEDT2;
  COSTS += (hourThis.ERT1 - hourPrev.ERT1) * settingERT1 * -1.0;
  COSTS += (hourThis.ERT2 - hourPrev.ERT2) * settingERT2 * -1.0;
  COSTS += (hourThis.GDT  - hourPrev.GDT)  * settingGDT;
  if (COSTS < 0) COSTS = 0.01;
  sprintf(cMsg, ",COSTS=%.2f", COSTS); 
  wsString +=  String(cMsg);

  _dThis = true;
  if (Verbose1) Debugln(wsString);
  webSocket.sendTXT(wsClient, "msgType=" + callBack + wsString);

} // updateLastHours()


//===========================================================================================
void updateActual(uint8_t wsClient) {
//===========================================================================================
  String wsString;
  float PD, PR;

  String DT   = buildDateTimeString(pTimestamp);

  wsString  = ",TS=" + pTimestamp;
  wsString += ",ED=" + String(EnergyDelivered, 3);
  wsString += ",EDT1=" + String(EnergyDeliveredTariff1, 3);
  wsString += ",EDT2=" + String(EnergyDeliveredTariff2, 3);
  wsString += ",ER=" + String(EnergyReturned, 3);
  wsString += ",ERT1=" + String(EnergyReturnedTariff1, 3);
  wsString += ",ERT2=" + String(EnergyReturnedTariff2, 3);
  wsString += ",GD=" + String(GasDelivered, 2);
  wsString += ",ET=" + String(ElectricityTariff);
  PD = (float)(PowerDelivered_l1 + PowerDelivered_l2 + PowerDelivered_l3) / 1000.0;
  //PD = PowerDelivered;
  wsString += ",PD=" + String(PD, 3);
  wsString += ",PD_l1=" + String(PowerDelivered_l1);
  wsString += ",PD_l2=" + String(PowerDelivered_l2);
  wsString += ",PD_l3=" + String(PowerDelivered_l3);
  PR = (float)(PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3) / 1000.0;
  //PR = PowerReturned;
  wsString += ",PR=" + String(PR, 3);
  wsString += ",PR_l1=" + String(PowerReturned_l1);
  wsString += ",PR_l2=" + String(PowerReturned_l2);
  wsString += ",PR_l3=" + String(PowerReturned_l3);
  wsString += ",V_l1=" + String(Voltage_l1, 1);
  wsString += ",V_l2=" + String(Voltage_l2, 1);
  wsString += ",V_l3=" + String(Voltage_l3, 1);
  wsString += ",C_l1=" + String(Current_l1);
  wsString += ",C_l2=" + String(Current_l2);
  wsString += ",C_l3=" + String(Current_l3);
  wsString += ",MPD=" + String((maxPowerDelivered / 1000.0), 3) + "  @" + maxTimePD;
  wsString += ",MPR=" + String((maxPowerReturned / 1000.0), 3) + "  @" + maxTimePR;
  wsString += ",theTime=" + DT.substring(0, 16);

  webSocket.sendTXT(wsClient, "msgType=Actual" + wsString);
  
} // updateActual()

//===========================================================================================
void updateGraphActual(uint8_t wsClient) {
//===========================================================================================
  String wsString;
  
  if (graphActual) {
      wsString = "";
      prevTimestamp = pTimestamp;
      sprintf(cMsg, ",A=1,T=%02d:%02d:%02d", HourFromTimestamp(pTimestamp)
                                           , MinuteFromTimestamp(pTimestamp)
                                           , SecondFromTimestamp(pTimestamp)); 
      wsString +=  String(cMsg);
      sprintf(cMsg, ",ER=%.1f", (float)((PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3) / 1.0));     // Watt
      wsString +=  String(cMsg);
      sprintf(cMsg, ",EDL1=%.1f", (float)(PowerDelivered_l1 / 1.0));  // Watt
      wsString +=  String(cMsg);
      sprintf(cMsg, ",EDL2=%.1f", (float)(PowerDelivered_l2 / 1.0));  // Watt 
      wsString +=  String(cMsg);
      sprintf(cMsg, ",EDL3=%.1f", (float)(PowerDelivered_l3 / 1.0));  // Watt 
      wsString +=  String(cMsg);

      _dThis = true;
      if (Verbose2) Debugf("webSocket.sendTXT(%d, msgType=graphRow,R=0%s)\n\r", wsClient, wsString.c_str());
      webSocket.sendTXT(wsClient, "msgType=graphRow,R=0" + wsString);

    }

} // updateGraphActual()


//=======================================================================
void updateGraphFinancial(uint8_t wsClient, String callBack, int8_t slot) {
//=======================================================================
  String  wsString;
  char    cYear1[10], cYear2[10];
//int8_t  iMonth, nextSlot, slot, nextSlot12, slot12;
  int8_t  iMonth, nextSlot, nextSlot12, slot12;
  float   ED1C, ED2C, ER1C, ER2C, GD1C, GD2C;
  dataStruct wrkDat, wrkDat12, nxtDat, nxtDat12;

  if (slot == 0) {
    _dThis = true;
    if (Verbose1) Debugf("webSocket.sendTXT(%d, msgType=%s,MaxRows=12)\n\r", wsClient, callBack.c_str());
    webSocket.sendTXT(wsClient, "msgType=" + callBack + ",MaxRows=12");
    return;
  }
  
  wsString    = "";

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
    
    if (Verbose1) Debugf("slot[%02d], slotLabel[%04d], nextSlot[%02d], slot12[%02d], slot12Label[%04d], nextSlot12[%02d] \r\n"
                                                       , slot, wrkDat.Label, nextSlot, slot12, wrkDat12.Label, nextSlot12);


    iMonth = (wrkDat.Label % 100);
    sprintf(cYear1, "20%02d", (wrkDat.Label / 100));
    sprintf(cYear2, "20%02d", (wrkDat12.Label / 100));
    
    sprintf(cMsg, ",R=%d,M=%s,Y1=%s,Y2=%s", slot, monthName[iMonth], String(cYear1).c_str(), String(cYear2).c_str()); 
    _dThis = true;
    if (Verbose2) Debugln(cMsg);
    wsString +=  String(cMsg);
    if (Verbose2) Debugf("ED[%04d]=[%.3f], nxtED[%04d=[%.3f]\n\r", wrkDat.Label, (wrkDat.EDT1 + wrkDat.EDT2)
                                                                 , nxtDat.Label, (nxtDat.EDT1 + nxtDat.EDT2));
    ED1C  = (wrkDat.EDT1 - nxtDat.EDT1) * settingEDT1;
    ED1C += (wrkDat.EDT2 - nxtDat.EDT2) * settingEDT2;
    if (ED1C < 0) ED1C = 0;
    sprintf(cMsg, ",ED1C=%s", String(ED1C, 2).c_str()); 
    wsString +=  String(cMsg);
    ED2C  = (wrkDat12.EDT1 - nxtDat12.EDT1) * settingEDT1;
    ED2C += (wrkDat12.EDT2 - nxtDat12.EDT2) * settingEDT2;
    if (ED2C < 0) ED2C = 0;
    sprintf(cMsg, ",ED2C=%s", String(ED2C, 2).c_str()); 
    wsString +=  String(cMsg);
    ER1C  = (wrkDat.ERT1  - nxtDat.ERT1) * settingERT1;
    ER1C += (wrkDat.ERT2  - nxtDat.ERT2) * settingERT2;
    if (ER1C < 0) ER1C = 0;
    sprintf(cMsg, ",ER1C=%s",String(ER1C, 2).c_str()); 
    wsString +=  String(cMsg);
    ER2C  = (wrkDat12.ERT1  - nxtDat12.ERT1) * settingERT1;
    ER2C += (wrkDat12.ERT2  - nxtDat12.ERT2) * settingERT2;
    if (ER2C < 0) ER2C = 0;
    sprintf(cMsg, ",ER2C=%s", String(ER2C, 2).c_str()); 
    wsString +=  String(cMsg);
    GD1C = (wrkDat.GDT - nxtDat.GDT) * settingGDT;
    _dThis = true;
    Debugf("[%04d]: actGDT[%.2f] - nxtGDT[%.2f] => GD1C[%.2f] * [%.5f] = [%.2f]\n", wrkDat.Label
                                                                          , wrkDat.GDT, nxtDat.GDT
                                                                          , GD1C 
                                                                          , settingGDT, (GD1C * settingGDT)); 
    if (GD1C < 0) GD1C = 0;
    sprintf(cMsg, ",GD1C=%s",String(GD1C, 2).c_str()); 
    wsString +=  String(cMsg);
    GD2C = (wrkDat12.GDT - nxtDat12.GDT) * settingGDT;
    if (GD2C < 0) GD2C = 0;
    sprintf(cMsg, ",GD2C=%s",String(GD2C, 2).c_str()); 
    wsString +=  String(cMsg);
    _dThis = true;
    if (Verbose2) Debugf("webSocket.sendTXT(%d, msgType=%s,%s)\n\r", wsClient, callBack.c_str(), wsString.c_str());
    webSocket.sendTXT(wsClient, "msgType="+ callBack + wsString);
    wsString = "";


} // updateGraphFinancial()


//=======================================================================
void editMonths(uint8_t wsClient, String callBack, int8_t slot) {
//=======================================================================
  String  wsString;
  char    cYear1[10];
  int8_t  iMonth;
  dataStruct wrkDat;

  if (slot == 0) {
    _dThis = true;
    if (Verbose2) Debugf("webSocket.sendTXT(%d, msgType=%s)\n\r", wsClient, callBack.c_str());
    webSocket.sendTXT(wsClient, "msgType=" + callBack);
    return;
  }
  
  wsString    = "";

  if (slot < 1 || slot > MONTHS_RECS) {
    _dThis = true;
    Debugf("Error slot must be >= 1 and <= 25 but is [%02d]\n", slot);
    return;
  }

    wrkDat    = fileReadData(MONTHS, slot);
    
    if (Verbose1) Debugf("sendTableMonths(): slotLabel[%04d], slot[%02d]\r\n", wrkDat.Label, slot);


    iMonth = (wrkDat.Label % 100);
    sprintf(cYear1, "20%02d", (wrkDat.Label / 100));
    
    sprintf(cMsg, ",R=%d,M=%d,Y=%s", slot, iMonth, String(cYear1).c_str()); 
    wsString +=  String(cMsg);
    sprintf(cMsg, ",EDT1=%s", String(wrkDat.EDT1, 3).c_str()); 
    wsString +=  String(cMsg);
    sprintf(cMsg, ",EDT2=%s", String(wrkDat.EDT2, 3).c_str()); 
    wsString +=  String(cMsg);
    sprintf(cMsg, ",ERT1=%s",String(wrkDat.ERT1, 3).c_str()); 
    wsString +=  String(cMsg);
    sprintf(cMsg, ",ERT2=%s",String(wrkDat.ERT2, 3).c_str()); 
    wsString +=  String(cMsg);
    sprintf(cMsg, ",GAS=%s",String(wrkDat.GDT, 2).c_str()); 
    wsString +=  String(cMsg);

    webSocket.sendTXT(wsClient, "msgType="+ callBack + wsString);
    wsString = "";


} // editMonths()

              
//=======================================================================
void doLastHoursRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  _dThis = true;
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) Debugf("now update updateLastHours(%d, lastHoursRow, %ld)!\n", wsClient, wParm[1].toInt());
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
  _dThis = true;
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) Debugf("now update updateLastDays(%d, LastDaysRow, %ld)!\n", wsClient, wParm[1].toInt());
  actTab = TAB_LAST7DAYS;
  if (wParm[1].toInt() > 0 && wParm[1].toInt() < DAYS_RECS) {
    updateLastDays(wsClient, "lastDaysRow", wParm[1].toInt());
  }

} // doLastDaysRow()


//=======================================================================
void doLastMonthsRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  _dThis = true;
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) Debugf("now update updateLastMonths(%d, %ld)!\n", wsClient, wParm[1].toInt());

  if (wParm[1].toInt() > 0 && wParm[1].toInt() <= 12) {
    updateLastMonths(wsClient, "lastMonthsRow", wParm[1].toInt());
  }

} // doLastMonthsRow()


//=======================================================================
void doGraphMonthRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  _dThis = true;
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (wParm[1].toInt() == 1) {
    fileWriteData(MONTHS, monthData);
  }
  if (Verbose1) Debugf("now update graphRow(%d, %ld)!\n", wsClient, wParm[1].toInt());
  if (wParm[1].toInt() > 0 && wParm[1].toInt() <= 12) {
    updateLastMonths(wsClient, "graphRow", wParm[1].toInt());
  }

} // doGraphMonthRow()


//=======================================================================
void doGraphDayRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  _dThis = true;
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) Debugf("now update graphRow(%d, %ld)!\n", wsClient, wParm[1].toInt());
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
  _dThis = true;
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) Debugf("now update graphRow(%d, %ld)!\n", wsClient, wParm[1].toInt());
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
  _dThis = true;
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (wParm[1].toInt() == 1) {
    fileWriteData(MONTHS, monthData);
  }
  if (Verbose1) Debugf("now update graphRow(%d, %ld)!\n", wsClient, wParm[1].toInt());
  if (wParm[1].toInt() > 0 && wParm[1].toInt() <= 12) {
    updateGraphFinancial(wsClient, "graphRow", wParm[1].toInt());
  }

} // doGraphFinancialRow()


//=======================================================================
void doSendMonths(uint8_t wsClient, String wsPayload) {
//=======================================================================
  _dThis = true;
  if (Verbose1) Debugf("now editMonths(%d, 'editMonthsHeaders', 0)!\n", wsClient);
  editMonths(wsClient, "editMonthsHeaders", 0);

} // doSendMonths()

              
//=======================================================================
void doEditMonthsRow(uint8_t wsClient, String wsPayload) {
//=======================================================================
  _dThis = true;
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  wc = splitString(wOut[1].c_str(), '=', wParm, 10);
  if (Verbose1) Debugf("now editMonthsRow(%d, %ld)!\n", wsClient, wParm[1].toInt());
  if (wParm[1].toInt() == 1) {
     fileWriteData(MONTHS, monthData, 1);
  }
  if (wParm[1].toInt() > 0 && wParm[1].toInt() <= MONTHS_RECS) {  // we need to be able to also edit record 25!
    _dThis = true;
    if (Verbose1) Debugf("next: editMonths(%d, %s, %ld)!\n", wsClient, "editMonthsRow", wParm[1].toInt());
    editMonths(wsClient, "editMonthsRow", wParm[1].toInt());
  }

} // doEditMonthsRow()


//=======================================================================
void doUpdateMonth(uint8_t wsClient, String wsPayload) {
//=======================================================================
int16_t YY=2012, MM=1;
dataStruct updDat;

  _dThis = true;
  int8_t wc = splitString(wsPayload.c_str(), '?', wOut, 10);
  for (int x=0; x<wc; x++) {
    _dThis = true;
    if (Verbose1) Debugf("wOut[%d] => [%s]\n", x, wOut[x].c_str());
  }
  wc = splitString(wOut[1].c_str(), ',', wParm, 10);
  int8_t wp = splitString(wParm[0].c_str(), '=', wPair, 3);
  int16_t recNo = wPair[1].toInt();
  _dThis = true;
  if (Verbose1) Debugf("now updateMonth(%d, %d)!\n", wsClient, recNo);
  updDat = fileReadData(MONTHS, recNo);

  for (int x=1; x<wc; x++) {
    int8_t wp = splitString(wParm[x].c_str(), '=', wPair, 3);
    _dThis = true;
    if (Verbose1) Debugf("wPair[0] (%s)-> [%s] \n", wPair[0].c_str(), wPair[1].c_str());
    _dThis = true;
    if (wPair[0] == "Y") {
      YY = wPair[1].toInt();
      if (Verbose1) Debugf("Y set to [%02d]\n", YY);
    } else if (wPair[0] == "M") {
      MM = wPair[1].toInt();
      if (Verbose1) Debugf("M set to [%02d]\n", MM);
      updDat.Label = ((YY - 2000)*100) + MM;
      if (Verbose1) Debugf("Label is now [%04d]\n", updDat.Label);
    } else if (wPair[0] == "EDT1") {
      updDat.EDT1 = wPair[1].toFloat();
      if (Verbose1) Debugf("EDT1 set to [%.3f]\n", updDat.EDT1);
    } else if (wPair[0] == "EDT2") {
      updDat.EDT2 = wPair[1].toFloat();
      if (Verbose1) Debugf("EDT2 set to [%.3f]\n", updDat.EDT2);
    } else if (wPair[0] == "ERT1") {
      updDat.ERT1 = wPair[1].toFloat();
      if (Verbose1) Debugf("ERT1 set to [%.3f]\n", updDat.ERT1);
    } else if (wPair[0] == "ERT2") {
      updDat.ERT2 = wPair[1].toFloat();
      if (Verbose1) Debugf("ERT2 set to [%.3f]\n", updDat.ERT2);
    } else if (wPair[0] == "GAS") {
      updDat.GDT  = wPair[1].toFloat();
      if (Verbose1) Debugf("GDT set to [%.2f]\n", updDat.GDT);
    }
  } // for ...
     
  fileWriteData(MONTHS, updDat, recNo);

} // doUpdateMonth()


//=======================================================================
void doSendSettings(uint8_t wsClient, String wsPayload) {
//=======================================================================
  String wsString;
  
  _dThis = true;
  if (Verbose1) Debugf("now sendSettings(%d)!\n", wsClient);

  readSettings();
  
  wsString  = ",DT1="  + String(settingEDT1, 5);
  wsString += ",DT2="  + String(settingEDT2, 5);
  wsString += ",RT1="  + String(settingERT1, 5);
  wsString += ",RT2="  + String(settingERT2, 5);
  wsString += ",GAST=" + String(settingGDT,  5);
  wsString += ",ENBK=" + String(settingENBK, 2);
  wsString += ",GNBK=" + String(settingGNBK, 2);
  wsString += ",BgColor=" + String(settingBgColor);
  wsString += ",FontColor=" + String(settingFontColor);
  wsString += ",Interval=" + String(settingInterval);
  wsString += ",SleepTime=" + String(settingSleepTime);
  wsString += ",LEDC="    + String(iniBordEDC)    + ",BEDC="    + String(iniFillEDC);
  wsString += ",LERC="    + String(iniBordERC)    + ",BERC="    + String(iniFillERC);
  wsString += ",LGDC="    + String(iniBordGDC)    + ",BGDC="    + String(iniFillGDC);
  wsString += ",LED2C="   + String(iniBordED2C)   + ",BED2C="   + String(iniFillED2C);
  wsString += ",LER2C="   + String(iniBordER2C)   + ",BER2C="   + String(iniFillER2C);
  wsString += ",LGD2C="   + String(iniBordGD2C)   + ",BGD2C="   + String(iniFillGD2C);
  wsString += ",LPR123C=" + String(iniBordPR123C) + ",BPR123C=" + String(iniFillPR123C);
  wsString += ",LPD1C="   + String(iniBordPD1C)   + ",BPD1C="   + String(iniFillPD1C);
  wsString += ",LPD2C="   + String(iniBordPD2C)   + ",BPD2C="   + String(iniFillPD2C);
  wsString += ",LPD3C="   + String(iniBordPD3C)   + ",BPD3C="   + String(iniFillPD3C);

  webSocket.sendTXT(wsClient, "msgType=settings" + wsString);

} // doSendSettings()


//=======================================================================
void doSaveSettings(uint8_t wsClient, String wsPayload) {
//=======================================================================
  String wParm[35], nColor;
  _dThis = true;
  Debugf("now saveSettings(%d) with [%s]!\n", wsClient, wsPayload.c_str());
  uint8_t wc = splitString(wsPayload.c_str(), ',', wParm, 34);
  //Debugf("-> found [%d] pairs!\n", wc);
  for(int p=1; p<wc; p++) {
    int wp = splitString(wParm[p].c_str(), '=', wPair, 3);
    nColor = wPair[1].substring(0, (MAXCOLORNAME - 1));
    _dThis = true;
    Debugf("wParm[%d] => [%s]=[%s]\n", p, wPair[0].c_str(), wPair[1].c_str());
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
    } else if (wPair[0] == "LEDC") {
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
  }

  writeSettings();
  
} // doSaveSettings()

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
