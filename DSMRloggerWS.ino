/*
***************************************************************************  
**  Program  : DSMRloggerWS (WebSockets)
*/
#define _FW_VERSION "v0.4.1 (" +String( __DATE__) + ")"
/*
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
  Arduino-IDE settings for ESP01 (black):

    - Board: "Generic ESP8266 Module"
    - Flash mode: "DOUT"
    - Flash size: "1M (256K SPIFFS)"  // ESP-12 4M (1M SPIFFS)
    - Debug port: "Disabled"
    - Debug Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "nodemcu"   // but will depend on the programmer!
    - Crystal Frequency: "26 MHz" 
    - VTables: "Flash"
    - Flash Frequency: "40MHz"
    - CPU Frequency: "80 MHz"
    - Buildin Led: "1"  // GPIO01 - Pin 2 // "2" for ESP12/Wemos D1 and ESP-01S
    - Upload Speed: "115200"                                                                                                                                                                                                                                                 
    - Erase Flash: "Only Sketch"
    - Port: "ESP01-DSMR at <-- IP address -->"
*/

/******************** compiler options  ********************************************/
#define IS_ESP12              // define if it's an ESP-12
#define USE_ARDUINO_OTA       // define if there is enough memory
#define HAS_OLED_SSD1306      // define if an OLED display is present
// #define HAS_NO_METER          // define if No "Slimme Meter" is attached
/******************** don't change anything below this comment **********************/


#include <TimeLib.h>            //  https://github.com/PaulStoffregen/Time

//  https://github.com/matthijskooijman/arduino-dsmr
#include <dsmr.h>               // Version 0.1 - Commit f79c906 on 18 Sep 2018

#ifdef USE_ARDUINO_OTA
  #include <ArduinoOTA.h>       // Version 1.0.0 - part of ESP8266 Core https://github.com/esp8266/Arduino
#endif

#ifdef ARDUINO_ESP8266_GENERIC
  #ifdef HAS_NO_METER
    #define _HOSTNAME     "TEST-DSMR"
  #else
    #define _HOSTNAME     "DSMR-WS"  
    #ifdef IS_ESP12
      #define DTR_ENABLE  12
    #endif
  #endif
#else
  #ifdef HAS_NO_METER
    #define _HOSTNAME     "TEST-DSMR"
  #else
    #define _HOSTNAME     "ESP12-DSMR"
    #ifdef IS_ESP12
      #define DTR_ENABLE  12
    #endif
  #endif
#endif
#ifdef HAS_NO_METER
    #define HOURS_FILE     "/TSThours.csv"
    #define DAYS_FILE      "/TSTdays.csv"
    #define MONTHS_FILE    "/TSTmonths.csv"
#else
    #define HOURS_FILE     "/PRDhours.csv"
    #define DAYS_FILE      "/PRDdays.csv"
    #define MONTHS_FILE    "/PRDmonths.csv"
#endif
#define SETTINGS_FILE      "/DSMRsettings.ini"
//-------------------------.........1....1....2....2....3....3....4....4....5....5....6....6....7....7
//-------------------------1...5....0....5....0....5....0....5....0....5....0....5....0....5....0....5
#define MONTHS_FORMAT     "%04d;%10s;%10s;%10s;%10s;%10s;\n"
#define MONTHS_CSV_HEADER "YYMM;      EDT1;      EDT2;      ERT1;      ERT2;       GDT;"
#define MONTHS_RECLEN      65
#define MONTHS_RECS        25
#define DAYS_FORMAT       "%06d;%10s;%10s;%10s;%10s;%10s;\n"
#define DAYS_CSV_HEADER   "YYMMDD;      EDT1;      EDT2;      ERT1;      ERT2;       GDT;"  
#define DAYS_RECLEN        67
#define DAYS_RECS          15
#define HOURS_FORMAT      "%08d;%10s;%10s;%10s;%10s;%10s;\n"
#define HOURS_CSV_HEADER  "YYMMDDHH;      EDT1;      EDT2;      ERT1;      ERT2;       GDT;"
#define HOURS_RECLEN       69
#define HOURS_RECS         49
#define LED_ON            LOW
#define LED_OFF          HIGH
#define FLASH_BUTTON        0
#define MAXCOLORNAME       15

#include "Debug.h"
#include "networkStuff.h"

uint8_t   settingSleepTime; // needs to be declared before the oledStuff.h include
#ifdef HAS_OLED_SSD1306
  #include "oledStuff.h"
#endif

/**
 * Define the DSMRdata we're interested in, as well as the DSMRdatastructure to
 * hold the parsed DSMRdata. This list shows all supported fields, remove
 * any fields you are not using from the below list to make the parsing
 * and printing code smaller.
 * Each template argument below results in a field of the same name.
 */
using MyData = ParsedData<
  /* String */         identification
  /* String */        ,p1_version
  /* String */        ,timestamp
  /* String */        ,equipment_id
  /* FixedValue */    ,energy_delivered_tariff1
  /* FixedValue */    ,energy_delivered_tariff2
  /* FixedValue */    ,energy_returned_tariff1
  /* FixedValue */    ,energy_returned_tariff2
  /* String */        ,electricity_tariff
  /* FixedValue */    ,power_delivered
  /* FixedValue */    ,power_returned
//  /* FixedValue */    ,electricity_threshold
//  /* uint8_t */       ,electricity_switch_position
//  /* uint32_t */      ,electricity_failures
//  /* uint32_t */      ,electricity_long_failures
//  /* String */        ,electricity_failure_log
//  /* uint32_t */      ,electricity_sags_l1
//  /* uint32_t */      ,electricity_sags_l2
//  /* uint32_t */      ,electricity_sags_l3
//  /* uint32_t */      ,electricity_swells_l1
//  /* uint32_t */      ,electricity_swells_l2
//  /* uint32_t */      ,electricity_swells_l3
//  /* String */        ,message_short
//  /* String */        ,message_long
  /* FixedValue */    ,voltage_l1
  /* FixedValue */    ,voltage_l2
  /* FixedValue */    ,voltage_l3
  /* FixedValue */    ,current_l1
  /* FixedValue */    ,current_l2
  /* FixedValue */    ,current_l3
  /* FixedValue */    ,power_delivered_l1
  /* FixedValue */    ,power_delivered_l2
  /* FixedValue */    ,power_delivered_l3
  /* FixedValue */    ,power_returned_l1
  /* FixedValue */    ,power_returned_l2
  /* FixedValue */    ,power_returned_l3
  /* uint16_t */      ,gas_device_type
  /* String */        ,gas_equipment_id
//  /* uint8_t */       ,gas_valve_position
  /* TimestampedFixedValue */ ,gas_delivered
//  /* uint16_t */      ,thermal_device_type
//  /* String */        ,thermal_equipment_id
//  /* uint8_t */       ,thermal_valve_position
//  /* TimestampedFixedValue */ ,thermal_delivered
//  /* uint16_t */      ,water_device_type
//  /* String */        ,water_equipment_id
//  /* uint8_t */       ,water_valve_position
//  /* TimestampedFixedValue */ ,water_delivered
//  /* uint16_t */      ,slave_device_type
//  /* String */        ,slave_equipment_id
//  /* uint8_t */       ,slave_valve_position
//  /* TimestampedFixedValue */ ,slave_delivered
>;
 
enum    { TAB_UNKNOWN, TAB_ACTUEEL, TAB_LAST24HOURS, TAB_LAST7DAYS, TAB_LAST24MONTHS, TAB_GRAPHICS, TAB_SYSINFO, TAB_EDITOR };
enum    { PERIOD_UNKNOWN, HOURS, MONTHS, DAYS };

typedef struct {
    uint32_t  Label;
    float     EDT1;
    float     EDT2;
    float     ERT1;
    float     ERT2;
    float     GDT;
} dataStruct;

static dataStruct hourData;   // 0 + 1-24
static dataStruct dayData;    // 1 - 7 (0=header, 1=sunday)
static dataStruct monthData;  // 0 + year1 1 t/m 12 + year2 1 t/m 12

const char *weekDayName[]  { "UnKnown", "Zondag", "Maandag", "Dinsdag", "Woensdag"
                            , "Donderdag", "Vrijdag", "Zaterdag", "UnKnown" };
const char *monthName[]    { "00", "Januari", "Februari", "Maart", "April", "Mei", "Juni", "Juli"
                            , "Augustus", "September", "Oktober", "November", "December", "13" };
const char *flashMode[]    { "QIO", "QOUT", "DIO", "DOUT", "UnKnown" };

/**
struct FSInfo {
    size_t totalBytes;
    size_t usedBytes;
    size_t blockSize;
    size_t pageSize;
    size_t maxOpenFiles;
    size_t maxPathLength;
};
**/
#ifdef DTR_ENABLE
  P1Reader    slimmeMeter(&Serial, DTR_ENABLE);
#else
  P1Reader    slimmeMeter(&Serial, 0);
#endif
WiFiClient  wifiClient;

int8_t    actTab = 0;
uint32_t  telegramInterval, noMeterWait, telegramCount, telegramErrors, waitForATOupdate;
char      cMsg[150], fChar[10];
float     EnergyDelivered, EnergyReturned;
float     PowerDelivered, PowerReturned, maxPowerDelivered, maxPowerReturned;
char      maxTimePD[7], maxTimePR[7]; // hh:mm
int16_t   PowerDelivered_l1, PowerDelivered_l2, PowerDelivered_l3;  // Watt in 1 watt resolution
int16_t   PowerReturned_l1,  PowerReturned_l2,  PowerReturned_l3;   // Watt in 1 watt resolution
float     GasDelivered;
String    pTimestamp;
String    P1_Version, Equipment_Id, GasEquipment_Id, ElectricityTariff;
char      Identification[100];  // Sn (0..96)
float     EnergyDeliveredTariff1, EnergyDeliveredTariff2, EnergyReturnedTariff1, EnergyReturnedTariff2;
float     Voltage_l1, Voltage_l2, Voltage_l3;
uint8_t   Current_l1, Current_l2, Current_l3;
uint16_t  GasDeviceType;

String    lastReset = "";
bool      OTAinProgress = false, doLog = false, Verbose1 = false, Verbose2 = false, showRaw = false;
int8_t    thisHour = -1, thisDay = -1, thisMonth = -1, lastMonth, thisYear = 15;
int32_t   thisHourKey = -1;
int8_t    forceMonth = 0, forceDay = 0;
int8_t    showRawCount;
uint32_t  nextSecond, unixTimestamp;
uint64_t  upTimeSeconds;
IPAddress ipDNS, ipGateWay, ipSubnet;
float     settingEDT1, settingEDT2, settingERT1, settingERT2, settingGDT;
float     settingENBK, settingGNBK;
uint8_t   settingInterval;
char      settingBgColor[MAXCOLORNAME], settingFontColor[MAXCOLORNAME];
char      iniBordEDC[MAXCOLORNAME],    iniBordERC[MAXCOLORNAME],  iniBordGDC[MAXCOLORNAME],    iniBordED2C[MAXCOLORNAME];
char      iniBordER2C[MAXCOLORNAME],   iniBordGD2C[MAXCOLORNAME], iniBordPR123C[MAXCOLORNAME], iniBordPD1C[MAXCOLORNAME];
char      iniBordPD2C[MAXCOLORNAME],   iniBordPD3C[MAXCOLORNAME], iniFillEDC[MAXCOLORNAME],    iniFillERC[MAXCOLORNAME];
char      iniFillGDC[MAXCOLORNAME],    iniFillED2C[MAXCOLORNAME], iniFillER2C[MAXCOLORNAME],   iniFillGD2C[MAXCOLORNAME];
char      iniFillPR123C[MAXCOLORNAME], iniFillPD1C[MAXCOLORNAME], iniFillPD2C[MAXCOLORNAME],   iniFillPD3C[MAXCOLORNAME];
String    settingUpdateURL, settingFingerPrint;

struct showValues {
  template<typename Item>
  void apply(Item &i) {
    if (i.present()) {
        Debug(Item::name);
        Debug(F(": "));
        Debug(i.val());
        Debug(Item::unit());
        Debugln();
    }
  }
};


//===========================================================================================
String macToStr(const uint8_t* mac) {
//===========================================================================================
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
} // macToStr()



//=======================================================================
int8_t splitString(String inStrng, char delimiter, String wOut[], uint8_t maxWords) {
//=======================================================================
  uint16_t inxS = 0, inxE = 0, wordCount = 0;
    inStrng.trim();
    while(inxE < inStrng.length() && wordCount < maxWords) {
      inxE  = inStrng.indexOf(delimiter, inxS);             //finds location of first ,
      wOut[wordCount] = inStrng.substring(inxS, inxE);  //captures first data String
      wOut[wordCount].trim();
      _dThis = true;
      //Debugf("[%d] => [%c] @[%d] found[%s]\n", wordCount, delimiter, inxE, wOut[wordCount].c_str());
      inxS = inxE;
      inxS++;
      wordCount++;
    }
    if (inxS < inStrng.length()) {
      wOut[wordCount] = inStrng.substring(inxS, inStrng.length());  //captures first data String      
      _dThis = true;
      //Debugf("[%d] rest => [%s]\n", wordCount, wOut[wordCount].c_str());
    }

    return wordCount;
    
} // splitString()


//===========================================================================================
String upTime() {
//===========================================================================================

  char    calcUptime[20];

  sprintf(calcUptime, "%d(d):%02d:%02d", int((upTimeSeconds / (60 * 60 * 24)) % 365)
                                       , int((upTimeSeconds / (60 * 60)) % 24)
                                       , int((upTimeSeconds / (60)) % 60));

  return calcUptime;

} // upTime()


//===========================================================================================
void printData() {
//===========================================================================================
  String dateTime;
  
    Debugln("-Totalen----------------------------------------------------------");
    dateTime = buildDateTimeString(pTimestamp);
    sprintf(cMsg, "Datum / Tijd         :  %s", dateTime.c_str());
    Debugln(cMsg);

    //dtostrf(EnergyDelivered, 9, 3, fChar);
    sprintf(cMsg, "Energy Delivered     : %12.3fkWh", EnergyDelivered);
    Debugln(cMsg);

    //dtostrf(EnergyReturned, 9, 3, fChar);
    sprintf(cMsg, "Energy Returned      : %12.3fkWh", EnergyReturned);
    Debugln(cMsg);

    dtostrf(PowerDelivered, 9, 3, fChar);
    sprintf(cMsg, "Power Delivered      : %skW", fChar);
    Debugln(cMsg);

    dtostrf(PowerReturned, 9, 3, fChar);
    sprintf(cMsg, "Power Returned       : %skW", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerDelivered_l1, 9, 0, fChar);
    sprintf(cMsg, "Power Delivered (l1) : %sWatt", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerDelivered_l2, 9, 0, fChar);
    sprintf(cMsg, "Power Delivered (l2) : %sWatt", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerDelivered_l3, 9, 0, fChar);
    sprintf(cMsg, "Power Delivered (l3) : %sWatt", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerReturned_l1, 9, 0, fChar);
    sprintf(cMsg, "Power Returned (l1)  : %sWatt", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerReturned_l2, 9, 0, fChar);
    sprintf(cMsg, "Power Returned (l2)  : %sWatt", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerReturned_l3, 9, 0, fChar);
    sprintf(cMsg, "Power Returned (l3)  : %sWatt", fChar);
    Debugln(cMsg);

    dtostrf(GasDelivered, 9, 2, fChar);
    sprintf(cMsg, "Gas Delivered        : %sm3", fChar);
    Debugln(cMsg);
    Debugln("==================================================================");
  
} // printData()


//===========================================================================================
void processData(MyData DSMRdata) {
//===========================================================================================
  int8_t slot, nextSlot, prevSlot;
  
#ifndef HAS_NO_METER
    strcpy(Identification, DSMRdata.identification.c_str());
    P1_Version                        = DSMRdata.p1_version;

    pTimestamp                        = DSMRdata.timestamp;
    if (DSMRdata.equipment_id_present) {
            Equipment_Id              = DSMRdata.equipment_id;
    } else  Equipment_Id              = "UnKnown";
    if (DSMRdata.energy_delivered_tariff1_present) {
            EnergyDeliveredTariff1    = (float)DSMRdata.energy_delivered_tariff1;
    } else  EnergyDeliveredTariff1    = 0.0;
    if (DSMRdata.energy_delivered_tariff2_present) {
            EnergyDeliveredTariff2    = (float)DSMRdata.energy_delivered_tariff2;
    } else  EnergyDeliveredTariff2    = 0.0;
    if (DSMRdata.energy_returned_tariff1_present) {
            EnergyReturnedTariff1     = (float)DSMRdata.energy_returned_tariff1;
    } else  EnergyReturnedTariff1     = 0.0;
    if (DSMRdata.energy_returned_tariff2_present) {
            EnergyReturnedTariff2     = (float)DSMRdata.energy_returned_tariff2;
    } else  EnergyReturnedTariff2     = 0.0; 
    if (DSMRdata.electricity_tariff_present) {
            ElectricityTariff         = DSMRdata.electricity_tariff;
    } else  ElectricityTariff         = "-"; 
    if (DSMRdata.voltage_l1_present) {
            Voltage_l1                = (float)DSMRdata.voltage_l1;
    } else  Voltage_l1                = 0.0; 
    if (DSMRdata.voltage_l2_present) {
            Voltage_l2                = (float)DSMRdata.voltage_l2;
    } else  Voltage_l2                = 0.0; 
    if (DSMRdata.voltage_l3_present) {
            Voltage_l3                = (float)DSMRdata.voltage_l3;
    } else  Voltage_l3                = 0.0;
    if (DSMRdata.current_l1_present) {
            Current_l1                = DSMRdata.current_l1;
    } else  Current_l1                = 0;
    if (DSMRdata.current_l2_present) {
            Current_l2                = DSMRdata.current_l2;
    } else  Current_l2                = 0;
    if (DSMRdata.current_l3_present) {
            Current_l3                = DSMRdata.current_l3;
    } else  Current_l3                = 0;
    if (DSMRdata.power_delivered_present) {
            PowerDelivered            = (float)DSMRdata.power_delivered;
    } else  PowerDelivered            = 0.0;
    if (DSMRdata.power_delivered_l1_present) {
            PowerDelivered_l1         = DSMRdata.power_delivered_l1.int_val();
    } else  PowerDelivered_l1         = 0;
    if (DSMRdata.power_delivered_l2_present) {
            PowerDelivered_l2         = DSMRdata.power_delivered_l2.int_val();
    } else  PowerDelivered_l2         = 0;
    if (DSMRdata.power_delivered_l3_present) {
            PowerDelivered_l3         = DSMRdata.power_delivered_l3.int_val();
    } else  PowerDelivered_l3         = 0;
    if (DSMRdata.power_returned_present) {
            PowerReturned             = (float)DSMRdata.power_returned;
    } else  PowerReturned             = 0.0;
    if (DSMRdata.power_returned_l1_present) {
            PowerReturned_l1          = DSMRdata.power_returned_l1.int_val();
    } else  PowerReturned_l1          = 0;
    if (DSMRdata.power_returned_l2_present) {
            PowerReturned_l2          = DSMRdata.power_returned_l2.int_val();
    } else  PowerReturned_l2          = 0;
    if (DSMRdata.power_returned_l3_present) {
            PowerReturned_l3          = DSMRdata.power_returned_l3.int_val();
    } else  PowerReturned_l3          = 0;
    if (DSMRdata.gas_device_type_present) {
            GasDeviceType             = DSMRdata.gas_device_type;
    } else  GasDeviceType             = 0;
    if (DSMRdata.gas_equipment_id_present) {
            GasEquipment_Id           = DSMRdata.gas_equipment_id;
    } else  GasEquipment_Id           = "UnKnown";
    if (DSMRdata.gas_delivered_present) {
            GasDelivered              = (float)DSMRdata.gas_delivered;
    } else  GasDelivered              = 0.0;

    EnergyDelivered   = EnergyDeliveredTariff1 + EnergyDeliveredTariff2; 
    EnergyReturned    = EnergyReturnedTariff1  + EnergyReturnedTariff2;
#endif
    unixTimestamp       = epoch(pTimestamp);

//================= update data set's =======================================================
    monthData.EDT1 = EnergyDeliveredTariff1;
    monthData.ERT1 = EnergyReturnedTariff1;
    monthData.EDT2 = EnergyDeliveredTariff2;
    monthData.ERT2 = EnergyReturnedTariff2;
    monthData.GDT  = GasDelivered;
    dayData.EDT1   = EnergyDeliveredTariff1;
    dayData.ERT1   = EnergyReturnedTariff1;
    dayData.EDT2   = EnergyDeliveredTariff2;
    dayData.ERT2   = EnergyReturnedTariff2;
    dayData.GDT    = GasDelivered;
    hourData.EDT1  = EnergyDeliveredTariff1;
    hourData.ERT1  = EnergyReturnedTariff1;
    hourData.EDT2  = EnergyDeliveredTariff2;
    hourData.ERT2  = EnergyReturnedTariff2;
    hourData.GDT   = GasDelivered;
    if ((PowerDelivered_l1 + PowerDelivered_l2 + PowerDelivered_l3) > maxPowerDelivered) {
      maxPowerDelivered = PowerDelivered_l1 + PowerDelivered_l2 + PowerDelivered_l3;
      sprintf(maxTimePD, "%02d:%02d", HourFromTimestamp(pTimestamp), MinuteFromTimestamp(pTimestamp));
    }
    if ((PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3)  > maxPowerReturned) {
      maxPowerReturned  = PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3;
      sprintf(maxTimePR, "%02d:%02d", HourFromTimestamp(pTimestamp), MinuteFromTimestamp(pTimestamp));
    }
    
//----- update OLED display ---------
#ifdef HAS_OLED_SSD1306
    String DT   = buildDateTimeString(pTimestamp);

    sprintf(cMsg, "%s", DT.substring(0, 16).c_str());
    oled_Print_Msg(0, cMsg, 0);
    sprintf(cMsg, "Power-P%6d Watt", (PowerDelivered_l1 + PowerDelivered_l2 + PowerDelivered_l3));
    oled_Print_Msg(1, cMsg, 0);
    sprintf(cMsg, "Power+P%6d Watt", (PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3));
    oled_Print_Msg(2, cMsg, 0);
    // rotate output last line every 20 deconds
    int s = SecondFromTimestamp(pTimestamp);
    if ((s >= 1 && s < 10) || (s >= 30 && s < 40))
          sprintf(cMsg, "Telegram:%5d/%3d", telegramCount, telegramErrors);
    else if ((s >= 10 && s < 20) || (s >= 40 && s < 50))
          sprintf(cMsg, "Heap:%7d Bytes", ESP.getFreeHeap());
    else  sprintf(cMsg, "WiFi RSSI:%4d dBm", WiFi.RSSI());
    oled_Print_Msg(3, cMsg, 0);
#endif

//================= handle Hour change ======================================================
    if (thisHourKey != HoursKeyTimestamp(pTimestamp)) {
      thisHourKey = HoursKeyTimestamp(pTimestamp);
      _dThis = true;
      Debugf("Saving data for thisHourKey[%08d]\n", thisHourKey);
      hourData.Label = thisHourKey;
      hourData.EDT1  = EnergyDeliveredTariff1;
      hourData.EDT2  = EnergyDeliveredTariff2;
      hourData.ERT1  = EnergyReturnedTariff1;
      hourData.ERT2  = EnergyReturnedTariff2;
      hourData.GDT   = GasDelivered;
      fileWriteData(HOURS, hourData);
      
    } // if (thisHourKey != HourFromTimestamp(pTimestamp)) 

//================= handle Day change ======================================================
    if (thisDay != DayFromTimestamp(pTimestamp)) {
      thisDay = DayFromTimestamp(pTimestamp);
      thisHourKey = -1;
      sprintf(cMsg, "%02d%02d%02d", YearFromTimestamp(pTimestamp), MonthFromTimestamp(pTimestamp), DayFromTimestamp(pTimestamp));
      _dThis = true;
      Debugf("Saving data for Day[%02d] => newLabel[%06ld]\r\n", thisDay, String(cMsg).toInt());
      dayData.Label = String(cMsg).toInt();
      dayData.EDT1  = EnergyDeliveredTariff1;
      dayData.EDT2  = EnergyDeliveredTariff2;
      dayData.ERT1  = EnergyReturnedTariff1;
      dayData.ERT2  = EnergyReturnedTariff2;
      dayData.GDT   = GasDelivered;
      fileWriteData(DAYS, dayData);
      maxPowerDelivered = PowerDelivered_l1 + PowerDelivered_l2 + PowerDelivered_l3;
      sprintf(maxTimePD, "00:01");
      maxPowerReturned  = PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3;
      sprintf(maxTimePR, "00:01");
    }
    dayData.EDT1  = EnergyDeliveredTariff1;
    dayData.EDT2  = EnergyDeliveredTariff2;
    dayData.ERT1  = EnergyReturnedTariff1;
    dayData.ERT2  = EnergyReturnedTariff2;
    dayData.GDT   = GasDelivered;

//================= handle Month change ======================================================
    if (thisMonth != MonthFromTimestamp(pTimestamp)) {
      thisMonth = MonthFromTimestamp(pTimestamp);
      thisYear  = YearFromTimestamp(pTimestamp);
      thisDay   = -1;
      if (Verbose1) Debugf("processData(): thisYear[20%02d] => thisMonth[%02d]\r\n", thisYear, thisMonth);
      _dThis = true;
      Debugf("processData(): Saving data for thisMonth[20%02d-%02d] \n", thisYear, thisMonth);
      sprintf(cMsg, "%02d%02d", thisYear, thisMonth);
      monthData.Label  = String(cMsg).toInt();
      monthData.EDT1   = EnergyDeliveredTariff1;
      monthData.EDT2   = EnergyDeliveredTariff2;
      monthData.ERT1   = EnergyReturnedTariff1;
      monthData.ERT2   = EnergyReturnedTariff2;
      monthData.GDT    = GasDelivered;
      fileWriteData(MONTHS, monthData);
      if (Verbose1) Debugf("processData(): monthData for [20%04ld] saved!\r\n", String(cMsg).toInt());

    } // if (thisMonth != MonthFromTimestamp(pTimestamp)) 

    if (Verbose1) {
      _dThis = true;
      Debugf("Put data for Month[20%02d-%02d] in Record[01]\n", thisYear, thisMonth);
    }
    monthData.EDT1 = EnergyDeliveredTariff1;
    monthData.ERT1 = EnergyReturnedTariff1;
    monthData.EDT2 = EnergyDeliveredTariff2;
    monthData.ERT2 = EnergyReturnedTariff2;
    monthData.GDT  = GasDelivered;
   
} // processData()


//===========================================================================================
void setup() {
//===========================================================================================
  Serial.begin(115200, SERIAL_8N1);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLASH_BUTTON, INPUT);
#ifdef DTR_ENABLE
  pinMode(DTR_ENABLE, OUTPUT);
#endif

#ifdef HAS_OLED_SSD1306
  oledSleepTimer = millis() + (10 * 60000); // initially 10 minutes on
  oled_Init();
  oled_Clear();  // clear the screen so we can paint the menu.
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  sprintf(cMsg, "(c) 2019 [%s]", String(_FW_VERSION).substring(0,6).c_str());
  oled_Print_Msg(1, cMsg, 0);
  oled_Print_Msg(2, "Willem Aandewiel", 0);
  oled_Print_Msg(3, "Have fun!!", 500);
  yield();
#endif
  for(int I=0; I<5; I++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    #ifdef DTR_ENABLE
      digitalWrite(DTR_ENABLE, digitalRead(LED_BUILTIN));
    #endif
    delay(2000);
  }
  digitalWrite(LED_BUILTIN, LED_OFF);  // HIGH is OFF
  lastReset     = ESP.getResetReason();
  
  Serial.println("\nBooting....\n");

#ifdef HAS_OLED_SSD1306
  oled_Clear();  // clear the screen 
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  oled_Print_Msg(1, "Connecting to WiFi", 500);
#endif
  digitalWrite(LED_BUILTIN, LED_ON);
  startWiFi();
#ifdef HAS_OLED_SSD1306
  oled_Print_Msg(1, WiFi.SSID(), 0);
  sprintf(cMsg, "IP %s", WiFi.localIP().toString().c_str());
  oled_Print_Msg(2, cMsg, 1500);
#endif
  digitalWrite(LED_BUILTIN, LED_OFF);

  startTelnet();
#ifdef HAS_OLED_SSD1306
  oled_Print_Msg(3, "telnet started", 500);
#endif

  Serial.println ( "" );
  Serial.print ( "Connected to " ); Serial.println (WiFi.SSID());
  Serial.print ( "IP address: " );  Serial.println (WiFi.localIP());

  for (int L=0; L < 10; L++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(200);
  }
  digitalWrite(LED_BUILTIN, LED_OFF);
  
  startMDNS(_HOSTNAME);
#ifdef HAS_OLED_SSD1306
  oled_Print_Msg(3, "mDNS started", 500);
#endif
  MDNS.addService("arduino", "tcp", 81);
  MDNS.port(81);  // webSockets

//================ SPIFFS =========================================
  if (!SPIFFS.begin()) {
    _dThis = true;
    Debugln("SPIFFS Mount failed");   // Serious problem with SPIFFS 
    SPIFFSmounted = false;
#ifdef HAS_OLED_SSD1306
    oled_Print_Msg(3, "SPIFFS FAILED!", 2000);
#endif
    
  } else { 
    _dThis = true;
    Debugln("SPIFFS Mount succesfull");
    SPIFFSmounted = true;
#ifdef HAS_OLED_SSD1306
    oled_Print_Msg(3, "SPIFFS mounted", 500);
#endif
  }
//=============end SPIFFS =========================================
  sprintf(cMsg, "Last reset reason: [%s]", ESP.getResetReason().c_str());
  Debugln(cMsg);

  Serial.print("use 'telnet ");
  Serial.print (WiFi.localIP());
  Serial.println("' for further debugging");

//===========================================================================================
#ifdef USE_ARDUINO_OTA
  startArduinoOTA();
  //==============OTA end=====================================
#endif

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

//--- read most recent data from SPIFFS -----
  monthData = fileReadData(MONTHS, 1);
  dayData   = fileReadData(DAYS, 1);
  hourData  = fileReadData(HOURS, 1);
  label2Fields(hourData.Label, thisYear, thisMonth, thisDay, thisHour);
  if (thisYear == 0) thisYear = 10;
  sprintf(cMsg, "%02d%02d%02d%02d0101W\0\0", thisYear, thisMonth, thisDay, thisHour);
  pTimestamp = cMsg;
  epoch(pTimestamp);
  
  readSettings();

#ifdef HAS_NO_METER
/**/  //      createDummyData(); use "Z" in menu!
/**/  Equipment_Id            = "Equipment01";
/**/  GasDeviceType           = 1;
/**/  GasEquipment_Id         = "GasEquipment12";
/**/  EnergyDeliveredTariff1  = monthData.EDT1;
/**/  EnergyReturnedTariff1   = monthData.ERT1;
/**/  EnergyDeliveredTariff2  = monthData.EDT2;
/**/  EnergyReturnedTariff2   = monthData.ERT2;
/**/  GasDelivered            = monthData.GDT;
#endif

  telegramCount   = 0;
  telegramErrors  = 0;

  if (SPIFFS.exists("/DSMRlogger.html")) {
    Debugln("Found DSMRlogger.html -> normal operation!");
#ifdef HAS_OLED_SSD1306
    oled_Print_Msg(0, "OK, found:", 0);
    oled_Print_Msg(1, "DSMRlogger.html", 0);
    oled_Print_Msg(2, "Continue Normal", 0);
    oled_Print_Msg(3, "Operation ;-)", 500);
#endif
    HttpServer.serveStatic("/",               SPIFFS, "/DSMRlogger.html");
    HttpServer.serveStatic("/DSMRlogger.html",SPIFFS, "/DSMRlogger.html");
    HttpServer.serveStatic("/index",          SPIFFS, "/DSMRlogger.html");
    HttpServer.serveStatic("/index.html",     SPIFFS, "/DSMRlogger.html");
  } else {
    Debugln("Oeps! DSMRlogger.html not found -> present errorIndexPage!");
#ifdef HAS_OLED_SSD1306
    oled_Print_Msg(0, "OEPS!", 0);
    oled_Print_Msg(1, "Couldn't find ", 0);
    oled_Print_Msg(2, "DSMRlogger.html", 0);
    oled_Print_Msg(3, "Start ErrorPage", 2000);
#endif

    HttpServer.on("/", handleErrorIndexPage);
  }
  HttpServer.serveStatic("/DSMRlogger.css",   SPIFFS, "/DSMRlogger.css");
  HttpServer.serveStatic("/DSMRlogger.js",    SPIFFS, "/DSMRlogger.js");
  HttpServer.serveStatic("/DSMReditor.html",  SPIFFS, "/DSMReditor.html");
  HttpServer.serveStatic("/DSMReditor.js",    SPIFFS, "/DSMReditor.js");
  HttpServer.serveStatic("/dialog.css",       SPIFFS, "/dialog.css");
  HttpServer.serveStatic("/dialog.js",        SPIFFS, "/dialog.js");
  HttpServer.serveStatic("/DSMRgraphics.js",  SPIFFS, "/DSMRgraphics.js");
  HttpServer.serveStatic("/FSexplorer.png",   SPIFFS, "/FSexplorer.png");

  HttpServer.on("/restAPI", HTTP_GET, restAPI);
  HttpServer.on("/restapi", HTTP_GET, restAPI);
  HttpServer.on("/ReBoot", HTTP_POST, handleReBoot);

  HttpServer.on("/FSexplorer", HTTP_POST, handleFileDelete);
  HttpServer.on("/FSexplorer", handleRoot);
  HttpServer.on("/FSexplorer/upload", HTTP_POST, []() {
    HttpServer.send(200, "text/plain", "");
  }, handleFileUpload);

  HttpServer.onNotFound([]() {
    _dThis = true;
    Debugf("onNotFound(%s)\n", HttpServer.uri().c_str());
    if (HttpServer.uri() == "/") {
      reloadPage("/");
    }
    if (!handleFileRead(HttpServer.uri())) {
      HttpServer.send(404, "text/plain", "FileNotFound");
    }
  });

  HttpServer.begin();
  Debugln( "HTTP server started" );
  for (int i = 0; i< 10; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(200);
    #ifdef DTR_ENABLE
      digitalWrite(DTR_ENABLE, digitalRead(LED_BUILTIN));
    #endif
  }

  _dThis = true;
  Debugln("Enable slimmeMeter..");
  delay(100);
  slimmeMeter.enable(true);

  //test(); monthTabel

  _dThis = true;
  Debugf("Startup complete! pTimestamp[%s]\r\n", pTimestamp.c_str());  

#ifdef IS_ESP12
#ifndef HAS_NO_METER
  Serial.swap();
#endif
#endif

  sprintf(cMsg, "Last reset reason: [%s]", ESP.getResetReason().c_str());
  Debugln(cMsg);

  telegramInterval = millis() + 5000;
  noMeterWait      = millis() + 5000;
  upTimeSeconds    = (millis() / 1000) + 50;
  nextSecond       = millis() + 1000;
  
} // setup()


//===========================================================================================
void loop () {
//===========================================================================================
#ifdef USE_ARDUINO_OTA
  ArduinoOTA.handle();
#endif
  HttpServer.handleClient();
  webSocket.loop();
  MDNS.update();
  handleKeyInput();
  handleRefresh();
#ifdef HAS_OLED_SSD1306
  checkFlashButton();
#endif
  if (!showRaw) {
    slimmeMeter.loop();
  }
  
//if (!OTAinProgress) {
    if (!showRaw) {
      //---- capture new telegram ??
      if (millis() > telegramInterval) {
        telegramInterval = millis() + (settingInterval * 1000);  // test 10 seconden

        slimmeMeter.enable(true);
#ifdef ARDUINO_ESP8266_GENERIC
        digitalWrite(LED_BUILTIN, LED_ON);
#else
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
#endif
      }
    } // !showRaw 
//} else {  // waiting for ATO-update (or in progress) ...
//    if (millis() > waitForATOupdate) {
//      OTAinProgress     = false;
//      waitForATOupdate  = millis();
//    }
//}
  
#ifdef HAS_NO_METER
/**/  // ---- create some dummy data for testing without a Slimme Meter connected (HAS_NO_METER is defined)
/**/  static  MyData    DSMRdata;
/**/  static  uint8_t   sMinute = 1, sHour = thisHour, sDay = thisDay, sMonth = thisMonth, sYear = thisYear;
/**/          char      testID[100];
/**/          int8_t    maxDaysInMonth;
/**/          
/**/  if (millis() > noMeterWait) {
/**/    noMeterWait += 2000;
/**/
/**/    strcpy(Identification, "/ABCD(*)EFGHIJ(*)KLMNOPQRSTUVWXYZ");
/**/    P1_Version        = "TST";
/**/
/**/    EnergyDeliveredTariff1 += (float)(random(1, 50) / 15.0);
/**/    EnergyDeliveredTariff2 += (float)(random(1, 25) / 15.0);
/**/    EnergyDelivered         = EnergyDeliveredTariff1 + EnergyDeliveredTariff2;
/**/    EnergyReturnedTariff1  += (float)(random(1, 40) / 55.0);
/**/    EnergyReturnedTariff2  += (float)(random(1, 20) / 55.0);
/**/    EnergyReturned          = EnergyReturnedTariff1 + EnergyReturnedTariff2;
/**/    GasDelivered           += (float)(random(1, 30) / 100.0);
/**/
/**/    PowerDelivered_l1 = (int)((PowerDelivered_l1 * 4) + random(0,400)) / 5.0;   // Watt
/**/    PowerReturned_l1  = (int)((PowerReturned_l1  * 3) + random(0,100)) / 4.0;   // Watt
/**/    PowerDelivered_l2 = (int)((PowerDelivered_l2 * 4) + random(0,400)) / 5.0;   // Watt
/**/    PowerReturned_l2  = (int)((PowerReturned_l2  * 3) + random(0,100)) / 4.0;   // Watt
/**/    PowerDelivered_l3 = (int)((PowerDelivered_l3 * 4) + random(0,400)) / 5.0;   // Watt
/**/    PowerReturned_l3  = (int)((PowerReturned_l3  * 3) + random(0,100)) / 4.0;   // Watt
/**/    PowerDelivered    = (float)((PowerDelivered_l1 + PowerDelivered_l2 + PowerDelivered_l3) / 1000.0);  // kW
/**/    PowerReturned     = (float)((PowerReturned_l1  + PowerReturned_l2  + PowerReturned_l3)  / 1000.0);  // kW
/**/
/**/    ElectricityTariff = 1;
/**/    
/**/    sMinute += 10;
/**/    if (forceDay > 0) {
/**/      sDay      += forceDay;
/**/      forceDay   = 0;
/**/    }
/**/    if (forceMonth > 0) {
/**/      sMonth    += forceMonth;    
/**/      sDay      += 1;
/**/      forceMonth = 0;
/**/    }
/**/    if (sMinute >= 60) {
/**/      sMinute -= 59;
/**/      sHour++;
/**/    }
/**/    if (sHour >= 24) {  // 0 .. 23
/**/      sHour = 0;
/**/      sDay += 1;
/**/    }
/**/    if (sMonth == 4 || sMonth == 6 || sMonth == 9 || sMonth == 11)
/**/         maxDaysInMonth = 30;    
/**/    else if (sMonth == 2)
/**/         maxDaysInMonth = 28;
/**/    else maxDaysInMonth = 31;      
/**/
/**/    if (sDay > maxDaysInMonth) {
/**/      sDay = 1;
/**/      sMonth++;
/**/    }
/**/    if (sMonth <  1) sMonth = 1;
/**/    if (sMonth > 12) {
/**/      sMonth = 1;
/**/      sYear++;
/**/    }
/**/
/**/    telegramCount++;
/**/    Debugln("\n==================================================================\r");
/**/    _dThis = true;
/**/    Debugf("NO METER! read telegram [%d]\r\n", telegramCount);
/**/    if (telegramCount > 1563000000) {
/**/       ESP.reset();
/**/    }
/**/    sprintf(cMsg, "%02d%02d%02d%02d%02d15S", sYear, sMonth, sDay, sHour, sMinute);
/**/    DSMRdata.timestamp = String(cMsg);
/**/    pTimestamp  = DSMRdata.timestamp;
/**/    _dThis = true;
/**/    if (Verbose1) Debugf("pTimestamp [%s] sYear[%02d] sMonth[%02d] sDay[%02d] sHour[%02d] sMinute[%02d]\r\n"
/**/                       , pTimestamp.c_str(), sYear,  sMonth,      sDay,      sHour,      sMinute);
/**/    if (!showRaw) {
/**/      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
/**/      processData(DSMRdata);
/**/      sprintf(cMsg, "%02d%02d%02d%02d%02d15S", sYear, sMonth, sDay, sHour, (sMinute + 1));
/**/      pTimestamp = String(cMsg);
/**/      processData(DSMRdata);
/**/    }
/**/
/**/  } // noMeterWait > millis()

#else
  //---- this part is processed when HAS_NO_METER is NOT defined!
  if (showRaw) {
      while(Serial.available() > 0) {
        char rIn = Serial.read();
        if (rIn == '!') showRawCount++;
        TelnetStream.write((char)rIn);
      }
      if (showRawCount > 20) {
        showRaw = false;
      }
  } else {
      if (slimmeMeter.available()) {
        Debugln("\n==================================================================\r");
        telegramCount++;
        _dThis = true;
        Debugf("read telegram [%d]\r\n", telegramCount);
#ifdef USE_ARDUINO_OTA
        ArduinoOTA.handle();
#endif

        MyData    DSMRdata;
        String    DSMRerror;

        if (slimmeMeter.parse(&DSMRdata, &DSMRerror)) {  // Parse succesful, print result
          if (telegramCount > 1563000000) {
            ESP.reset();
          }
          digitalWrite(LED_BUILTIN, LED_OFF);
          processData(DSMRdata);
          if (Verbose1) {
            DSMRdata.applyEach(showValues());
            printData();
          }
          
        } else {                                    // Parser error, print error
          telegramErrors++;
          _dThis = true;
          Debugf("Parse error %s\r\n", DSMRerror.c_str());
        }
        
      } // if (slimmeMeter.available()) 
  }
#endif

  if (millis() > nextSecond) {
    nextSecond += 1000; // nextSecond is ahead of millis() so it will "rollover" 
    upTimeSeconds++;    // before millis() and this will probably work just fine
  }

} // loop()



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
