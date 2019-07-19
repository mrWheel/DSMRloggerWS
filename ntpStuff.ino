/*
***************************************************************************  
**  Program  : ntpStuff, part of DSMRloggerWS
**  Version  : v0.4.4
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
#ifdef SM_GIVES_NO_TIMESTAMP

#include <WiFiUdp.h>            //                - part of ESP8266 Core https://github.com/esp8266/Arduino
WiFiUDP                 Udp;

const int         timeZone = 1;       // Central European (Winter) Time
unsigned int      localPort = 8888;   // local port to listen for UDP packets

// NTP Servers:
static const char ntpServerName1[] = "time.google.com";
static const char ntpServerName0[] = "nl.pool.ntp.org";
static const char ntpServerName2[] = "0.nl.pool.ntp.org";
static const char ntpServerName3[] = "1.nl.pool.ntp.org";
static const char ntpServerName4[] = "3.nl.pool.ntp.org";

//const int timeZone = 1;     // Central European (Winter) Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

static int        ntpServerNr = 0;
static bool       externalNtpTime = false;
static IPAddress  ntpServerIP; // NTP server's ip address


//=======================================================================
bool externalNtpSync() {
//=======================================================================
  return externalNtpTime;
  
} // externalNtpSync()

//=======================================================================
String externalNtpIP() {
//=======================================================================
  char cIP[25];

  sprintf(cIP, "%d.%d.%d.%d", ntpServerIP[0], ntpServerIP[1], ntpServerIP[2], ntpServerIP[3]);

  return cIP;
  
} // externalNtpIP()

//=======================================================================
bool startNTP() {
//=======================================================================
  
  _dThis = true;
  Debugln("Starting UDP");
  Udp.begin(localPort);
  _dThis = true;
  Debug("Local port: ");
  Debugln(String(Udp.localPort()));

  if (loopNTP()) {
    return true;
  }
  return false;

} // startNTP()


//=======================================================================
bool loopNTP() {
//=======================================================================
  int waitForTime = 3;

  if (externalNtpTime) return true;

  _dThis = true;
  Debugln("waiting for sync on NTP server ..");
  DebugFlush();

  while (!externalNtpSync() && waitForTime > 0) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    setSyncProvider(getNtpTime);
    setSyncInterval(600);
    _dThis = true;
    if (externalNtpTime) {
      Debugf("externalNtpSync(): [%d] from [%s]\n", externalNtpSync(), externalNtpIP().c_str());
    } else {
      Debugf("[%s] did not respond!\n", externalNtpIP().c_str());
    }
    waitForTime--;
  }
  _dThis = true;
  if (externalNtpTime) {
    Debugf("NTP time: %02d:%02d:%02d from [%s]\n", hour(), minute(), second(), externalNtpIP().c_str());
  } else {
    Debugln("time not (yet) set!!");
  }
  DebugFlush();
  
  digitalWrite(LED_BUILTIN, LED_OFF);
  return externalNtpTime;

} // loopNTP()


//=======================================================================
time_t getNtpTime() {
//=======================================================================
  //IPAddress ntpServerIP; // NTP server's ip address
  
  while (Udp.parsePacket() > 0) { yield(); }  // discard any previously received packets

  // get a random server from the pool
  switch(ntpServerNr) {
    case 0:   WiFi.hostByName(ntpServerName0, ntpServerIP);
              ntpServerNr = 1;
              break;
    case 1:   WiFi.hostByName(ntpServerName1, ntpServerIP);
              ntpServerNr = 2;
              break;
    case 2:   WiFi.hostByName(ntpServerName2, ntpServerIP);
              ntpServerNr = 3;
              break;
    case 3:   WiFi.hostByName(ntpServerName3, ntpServerIP);
              ntpServerNr = 1;
              break;
    case 4:   WiFi.hostByName(ntpServerName4, ntpServerIP);
              ntpServerNr = 0;
              break;
    default:  WiFi.hostByName(ntpServerName0, ntpServerIP);
              ntpServerNr = 1;
              break;
  }
  
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      externalNtpTime = true;
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
    yield();
  }
  ntpServerNr++;
  if (ntpServerNr > 4) ntpServerNr = 0;
  _dThis = true;
  Debugln("No NTP Response :-(");
  externalNtpTime = false;
  return 0; // return 0 if unable to get the time

} // getNtpTime()


// send an NTP request to the time server at the given address
//=======================================================================
void sendNTPpacket(IPAddress &address) {
//=======================================================================
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
  
} // sendNTPpacket()


//=======================================================================
time_t dateTime2Epoch(char const *date, char const *time) {
//=======================================================================
    char s_month[5];
    int year, day, h, m, s;
    tmElements_t t;

    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    if (sscanf(date, "%s %d %d", s_month, &day, &year) != 3) {
      _dThis = true;
      Debugf("Not a valid date string [%s]\n", date);
      return 0;
    }
    t.Day  = day;
    // Find where is s_month in month_names. Deduce month value.
    t.Month = (strstr(month_names, s_month) - month_names) / 3 + 1;
    
  //_dThis = true;
  //Debugf("date=>[%d-%02d-%02d]\n", t.Year, t.Month, t.Day);
  //DebugFlush();
    
    // Find where is s_month in month_names. Deduce month value.
    t.Month = (strstr(month_names, s_month) - month_names) / 3 + 1;

    if (sscanf(time, "%2d:%2d:%2d", &h, &m, &s) != 3) {
      _dThis = true;
      Debugf("Not a valid time string [%s] (time set to '0')\n", time);
      h = 0;
      m = 0;
      s = 0;
    }
    t.Hour    = h;
    t.Minute  = m;
    t.Second  = s;
    
  //_dThis = true;
  //Debugf("time=>[%02d:%02d:%02d]\n", t.Hour, t.Minute, t.Second);
  //DebugFlush();

    t.Year = CalendarYrToTm(year);
    
  //_dThis = true;
  //Debugf("converted=>[%d-%02d-%02d @ %02d:%02d:%02d]\n", t.Year, t.Month, t.Day, t.Hour, t.Minute, t.Second);
  //DebugFlush();

    return makeTime(t);
    
} // dateTime2Epoch()

#endif
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
