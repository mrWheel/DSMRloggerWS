/*
***************************************************************************  
**  Program  : DSMRloggerWS (WebSockets)
*/
#define _FW_VERSION "v1.0.4 (07-12-2019)"
/*
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*      
  Arduino-IDE settings for DSMR-logger Version 4 (ESP-12):

    - Board: "Generic ESP8266 Module"
    - Flash mode: "DOUT" | "DIO"    // if you change from one to the other OTA may fail!
    - Flash size: "4M (1M SPIFFS)"  // ESP-01 "1M (256K SPIFFS)"  // PUYA flash chip won't work
    - DebugT port: "Disabled"
    - DebugT Level: "None"
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
#define HAS_OLED_SSD1306          // define if a 0.96" OLED display is present
//  #define HAS_OLED_SH1106           // define if a 1.3" OLED display is present
//  #define USE_PRE40_PROTOCOL        // define if Slimme Meter is pre DSMR 4.0 (2.2 .. 3.0)
//  #define USE_NTP_TIME              // define to generate Timestamp from NTP (Only Winter Time for now)
//  #define SM_HAS_NO_FASE_INFO       // if your SM does not give fase info use total delevered/returned
#define USE_MQTT                  // define if you want to use MQTT
#define USE_MINDERGAS             // define if you want to update mindergas (also add token down below)
//  #define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
//  #define HAS_NO_METER              // define if No "Slimme Meter" is attached (*TESTING*)
/******************** don't change anything below this comment **********************/

#include <TimeLib.h>            // https://github.com/PaulStoffregen/Time
#include <TelnetStream.h>       // Version 0.0.1 - https://github.com/jandrassy/TelnetStream
#include <ArduinoJson.h>

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
#if defined( HAS_OLED_SSD1306 ) && defined( HAS_OLED_SH1106 )
  #error Only one OLED display can be defined
#endif
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
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
uint32_t  telegramInterval, noMeterWait, telegramCount, telegramErrors, lastOledStatus;
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
bool      spiffsNotPopulated = false; // v1.0.3b
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

char      settingMindergasAuthtoken[21];
uint16_t  intStatuscodeMindergas=0; 
char      txtResponseMindergas[30];  
byte      byteUpdateMindergasCountdown = 0;
char      txtLastUpdateMindergas[30];

MyData    DSMR4mqtt;

struct showValues {
  template<typename Item>
  void apply(Item &i) {
    TelnetStream.print("showValues: ");
    if (i.present()) {
      TelnetStream.print(Item::name);
      TelnetStream.print(F(": "));
      TelnetStream.print(i.val());
      TelnetStream.print(Item::unit());
    } else {
      TelnetStream.print(F("<no value>"));
    }
    TelnetStream.println();
  }
};


//===========================================================================================
void displayStatus() {
//===========================================================================================
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
  switch(msgMode) {
    case 1:   sprintf(cMsg, "Up:%15.15s", upTime().c_str());
              break;
    case 2:   sprintf(cMsg, "WiFi RSSI:%4d dBm", WiFi.RSSI());
              break;
    case 3:   sprintf(cMsg, "Heap:%7d Bytes", ESP.getFreeHeap());
              break;
    case 4:   sprintf(cMsg, "IP %s", WiFi.localIP().toString().c_str());
              break;
    default:  sprintf(cMsg, "Telgrms:%6d/%3d", telegramCount, telegramErrors);
              msgMode = 0;
  }
  oled_Print_Msg(3, cMsg, 0);
  msgMode++;
#endif
  
} // displayStatus()


//===========================================================================================
void printData() {
//===========================================================================================
  String dateTime;

    DebugTln("\r");
    Debugln(F("-Totalen----------------------------------------------------------\r"));
    dateTime = buildDateTimeString(pTimestamp);
    sprintf(cMsg, "Datum / Tijd         :  %s\r", dateTime.c_str());
    Debugln(cMsg);

    //dtostrf(EnergyDelivered, 9, 3, fChar);
    sprintf(cMsg, "Energy Delivered     : %12.3fkWh\r", EnergyDelivered);
    Debugln(cMsg);

    //dtostrf(EnergyReturned, 9, 3, fChar);
    sprintf(cMsg, "Energy Returned      : %12.3fkWh\r", EnergyReturned);
    Debugln(cMsg);

    dtostrf(PowerDelivered, 9, 3, fChar);
    sprintf(cMsg, "Power Delivered      : %skW\r", fChar);
    Debugln(cMsg);

    dtostrf(PowerReturned, 9, 3, fChar);
    sprintf(cMsg, "Power Returned       : %skW\r", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerDelivered_l1, 9, 0, fChar);
    sprintf(cMsg, "Power Delivered (l1) : %sWatt\r", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerDelivered_l2, 9, 0, fChar);
    sprintf(cMsg, "Power Delivered (l2) : %sWatt\r", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerDelivered_l3, 9, 0, fChar);
    sprintf(cMsg, "Power Delivered (l3) : %sWatt\r", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerReturned_l1, 9, 0, fChar);
    sprintf(cMsg, "Power Returned (l1)  : %sWatt\r", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerReturned_l2, 9, 0, fChar);
    sprintf(cMsg, "Power Returned (l2)  : %sWatt\r", fChar);
    Debugln(cMsg);
    
    dtostrf(PowerReturned_l3, 9, 0, fChar);
    sprintf(cMsg, "Power Returned (l3)  : %sWatt\r", fChar);
    Debugln(cMsg);

    dtostrf(GasDelivered, 9, 3, fChar);
    sprintf(cMsg, "Gas Delivered        : %sm3\r", fChar);
    Debugln(cMsg);
    Debugln(F("==================================================================\r"));
  
} // printData()


//===========================================================================================
void processData(MyData DSMRdata) {
//===========================================================================================
//v1.0.3b  int8_t slot, nextSlot, prevSlot;
  
#ifndef HAS_NO_METER
    strcpy(Identification, DSMRdata.identification.c_str());
    P1_Version                        = DSMRdata.p1_version;

#if defined(USE_NTP_TIME)                                                               //USE_NTP
    time_t t = now(); // store the current time in time variable t                      //USE_NTP
    sprintf(cMsg, "%02d%02d%02d%02d%02d%02dW\0\0", (year(t) - 2000), month(t), day(t)   //USE_NTP
                                                 , hour(t), minute(t), second(t));      //USE_NTP
    pTimestamp = cMsg;                                                                  //USE_NTP
  //DebugTf("Time from NTP is [%s]\r\n", pTimestamp.c_str());                              //USE_NTP
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
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
    String DT   = buildDateTimeString(pTimestamp);

    sprintf(cMsg, "%s - %s", DT.substring(0, 10).c_str(), DT.substring(11, 16).c_str());
    oled_Print_Msg(0, cMsg, 0);
    sprintf(cMsg, "-Power%7d Watt", (PowerDelivered_l1 + PowerDelivered_l2 + PowerDelivered_l3));
    oled_Print_Msg(1, cMsg, 0);
    sprintf(cMsg, "+Power%7d Watt", (PowerReturned_l1 + PowerReturned_l2 + PowerReturned_l3));
    oled_Print_Msg(2, cMsg, 0);
#endif  // has_oled_ssd1206


//================= handle Month change ======================================================
    if (thisMonth != MonthFromTimestamp(pTimestamp)) {
      if (Verbose1) DebugTf("thisYear[20%02d] => thisMonth[%02d]\r\n", thisYear, thisMonth);
      if (thisMonth > -1) {
        DebugTf("Saving data for thisMonth[20%02d-%02d] \r\n", thisYear, thisMonth);
        sprintf(cMsg, "%02d%02d", thisYear, thisMonth);
        monthData.Label  = String(cMsg).toInt();
        fileWriteData(MONTHS, monthData);
        if (Verbose1) DebugTf("monthData for [20%04ld] saved!\r\n", String(cMsg).toInt());
      }
      
      //-- write same data to the new month -------
      thisMonth = MonthFromTimestamp(pTimestamp);
      thisYear  = YearFromTimestamp(pTimestamp);
      sprintf(cMsg, "%02d%02d", thisYear, thisMonth);
      monthData.Label  = String(cMsg).toInt();
      fileWriteData(MONTHS, monthData);

      DebugTf("Rollover on the Month: thisMonth [%02d%02d]\r\n", thisYear, thisMonth);
    } // if (thisMonth != MonthFromTimestamp(pTimestamp)) 
    
//================= handle Day change ======================================================
    if (thisDay != DayFromTimestamp(pTimestamp)) {
      DebugTf("actual thisDay is [%08d] NEW thisDay is [%08d]\r\n", thisDay, DayFromTimestamp(pTimestamp));
      // Once a day setup mindergas update cycle
      if (thisDay > -1) {
        DebugTf("Saving data for Day[%02d]\r\n", thisDay);
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
      DebugTf("Rollover on the Day: thisDay [%02d]\r\n", thisDay);
    }

//================= handle Hour change ======================================================
    if (Verbose1) DebugTf("actual hourKey is [%08d] NEW hourKey is [%08d]\r\n", thisHourKey, HoursKeyTimestamp(pTimestamp));
    if (thisHourKey != HoursKeyTimestamp(pTimestamp)) {
      
      if (thisHourKey > -1) {
        DebugTf("Saving data for thisHourKey[%08d]\r\n", thisHourKey);
        hourData.Label = thisHourKey;
        fileWriteData(HOURS, hourData);
      }
        
      //-- write same data to the new hour -------------------
      thisHourKey    = HoursKeyTimestamp(pTimestamp);
      hourData.Label = thisHourKey;
      fileWriteData(HOURS, hourData);
      DebugTf("Rollover on the Hour: thisHourKey is [%08d]\r\n", thisHourKey);
    } // if (thisHourKey != HourFromTimestamp(pTimestamp)) 

} // processData()


//===========================================================================================
void setup() {
//===========================================================================================
txtResponseMindergas[0] = '\0';
strcpy(txtLastUpdateMindergas, "Waiting for first telegram..."); 

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
  
  Serial.printf("\n\nBooting....[%s]\r\n\r\n", String(_FW_VERSION).c_str());

#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
  oledSleepTimer = millis() + (10 * 60000); // initially 10 minutes on
  oled_Init();
  oled_Clear();  // clear the screen so we can paint the menu.
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  int8_t sPos = String(_FW_VERSION).indexOf(' ');
  sprintf(cMsg, "(c)2019 [%s]", String(_FW_VERSION).substring(0,sPos).c_str());
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

  startTelnet();
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  oled_Print_Msg(3, "telnet (poort 23)", 2500);
#endif  // has_oled_ssd1306

//================ SPIFFS ===========================================
  if (!SPIFFS.begin()) {
    DebugTln("SPIFFS Mount failed\r");   // Serious problem with SPIFFS 
    SPIFFSmounted = false;
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
    oled_Print_Msg(0, "** DSMRloggerWS **", 0);
    oled_Print_Msg(3, "SPIFFS FAILED!", 2000);
#endif  // has_oled_ssd1306
    
  } else { 
    DebugTln("SPIFFS Mount succesfull\r");
    SPIFFSmounted = true;
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
    oled_Print_Msg(0, "** DSMRloggerWS **", 0);
    oled_Print_Msg(3, "SPIFFS mounted", 1500);
#endif  // has_oled_ssd1306
  }
//=============now test if SPIFFS is correct populated!============
  DSMRfileExist("/DSMRlogger.html");
  DSMRfileExist("/DSMRlogger.js");
  DSMRfileExist("/DSMRgraphics.js");
  DSMRfileExist("/DSMRlogger.css");
  DSMRfileExist("/DSMReditor.html");
  DSMRfileExist("/DSMReditor.js");
  DSMRfileExist("/FSexplorer.html");
  DSMRfileExist("/FSexplorer.css");
//=============end SPIFFS =========================================

#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
  oled_Clear();  // clear the screen 
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  oled_Print_Msg(1, "Verbinden met WiFi", 500);
#endif  // has_oled_ssd1306
  digitalWrite(LED_BUILTIN, LED_ON);
  if (WiFi.status() != WL_CONNECTED)  //if NOT connected start WiFi
    startWiFi();
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  oled_Print_Msg(1, WiFi.SSID(), 0);
  sprintf(cMsg, "IP %s", WiFi.localIP().toString().c_str());
  oled_Print_Msg(2, cMsg, 1500);
#endif  // has_oled_ssd1306
  digitalWrite(LED_BUILTIN, LED_OFF);
  
  Debugln("");
  Debug ( "Connected to " ); Debugln (WiFi.SSID());
  Debug ( "IP address: " );  Debugln (WiFi.localIP());

  for (int L=0; L < 10; L++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(200);
  }
  digitalWrite(LED_BUILTIN, LED_OFF);
  
  startMDNS(_HOSTNAME);
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
  oled_Print_Msg(3, "mDNS gestart", 1500);
#endif  // has_oled_ssd1306
  MDNS.addService("arduino", "tcp", 81);
  MDNS.port(81);  // webSockets


#if defined(USE_NTP_TIME)                                   //USE_NTP
//================ startNTP =========================================
  #if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )                                   //USE_NTP
    oled_Print_Msg(3, "setup NTP server", 100);             //USE_NTP
  #endif  // has_oled_ssd1306                               //USE_NTP
                                                            //USE_NTP
  if (!startNTP()) {                                        //USE_NTP
    DebugTln("ERROR!!! No NTP server reached!\r\n\r");      //USE_NTP
  #if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )                                   //USE_NTP
    oled_Print_Msg(0, "** DSMRloggerWS **", 0);             //USE_NTP
    oled_Print_Msg(2, "geen reactie van", 100);             //USE_NTP
    oled_Print_Msg(2, "NTP server's", 100);                 //USE_NTP 
    oled_Print_Msg(3, "Reboot DSMR-logger", 2000);          //USE_NTP
  #endif  // has_oled_ssd1306                               //USE_NTP
    delay(2000);                                            //USE_NTP
    ESP.restart();                                          //USE_NTP
    delay(3000);                                            //USE_NTP
  }                                                         //USE_NTP
  #if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )                                   //USE_NTP
    oled_Print_Msg(0, "** DSMRloggerWS **", 0);             //USE_NTP
    oled_Print_Msg(3, "NTP gestart", 1500);                 //USE_NTP
    prevNtpHour = hour();                                   //USE_NTP
  #endif                                                    //USE_NTP
                                                            //USE_NTP
#endif  //USE_NTP_TIME                                      //USE_NTP

  sprintf(cMsg, "Last reset reason: [%s]\r", ESP.getResetReason().c_str());
  DebugTln(cMsg);

  Serial.print("\nGebruik 'telnet ");
  Serial.print (WiFi.localIP());
  Serial.println("' voor verdere debugging\r\n");

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
  DebugTf("Time is set to [%s] from NTP\r\n", cMsg);                                //USE_NTP
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
  DebugTf("Time is set to [%s] from hourData\r\n", cMsg);
#endif  // use_dsmr_30

#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
  sprintf(cMsg, "DT: %02d%02d%02d%02d0101W", thisYear, thisMonth, thisDay, thisHour);
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);
  oled_Print_Msg(3, cMsg, 1500);
#endif  // has_oled_ssd1306

  epoch(pTimestamp);

  readSettings(false);
  readColors(false);

#ifdef USE_MQTT                                               //USE_MQTT
  startMQTT();
  #if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )                                     //USE_MQTT
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

  if (!spiffsNotPopulated) {
    DebugTln("SPIFFS correct populated -> normal operation!\r");
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
    oled_Print_Msg(0, "** DSMRloggerWS **", 0); 
    oled_Print_Msg(1, "OK, SPIFFS correct", 0);
    oled_Print_Msg(2, "Verder met normale", 0);
    oled_Print_Msg(3, "Verwerking ;-)", 2500);
#endif  // has_oled_ssd1306
    httpServer.serveStatic("/",               SPIFFS, "/DSMRlogger.html");
    httpServer.serveStatic("/DSMRlogger.html",SPIFFS, "/DSMRlogger.html");
    httpServer.serveStatic("/index",          SPIFFS, "/DSMRlogger.html");
    httpServer.serveStatic("/index.html",     SPIFFS, "/DSMRlogger.html");
  } else {
    DebugTln("Oeps! not all files found on SPIFFS -> present FSexplorer!\r");
    spiffsNotPopulated = true;
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
    oled_Print_Msg(0, "!OEPS! niet alle", 0);
    oled_Print_Msg(1, "files op SPIFFS", 0);
    oled_Print_Msg(2, "gevonden! (fout!)", 0);
    oled_Print_Msg(3, "Start FSexplorer", 2000);
#endif  // has_oled_ssd1306
  }
  if (spiffsNotPopulated) {
    DebugTln("Setting Alternative Path's ..");
    //httpServer.on("/",                handleFSexplorer); // v1.0.3b
    //httpServer.on("/DSMRlogger.html", handleFSexplorer);
    //httpServer.on("/index",           handleFSexplorer);
    //httpServer.serveStatic("DSMRlogger.html", SPIFFS, "/FSexplorer.html");

  }
  setupFSexplorer();
  httpServer.serveStatic("/DSMRlogger.css",   SPIFFS, "/DSMRlogger.css");
  httpServer.serveStatic("/DSMRlogger.js",    SPIFFS, "/DSMRlogger.js");
  httpServer.serveStatic("/DSMReditor.html",  SPIFFS, "/DSMReditor.html");
  httpServer.serveStatic("/DSMReditor.js",    SPIFFS, "/DSMReditor.js");
  httpServer.serveStatic("/DSMRgraphics.js",  SPIFFS, "/DSMRgraphics.js");
  httpServer.serveStatic("/FSexplorer.png",   SPIFFS, "/FSexplorer.png");

  httpServer.on("/restAPI", HTTP_GET, restAPI);
  httpServer.on("/restapi", HTTP_GET, restAPI);

  httpServer.begin();
  DebugTln( "HTTP server gestart\r" );
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )                                     //HAS_OLED
  oled_Clear();                                             //HAS_OLED
  oled_Print_Msg(0, "** DSMRloggerWS **", 0);               //HAS_OLED
  oled_Print_Msg(2, "HTTP server ..", 0);                   //HAS_OLED
  oled_Print_Msg(3, "gestart (poort 80)", 0);               //HAS_OLED
#endif  // has_oled_ssd1306                                 //HAS_OLED

  for (int i = 0; i< 10; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(250);
  }

  DebugTln("Enable slimmeMeter..\r");
  delay(100);
  slimmeMeter.enable(true);

  //test(); monthTabel

  DebugTf("Startup complete! pTimestamp[%s]\r\n", pTimestamp.c_str());  

#ifdef IS_ESP12
  #ifndef HAS_NO_METER
    Serial.swap();
  #endif
#endif // is_esp12

  sprintf(cMsg, "Last reset reason: [%s]\r", ESP.getResetReason().c_str());
  DebugTln(cMsg);

  telegramInterval = millis() + 5000;
  noMeterWait      = millis() + 5000;
  upTimeSeconds    = (millis() / 1000) + 50;
  nextSecond       = millis() + 1000;

#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
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

  //once every second, increment uptime seconds
    if (millis() > nextSecond) {
    nextSecond += 1000; // nextSecond is ahead of millis() so it will "rollover" 
    upTimeSeconds++;    // before millis() and this will probably work just fine

    #ifdef USE_MINDERGAS
      handleMindergas();
    #endif //Mindergas
  }
  
#if defined(USE_NTP_TIME)                                                         //USE_NTP
  if (timeStatus() == timeNeedsSync || prevNtpHour != hour()) {                   //USE_NTP
    prevNtpHour = hour();                                                         //USE_NTP
    setSyncProvider(getNtpTime);                                                  //USE_NTP
    setSyncInterval(600);                                                         //USE_NTP
  }                                                                               //USE_NTP
#endif                                                                            //USE_NTP

#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
  checkFlashButton();
  if (millis() - lastOledStatus > 5000) {
    lastOledStatus = millis();
    displayStatus();
  }
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
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
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
#if defined( HAS_OLED_SSD1306 ) || defined( HAS_OLED_SH1106 )
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
        DebugTln(F("\r\n[Time----][FreeHeap/mBlck][Function----(line):\r"));
        // Voorbeeld: [21:00:11][   9880/  8960] loop        ( 997): read telegram [28] => [140307210001S]
        telegramCount++;
        DebugTf("read telegram [%d] => [%s]\r\n", telegramCount, pTimestamp.c_str());
        MyData    DSMRdata;
        String    DSMRerror;
        DSMR4mqtt = DSMRdata;
        
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
          DebugTf("Parse error\r\n%s\r\n\r\n", DSMRerror.c_str());
        }
        
      } // if (slimmeMeter.available()) 

  }   
#endif // else has_no_meter



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
