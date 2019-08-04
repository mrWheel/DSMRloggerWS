/*
***************************************************************************  
**  Program  : oledStuff.h, part of DSMRloggerWS
**  Version  : v0.4.6
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
    
#include "SSD1306Ascii.h"       // https://github.com/greiman/SSD1306Ascii
#include "SSD1306AsciiWire.h"   // Version 1.2.x - Commit 97a05cd on 24 Mar 2019

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

void oled_Print_Msg(uint8_t, String, uint16_t);

static bool buttonState = LOW;
static uint8_t msgMode = 1;

uint32_t    oledSleepTimer = 120000;

uint8_t     lineHeight, charHeight;


//===========================================================================================
void checkFlashButton() {
//===========================================================================================

  if (settingSleepTime == 0) return; // don't switch OLED off
  
  if (oledSleepTimer > 0 && millis() > oledSleepTimer) {
    //Serial.println("Switching display off..");
    oled.clear();
    oledSleepTimer = 0;
  }
  if (digitalRead(FLASH_BUTTON) == LOW && buttonState == LOW) {
      buttonState = HIGH;
  } else if (digitalRead(FLASH_BUTTON) == HIGH && buttonState == HIGH) {
      buttonState = LOW;
      oledSleepTimer = millis() + (settingSleepTime * 60000);
      //Serial.println("Switching display on..");
      oled.clear();
      oled_Print_Msg(0, "** DSMRloggerWS **", 0);
      oled_Print_Msg(2, "Wait ...", 5);
      msgMode = 1;
  }   
} // checkFlashButton()


//===========================================================================================
void oled_Init() {
//===========================================================================================
    Wire.begin();
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
    oled.setFont(X11fixed7x14B);  // this gives us 4 rows by 18 chars
    charHeight  = oled.fontHeight();
    lineHeight  = oled.displayHeight() / 4;
    Serial.printf("OLED is [%3dx%3d], charHeight[%d], lineHeight[%d], nrLines[%d]\n", oled.displayWidth()
                                                        , oled.displayHeight()
                                                        , charHeight, lineHeight, 4);

}   // oled_Init()

//===========================================================================================
void oled_Clear() {
//===========================================================================================
    oled.clear();
    
}   // oled_Clear


//===========================================================================================
void oled_Print_Msg(uint8_t line, String message, uint16_t wait) {
//===========================================================================================
  uint32_t sleeper;

    if (settingSleepTime > 0 && oledSleepTimer == 0) return;  // v0.4.6
    
    message += "                    ";
    
    //Serial.printf("oled.setCursor(0, %2d)\n", ((line * lineHeight)/8));
    oled.setCursor(0, ((line * lineHeight)/8));
    oled.print(message.c_str());

    if (wait > 0) {
      sleeper = millis() + wait;
      while (millis() < sleeper) {
        checkFlashButton();
        yield();
      }
    } 
    
}   // oled_Print_Msg()




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
