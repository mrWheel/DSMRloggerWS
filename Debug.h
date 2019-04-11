/*
***************************************************************************  
**  Program  : Debug.h, part of DSMRloggerWS
**  Version  : v0.4.1
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

bool _dThis = false;

#define Debug(...)      ({  if (_dThis == true) { \
                                   Serial.printf("[%02d:%02d:%02d] ", hour(), minute(), second());        \
                                   Serial.printf("[%8d] ", ESP.getFreeHeap());                           \
                                   TelnetStream.printf("[%02d:%02d:%02d] ", hour(), minute(), second());  \
                                   TelnetStream.printf("[%8d] ", ESP.getFreeHeap());                     \
                                   Serial.printf("%-12.12s(%4d): ",__FUNCTION__, __LINE__);               \
                                   TelnetStream.printf("%-12.12s(%4d): ",__FUNCTION__, __LINE__);         \
                                   _dThis = false;  \
                            } \
                            Serial.print(__VA_ARGS__);          \
                            TelnetStream.print(__VA_ARGS__);    \
                            DebugFlush(); \
                        })
#define Debugln(...)    ({  if (_dThis == true) { \
                                   Serial.printf("[%02d:%02d:%02d] ", hour(), minute(), second());        \
                                   Serial.printf("[%8d] ", ESP.getFreeHeap());                            \
                                   TelnetStream.printf("[%02d:%02d:%02d] ", hour(), minute(), second());  \
                                   TelnetStream.printf("[%8d] ", ESP.getFreeHeap());                      \
                                   Serial.printf("%-12.12s(%4d): ",__FUNCTION__, __LINE__);               \
                                   TelnetStream.printf("%-12.12s(%4d): ",__FUNCTION__, __LINE__);         \
                            } \
                            Serial.println(__VA_ARGS__);        \
                            TelnetStream.println(__VA_ARGS__);  \
                            DebugFlush(); \
                        })
#define Debugf(...)     ({  if (_dThis == true) { \
                                   Serial.printf("[%02d:%02d:%02d] ", hour(), minute(), second());        \
                                   Serial.printf("[%8d] ", ESP.getFreeHeap());                            \
                                   TelnetStream.printf("[%02d:%02d:%02d] ", hour(), minute(), second());  \
                                   TelnetStream.printf("[%8d] ", ESP.getFreeHeap());                      \
                                   Serial.printf("%-12.12s(%4d): ",__FUNCTION__, __LINE__);               \
                                   TelnetStream.printf("%-12.12s(%4d): ",__FUNCTION__, __LINE__);         \
                                   _dThis = false; \
                            } \
                            Serial.printf(__VA_ARGS__);       \
                            TelnetStream.printf(__VA_ARGS__); \
                            DebugFlush(); \
                        })
#define DebugFlush()    ({ Serial.flush();        \
                           TelnetStream.flush();  \
                        })
                        
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
