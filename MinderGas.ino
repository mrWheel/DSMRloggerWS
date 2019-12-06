/*
**  Copyright (c) 2019 Robert van den Breemen
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
* Inspired by the code from Harold - SolarMeter code
* Created by Robert van den Breemen (16 nov 2019)
* 
*   - RB - added AuthToken to settings
*   - RB - many more formatting for gas changed to 3 digits
*      
*   - RB - gas delivered should be [.3f] - lots of formatting of gasdelivered changed to 3 digits
*   - RB - changed around the way debug is done in rollover on month, day and hour
*   - RB - fixing the mindergas integration - mindergas.ino
*   - RB - added initial support for mindergas
*
*/

#ifdef USE_MINDERGAS

byte GasCountdown = 0;
uint32_t const WAIT_TIME = 60000; //milliseconds = 60 seconds
uint32_t lastTime = millis();
bool validToken = false;
File dataFile;
bool bHandleMindergas = false;

// this function is called at midnight
void updateMindergas(float GasDelivered)
{
    // select a number between 1 and 59
    // this will be the minutes to wait before uploading
    if (String(settingMindergasAuthtoken).length()!=0) {
      // If authtoken exists, then start countdown
      GasCountdown = random(1,60);
      DebugTf("MinderGas Countdown started... in [%6d] minute(s)\r\n", GasCountdown);
  
      // the actual total-gas value is saved
      TotalGas = GasDelivered;
      DebugTf("GasDelivered = [%.3f]\r\n", GasDelivered);
      //now lets wait until the random waittime has passed
    } else {
      // no authtoken set, report on debug
      DebugTln("MinderGas Authtoken is not set, no update is done.");
    }
}

// handle finite state machine of mindergas
void handleMindergas(){
  if (bHandleMindergas) //if busy, then return...
    return; // you may only enter this once
  //signal that we are busy...
  bHandleMindergas=true;
  
  yield(); 
  
  switch(stateMindergas){
    case MG_INIT :  //after reboot
      if (Verbose1) DebugTln(F("Mindergas State: MG_INIT"));
      //Next state is wait for first telegram
      stateMindergas = MG_WAIT_FOR_FIRST_TELEGRAM; 
      // check to see if there is a authtoken
      validToken = (String(settingMindergasAuthtoken).length()>0); //Assume there is a valid token, if there is a string. To be proven later.
      if  (!validToken) {
        //No AuthToken
        DebugTln(F("MinderGas Authtoken is not set, no update can be done."));
        stateMindergas = MG_NO_AUTHTOKEN; //no token, no mindergas
      } else { 
        //check to see if update in progress
        SPIFFS.begin();
        if (SPIFFS.exists(MG_FILENAME)){
          //restart countdown
          GasCountdown = random(1,60); //within one hour   
          DebugTf("MinderGas Countdown re-started... in [%6d] minute(s)\r\n", GasCountdown);
          stateMindergas = MG_DO_COUNTDOWN;
        } //if Reboot File exists
      }// end-if 
    break;
    case MG_WAIT_FOR_FIRST_TELEGRAM:
      if (Verbose1) DebugTln(F("Mindergas State: MG_WAIT_FOR_FIRST_TELEGRAM"));
      //if you received at least one telegram, then wait for midnight
      if (telegramCount>0) {
        stateMindergas = MG_WAIT_FOR_MIDNIGHT;
        //Now you know what day it is, do setup today. This to enable day change detection.
        Today = thisDay; 
      }
    break;
    case MG_WAIT_FOR_MIDNIGHT:
      if (Verbose1) DebugTln(F("Mindergas State: MG_WAIT_FOR_MIDNIGHT"));
      //Detect day change at midnight, then...
      if (thisDay!=Today) { //It is no longer the same day, so it must be midnight
        Today = thisDay;  //make it today...
        stateMindergas = MG_WRITE_TO_FILE;  //write file is next state
      }
      break;
    case MG_WRITE_TO_FILE:
      if (Verbose1) DebugTln(F("Mindergas State: MG_WRITE_TO_FILE"));
      //create POST and write to file, so it will survive a reset within the countdown period
      if (!SPIFFS.begin()){
        DebugTln("Serious problem with SPIFFS, not mounted");
      };
      //yield();
      Debug(F("Writing to ")); Debug(MG_FILENAME); Debugln(F("..."));
      dataFile = SPIFFS.open(MG_FILENAME, "a"); //  create File
      if (!dataFile) {
        //cannot create file, thus error
        DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", MG_FILENAME);
        //no state change, stay in failure mode
        stateMindergas = MG_ERROR;
      } else {
        //write POST respons into file
        yield();
        DebugT(F("\r\nStart writing setting data "));
        time_t t = now() - SECS_PER_DAY;  // we want to upload the gas usage of yesterday so rewind the clock for 1 day
        char dataString[80];
        sprintf(dataString,"{ \"date\": \"%04d-%02d-%02d\", \"reading\": \"%.3f\" }", year(t), month(t), day(t), GasDelivered);
        //write the POST to a file...
        dataFile.println(F("POST /api/gas_meter_readings HTTP/1.1"));
        dataFile.print(F("AUTH-TOKEN:")); dataFile.println(settingMindergasAuthtoken);
        dataFile.println(F("Host: mindergas.nl"));
        dataFile.println(F("User-Agent: DSMRWS"));
        dataFile.println(F("Content-Type: application/json"));
        dataFile.println(F("Accept: application/json"));
        dataFile.print(F("Content-Length: ")); dataFile.println(strlen(dataString));
        dataFile.println();
        dataFile.println(dataString);        
      }
      dataFile.close();
      //check to see if there is now a file that can be opened
      dataFile  = SPIFFS.open(MG_FILENAME, "r+");       // open for Read & writing
      if (dataFile) {
        //if you can open the file, then goto next state
        stateMindergas = MG_START_COUNTDOWN;
      } else {
          DebugTf("Something is very wrong writing to [%s]\r\n", MG_FILENAME);
      }
      dataFile.close();
    break;
    case MG_START_COUNTDOWN :
      if (Verbose1) DebugTln(F("Mindergas State: MG_START_COUNTDOWN"));
      //start countdown
      GasCountdown = random(1,60); //within one hour
      DebugTf("MinderGas update in [%2d] minute(s)\r\n", GasCountdown);
      //Lets'do the countdown
      stateMindergas = MG_DO_COUNTDOWN;
    break;
    case MG_DO_COUNTDOWN:
      if (Verbose1) DebugTln(F("Mindergas State: MG_DO_COUNTDOWN"));
      if (millis() - lastTime > WAIT_TIME) {
        // wait time has passed, countdown by 1 minute
        lastTime = millis();
        // Countdown to 0, then update the Gas Delivered, and write it with date from yesterday.
        GasCountdown--;
        DebugTf("MinderGas update in [%2d] minute(s)\r\n", GasCountdown);
        if (GasCountdown==0) {
          //when waitime is done, then it's time to send the POST string
          stateMindergas = MG_SENDING_MINDERGAS;
        } // else no-state-change, and keep waiting...
      }
    break;
    case MG_SENDING_MINDERGAS:
      if (Verbose1) DebugTln(F("Mindergas State: MG_SENDING_MINDERGAS"));
      // if POST response for Mindergas exists, then send it... btw it should exist by now :)
      if ((validToken) && SPIFFS.exists(MG_FILENAME)) {  
         // start the update of mindergas, when the countdown counter reaches 0
          WiFiClient client;   
          //WiFiClientSecure client;          
          // try to connect to minderGas
          DebugTln(F("Connecting to Mindergas..."));
          //connect over https with mindergas
          if (client.connect((char*)"mindergas.nl",80)) {
            // create a string with the date and the meter value
            dataFile = SPIFFS.open(MG_FILENAME, "r");
            String sBuffer;
            sBuffer = "";
            while(dataFile.available()) { 
              char ltr = dataFile.read();
              sBuffer += ltr;
            }
            dataFile.close();
            //then post to mindergas...
            DebugTln(F("Reading POST from file:"));
            Debugln(sBuffer);
            DebugTln(F("Send to Mindergas.nl..."));
            client.println(sBuffer);
            // read response from mindergas.nl
            DebugT(F("Mindergas response: "));
            bool bDoneResponse = false;
            while (!bDoneResponse && (client.connected() || client.available())) {
              if (client.available()) {
                  // read the status code the response
                  if (client.find("HTTP/1.1")){
                    // skip to find HTTP/1.1
                    //then parse response code
                    intStatuscodeMindergas = client.parseInt(); // parse status code
                    Debugln();
                    DebugT("Statuscode: "); Debugln(intStatuscodeMindergas);
                    switch (intStatuscodeMindergas){
                      case 401:
                        validToken = false;
                        strcpy(settingMindergasAuthtoken, "Invalid token"); 
                        strcpy(txtResponseMindergas, "Unauthorized, token invalid!"); //report error back to see in settings page
                        DebugTln("Invalid Mindergas Authenication Token");
                        stateMindergas = MG_NO_AUTHTOKEN;
                      break;
                      case 422:
                        validToken = true;
                        strcpy(txtResponseMindergas, "Unprocessed entity"); //report error back to see in settings page
                        DebugTln("Unprocessed entity, goto website mindergas for more information"); 
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;              
                      break;
                      case 201:  
                        validToken = true;
                        strcpy(txtResponseMindergas, "Created entry"); //report error back to see in settings page
                        DebugTln("Succes, the gas delivered has been added to your mindergas.nl account");
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;               
                      break;
                      default:
                        validToken = true;
                        strcpy(txtResponseMindergas, "Unknown response code"); //report error back to see in settings page
                        DebugTln("Unknown responsecode, goto mindergas for information");
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;           
                        break;
                    } //end switch-case             
                  }  //end-if find HTTP/1.1
                  
                  //close HTTP connection
                  client.stop();
                  DebugTln(F("Disconnected from mindergas.nl"));
                  //delete POST file from SPIFFS
                  if (SPIFFS.remove(MG_FILENAME)) {
                    DebugTln(F("POST Mindergas file succesfully deleted!"));
                  } else {
                    //help, this should just not happen, but if it does, it will not influence behaviour in a negative way
                    DebugTln(F("Failed to delete POST Mindergas file"));
                  } 
                  bDoneResponse = true;    
              } //end-if client.available() 
              else {
                //wait for connections, just keep trying...
                Debug(F("."));
                delay(100); 
              }// end-else
            }//end-while
        }//sending done
      } //end-if file exists
      break;
    case MG_NO_AUTHTOKEN:
      if (Verbose1) DebugTln(F("Mindergas State: MG_NO_AUTHTOKEN"));
      //Do not update mindergas when a failing token is detected
      break;
    case MG_ERROR:
      if (Verbose1) DebugTln(F("Mindergas State: MG_ERROR"));
      break;
    default:
      if (Verbose1) DebugTln(F("Mindergas State: Impossible, default state!"));      
      break;  
          
  }

  //on exit, allow next handle state event
  bHandleMindergas = false;
}

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
