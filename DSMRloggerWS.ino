/*
***************************************************************************  
**  Program  : DSMRloggerWS (WebSockets)
*/
#define _FW_VERSION "v0.4.7 (06-08-2019)"
/*
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
  Arduino-IDE settings for DSMR-logger Version 4 (ESP-12):

    - Board: "Generic ESP8266 Module"
    - Flash mode: "DIO" | "DOUT"    // if you change from one to the other OTA will fail!
    - Flash size: "4M (1M SPIFFS)"  // ESP-01 "1M (256K SPIFFS)"  // PUYA flash chip won't work
    - Debug port: "Disabled"
    - Debug Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "none"   // but will depend on the programmer!
    - Crystal Frequency: "26 MHz" 
    - VTables: "Flash"
    - Flash Frequency: "40MHz"
    - CPU Frequency: "80 MHz"
    - Buildin Led: "2"  // ESP-01 (Black) GPIO01 - Pin 2 // "2" for Wemos and ESP-01S
    - Upload Speed: "115200"                                                                                                                                                                                                                                                 
    - Erase Flash: "Only Sketch"
    - Port: <select correct port>
*/

/******************** compiler options  ********************************************/
#define IS_ESP12                  // define if it's a 'bare' ESP-12 (no reset/flash functionality on board)
#define USE_UPDATE_SERVER         // define if there is enough memory and updateServer to be used
#define HAS_OLED_SSD1306          // define if an OLED display is present
//  #define USE_PRE40_PROTOCOL        // define if Slimme Meter is pre DSMR 4.0 (2.2 .. 3.0)
//  #define USE_NTP_TIME              // define to generate Timestamp from NTP (Only Winter Time for now)
//  #define SM_HAS_NO_FASE_INFO       // if your SM does not give fase info use total delevered/returned
#define USE_MQTT                  // define if you want to use MQTT
//  #define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
//  #define HAS_NO_METER              // define if No "Slimme Meter" is attached (*TESTING*)
/******************** don't change anything below this comment **********************/

#include <TimeLib.h>            //  https://github.com/PaulStoffregen/Time

#ifdef USE_PRE40_PROTOCOL                                       //PRE40
  //  https://github.com/mrWheel/arduino-dsmr30.git             //PRE40
  #include <dsmr30.h>                                           //PRE40
#else                                                           //else
  //  https://github.com/matthijskooijman/arduino-dsmr
  #include <dsmr.h>               // Version 0.1 - Commit f79c906 on 18 Sep 2018
#endif

#ifdef ARDUINO_ESP8266_GENERIC
  #ifdef HAS_NO_METER
    #define _HOSTNAME     "TEST-DSMR"
  #else
    #define _HOSTNAME     "DSMR-WS"  
    #ifdef IS_ESP12
      #define DTR_ENABLE  12
    #endif  // is_esp12
  #endif  // has_no_meter
#else // not arduino_esp8266_generic
  #ifdef HAS_NO_METER
    #define _HOSTNAME     "TEST-DSMR"
  #else // not has_no_meter
    #define _HOSTNAME     "ESP12-DSMR"
    #ifdef IS_ESP12
      #define DTR_ENABLE  12
    #endif
  #endif  // has_no_meter
#endif  // arduino_esp8266_generic
#ifdef HAS_NO_METER
    #define HOURS_FILE     "/TSThours.csv"
    #define DAYS_FILE      "/TSTdays.csv"
    #define MONTHS_FILE    "/TSTmonths.csv"
#else // not has_no_meter
    #define HOURS_FILE     "/PRDhours.csv"
    #define DAYS_FILE      "/PRDdays.csv"
    #define MONTHS_FILE    "/PRDmonths.csv"
#endif  // has_no_meter
#define SETTINGS_FILE      "/DSMRsettings.ini"
#define GUI_COLORS_FILE    "/DSMRchartColors.ini"
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
uint8_t   settingSleepTime; // needs to be declared before the oledStuff.h include
#ifdef HAS_OLED_SSD1306
  #include "oledStuff.h"
#endif
#include "networkStuff.h"

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
#ifdef USE_PRE40_PROTOCOL                                     //PRE40
  /* TimestampedFixedValue */ ,gas_delivered2                 //PRE40
#endif                                                        //PRE40
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

const char *weekDayName[]  { "Unknown", "Zondag", "Maandag", "Dinsdag", "Woensdag"
                            , "Donderdag", "Vrijdag", "Zaterdag", "Unknown" };
const char *monthName[]    { "00", "Januari", "Februari", "Maart", "April", "Mei", "Juni", "Juli"
                            , "Augustus", "September", "Oktober", "November", "December", "13" };
const char *flashMode[]    { "QIO", "QOUT", "DIO", "DOUT", "Unknown" };

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
uint32_t  telegramInterval, noMeterWait, telegramCount, telegramErrors;
char      cMsg[150], fChar[10];
float     EnergyDelivered, EnergyReturned, prevEnergyDelivered=0.0, prevEnergyReturned=0.0;
float     PowerDelivered, PowerReturned, maxPowerDelivered, maxPowerReturned;
char      maxTimePD[7], maxTimePR[7]; // hh:mm
int32_t   PowerDelivered_l1, PowerDelivered_l2, PowerDelivered_l3;  // Watt in 1 watt resolution
int32_t   PowerReturned_l1,  PowerReturned_l2,  PowerReturned_l3;   // Watt in 1 watt resolution
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
int8_t    thisHour = -1, prevNtpHour = 0, thisDay = -1, thisMonth = -1, lastMonth, thisYear = 15;
int32_t   thisHourKey = -1;
int8_t    forceMonth = 0, forceDay = 0;
int8_t    showRawCount = 0;
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
char      settingMQTTbroker[101], settingMQTTuser[21], settingMQTTpasswd[21], settingMQTTtopTopic[21];
uint32_t  settingMQTTinterval;

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

#if defined(USE_NTP_TIME)                                                               //USE_NTP
    time_t t = now(); // store the current time in time variable t                      //USE_NTP
    sprintf(cMsg, "%02d%02d%02d%02d%02d%02dW\0\0", (year(t) - 2000), month(t), day(t)   //USE_NTP
                                                 , hour(t), minute(t), second(t));      //USE_NTP
    pTimestamp = cMsg;                                                                  //USE_NTP
  //_dThis = true;                                                                      //USE_NTP
  //Debugf("Time from NTP is [%s]\n", pTimestamp.c_str());                              //USE_NTP
#else   //                                                                              //else
    pTimestamp                        = DSMRdata.timestamp;                             //
#endif                                                                                  //USE_NTP

    if (DSMRdata.equipment_id_present) {
            Equipment_Id              = DSMRdata.equipment_id;
    } else  Equipment_Id              = "Unknown";
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
    } else  GasEquipment_Id           = "Unknown";
    if (DSMRdata.gas_delivered_present) {
            GasDelivered              = (float)DSMRdata.gas_delivered;
 #ifdef USE_PRE40_PROTOCOL                                                      //PRE40
    } else if (DSMRdata.gas_delivered2_present) {                               //PRE40
            GasDelivered              = (float)DSMRdata.gas_delivered2;         //PRE40
 #endif                                                                         //PRE40
    } else  GasDelivered              = 0.0;

    EnergyDelivered   = EnergyDeliveredTariff1 + EnergyDeliveredTariff2; 
    EnergyReturned    = EnergyReturnedTariff1  + EnergyReturnedTariff2;
#endif  // has_no_meter
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
    
#ifdef SM_HAS_NO_FASE_INFO                                                                  //NO_FASE
    PowerDelivered_l1   = PowerDelivered * 1000;  // kW * 1000 => Watt                      //NO_FASE
    PowerDelivered_l2   = 0;                                                                //NO_FASE
    PowerDelivered_l3   = 0;                                                                //NO_FASE
                                                                                            //NO_FASE
    PowerReturned_l1    = PowerReturned * 1000;   // kW * 1000 => Watt                      //NO_FASE
    PowerReturned_l2    = 0;                                                                //NO_FASE
    PowerReturned_l3    = 0;                                                                //NO_FASE
#endif                                                                                      //NO_FASE

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

    sprintf(cMsg, "%s - %s", DT.substring(0, 10).c_str(), DT.substring(11, 16).c_str());
    oled_Print_Msg(0, cMsg, 0);
    sprintf(cMsg, "-Power%7d Watt", (PowerDelivered_l1 + PowerDelivered_l2 + PowerDelivered_l3));
    oled_Print_Msg(1, cMsg, 0);
    sprintf(cMsg, "+Power%7d Watt", (PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3));
    oled_Print_Msg(2, cMsg, 0);
    // rotate output last line 
    if (msgMode == 1 || msgMode== 2)
          sprintf(cMsg, "Telegram:%5d/%3d", telegramCount, telegramErrors);
    else if (msgMode == 3)
          sprintf(cMsg, "Up: %s", upTime().c_str());
    else if (msgMode == 4)
          sprintf(cMsg, "Telegram:%5d/%3d", telegramCount, telegramErrors);
    else if (msgMode == 5) 
          sprintf(cMsg, "WiFi RSSI:%4d dBm", WiFi.RSSI());
    else if (msgMode == 6) 
          sprintf(cMsg, "Heap:%7d Bytes", ESP.getFreeHeap());
    else if (msgMode == 7) 
          sprintf(cMsg, "IP %s", WiFi.localIP().toString().c_str());
    else  sprintf(cMsg, "Telegram:%5d/%3d", telegramCount, telegramErrors);
    oled_Print_Msg(3, cMsg, 0);
    msgMode++;
    if (msgMode < 1 || msgMode > 8) msgMode = 1;
#endif  // has_oled_ssd1206


//================= handle Month change ======================================================
    if (thisMonth != MonthFromTimestamp(pTimestamp)) {
      if (Verbose1) Debugf("processData(): thisYear[20%02d] => thisMonth[%02d]\r\n", thisYear, thisMonth);
      _dThis = true;
      if (thisMonth > -1) {
        Debugf("processData(): Saving data for thisMonth[20%02d-%02d] \n", thisYear, thisMonth);
        sprintf(cMsg, "%02d%02d", thisYear, thisMonth);
        monthData.Label  = String(cMsg).toInt();
        fileWriteData(MONTHS, monthData);
        if (Verbose1) Debugf("processData(): monthData for [20%04ld] saved!\r\n", String(cMsg).toInt());
      }
      
      //-- write same data to the new month -------
      thisMonth = MonthFromTimestamp(pTimestamp);
      thisYear  = YearFromTimestamp(pTimestamp);
      sprintf(cMsg, "%02d%02d", thisYear, thisMonth);
      monthData.Label  = String(cMsg).toInt();
      fileWriteData(MONTHS, monthData);

    } // if (thisMonth != MonthFromTimestamp(pTimestamp)) 
    
//================= handle Day change ======================================================
    if (thisDay != DayFromTimestamp(pTimestamp)) {
      _dThis = true;
      if (thisDay > -1) {
        Debugf("Saving data for Day[%02d]\r\n", thisDay);
        fileWriteData(DAYS, dayData);
      }
      maxPowerDelivered = PowerDelivered_l1 + PowerDelivered_l2 + PowerDelivered_l3;
      sprintf(maxTimePD, "00:01");
      maxPowerReturned  = PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3;
      sprintf(maxTimePR, "00:01");
      
      //-- write same data to the new day ---------------
      sprintf(cMsg, "%02d%02d%02d", YearFromTimestamp(pTimestamp), MonthFromTimestamp(pTimestamp), DayFromTimestamp(pTimestamp));
      dayData.Label = String(cMsg).toInt();
      fileWriteData(DAYS, dayData);
      thisDay           = DayFromTimestamp(pTimestamp);
    }

//================= handle Hour change ======================================================
    _dThis = true;
    Debugf("actual hourKey is [%08d] NEW hourKey is [%08d]\n", thisHourKey, HoursKeyTimestamp(pTimestamp));
    if (thisHourKey != HoursKeyTimestamp(pTimestamp)) {
      if (thisHourKey > -1) {
        _dThis = true;
        Debugf("Saving data for thisHourKey[%08d]\n", thisHourKey);
        hourData.Label = thisHourKey;
        fileWriteData(HOURS, hourData);
      }
        
      //-- write same data to the new hour -------------------
      thisHourKey    = HoursKeyTimestamp(pTimestamp);
      hourData.Label = thisHourKey;
      fileWriteData(HOURS, hourData);
      
    } // if (thisHourKey != HourFromTimestamp(pTimestamp)) 
   
} // processData()


//===========================================================================================
void setup() {
//===========================================================================================
#ifdef USE_PRE40_PROTOCOL                                                         //PRE40
//Serial.begin(115200);                                                           //DEBUG
  Serial.begin(9600, SERIAL_7E1);                                                 //PRE40
#else   // not use_dsmr_30                                                        //PRE40
  Serial.begin(115200, SERIAL_8N1);
#endif  // use_dsmr_30
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLASH_BUTTON, INPUT);
#ifdef DTR_ENABLE
  pinMode(DTR_ENABLE, OUTPUT);
#endif
  
  Serial.printf("\n\nBooting....[%s]\n\n", String(_FW_VERSION).c_str());

#ifdef HAS_OLED_SSD1306
  oledSleepTimer = millis() + (10 * 60000); // initially 10 minutes on
  oled_Init();
  oled_Clear();  // clear the screen so we can paint the menu.
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  sprintf(cMsg, "(c) 2019 [%s]", String(_FW_VERSION).substring(0,6).c_str());
  oled_Print_Msg(1, cMsg, 0);
  oled_Print_Msg(2, " Willem Aandewiel", 0);
  oled_Print_Msg(3, " >> Have fun!! <<", 1000);
  yield();
#else  // don't blink if oled-screen attatched
  for(int I=0; I<8; I++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(2000);
  }
#endif
  digitalWrite(LED_BUILTIN, LED_OFF);  // HIGH is OFF
  lastReset     = ESP.getResetReason();

#ifdef HAS_OLED_SSD1306
  oled_Clear();  // clear the screen 
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  oled_Print_Msg(1, "Connecting to WiFi", 500);
#endif  // has_oled_ssd1306
  digitalWrite(LED_BUILTIN, LED_ON);
  startWiFi();
#ifdef HAS_OLED_SSD1306
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  oled_Print_Msg(1, WiFi.SSID(), 0);
  sprintf(cMsg, "IP %s", WiFi.localIP().toString().c_str());
  oled_Print_Msg(2, cMsg, 1500);
#endif  // has_oled_ssd1306
  digitalWrite(LED_BUILTIN, LED_OFF);

  startTelnet();
#ifdef HAS_OLED_SSD1306
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  oled_Print_Msg(3, "telnet started (23)", 2500);
#endif  // has_oled_ssd1306
  
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
  oled_Print_Msg(3, "mDNS started", 1500);
#endif  // has_oled_ssd1306
  MDNS.addService("arduino", "tcp", 81);
  MDNS.port(81);  // webSockets


#if defined(USE_NTP_TIME)                                   //USE_NTP
//================ startNTP =========================================
  #ifdef HAS_OLED_SSD1306                                   //USE_NTP
    oled_Print_Msg(3, "setup NTP server", 100);             //USE_NTP
  #endif  // has_oled_ssd1306                               //USE_NTP
                                                            //USE_NTP
  if (!startNTP()) {                                        //USE_NTP
    _dThis = true;                                          //USE_NTP
    Debugln("ERROR!!! No NTP server reached!\n");           //USE_NTP
  #ifdef HAS_OLED_SSD1306                                   //USE_NTP
    oled_Print_Msg(0, "** DSMRloggerWS **", 0);             //USE_NTP
    oled_Print_Msg(2, "no response from", 100);             //USE_NTP
    oled_Print_Msg(2, "NTP server's", 100);                 //USE_NTP 
    oled_Print_Msg(3, "Reboot System", 2000);               //USE_NTP
  #endif  // has_oled_ssd1306                               //USE_NTP
    delay(2000);                                            //USE_NTP
    ESP.restart();                                          //USE_NTP
    delay(3000);                                            //USE_NTP
  }                                                         //USE_NTP
  #ifdef HAS_OLED_SSD1306                                   //USE_NTP
    oled_Print_Msg(0, "** DSMRloggerWS **", 0);             //USE_NTP
    oled_Print_Msg(3, "NTP started", 1500);                 //USE_NTP
    prevNtpHour = hour();                                   //USE_NTP
  #endif                                                    //USE_NTP
                                                            //USE_NTP
#endif  //USE_NTP_TIME                                      //USE_NTP

//================ SPIFFS ===========================================
  if (!SPIFFS.begin()) {
    _dThis = true;
    Debugln("SPIFFS Mount failed");   // Serious problem with SPIFFS 
    SPIFFSmounted = false;
#ifdef HAS_OLED_SSD1306
    oled_Print_Msg(0, "** DSMRloggerWS **", 0);
    oled_Print_Msg(3, "SPIFFS FAILED!", 2000);
#endif  // has_oled_ssd1306
    
  } else { 
    _dThis = true;
    Debugln("SPIFFS Mount succesfull");
    SPIFFSmounted = true;
#ifdef HAS_OLED_SSD1306
    oled_Print_Msg(0, "** DSMRloggerWS **", 0);
    oled_Print_Msg(3, "SPIFFS mounted", 1500);
#endif  // has_oled_ssd1306
  }
//=============end SPIFFS =========================================

  sprintf(cMsg, "Last reset reason: [%s]", ESP.getResetReason().c_str());
  Debugln(cMsg);

  Serial.print("use 'telnet ");
  Serial.print (WiFi.localIP());
  Serial.println("' for further debugging");

//===========================================================================================

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

//--- read most recent data from SPIFFS -----
  monthData = fileReadData(MONTHS, 1);
  dayData   = fileReadData(DAYS, 1);
  hourData  = fileReadData(HOURS, 1);

#if defined(USE_NTP_TIME)                                                           //USE_NTP
  time_t t = now(); // store the current time in time variable t                    //USE_NTP
  sprintf(cMsg, "%02d%02d%02d%02d%02d%02dW\0\0", (year(t) - 2000), month(t), day(t) //USE_NTP
                                               , hour(t), minute(t), second(t));    //USE_NTP
  pTimestamp = cMsg;                                                                //USE_NTP
  _dThis = true;                                                                    //USE_NTP
  Debugf("Time is set to [%s] from NTP\n", cMsg);                                   //USE_NTP
  thisYear  = (year(t) - 2000);                                                     //USE_NTP
  thisMonth = month(t);                                                             //USE_NTP
  thisDay   = day(t);                                                               //USE_NTP
  thisHour  = hour(t);                                                              //USE_NTP
                                                                                    //USE_NTP
#else // not use_ntp_time
  label2Fields(monthData.Label, thisYear, thisMonth);
  label2Fields(dayData.Label,   thisYear, thisMonth, thisDay);
  label2Fields(hourData.Label,  thisYear, thisMonth, thisDay, thisHour);
  if (thisYear == 0) thisYear = 10;
  sprintf(cMsg, "%02d%02d%02d%02d0101W", thisYear, thisMonth, thisDay, thisHour);
  pTimestamp = cMsg;
  _dThis = true;
  Debugf("Time is set to [%s] from hourData\n", cMsg);
#endif  // use_dsmr_30

#ifdef HAS_OLED_SSD1306
  sprintf(cMsg, "DT: %02d%02d%02d%02d0101W", thisYear, thisMonth, thisDay, thisHour);
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  oled_Print_Msg(3, cMsg, 1500);
#endif  // has_oled_ssd1306

  epoch(pTimestamp);

  readSettings();
  readColors();

#ifdef USE_MQTT                                               //USE_MQTT
  startMQTT();
  #ifdef HAS_OLED_SSD1306                                     //USE_MQTT
    oled_Print_Msg(0, "** DSMRloggerWS **", 0);               //USE_MQTT
    oled_Print_Msg(3, "MQTT server set!", 1500);              //USE_MQTT
  #endif  // has_oled_ssd1306                                 //USE_MQTT
#endif                                                        //USE_MQTT


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
#endif  // has_no_meter

  telegramCount   = 0;
  telegramErrors  = 0;

  _dThis = true;
  if (SPIFFS.exists("/DSMRlogger.html")) {
    Debugln("Found DSMRlogger.html -> normal operation!");
#ifdef HAS_OLED_SSD1306
    oled_Print_Msg(0, "OK, found:", 0);
    oled_Print_Msg(1, "DSMRlogger.html", 0);
    oled_Print_Msg(2, "Continue Normal", 0);
    oled_Print_Msg(3, "Operation ;-)", 500);
#endif  // has_oled_ssd1306
    httpServer.serveStatic("/",               SPIFFS, "/DSMRlogger.html");
    httpServer.serveStatic("/DSMRlogger.html",SPIFFS, "/DSMRlogger.html");
    httpServer.serveStatic("/index",          SPIFFS, "/DSMRlogger.html");
    httpServer.serveStatic("/index.html",     SPIFFS, "/DSMRlogger.html");
  } else {
    Debugln("Oeps! DSMRlogger.html not found -> present errorIndexPage!");
#ifdef HAS_OLED_SSD1306
    oled_Print_Msg(0, "OEPS!", 0);
    oled_Print_Msg(1, "Couldn't find ", 0);
    oled_Print_Msg(2, "DSMRlogger.html", 0);
    oled_Print_Msg(3, "Start ErrorPage", 2000);
#endif  // has_oled_ssd1306

    httpServer.on("/", handleErrorIndexPage);
  }
  httpServer.serveStatic("/DSMRlogger.css",   SPIFFS, "/DSMRlogger.css");
  httpServer.serveStatic("/DSMRlogger.js",    SPIFFS, "/DSMRlogger.js");
  httpServer.serveStatic("/DSMReditor.html",  SPIFFS, "/DSMReditor.html");
  httpServer.serveStatic("/DSMReditor.js",    SPIFFS, "/DSMReditor.js");
  httpServer.serveStatic("/dialog.css",       SPIFFS, "/dialog.css");
  httpServer.serveStatic("/dialog.js",        SPIFFS, "/dialog.js");
  httpServer.serveStatic("/DSMRgraphics.js",  SPIFFS, "/DSMRgraphics.js");
  httpServer.serveStatic("/FSexplorer.png",   SPIFFS, "/FSexplorer.png");

  httpServer.on("/restAPI", HTTP_GET, restAPI);
  httpServer.on("/restapi", HTTP_GET, restAPI);
  httpServer.on("/ReBoot", HTTP_POST, handleReBoot);

  httpServer.on("/FSexplorer", HTTP_POST, handleFileDelete);
  httpServer.on("/FSexplorer", handleRoot);
  httpServer.on("/FSexplorer/upload", HTTP_POST, []() {
    httpServer.send(200, "text/plain", "");
  }, handleFileUpload);

  httpServer.onNotFound([]() {
    if (httpServer.uri() == "/update") {
      httpServer.send(200, "text/html", "/update" );
    } else {
      _dThis = true;
      Debugf("onNotFound(%s)\n", httpServer.uri().c_str());
      if (httpServer.uri() == "/") {
        reloadPage("/");
      }
    }
    if (!handleFileRead(httpServer.uri())) {
      httpServer.send(404, "text/plain", "FileNotFound");
    }
  });

  httpServer.begin();
  _dThis = true;
  Debugln( "HTTP server started" );
  for (int i = 0; i< 10; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(200);
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
#endif // is_esp12

  sprintf(cMsg, "Last reset reason: [%s]", ESP.getResetReason().c_str());
  Debugln(cMsg);

  telegramInterval = millis() + 5000;
  noMeterWait      = millis() + 5000;
  upTimeSeconds    = (millis() / 1000) + 50;
  nextSecond       = millis() + 1000;

#ifdef HAS_OLED_SSD1306
    oled_Print_Msg(0, "** DSMRloggerWS **", 0);
    oled_Print_Msg(1, "Startup complete", 0);
    oled_Print_Msg(2, "Wait for first", 0);
    oled_Print_Msg(3, "telegram .....", 500);
#endif  // has_oled_ssd1306

  
} // setup()


//===========================================================================================
void loop () {
//===========================================================================================
  httpServer.handleClient();
  webSocket.loop();
  MDNS.update();
  handleKeyInput();
  handleRefresh();
  handleMQTT();
  
#if defined(USE_NTP_TIME)                                                         //USE_NTP
  if (timeStatus() == timeNeedsSync || prevNtpHour != hour()) {                   //USE_NTP
    prevNtpHour = hour();                                                         //USE_NTP
    setSyncProvider(getNtpTime);                                                  //USE_NTP
    setSyncInterval(600);                                                         //USE_NTP
  }                                                                               //USE_NTP
#endif                                                                            //USE_NTP

#ifdef HAS_OLED_SSD1306
  checkFlashButton();
#endif

  if (!showRaw) {
    slimmeMeter.loop();
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
  
#ifdef HAS_NO_METER
  #include "has_no_meter.h"
  
#else
  //---- this part is processed in 'normal' operation mode!
  if (showRaw) {
#ifdef HAS_OLED_SSD1306
      if (showRawCount == 0) {
        oled_Print_Msg(0, "** DSMRloggerWS **", 0);
        oled_Print_Msg(1, "-------------------------",0);
        oled_Print_Msg(2, "Raw Format",0);
        sprintf(cMsg, "Raw Count %4d", showRawCount);
        oled_Print_Msg(3, cMsg, 0);
      }
#endif

      while(Serial.available() > 0) {   
        char rIn = Serial.read();       
        if (rIn == '!') {
          showRawCount++;
#ifdef HAS_OLED_SSD1306
          sprintf(cMsg, "Raw Count %4d", showRawCount);
          oled_Print_Msg(3, cMsg, 0);
#endif
        }
        TelnetStream.write((char)rIn);
      }   // while Serial.available()
      
      if (showRawCount > 20) {
        showRaw       = false;
        showRawCount  = 0;
      }
  } else {
      if (slimmeMeter.available()) {
        Debugln("\n[Time]=====[FreeHeap]=[Function]==(line)====================================================\r");
        telegramCount++;
        _dThis = true;
        Debugf("read telegram [%d] => [%s]\r\n", telegramCount, pTimestamp.c_str());
        MyData    DSMRdata;
        String    DSMRerror;

        if (slimmeMeter.parse(&DSMRdata, &DSMRerror)) {  // Parse succesful, print result
          if (telegramCount > 1563000000) {
            delay(1000);
            ESP.reset();
            delay(1000);
          }
          digitalWrite(LED_BUILTIN, LED_OFF);
          processData(DSMRdata);
          sendMQTTData();

          if (Verbose1) {
            DSMRdata.applyEach(showValues());
            printData();
          }
          
        } else {                                    // Parser error, print error
          telegramErrors++;
          _dThis = true;
          Debugf("Parse error\r\n%s\r\n\r\n", DSMRerror.c_str());
        }
        
      } // if (slimmeMeter.available()) 

  }   
#endif // else has_no_meter

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
