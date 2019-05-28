/*
***************************************************************************  
**  Program  : settingsStuff, part of DSMRloggerWS
**  Version  : v0.4.3
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//=======================================================================
void writeSettings() {
//=======================================================================

  _dThis = true;
  Debugf(" %s .. ", String(SETTINGS_FILE).c_str());
  _dThis = true;
  
  File file = SPIFFS.open(SETTINGS_FILE, "w");
  
  file.print("EnergyDeliveredT1 = "); file.println(String(settingEDT1, 5));
  file.print("EnergyDeliveredT2 = "); file.println(String(settingEDT2, 5));
  file.print("EnergyReturnedT1 = ");  file.println(String(settingERT1, 5));
  file.print("EnergyReturnedT2 = ");  file.println(String(settingERT2, 5));
  file.print("GASDeliveredT = ");     file.println(String(settingGDT,  5));
  file.print("EnergyVasteKosten = "); file.println(String(settingENBK, 2));
  file.print("GasVasteKosten = ");    file.println(String(settingGNBK, 2));
  file.print("BackGroundColor = ");   file.println(settingBgColor);
  file.print("FontColor = ");         file.println(settingFontColor);
  file.print("SleepTime = ");         file.println(settingSleepTime);
  file.print("TelegramInterval = ");  file.println(settingInterval);
  file.print("iniBordEDC = ");        file.println(iniBordEDC);
  file.print("iniFillEDC = ");        file.println(iniFillEDC);
  file.print("iniBordERC = ");        file.println(iniBordERC);
  file.print("iniFillERC = ");        file.println(iniFillERC);
  file.print("iniBordGDC = ");        file.println(iniBordGDC);
  file.print("iniFillGDC = ");        file.println(iniFillGDC);
  file.print("iniBordED2C = ");       file.println(iniBordED2C);
  file.print("iniFillED2C = ");       file.println(iniFillED2C);
  file.print("iniBordER2C = ");       file.println(iniBordER2C);
  file.print("iniFillER2C = ");       file.println(iniFillER2C);
  file.print("iniBordGD2C = ");       file.println(iniBordGD2C);
  file.print("iniFillGD2C = ");       file.println(iniFillGD2C);
  file.print("iniBordPR123C = ");     file.println(iniBordPR123C);
  file.print("iniFillPR123C = ");     file.println(iniFillPR123C);
  file.print("iniBordPD1C = ");       file.println(iniBordPD1C);
  file.print("iniFillPD1C = ");       file.println(iniFillPD1C);
  file.print("iniBordPD2C = ");       file.println(iniBordPD2C);
  file.print("iniFillPD2C = ");       file.println(iniFillPD2C);
  file.print("iniBordPD3C = ");       file.println(iniBordPD3C);
  file.print("iniFillPD3C = ");       file.println(iniFillPD3C);
  file.print("UpdateURL = ");         file.println(settingUpdateURL);
  file.print("UpdateFingerprint = "); file.println(settingFingerPrint);

  file.close();  
  
  Debugln(" .. done");

} // writeSettings()


//=======================================================================
void readSettings() {
//=======================================================================
  String sTmp, nColor;
  String words[10];

  _dThis = true;
  Debugf(" %s ..", String(SETTINGS_FILE).c_str());

  settingEDT1       = 0.1;
  settingEDT2       = 0.2;
  settingERT1       = 0.3;
  settingERT2       = 0.4;
  settingGDT        = 0.5;
  settingENBK       = 15.15;
  settingGNBK       = 11.11;
  strcpy(settingBgColor, "deepskyblue");
  strcpy(settingFontColor, "white");

  settingInterval   = 10; // seconds
  settingSleepTime  = 10;
  
  strcpy(iniFillEDC   , "red");
  strcpy(iniBordEDC   , "red");
  strcpy(iniFillERC   , "green");
  strcpy(iniBordERC   , "green");
  strcpy(iniFillGDC   , "blue");
  strcpy(iniBordGDC   , "blue");
  strcpy(iniFillED2C  , "tomato");
  strcpy(iniBordED2C  , "tomato");
  strcpy(iniFillER2C  , "lightgreen");
  strcpy(iniBordER2C  , "lightgreen");
  strcpy(iniFillGD2C  , "lightblue");
  strcpy(iniBordGD2C  , "lightblue");
  strcpy(iniFillPR123C, "green");
  strcpy(iniBordPR123C, "green");
  strcpy(iniFillPD1C  , "yellow");
  strcpy(iniBordPD1C  , "yellow");
  strcpy(iniFillPD2C  , "lightgreen");
  strcpy(iniBordPD2C  , "lightgreen");
  strcpy(iniFillPD3C  , "lime");
  strcpy(iniBordPD3C  , "lime");

  settingUpdateURL   = "https://willem.aandewiel.nl/wp-content/uploads/2019/01/DSMRloggerWS.bin";
  settingFingerPrint = "ERC:10:43:58:12:74:4A:31:A6:19:E3:32:81:92:30:GDC:40:PD2C:50:D7";

  if (!SPIFFS.exists(SETTINGS_FILE)) {

    _dThis = true;
    writeSettings();
    Debugln(" .. file not found! --> created file!");
  }

  File file = SPIFFS.open(SETTINGS_FILE, "r");

  _dThis = false;
  Debugln();
  while(file.available()) {
    sTmp                = file.readStringUntil('\n');
    sTmp.replace("\r", "");
    //_dThis = true;
    //Debugf("[%s] (%d)\n", sTmp.c_str(), sTmp.length());
    int8_t wc = splitString(sTmp.c_str(), '=', words, 10);
    words[0].toLowerCase();
    nColor = words[1].substring(0,15);
    if (words[0].equalsIgnoreCase("EnergyDeliveredT1")) settingEDT1         = words[1].toFloat();  
    if (words[0].equalsIgnoreCase("EnergyDeliveredT2")) settingEDT2         = words[1].toFloat();  
    if (words[0].equalsIgnoreCase("EnergyReturnedT1"))  settingERT1         = words[1].toFloat();  
    if (words[0].equalsIgnoreCase("EnergyReturnedT2"))  settingERT2         = words[1].toFloat();  
    if (words[0].equalsIgnoreCase("GasDeliveredT"))     settingGDT          = words[1].toFloat();  
    if (words[0].equalsIgnoreCase("EnergyVasteKosten")) settingENBK         = words[1].toFloat();
    if (words[0].equalsIgnoreCase("GasVasteKosten"))    settingGNBK         = words[1].toFloat();

    if (words[0].equalsIgnoreCase("BackgroundColor"))   strcpy(settingBgColor,   String(nColor).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("FontColor"))         strcpy(settingFontColor, String(nColor).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("SleepTime"))         settingSleepTime    = words[1].toInt();  
    if (words[0].equalsIgnoreCase("TelegramInterval"))  settingInterval     = words[1].toInt();  

    if (words[0].equalsIgnoreCase("iniBordEDC"))        strcpy(iniBordEDC   , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniFillEDC"))        strcpy(iniFillEDC   , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniBordERC"))        strcpy(iniBordERC   , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniFillERC"))        strcpy(iniFillERC   , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniBordGDC"))        strcpy(iniBordGDC   , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniFillGDC"))        strcpy(iniFillGDC   , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniBordED2C"))       strcpy(iniBordED2C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniFillED2C"))       strcpy(iniFillED2C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniBordER2C"))       strcpy(iniBordER2C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniFillER2C"))       strcpy(iniFillER2C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniBordGD2C"))       strcpy(iniBordGD2C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniFillGD2C"))       strcpy(iniFillGD2C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniBordPR123C"))     strcpy(iniBordPR123C, String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniFillPR123C"))     strcpy(iniFillPR123C, String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniBordPD1C"))       strcpy(iniBordPD1C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniFillPD1C"))       strcpy(iniFillPD1C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniBordPD2C"))       strcpy(iniBordPD2C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniFillPD2C"))       strcpy(iniFillPD2C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniBordPD3C"))       strcpy(iniBordPD3C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("iniFillPD3C"))       strcpy(iniFillPD3C  , String(words[1]).substring(0,(MAXCOLORNAME - 1)).c_str());  
    
    if (words[0].equalsIgnoreCase("updateURL"))         settingUpdateURL    = words[1];  
    if (words[0].equalsIgnoreCase("updateFingerprint")) settingFingerPrint  = words[1];  
    
  } // while available()

  _dThis = false;
  Debugln("\n==== Financial Settings =========================================");
  Debugf("   Energy Delivered Tarief 1 : %9.7f\n",  settingEDT1);
  Debugf("   Energy Delivered Tarief 2 : %9.7f\n",  settingEDT2);
  Debugf("   Energy Delivered Tarief 1 : %9.7f\n",  settingERT1);
  Debugf("   Energy Delivered Tarief 2 : %9.7f\n",  settingERT2);
  Debugf("        Gas Delivered Tarief : %9.7f\n",  settingGDT);
  Debugf("     Energy Netbeheer Kosten : %9.2f\n",  settingENBK);
  Debugf("        Gas Netbeheer Kosten : %9.2f\n",  settingGNBK);

  Debugf("            BackGround Color : %s\n", settingBgColor);
  Debugf("                  Font Color : %s\n", settingFontColor);
  Debugf("   Telegram Process Interval : %d\n", settingInterval);
  Debugf("OLED Sleep Min. (0=oneindig) : %d\n", settingSleepTime);
  
  Debugln("\n==== Chart colors ===============================================");
  Debugf("  Energie Verbruik LineColor : %s\n", iniBordEDC);  
  Debugf("  Energie Verbruik BackColor : %s\n", iniFillEDC);  
  Debugf("  Energie Returned LineColor : %s\n", iniBordERC);  
  Debugf("  Energie Returned BackColor : %s\n", iniFillERC);  
  Debugf("      Gas Verbruik LineColor : %s\n", iniBordGDC);  
  Debugf("      Gas Verbruik BackColor : %s\n", iniFillGDC);  
  Debugf("Energie Verbruik H LineColor : %s\n", iniBordED2C);  
  Debugf("Energie Verbruik H BackColor : %s\n", iniFillED2C);  
  Debugf("Energie Returned H LineColor : %s\n", iniBordER2C);  
  Debugf("Energie Returned H BackColor : %s\n", iniFillER2C);  
  Debugf("    Gas Verbruik H LineColor : %s\n", iniBordGD2C);  
  Debugf("    Gas Verbruik H BackColor : %s\n", iniFillGD2C);  
  Debugf(" Power Verbruik L1 LineColor : %s\n", iniBordPD1C);  
  Debugf(" Power Verbruik L1 BackColor : %s\n", iniFillPD1C);  
  Debugf(" Power Verbruik L2 LineColor : %s\n", iniBordPD2C);  
  Debugf(" Power Verbruik L2 BackColor : %s\n", iniFillPD2C);  
  Debugf(" Power Verbruik L3 LineColor : %s\n", iniBordPD3C);
  Debugf(" Power Verbruik L3 BackColor : %s\n", iniFillPD3C);  
  Debugf("   Power Ret. L123 LineColor : %s\n", iniBordPR123C);  
  Debugf("   Power Ret. L123 BackColor : %s\n", iniFillPR123C);  
  
  Debugln("\n==== Future Expantion ===========================================");
  Debugf("                   updateURL : %s\n", settingUpdateURL.c_str());
  Debugf("                 FingerPrint : %s\n", settingFingerPrint.c_str());
  Debugln("-");
  
  file.close();  
  _dThis = true;
  Debugln(" .. done");
  DebugFlush();

} // readSettings()

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
