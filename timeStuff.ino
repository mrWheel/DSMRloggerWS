/*
***************************************************************************  
**  Program  : timeStuff, part of DSMRloggerWS
**  Version  : v1.0.3
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

static time_t ntpTimeSav;

//===========================================================================================
String buildDateTimeString(String timeStamp) {
//===========================================================================================
  String DateTime = "";

  DateTime   = "20" + timeStamp.substring(0, 2);
  DateTime  += "-"  + timeStamp.substring(2, 4);
  DateTime  += "-"  + timeStamp.substring(4, 6);
  DateTime  += " "  + timeStamp.substring(6, 8);
  DateTime  += ":"  + timeStamp.substring(8, 10);
  DateTime  += ":"  + timeStamp.substring(10, 12);
  return DateTime;
    
} // buildDateTimeString()

//===========================================================================================
String getDayName(int weekDayNr) {
//===========================================================================================

  if (weekDayNr >=1 && weekDayNr <= 7)
      return weekDayName[weekDayNr];
      
  return weekDayName[0];    
    
} // getDayName()


//===========================================================================================
int8_t SecondFromTimestamp(String timeStamp) {
//===========================================================================================
  return timeStamp.substring(10, 12).toInt();
    
} // SecondFromTimestamp()

//===========================================================================================
int8_t MinuteFromTimestamp(String timeStamp) {
//===========================================================================================
  return timeStamp.substring(8, 10).toInt();
    
} // MinuteFromTimestamp()

//===========================================================================================
int8_t HourFromTimestamp(String timeStamp) {
//===========================================================================================
  return timeStamp.substring(6, 8).toInt();
    
} // HourFromTimestamp()

//===========================================================================================
int8_t DayFromTimestamp(String timeStamp) {
//===========================================================================================
  return timeStamp.substring(4, 6).toInt();
    
} // DayFromTimestamp()

//===========================================================================================
int8_t MonthFromTimestamp(String timeStamp) {
  return timeStamp.substring(2, 4).toInt();
    
} // MonthFromTimestamp()

//===========================================================================================
int8_t YearFromTimestamp(String timeStamp) {
  return timeStamp.substring(0, 2).toInt();
    
} // YearFromTimestamp()

//===========================================================================================
int32_t HoursKeyTimestamp(String timeStamp) {
//===========================================================================================
  return timeStamp.substring(0, 8).toInt();
    
} // HourFromTimestamp()

//===========================================================================================
time_t epoch(String timeStamp) {
//===========================================================================================
/**  
  DebugTf("DateTime: [%02d]-[%02d]-[%02d] [%02d]:[%02d]:[%02d]\r\n"
                                                                 ,DayFromTimestamp(timeStamp)
                                                                 ,MonthFromTimestamp(timeStamp)
                                                                 ,YearFromTimestamp(timeStamp)
                                                                 ,HourFromTimestamp(timeStamp)
                                                                 ,MinuteFromTimestamp(timeStamp)
                                                                 ,0
                       );
**/  
  ntpTimeSav = now();
  
  setTime(HourFromTimestamp(timeStamp)
         ,MinuteFromTimestamp(timeStamp)
         ,SecondFromTimestamp(timeStamp)
         ,DayFromTimestamp(timeStamp)
         ,MonthFromTimestamp(timeStamp)
         ,YearFromTimestamp(timeStamp));

  return now();

} // epoch()


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
