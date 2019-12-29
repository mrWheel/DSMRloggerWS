/* 
***************************************************************************  
**  Program  : restAPI, part of DSMRloggerWS
**  Version  : v1.0.4
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

// global vars
// Moved to main global firmware space, since it's used in more than one place now.
// static DynamicJsonDocument jsonDoc(1024);  // generic doc to return, clear() before use!

//=======================================================================
void restAPI() 
{
  String pName;
  String pVal;

  for (uint8_t i = 0; i < httpServer.args(); i++) 
  {
    pName  = httpServer.argName(i);
    pName.toUpperCase();
    pVal   = httpServer.arg(i);
    pVal.toUpperCase();
    DebugTf("[%d] [%s] => [%s]\r\n", i, pName.c_str(), pVal.c_str());
    if (pName == "GET") i = 99; 
  }

  if (pName == "GET" && pVal == "DEVINFO") 
  {
    sendDeviceInfo();
  } 
  else if (pName == "GET" && (pVal == "ACTUEEL" || pVal == "ACTUAL")) 
  {
    sendActual();
  }

} // restAPI()

// some helper functions
void _returnJSON(JsonObject obj);
void _returnJSON(JsonObject obj)
{
  String toReturn;

  serializeJson(obj, toReturn);
  DebugTf("JSON String is %d chars\r\n", toReturn.length());

  httpServer.send(200, "application/json", toReturn);
  
} // _returnJSON()

void _returnJSON400(const char * message)
{
  httpServer.send(400, "application/json", message);

} // _returnJSON400()


//=======================================================================
void sendDeviceInfo() 
{
  jsonDoc.clear();
    
//-Slimme Meter Info----------------------------------------------------------
  
  jsonDoc["Identification"]      = Identification;
  jsonDoc["P1_Version"]          = P1_Version;
  jsonDoc["Equipment_Id"]        = Equipment_Id;
  jsonDoc["Electricity_Tariff"]  = ElectricityTariff;
  jsonDoc["Gas_Device_Type"]     = GasDeviceType;
  jsonDoc["Gas_Equipment_Id"]    = GasEquipment_Id;
  
//-Device Info-----------------------------------------------------------------
  jsonDoc["Author"]              = "Willem Aandewiel (www.aandewiel.nl)";
  jsonDoc["FwVersion"]           = String( _FW_VERSION );
  jsonDoc["Compiled"]            = String( __DATE__ ) 
                                      + String( "  " )
                                      + String( __TIME__ );
  jsonDoc["FreeHeap"]            = ESP.getFreeHeap();
  jsonDoc["maxFreeBlock"]        = ESP.getMaxFreeBlockSize();
  jsonDoc["ChipID"]              = String( ESP.getChipId(), HEX );
  jsonDoc["CoreVersion"]         = String( ESP.getCoreVersion() );
  jsonDoc["SdkVersion"]          = String( ESP.getSdkVersion() );
  jsonDoc["CpuFreqMHz"]          = ESP.getCpuFreqMHz();
  jsonDoc["SketchSize"]          = formatFloat( (ESP.getSketchSize() / 1024.0), 3);
  jsonDoc["FreeSketchSpace"]     = formatFloat( (ESP.getFreeSketchSpace() / 1024.0), 3);

  if ((ESP.getFlashChipId() & 0x000000ff) == 0x85) 
        sprintf(cMsg, "%08X (PUYA)", ESP.getFlashChipId());
  else  sprintf(cMsg, "%08X", ESP.getFlashChipId());
  jsonDoc["FlashChipID"]         = cMsg;  // flashChipId
  jsonDoc["FlashChipSize_MB"]    = formatFloat((ESP.getFlashChipSize() / 1024.0 / 1024.0), 3);
  jsonDoc["FlashChipRealSize_MB"]= formatFloat((ESP.getFlashChipRealSize() / 1024.0 / 1024.0), 3);
  jsonDoc["FlashChipSpeed_MHz"]  = formatFloat((ESP.getFlashChipSpeed() / 1000.0 / 1000.0), 0);

  FlashMode_t ideMode = ESP.getFlashChipMode();
  jsonDoc["FlashChipMode"]       = flashMode[ideMode];
  jsonDoc["BoardType"] = 
#ifdef ARDUINO_ESP8266_NODEMCU
     "ESP8266_NODEMCU";
#endif
#ifdef ARDUINO_ESP8266_GENERIC
     "ESP8266_GENERIC";
#endif
#ifdef ESP8266_ESP01
     "ESP8266_ESP01";
#endif
#ifdef ESP8266_ESP12
     "ESP8266_ESP12";
#endif
  jsonDoc["SSID"]                = WiFi.SSID();
//jsonDoc["PskKey"]              = WiFi.psk();   // uncomment if you want to see this
  jsonDoc["IpAddress"]           = WiFi.localIP().toString();
  jsonDoc["WiFiRSSI"]            = WiFi.RSSI();
  jsonDoc["Hostname"]            = _HOSTNAME;
  jsonDoc["upTime"]              = upTime();
  jsonDoc["TelegramCount"]       = telegramCount;
  jsonDoc["TelegramErrors"]      = telegramErrors;
  jsonDoc["lastReset"]           = lastReset;
  
  _returnJSON( jsonDoc.as<JsonObject>() );

} // sendDeviceInfo()


//=======================================================================
void sendActual() 
{
  jsonDoc.clear();

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

  jsonDoc["Timestamp"]                 = pTimestamp;
  jsonDoc["Energy_Delivered"]          = formatFloat(EnergyDelivered, 3);
  jsonDoc["Energy_Returned"]           = formatFloat(EnergyReturned, 3);
  jsonDoc["Gas_Delivered"]             = formatFloat(GasDelivered, 2);
  jsonDoc["Energy_Delivered_Tariff1"]  = formatFloat(EnergyDeliveredTariff1, 3);
  jsonDoc["Energy_Delivered_Tariff2"]  = formatFloat(EnergyDeliveredTariff2, 3);
  jsonDoc["Energy_Returned_Tariff1"]   = formatFloat(EnergyReturnedTariff1, 3);
  jsonDoc["Energy_Returned_Tariff2"]   = formatFloat(EnergyReturnedTariff2, 3);
  jsonDoc["Energy_Tariff"]             = ElectricityTariff;
  jsonDoc["Power_Delivered"]           = formatFloat(PowerDelivered, 3);
  jsonDoc["Power_Returned"]            = formatFloat(PowerReturned, 3);
  jsonDoc["Voltage_l1"]                = formatFloat(Voltage_l1, 1);
  jsonDoc["Current_l1"]                = Current_l1;
  jsonDoc["Voltage_l2"]                = formatFloat(Voltage_l2, 1);
  jsonDoc["Current_l2"]                = Current_l2;
  jsonDoc["Voltage_l3"]                = formatFloat(Voltage_l3, 1);
  jsonDoc["Current_l3"]                = Current_l3;
  jsonDoc["Power_Delivered_l1"]        = PowerDelivered_l1;
  jsonDoc["Power_Returned_l1"]         = PowerReturned_l1;
  jsonDoc["Power_Delivered_l2"]        = PowerDelivered_l2;
  jsonDoc["Power_Returned_l2"]         = PowerReturned_l2;
  jsonDoc["Power_Delivered_l3"]        = PowerDelivered_l3;
  jsonDoc["Power_Returned_l3"]         = PowerReturned_l3;
  
  _returnJSON( jsonDoc.as<JsonObject>() );

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
