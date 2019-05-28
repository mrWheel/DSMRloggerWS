/*
***************************************************************************  
**  Program  : SPIFFSstuff, part of DSMRloggerWS
**  Version  : v0.4.3
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

int16_t  bytesWritten;

//===========================================================================================
int32_t freeSpace() {
//===========================================================================================
  int32_t space;
  
  SPIFFS.info(SPIFFSinfo);

  space = (int32_t)(SPIFFSinfo.totalBytes - SPIFFSinfo.usedBytes);

  return space;
  
} // freeSpace()

//===========================================================================================
void listSPIFFS() {
//===========================================================================================
  Dir dir = SPIFFS.openDir("/");

  Debugln();
  while (dir.next()) {
    File f = dir.openFile("r");
    Debugf("%-15s %d \r\n", dir.fileName().c_str(), f.size());
    yield();
  }

  SPIFFS.info(SPIFFSinfo);

  Debugln("\r");
  if (freeSpace() < (10 * SPIFFSinfo.blockSize))
        Debugf("Available SPIFFS space [%6d]kB (LOW ON SPACE!!!)\r\n", (freeSpace() / 1024));
  else  Debugf("Available SPIFFS space [%6d]kB\r\n", (freeSpace() / 1024));
  Debugf("           SPIFFS Size [%6d]kB\r\n", (SPIFFSinfo.totalBytes / 1024));
  Debugf("     SPIFFS block Size [%6d]bytes\r\n", SPIFFSinfo.blockSize);
  Debugf("      SPIFFS page Size [%6d]bytes\r\n", SPIFFSinfo.pageSize);
  Debugf(" SPIFFS max.Open Files [%6d]\r\n\n", SPIFFSinfo.maxOpenFiles);


} // listSPIFFS()


void fillRecord(char *record, int8_t len) {
  int8_t s = 0, l = 0;
  while (record[s] != '\0' && record[s]  != '\n') {s++;}
  _dThis = true;
  if (Verbose1) Debugf("Length of record is [%d] bytes\n", s);
  for (l = s; l < (len - 2); l++) {
    record[l] = ' ';
  }
  record[l]   = ';';
  record[l+1] = '\n';
  record[len] = '\0';

  _dThis = true;
  while (record[l] != '\0') {l++;}
  _dThis = true;
  if (Verbose1) Debugf("Length of record is now [%d] bytes\n", l);
  
} // fillRecord()


//===========================================================================================
int8_t fileLabel2Rec(int8_t fileType, uint32_t RecKey) {
//===========================================================================================
  int16_t recLen, offset, maxRecords;
  int32_t Label;
  File dataFile;
  
  _dThis = true;
  Debugf("RecKey is [%d]\n", RecKey);

  if (fileType == MONTHS) {
    recLen      = MONTHS_RECLEN;
    maxRecords  = MONTHS_RECS;
    dataFile    = SPIFFS.open(MONTHS_FILE, "r");    // open for Read
  } else if (fileType == DAYS) {
    recLen      = DAYS_RECLEN;
    maxRecords  = DAYS_RECS;
    dataFile    = SPIFFS.open(DAYS_FILE, "r");      // open for Read    
  } else if (fileType == HOURS) {
    recLen      = HOURS_RECLEN;
    maxRecords  = HOURS_RECS;
    dataFile    = SPIFFS.open(HOURS_FILE, "r");      // open for Read    
  } else {
    maxRecords  = 0;
    recLen      = 0;
  }
  if (!dataFile) return 0;

  for(int r = 1; r <= maxRecords; r++) {
    offset = r * recLen;
    dataFile.seek(offset, SeekSet);
    Label = (int)dataFile.readStringUntil(';').toInt();
    _dThis = true;
    if (fileType == MONTHS) {
      if (Verbose2) Debugf("Check record[%02d] for [%04d] -> found [%04d]\n", r, RecKey, Label);
    } else if (fileType == DAYS) {
      if (Verbose2) Debugf("Check record[%02d] for [%06d] -> found [%06d]\n", r, RecKey, Label);
    } else if (fileType == HOURS) {
      if (Verbose2) Debugf("Check record[%02d] for [%08d] -> found [%08d]\n", r, RecKey, Label);
    }
    if (Label == RecKey) {
      _dThis = true;
      if (Verbose1) Debugf("Found Label [%d]\n", Label);
      dataFile.close();
      return r;
    } else {
      if (Label < RecKey) {
        _dThis = true;
        Debugf("NotFound: Label [%d] < [%d]\n", Label, RecKey);
        dataFile.close();
        return -1;
      }
    }
    yield();
  }
  dataFile.close();
  return 0;

} // fileLabel2Rec()


//===========================================================================================
bool fileShiftDown(int8_t fileType) {
//===========================================================================================
  int16_t recLen, offset, maxRecords;
  String recData;
  bool   exitState = true;
  File dataFile;
  
  if (fileType == MONTHS) {
    dataFile    = SPIFFS.open(MONTHS_FILE, "r+");     // open for Read
    recLen      = MONTHS_RECLEN;
    maxRecords  = MONTHS_RECS;
    
  } else if (fileType == DAYS) {
    dataFile    = SPIFFS.open(DAYS_FILE, "r+");     // open for Read
    recLen      = DAYS_RECLEN;
    maxRecords  = DAYS_RECS;
    
  } else if (fileType == HOURS) {
    dataFile    = SPIFFS.open(HOURS_FILE, "r+");     // open for Read
    recLen      = HOURS_RECLEN;
    maxRecords  = HOURS_RECS;
  
  } else recLen = 0;
  
  if (!dataFile) return false;

  _dThis = true;
  if (Verbose2) Debugf("recLen is [%02d]\n", recLen);

  for (int r = maxRecords; r >= 1; r--) {
    offset = r * recLen;
    dataFile.seek(offset, SeekSet);
    recData  = dataFile.readStringUntil('\n');
    _dThis = true;
    if (Verbose2) Debugf("Move record[%02d] @[%04d] to [%02d] - [%20.20s**]\n", r, offset, (r+1), recData.c_str());
    offset = (r+1) * recLen;
    dataFile.seek(offset, SeekSet);
    bytesWritten = dataFile.print(recData);
    if (bytesWritten != (recLen -1)) {
      Debugf("ERROR! recNo[%02d]: written [%d] bytes but should have been [%d] for Label %8.8s\n", r, bytesWritten, recLen, recData.c_str());
      exitState = false;  // save State, still need to close file
    }
    dataFile.print('\n');

  }
  dataFile.close();
  if (!exitState) return false;

  return true;
  
} // fileShiftDown()


//===========================================================================================
void fileWriteData(int8_t fileType, dataStruct newDat) {
//===========================================================================================

  _dThis = true;
  if      (fileType == MONTHS)  { Debugf("newDat.label is [%04d]\n", newDat.Label); }
  else if (fileType == DAYS)    { Debugf("newDat.label is [%06d]\n", newDat.Label); }
  else if (fileType == HOURS)   { Debugf("newDat.label is [%08d]\n", newDat.Label); }
  else {
    Debugf("Unknown fileType [%d] .. abort!\n", fileType);
    return;
  }
  int16_t recNo = fileLabel2Rec(fileType, newDat.Label);
  fileWriteData(fileType, newDat, recNo);

} // fileWriteData(dataStruct newDat)


//===========================================================================================
void fileWriteData(int8_t fileType, dataStruct newDat, int16_t recNo) {
//===========================================================================================
  String  fileName;
  char *fileHeader, *fileFormat;
  int8_t  fileRecLen, fileNoRecs;
  
  _dThis = true;
  Debugf("----> write recNo[%d]\n", recNo);
  
  if (!SPIFFSmounted) {
    _dThis = true;
    Debugln("No SPIFFS filesystem..ABORT!!!\r");
    return;
  }
  
  if (fileType == MONTHS) {
      fileName    = MONTHS_FILE;
      fileHeader  = MONTHS_CSV_HEADER;
      fileFormat  = MONTHS_FORMAT;
      fileRecLen  = MONTHS_RECLEN;
      fileNoRecs  = MONTHS_RECS;
      
  } else if (fileType == DAYS) {
      fileName    = DAYS_FILE;
      fileHeader  = DAYS_CSV_HEADER;
      fileFormat  = DAYS_FORMAT;
      fileRecLen  = DAYS_RECLEN;
      fileNoRecs  = DAYS_RECS;
    
  } else if (fileType == HOURS) {
      fileName    = HOURS_FILE;
      fileHeader  = HOURS_CSV_HEADER;
      fileFormat  = HOURS_FORMAT;
      fileRecLen  = HOURS_RECLEN;
      fileNoRecs  = HOURS_RECS;
    
  } else {
      _dThis = true;
      Debugf("Unknown fileType [%d] .. abort!\n", fileType);
      return;
  }

  // --- check if the file exists and can be opened ---
  File dataFile  = SPIFFS.open(fileName, "r+");    // open for Read & writing
  if (!dataFile) {
    _dThis = true;
    Debugf("File [%s] does not exist, create one\n", fileName.c_str());
    if (!createFile(fileType, fileName, fileHeader, fileFormat, fileRecLen)) {
      return;
    }
  } // if (!dataFile)

  _dThis = true;
  Debugf("checkRecordsInFile [%s] ...\n", fileName.c_str());
  checkRecordsInFile(fileType, fileName, fileFormat, fileRecLen, fileNoRecs, newDat);

  //--- and now .. add or update the new data --------
  if (!dataFile) {                            // if last open failed, try again after createFile()
    dataFile  = SPIFFS.open(fileName, "r+");  // open for Read & writing
  }
  _dThis = true;
  if (recNo > 0) {
    //---- write new data
    sprintf(cMsg, fileFormat, newDat.Label   , String(newDat.EDT1, 3).c_str()
                                             , String(newDat.EDT2, 3).c_str()
                                             , String(newDat.ERT1, 3).c_str()
                                             , String(newDat.ERT2, 3).c_str()
                                             , String(newDat.GDT, 2).c_str());
    fillRecord(cMsg, fileRecLen);
    dataFile.seek((recNo * fileRecLen), SeekSet);
    bytesWritten = dataFile.print(cMsg);
    if (bytesWritten != fileRecLen) {
      Debugf("ERROR!! recNo[%02d]: written [%d] bytes but should have been [%d] for Label[%s]\n", recNo, bytesWritten, fileRecLen, cMsg);
    }
    if (Verbose1) Debugf("recNo[%02d] := %s", recNo, cMsg);
    
  } else if (recNo == -1) {
    Debugln("Need to shift down!");
    fileShiftDown(fileType);
    //---- write new data
    sprintf(cMsg, fileFormat, newDat.Label   , String(newDat.EDT1, 3).c_str()
                                             , String(newDat.EDT2, 3).c_str()
                                             , String(newDat.ERT1, 3).c_str()
                                             , String(newDat.ERT2, 3).c_str()
                                             , String(newDat.GDT,  2).c_str());
    fillRecord(cMsg, fileRecLen);
    dataFile.seek((1 * fileRecLen), SeekSet);
    bytesWritten = dataFile.print(cMsg);
    if (bytesWritten != fileRecLen) {
      Debugf("ERROR!! recNo[%d]: written [%02d] bytes but should have been [%d] for Label[%s]\n", 1, bytesWritten, fileRecLen, cMsg);
    }

    if (Verbose1) Debugf("recNo[%02d] Data[%s]", 1, cMsg);
  } else {
    Debugf("No record with label [%d] found!\n\r", newDat.Label);
  }
  yield();

  dataFile.close();  

  _dThis = true;
  if (Verbose1) Debugln(" ..Done\r");

  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

} // fileWriteData(fileType, dataStruct newDat, int8_t recNo)


//===========================================================================================
bool createFile(int8_t fileType, String fileName, const char *fileHeader, const char *fileFormat, uint16_t fileRecLen) {
//===========================================================================================
int8_t  YY, MM, DD, HH;

    File dataFile  = SPIFFS.open(fileName, "a");  // create File
    // -- first write fileHeader ----------------------------------------
    sprintf(cMsg, "%s", fileHeader);  // you cannot modify *fileHeader!!!
    _dThis = true;
    Debugln(cMsg);
    fillRecord(cMsg, fileRecLen);
    _dThis = true;
    Debugln(cMsg);
    bytesWritten = dataFile.print(cMsg);
    if (bytesWritten != fileRecLen) {
     _dThis = true;
      Debugf("ERROR!! recNo[%d]: written [%d] bytes but should have been [%d] for Header[%s]\n", 0, bytesWritten, fileRecLen, cMsg);
    }
    _dThis = true;
    Debugln(".. that went well! Now add next record ..");
    // -- as this file is empty, write one data record ------------
    YY = YearFromTimestamp(pTimestamp);
    MM = MonthFromTimestamp(pTimestamp);
    DD = DayFromTimestamp(pTimestamp);
    HH = HourFromTimestamp(pTimestamp);
    if (fileType == MONTHS)     sprintf(cMsg, "%02d%02d", YY, MM);
    else if (fileType == DAYS)  sprintf(cMsg, "%02d%02d%02d", YY, MM, DD);
    else                        sprintf(cMsg, "%02d%02d%02d%02d", YY, MM, DD, HH);
    
    sprintf(cMsg, fileFormat, String(cMsg).toInt(), String(0,3).c_str(), String(0,3).c_str()
                                                  , String(0,3).c_str(), String(0,3).c_str()
                                                  , String(0,2).c_str() );
    fillRecord(cMsg, fileRecLen);
    _dThis = true;
    Debugf("Write [%s] Data[%s]\n", fileName.c_str(), cMsg);
//  dataFile.seek((1 * fileRecLen), SeekSet);
    bytesWritten = dataFile.print(cMsg);
    if (bytesWritten != fileRecLen) {
     _dThis = true;
      Debugf("ERROR!! recNo[%d]: written [%d] bytes but should have been [%d] for record [1]\n", 0, bytesWritten, fileRecLen);
    }

    dataFile.close();
    dataFile  = SPIFFS.open(fileName, "r+");       // open for Read & writing
    if (!dataFile) {
      _dThis = true;
      Debugf("Something is very wrong writing to [%s]\n", fileName.c_str());
      return false;
    }
    dataFile.close();

    return true;
  
} //  createFile()


//=====================================================================================================
bool checkRecordsInFile(int8_t fileType, String fileName, const char *fileFormat
                                       , uint16_t fileRecLen, int8_t fileNoRecs, dataStruct newDat) {
//=====================================================================================================
  dataStruct lastRec;
  
  File dataFile  = SPIFFS.open(fileName, "a");        // add to File
  int8_t recsInFile  = dataFile.size() / fileRecLen;  // records in file
  _dThis = true;
  if (Verbose1) Debugf("[%s] needs [%d] records. Found [%02d] records\n", fileName.c_str(), fileNoRecs, recsInFile);
  lastRec = fileReadData(fileType, recsInFile); 
  
  if (recsInFile >= fileNoRecs) return true; // all records are there!
  
  _dThis = true;
  if (Verbose1) Debugf("Now adding records from [%d]\n", newDat.Label);
  for (int r = recsInFile; r <= fileNoRecs; r++) {
    lastRec.Label = updateLabel(fileType, lastRec.Label, -1);
    sprintf(cMsg, fileFormat, lastRec.Label, String(lastRec.EDT1, 3).c_str()
                                           , String(lastRec.EDT2, 3).c_str()
                                           , String(lastRec.ERT1, 3).c_str()
                                           , String(lastRec.ERT2, 3).c_str()
                                           , String(lastRec.GDT, 2).c_str());
    fillRecord(cMsg, fileRecLen);
    dataFile.seek((r * fileRecLen), SeekSet);
    bytesWritten = dataFile.print(cMsg);
    if (bytesWritten != fileRecLen) {
      Debugf("ERROR!! recNo[%02d]: written [%d] bytes but should have been [%d] for Data[%s]\n", r, bytesWritten, fileRecLen, cMsg);
      return false;
    }
    _dThis = true;
    if (Verbose2) Debugf("Add dummy record[%02d] @pos[%d] := %s", r, (r * fileRecLen), cMsg);
  } // for ..

  return true;
  
} // checkRecordsInFile()


//=======================================================================================================================
int32_t updateLabel(int8_t fileType, int32_t Label, int8_t offSet) {
//=======================================================================================================================
  int8_t  YY, MM, DD, HH;

  // -- offSet can only be -1, 0 or +1 ---
  if (offSet < -1) offSet = -1;
  if (offSet >  1) offSet =  1;
  _dThis = true;
  if (Verbose2) Debugf("offSet[%d], Label IN[%d] ", offSet, Label);
  
  if (fileType == HOURS) {
    label2Fields(Label, YY, MM, DD, HH);
    HH += offSet;

  } else if (fileType == DAYS) {
    label2Fields(Label, YY, MM, DD);
    HH = 0;
    DD += offSet;

  } else if (fileType == MONTHS) {
    label2Fields(Label, YY, MM);
    HH = 0;
    DD = 1;    
    MM += offSet;

  }
  if (offSet >= 1) {
    if (HH > 23) {
      HH = 0;
      DD++;    
    }
    if (MM == 4 || MM == 6 || MM == 9 || MM == 11) {
      if (DD > 30) {
        DD = 1;
        MM++ ;   
      }
    } else if (MM == 2) {
      if (DD > 28) {
        DD = 1;
        MM++;
      }
    } else if (DD > 31) {
      DD = 1;
      MM++;      
    }
    if (MM > 12) {
      MM = 1;
      YY++;
    }
    
  } else if (offSet <= -1) {
    if (HH < 0) {
      HH = 23;
      DD--;
    }
    if (DD < 1) {
        MM--;
        if (MM == 4 || MM == 6 || MM == 9 || MM == 11)
              DD = 30;    
        else if (MM == 2)
              DD = 28;
        else  DD = 31;      
      }
      if (MM < 1) {
        MM = 12;
        YY--;
      }
    } // offSet <= -1    
    if (YY <  0) YY =  0;
    if (YY > 99) YY = 99;
    if (MM <  1) MM =  1;
    if (MM > 12) MM = 12;
    if (DD <  1) DD =  1;
    if (DD > 31) DD = 31;
    if (HH <  0) HH =  0;
    if (HH > 23) HH = 23;

    _dThis = false;
    if (fileType == HOURS) {
      sprintf(cMsg, "%02d%02d%02d%02d", YY, MM, DD, HH);
      if (Verbose2) Debugf(" => Label OUT[%s]\n", cMsg);
      return (String(cMsg).toInt());
      
    } else if (fileType == DAYS) {
      sprintf(cMsg, "%02d%02d%02d", YY, MM, DD);
      if (Verbose2) Debugf(" => Label OUT[%s]\n", cMsg);
      return (String(cMsg).toInt());
      
    } else if (fileType == MONTHS) {
      sprintf(cMsg, "%02d%02d", YY, MM);
      if (Verbose2) Debugf(" => Label OUT[%s]\n", cMsg);
      return (String(cMsg).toInt());
      
    }

    return 0;
    
} // updateLabel()


//===========================================================================================
dataStruct fileReadData(int8_t fileType, uint8_t recNo) {
//===========================================================================================
  int16_t  recLen, offset;
  dataStruct tmpRec;
  File dataFile;
  
  tmpRec.Label = 0;
  tmpRec.EDT1  = 0;
  tmpRec.EDT2  = 0;
  tmpRec.ERT1  = 0;
  tmpRec.ERT2  = 0;
  tmpRec.GDT   = 0;

  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  _dThis = true;
  if (Verbose1) Debugf("fileReadData(%02d) ... \n\r", recNo);

  if (!SPIFFSmounted) {
    _dThis = true;
    Debugln("No SPIFFS filesystem..\r");
    return tmpRec;
  }

  if (fileType == MONTHS) {
    sprintf(cMsg, MONTHS_CSV_HEADER);
    dataFile = SPIFFS.open(MONTHS_FILE, "r");
    _dThis = true;
    if (Verbose2) Debugf("%s: size(%d) \n\r", MONTHS_FILE, dataFile.size());
    recLen = MONTHS_RECLEN;
  }
  else if (fileType == DAYS) {
    sprintf(cMsg, DAYS_CSV_HEADER);
    dataFile = SPIFFS.open(DAYS_FILE, "r");
    _dThis = true;
    if (Verbose2) Debugf("%s: size(%d) \n\r", DAYS_FILE, dataFile.size());
    recLen = DAYS_RECLEN;
  }
  else if (fileType == HOURS) {
    sprintf(cMsg, HOURS_CSV_HEADER);
    dataFile = SPIFFS.open(HOURS_FILE, "r");
    _dThis = true;
    if (Verbose2) Debugf("%s: size(%d) \n\r", HOURS_FILE, dataFile.size());
    recLen = HOURS_RECLEN;
  }
  else recLen = 0;
  
  if (dataFile.size() == 0) return tmpRec;

//-- seek() gives strange results ..  
  offset = recNo * recLen;
  dataFile.seek(offset, SeekSet); // skip header
  
  if (dataFile.available() > 0) {
    tmpRec.Label = (int)dataFile.readStringUntil(';').toInt();
    tmpRec.EDT1  = (float)dataFile.readStringUntil(';').toFloat();
    tmpRec.EDT2  = (float)dataFile.readStringUntil(';').toFloat();
    tmpRec.ERT1  = (float)dataFile.readStringUntil(';').toFloat();
    tmpRec.ERT2  = (float)dataFile.readStringUntil(';').toFloat();
    tmpRec.GDT   = (float)dataFile.readStringUntil(';').toFloat();
    String n = dataFile.readStringUntil('\n');
  }
  _dThis = true;
  if (Verbose2) Debugf("recNo[%02d] Label[%08d], EDT1[%s], EDT2[%s], ERT1[%s], ERT2[%s], GD[%s]\r\n"
                                        , recNo, tmpRec.Label
                                        ,    String(tmpRec.EDT1, 3).c_str()
                                        ,    String(tmpRec.EDT2, 3).c_str()
                                        ,    String(tmpRec.ERT1, 3).c_str()
                                        ,    String(tmpRec.ERT2, 3).c_str()
                                        ,    String(tmpRec.GDT,  2).c_str() );
  
  dataFile.close();  

  _dThis = true;
  if (Verbose1) Debugln(" ..Done\r");
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  return tmpRec;

} // fileReadData()


//===========================================================================================
int8_t getLastMonth() {
//===========================================================================================
  int16_t yearMonth, lastMonth;
  dataStruct tmpDat;

  tmpDat    = fileReadData(MONTHS, 1);
  yearMonth = tmpDat.Label;
  
  lastMonth = (yearMonth % 100);
  
  _dThis = true;
  if (Verbose1) Debugf(" ==> [%02d]\n", lastMonth);

  return lastMonth;

} // getLastMonth()


//===========================================================================================
int8_t getLastYear() {
//===========================================================================================
  int16_t yearMonth, lastYear;
  dataStruct tmpDat;

  tmpDat = fileReadData(MONTHS, 1);
  yearMonth = tmpDat.Label;
  
  lastYear = (yearMonth / 100);
  
  _dThis = true;
  if (Verbose1) Debugf(" ==> [20%02d]\n", lastYear);

  return lastYear;

} // getLastYear()

/***
void test() {
  uint8_t YY, MM;
  dataStruct tmp;
  YY = 12;
  MM = 1;
  tmp.Label = 0;
  tmp.EDT1 = 0;
  tmp.ERT1 = 0;
  tmp.GDT = 0;

  Debugf("Record Length %s is [%d]\n", MONTHS_FILE, MONTHS_RECLEN);

  File dataFile = SPIFFS.open(MONTHS_FILE, "w");  // open for writing, zero length
  sprintf(cMsg, MONTHS_CSV_HEADER);
  fillRecord(cMsg, MONTHS_RECLEN);
  bytesWritten = dataFile.print(cMsg); // header
  if (bytesWritten != MONTHS_RECLEN) {
    Debugf("ERROR!! recNo[%d]: written [%02d] bytes but should have been [%d] for Label[%4.4s]\n", recNo, bytesWritten, MONTHS_RECLEN, cMsg);
  }

  for (int r = 1; r <= 25; r++) {
    yield();
    sprintf(cMsg, "%02d%02d; %12s; %12s; %10s;\n", YY, MM
                                                 , String(1, 3).c_str()
                                                 , String(2, 3).c_str()
                                                 , String(3, 2).c_str());
                                           
    fillRecord(cMsg, MONTHS_RECLEN);
    bytesWritten = dataFile.print(cMsg); 
    if (bytesWritten != MONTHS_RECLEN) {
      Debugf("ERROR!! recNo[%d]: written [%02d] bytes but should have been [%d] for Label[%4.4s]\n", recNo, bytesWritten, MONTHYS_RECLEN, cMsg);
    }

    _dThis = true;
    Debugf("Add record => %s", cMsg);
    MM--;
    if (MM < 1) {
      MM = 12;
      YY--;
    }
  } // for ...
  
  dataFile.close();  

  YY = 11;
  MM = 1;
  tmp.Label = 0;
  for (int r = 0; r < 30; r++) {
    sprintf(cMsg, "%02d%02d", YY, MM);
    tmp.Label = String(cMsg).toInt();
    fileWriteData(MONTHS, tmp);
    MM++;
    if (MM > 12) {
      MM = 1;
      YY++;
    }
  }
  _dThis = true;
  if (!fileReadData(MONTHS, ))   Debugln("setup(): error fileReadData()!");

} // test()
***/

#ifdef HAS_NO_METER
void displayMonthsHist(bool);
void displayDaysHist(bool);
void displayHoursHist(bool);

//===========================================================================================
void createDummyData() {
//===========================================================================================
int8_t YY, MM, DD, HH;
  
  _dThis = true;
  Debugln(" ==> monthData.. \r");
  SPIFFS.remove(MONTHS_FILE);
  //--- write dummy month-data to file ---------
  char cLabel[10];
  float EDT1=9000, EDT2=4500, ERT1=3000, ERT2=1500, GDT=2000;
  for (int s=1; s<=MONTHS_RECS; s++) {
    if (s>24) {
      sprintf(cLabel, "14%02d", (37 - s));
    } else if (s>12) {   
      sprintf(cLabel, "15%02d", (25 - s));
    } else {
      sprintf(cLabel, "16%02d", (13 - s));
    }
    monthData.Label = String(cLabel).toInt();
    monthData.EDT1  = EDT1;
    monthData.ERT1  = ERT1;
    monthData.EDT2  = EDT2;
    monthData.ERT2  = ERT2;
    monthData.GDT   = GDT;
    _dThis = true;
    Debugf("Write MONTHS record [%02d] Label[%04d]\n", s, monthData.Label);

    fileWriteData(MONTHS, monthData, s);

    EDT1 -= 100.0;
    ERT1 -=  10.0;
    EDT2 -=  45.0;
    ERT2 -=   4.0;
    GDT  -=  50.0;
  }
  
  _dThis = false;
  displayMonthsHist(true);
  _dThis = true;
  Debugln("Done creating dummy monthData\r");

  monthData = fileReadData(MONTHS, 1);
  label2Fields(monthData.Label, YY, MM);
  _dThis = true;
  Debugf("First MONTHS record [01] Label[%04d]\n", monthData.Label);
  
  EnergyDeliveredTariff1  = EDT1;
  EnergyReturnedTariff1   = ERT1;
  EnergyDeliveredTariff2  = EDT2;
  EnergyReturnedTariff2   = ERT2;
  GasDelivered            = GDT;
  EnergyDelivered = EnergyDeliveredTariff1 + EnergyDeliveredTariff2;
  EnergyReturned  = EnergyReturnedTariff1  + EnergyReturnedTariff2;

  sprintf(cMsg, "%02d%02d%02d%02d%02d15S", YY, MM, 1, 1, 1);
  pTimestamp = String(cMsg);
  
  _dThis = true;
  Debugln(" ==> dayData.. \r");
  SPIFFS.remove(DAYS_FILE);
  DD = 22;
  sprintf(cMsg, "%02d%02d%02d", YY, MM, DD);
  dayData.Label = String(cMsg).toInt();
  _dThis = true;
  Debugf("Write DAYS record [01] Label[%06d]\n", dayData.Label);
  dayData.EDT1  = 8000.508;
  dayData.ERT1  = 2000.408;
  dayData.EDT2  = 6000.254;
  dayData.ERT2  = 1200.204;
  dayData.GDT   =  600.888;
  fileWriteData(DAYS, dayData, 1);
  for (int s=2; s<=DAYS_RECS; s++) {
    DD--;
    sprintf(cMsg, "%02d%02d%02d", YY, MM, DD);
    dayData.Label = String(cMsg).toInt();
    _dThis = true;
    Debugf("Write DAYS record [%02d] Label[%06d]\n", s, dayData.Label);
    dayData.EDT1  -=  250.01;
    dayData.ERT1  -=  125.02;
    dayData.EDT2  -=  100.03;
    dayData.ERT2  -=   50.04;
    dayData.GDT   -=    4.05;
    fileWriteData(DAYS, dayData, s);
  }
  _dThis = false;
  displayDaysHist(true);
  _dThis = true;
  Debugln("Done creating dummy dayData\r");

  _dThis = true;
  Debugln(" ==> hourData[].. \r");
  SPIFFS.remove(HOURS_FILE);

  dayData = fileReadData(DAYS, 1);
  label2Fields(dayData.Label, YY, MM, DD);
  _dThis = true;
  Debugf("First DAYS record [01] Label[%06d]\n", dayData.Label);

  HH =  12;
  sprintf(cMsg, "%02d%02d%02d%02d", YY, MM, DD, HH);
  hourData.Label = String(cMsg).toInt();
  _dThis = true;
  Debugf("Write HOURS record [01] Label[%08d]\n", hourData.Label);
  hourData.EDT1   = 3045.67;
  hourData.ERT1   = 1023.45;
  hourData.EDT2   = 1545.67;
  hourData.ERT2   =  823.45;
  hourData.GDT    =   10.23;
  fileWriteData(HOURS, hourData, 1);

  for (int s=2; s <= HOURS_RECS; s++) {
    HH--;
    if (HH < 0) {
      DD--;
      HH = 23;
    }
    sprintf(cMsg, "%02d%02d%02d%02d", YY, MM, DD, HH);
    hourData.Label = String(cMsg).toInt();
    _dThis = true;
    Debugf("Write HOURS record [%02d] Label[%08d]\n", s, hourData.Label);
    hourData.EDT1  += 100.0;
    hourData.ERT1  +=  50.0;
    hourData.EDT2  +=  66.0;
    hourData.ERT2  +=  22.0;
    hourData.GDT   +=   0.1;
    fileWriteData(HOURS, hourData, s);
  }
  _dThis = false;
  displayHoursHist(true);
  _dThis = true;
  Debugln("Done creating dummy hourData\r\n");

  _dThis = true;
  Debugln("Now rebooting\r");
  ESP.reset();

} // createDummyData()
#endif


//=======================================================================
uint32_t label2Fields(uint32_t Label, int8_t &YY, int8_t &MM, int8_t &DD, int8_t &HH) {
//=======================================================================
  char cKey[20];
  
  sprintf(cKey, "%08d", Label);
  _dThis = true;
  if (Verbose1) Debugf("Hours: Label in [%s] \n", cKey);
  YY = String(cKey).substring(0,2).toInt();
  MM = String(cKey).substring(2,4).toInt();
  DD = String(cKey).substring(4,6).toInt();
  HH = String(cKey).substring(6,8).toInt();

  if (YY <  0) YY =  0;
  if (YY > 99) YY = 99;
  if (MM <  1) MM =  1;
  if (MM > 12) MM = 12;
  if (DD <  1) DD =  1;
  if (DD > 31) DD = 31;
  if (HH <  0) HH =  0;
  if (HH > 24) HH = 24;

  sprintf(cKey, "%02d%02d%02d%02d", YY, MM, DD, HH);
  _dThis = true;
  if (Verbose1) Debugf("Label Out[%s] => YY[%2d], MM[%2d], DD[%2d], HH[%2d]\n", cKey, YY, MM, DD, HH);

  return String(cKey).toInt();
  
} // label2Fields()


//=======================================================================
uint32_t label2Fields(uint32_t Label, int8_t &YY, int8_t &MM, int8_t &DD) {
//=======================================================================
  char cKey[20];

  sprintf(cKey, "%06d", Label);
  _dThis = true;
  if (Verbose1) Debugf("Days: Label in [%s] \n", cKey);
  YY = String(cKey).substring(0,2).toInt();
  MM = String(cKey).substring(2,4).toInt();
  DD = String(cKey).substring(4,6).toInt();

  if (YY <  0) YY =  0;
  if (YY > 99) YY = 99;
  if (MM <  1) MM =  1;
  if (MM > 12) MM = 12;
  if (DD <  1) DD =  1;
  if (DD > 31) DD = 31;

  sprintf(cKey, "%02d%02d%02d", YY, MM, DD);
  _dThis = true;
  if (Verbose1) Debugf("Days: Label Out[%s] => YY[%2d], MM[%2d], DD[%2d]\n", cKey, YY, MM, DD);

  return String(cKey).toInt();
  
} // label2Fields()


//=======================================================================
uint32_t label2Fields(uint32_t Label, int8_t &YY, int8_t &MM) {
//=======================================================================
  char cKey[20];

  sprintf(cKey, "%04d", Label);
  _dThis = true;
  if (Verbose1) Debugf("Days: Label in [%s] \n", cKey);
  YY = String(cKey).substring(0,2).toInt();
  MM = String(cKey).substring(2,4).toInt();

  if (YY <  0) YY =  0;
  if (YY > 99) YY = 99;
  if (MM <  1) MM =  1;
  if (MM > 12) MM = 12;

  sprintf(cKey, "%02d%02d", YY, MM);
  _dThis = true;
  if (Verbose1) Debugf("Months: Label Out[%s] => YY[%2d], MM[%2d]\n", cKey, YY, MM);

  return String(cKey).toInt();
  
} // label2Fields()

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
