/*
***************************************************************************  
**  Program  : restAPI, part of DSMRloggerWS
**  Version  : v0.4.2
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//=======================================================================
void restAPI() {
//=======================================================================
  String pName;
  String pVal;

  for (uint8_t i = 0; i < httpServer.args(); i++) {
    pName  = httpServer.argName(i);
    pName.toUpperCase();
    pVal   = httpServer.arg(i);
    pVal.toUpperCase();
    _dThis = true;
    Debugf("[%d] [%s] => [%s]\n", i, pName.c_str(), pVal.c_str());
    if (pName == "GET") i = 99; 
  }

  if (pName == "GET" && pVal == "DEVINFO") {
    sendDeviceInfo();
  } else if (pName == "GET" && (pVal == "ACTUEEL" || pVal == "ACTUAL")) {
    sendActual();
  }

} // restAPI()


//=======================================================================
void sendDeviceInfo() {
//=======================================================================
String wsString;
  
//-Slimme Meter Info----------------------------------------------------------
  wsString  = "{";
  wsString += "\n\r \"Identification\":\"" + String(Identification) + "\"";
  wsString += "\n\r,\"P1_Version\":\"" + String(P1_Version) + "\"";
  wsString += "\n\r,\"Equipment_Id\":\"" + String(Equipment_Id) + "\"";
  wsString += "\n\r,\"Electricity_Tariff\":\"" + String(ElectricityTariff) + "\"";
  wsString += "\n\r,\"Gas_Device_Type\":\"" + String(GasDeviceType) + "\"";
  wsString += "\n\r,\"Gas_Equipment_Id\":\"" + String(GasEquipment_Id) + "\"";
  
//-Device Info-----------------------------------------------------------------
  wsString += "\n\r,\"Author\":\"Willem Aandewiel (www.aandewiel.nl)\"";
  wsString += "\n\r,\"FwVersion\":\""         + String( _FW_VERSION ) + "\"";
  wsString += "\n\r,\"Compiled\":\""          + String( __DATE__ ) 
                                            + String( "  " )
                                            + String( __TIME__ ) + "\"";
  wsString += "\n\r,\"FreeHeap\":\""          + String( ESP.getFreeHeap() ) + "\"";
  wsString += "\n\r,\"ChipID\":\""            + String( ESP.getChipId(), HEX ) + "\"";
  wsString += "\n\r,\"CoreVersion\":\""       + String( ESP.getCoreVersion() ) + "\"";
  wsString += "\n\r,\"SdkVersion\":\""        + String( ESP.getSdkVersion() ) + "\"";
  wsString += "\n\r,\"CpuFreqMHz\":\""        + String( ESP.getCpuFreqMHz() ) + "\"";
  wsString += "\n\r,\"SketchSize\":\""        + String( (ESP.getSketchSize() / 1024.0), 3) + "kB\"";
  wsString += "\n\r,\"FreeSketchSpace\":\""   + String( (ESP.getFreeSketchSpace() / 1024.0), 3 ) + "kB\"";

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) 
        sprintf(cMsg, "%08X (PUYA)", ESP.getFlashChipId());
  else  sprintf(cMsg, "%08X", ESP.getFlashChipId());
  wsString += "\n\r,\"FlashChipID\":\""       + String(cMsg) + "\"";  // flashChipId
  wsString += "\n\r,\"FlashChipSize\":\""     + String( (float)(ESP.getFlashChipSize() / 1024.0 / 1024.0), 3 ) + "MB\"";
  wsString += "\n\r,\"FlashChipRealSize\":\"" + String( (float)(ESP.getFlashChipRealSize() / 1024.0 / 1024.0), 3 ) + "MB\"";
  wsString += "\n\r,\"FlashChipSpeed\":\""    + String( (float)(ESP.getFlashChipSpeed() / 1000.0 / 1000.0) ) + "MHz\"";

  FlashMode_t ideMode = ESP.getFlashChipMode();
  wsString += "\n\r,\"FlashChipMode\":\""    + String( flashMode[ideMode] ) + "\"";
  wsString += "\n\r,\"BoardType\":";
#ifdef ARDUINO_ESP8266_NODEMCU
    wsString += String("\"ESP8266_NODEMCU\"");
#endif
#ifdef ARDUINO_ESP8266_GENERIC
    wsString += String("\"ESP8266_GENERIC\"");
#endif
#ifdef ESP8266_ESP01
    wsString += String("\"ESP8266_ESP01\"");
#endif
#ifdef ESP8266_ESP12
    wsString += String("\"ESP8266_ESP12\"");
#endif
  wsString += "\n\r,\"SSID\":\""              + String( WiFi.SSID() ) + "\"";
//wsString += "\n\r,\"PskKey\":\""            + String( WiFi.psk() ) + "\"";    // uncomment if you want to see this
  wsString += "\n\r,\"IpAddress\":\""         + WiFi.localIP().toString()  + "\"";
  wsString += "\n\r,\"WiFiRSSI\":\""          + String(WiFi.RSSI())  + "\"";
  wsString += "\n\r,\"Hostname\":\""          + String( _HOSTNAME ) + "\"";
  wsString += "\n\r,\"upTime\":\""            + String( upTime() ) + "\"";
  wsString += "\n\r,\"TelegramCount\":\""     + String( telegramCount ) + "\"";
  wsString += "\n\r,\"TelegramErrors\":\""    + String( telegramErrors ) + "\"";
  wsString += "\n\r,\"lastReset\":\"" + lastReset + "\"";
  wsString += "\n\r}\n\r";
  
  httpServer.send(200, "application/json", wsString);
  Debugln("sendDataDeviceInfo(): send JSON string\r\n");

} // sendDeviceInfo()

//=======================================================================
void sendActual() {
//=======================================================================
String wsString;

#ifdef HAS_NO_METER
  hourData.EDT1           += 0.33;
  EnergyDeliveredTariff1  = hourData.EDT1;
  hourData.EDT2           += 0.17;
  EnergyDeliveredTariff2  = hourData.EDT2;
  EnergyDelivered         = hourData.EDT1 + hourData.EDT2;
  hourData.ERT1           += 0.11;
  EnergyReturnedTariff1   = hourData.ERT1;
  hourData.ERT2           += 0.07;
  EnergyReturnedTariff2   = hourData.ERT2;
  EnergyReturned          = hourData.ERT1 + hourData.ERT2;
  hourData.GDT            += 0.08;
  GasDelivered            = hourData.GDT;
#endif

//-Totalen----------------------------------------------------------

  wsString  = "\n\r{";
  wsString += "\n\r \"Timestamp\":\"" + String(pTimestamp) + "\"";
  wsString += "\n\r,\"Energy_Delivered\":\"" + String(EnergyDelivered, 3) + "\"";
  wsString += "\n\r,\"Energy_Returned\":\"" + String(EnergyReturned, 3) + "\"";
  wsString += "\n\r,\"Gas_Delivered\":\"" + String(GasDelivered, 2) + "\"";
  wsString += "\n\r,\"Energy_Delivered_Tariff1\":\"" + String(EnergyDeliveredTariff1, 3) + "\"";
  wsString += "\n\r,\"Energy_Delivered_Tariff2\":\"" + String(EnergyDeliveredTariff2, 3) + "\"";
  wsString += "\n\r,\"Energy_Returned_Tariff1\":\"" + String(EnergyReturnedTariff1, 3) + "\"";
  wsString += "\n\r,\"Energy_Returned_Tariff2\":\"" + String(EnergyReturnedTariff2, 3) + "\"";
  wsString += "\n\r,\"Power_Delivered\":\"" + String(PowerDelivered, 3) + "\"";
  wsString += "\n\r,\"Power_Returned\":\"" + String(PowerReturned, 3) + "\"";
  wsString += "\n\r,\"Voltage_l1\":\"" + String(Voltage_l1, 1) + "\"";
  wsString += "\n\r,\"Current_l1\":\"" + String(Current_l1) + "\"";
  wsString += "\n\r,\"Voltage_l2\":\"" + String(Voltage_l2, 1) + "\"";
  wsString += "\n\r,\"Current_l2\":\"" + String(Current_l2) + "\"";
  wsString += "\n\r,\"Voltage_l3\":\"" + String(Voltage_l3, 1) + "\"";
  wsString += "\n\r,\"Current_l3\":\"" + String(Current_l3) + "\"";
  wsString += "\n\r,\"Power_Delivered_l1\":\"" + String(PowerDelivered_l1) + "\"";
  wsString += "\n\r,\"Power_Returned_l1\":\"" + String(PowerReturned_l1) + "\"";
  wsString += "\n\r,\"Power_Delivered_l2\":\"" + String(PowerDelivered_l2) + "\"";
  wsString += "\n\r,\"Power_Returned_l2\":\"" + String(PowerReturned_l2) + "\"";
  wsString += "\n\r,\"Power_Delivered_l3\":\"" + String(PowerDelivered_l3) + "\"";
  wsString += "\n\r,\"Power_Returned_l3\":\"" + String(PowerReturned_l3) + "\"";
  wsString += "\n\r}\n\r";
  
  httpServer.send(200, "application/json", wsString);
  Debugln("sendDataActual(): send JSON string\r\n");

} // sendActual()


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
