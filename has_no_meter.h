/*
***************************************************************************  
**  Program  : has_no_meter.h, part of DSMRloggerWS
**  Version  : v1.0.3
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

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
/**/    DebugTln("\n==================================================================\r");
/**/    DebugTf("NO METER! read telegram [%d]\r\n", telegramCount);
/**/    if (telegramCount > 1563000000) {
/**/       ESP.reset();
/**/    }
/**/    sprintf(cMsg, "%02d%02d%02d%02d%02d15S", sYear, sMonth, sDay, sHour, sMinute);
/**/    DSMRdata.timestamp = String(cMsg);
/**/    pTimestamp  = DSMRdata.timestamp;
/**/    if (Verbose1) DebugTf("pTimestamp [%s] sYear[%02d] sMonth[%02d] sDay[%02d] sHour[%02d] sMinute[%02d]\r\n"
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
