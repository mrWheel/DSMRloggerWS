/*
**  Copyright (c) 2019 Robert van den Breemen
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
* Inspired by the code from Harold - SolarMeter code
* Created by Robert van den Breemen (26 nov 2019)
*   - AaW  - cleanup code
*   - RvdB - changing into a statemachine and survive reboot
*   - RvdB - added AuthToken to settings
*   - RvdB - many more formatting for gas changed to 3 digits in DSMRlogger data
*   - RvdB - gas delivered should be [.3f] - lots of formatting of gasdelivered changed to 3 digits
*   - RvdB - changed around the way debug is done in rollover on month, day and hour
*   - RvdB - fixing the mindergas integration - mindergas.ino
*   - RvdB - added initial support for mindergas
*
*/
#define MINUTES             (60*1000)
#define MINDERGAS_INTERVAL  (5*MINUTES)  // 2 minuten -> mag ook 5 zijn .. toch?
#define MG_FILENAME         "/Mindergas.post"
#define WAIT_TIME           60000       // 60 seconden

uint32_t  mindergasTime                 = millis();
char      settingMindergasAuthtoken[21] = "";

//=======================================================================
void handleMindergas()
{
#ifdef USE_MINDERGAS
  if ((millis() - mindergasTime) > MINDERGAS_INTERVAL) 
  {
    mindergasTime = millis();
    processMindergas();
  }
#endif
} // handleMindergas()


#ifdef USE_MINDERGAS

enum states_of_MG { MG_INIT, MG_WAIT_FOR_FIRST_TELEGRAM, MG_WAIT_FOR_MIDNIGHT
                           , MG_WRITE_TO_FILE, MG_DO_COUNTDOWN
                           , MG_SEND_MINDERGAS, MG_NO_AUTHTOKEN, MG_ERROR };
                           
enum states_of_MG stateMindergas = MG_INIT;

int8_t    MG_Today                      = -1;
uint16_t  intStatuscodeMindergas        = 0; 
uint32_t  lastTime                      = millis();
uint32_t  MGcountdownTimer              = 0;
bool      validToken                    = false;
bool      handleMindergasSemaphore      = false;
char      txtResponseMindergas[30]      = "";
char      timeLastResponse[16]          = "";  


//=======================================================================
//force mindergas update, by skipping states
void forceMindergasUpdate()
{
  sprintf(timeLastResponse, "@%02d|%02d:%02d -> ", day(), hour(), minute());

  validToken = true;

  if (SPIFFS.exists(MG_FILENAME))
  {
    MGcountdownTimer = millis() + (1 *MINUTES);
    MG_Today = thisDay;                 // make it thisDay...
    strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "force Mindergas countdown");
    DebugTln(F("Force send data to mindergas.nl in ~1 minute"));
    stateMindergas = MG_DO_COUNTDOWN;
    processMindergas();
  }
  else
  {
    strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Force Write Mindergas.post");
    DebugTln(F("Force Write data to post file now!"));
    stateMindergas = MG_WRITE_TO_FILE;  // write file is next state
    processMindergas();
  }
  
} // forceMindergasUpdate()

//=======================================================================
// handle finite state machine of mindergas
void processMindergas()
{
  int8_t MGminuten = 0;
  time_t t;
  File   minderGasFile;

  if (handleMindergasSemaphore) // if already running ? then return...
  {
    DebugTln(F("already running .. bailing out!"));
    return; // you may only enter this once
  } 
  //signal that we are busy...
  handleMindergasSemaphore = true;
  
  yield(); 
  
  switch(stateMindergas) {
    case MG_INIT:  // only after reboot
      DebugTln(F("Mindergas State: MG_INIT"));
      sprintf(timeLastResponse, "@%02d|%02d:%02d -> ", day(), hour(), minute());
      if (SPIFFS.exists(MG_FILENAME))
      {
        MGcountdownTimer = millis() + (1 * MINUTES);
        strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "found Mindergas.post");
        validToken     = true;
        stateMindergas = MG_SEND_MINDERGAS;
        //DebugTln(F("Next State: MG_SEND_MINDERGAS"));
        break;
      } 
      // check to see if there is a authtoken
      validToken = (strlen(settingMindergasAuthtoken) > 5); // Assume there is a valid token, if there is a string. To be proven later.
      if  (validToken) 
      {
        //Next state is wait for first telegram
        //DebugTln(F("Next State: MG_WAIT_FOR_FIRST_TELEGRAM"));
        stateMindergas = MG_WAIT_FOR_FIRST_TELEGRAM; 
      }
      else
      {
        // No AuthToken
        DebugTln(F("MinderGas Authtoken is not set, no update can be done."));
        //DebugTln(F("Next State: MG_NO_AUTHTOKEN"));
        stateMindergas = MG_NO_AUTHTOKEN; // no token, no mindergas
      } // end-if 
      break;
      
    case MG_WAIT_FOR_FIRST_TELEGRAM:
      DebugTln(F("Mindergas State: MG_WAIT_FOR_FIRST_TELEGRAM"));
      // if you received at least one telegram, then wait for midnight
      if (telegramCount > 0) 
      {
        // Now you know what day it is, do setup MG_Today. This to enable day change detection.
        MG_Today = thisDay; 
        //DebugTln(F("Next State: MG_WAIT_FOR_MIDNIGHT"));
        stateMindergas = MG_WAIT_FOR_MIDNIGHT;
      }
      break;
      
    case MG_WAIT_FOR_MIDNIGHT:
      DebugTln(F("Mindergas State: MG_WAIT_FOR_MIDNIGHT"));
      // Detect day change at midnight, then...
      if (thisDay != MG_Today)              // It is no longer the same day, so it must be midnight
      {
        MG_Today = thisDay;                 // make it thisDay...
        //DebugTln(F("Next State: MG_WRITE_TO_FILE"));
        stateMindergas = MG_WRITE_TO_FILE;  // write file is next state
      }
      break;
      
    case MG_WRITE_TO_FILE:
      DebugTln(F("Mindergas State: MG_WRITE_TO_FILE"));
      // create POST and write to file, so it will survive a reset within the countdown period
      DebugTf("Writing to [%s] ..\r\n", MG_FILENAME);
      minderGasFile = SPIFFS.open(MG_FILENAME, "a"); //  create File
      if (!minderGasFile) 
      {
        // cannot create file, thus error
        DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", MG_FILENAME);
        // now in failure mode
        //DebugTln(F("Next State: MG_ERROR"));
        stateMindergas = MG_ERROR;
        break;
      } 
      // write POST respons into file
      yield();
      DebugTln(F("Start writing POST data "));
      t = now() - SECS_PER_DAY;  // we want to upload the gas usage of yesterday so rewind the clock for 1 day
      char dataString[80];
      sprintf(dataString,"{ \"date\": \"%04d-%02d-%02d\", \"reading\": \"%.3f\" }", year(t), month(t), day(t), GasDelivered);
      // write the POST to a file...
      minderGasFile.println(F("POST /api/gas_meter_readings HTTP/1.1"));
      minderGasFile.print(F("AUTH-TOKEN:")); minderGasFile.println(settingMindergasAuthtoken);
      minderGasFile.println(F("Host: mindergas.nl"));
      minderGasFile.println(F("User-Agent: DSMRWS"));
      minderGasFile.println(F("Content-Type: application/json"));
      minderGasFile.println(F("Accept: application/json"));
      minderGasFile.print(F("Content-Length: ")); minderGasFile.println(strlen(dataString));
      minderGasFile.println();
      minderGasFile.println(dataString);        

      minderGasFile.close();
      sprintf(timeLastResponse, "@%02d|%02d:%02d -> ", day(), hour(), minute());
      strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Mindergas.post aangemaakt");

      // start countdown
      MGminuten = random(10,120);
      MGcountdownTimer = millis() + (MGminuten * MINUTES); //within one hour   

      DebugTf("MinderGas update in [%d] minute(s)\r\n", MGminuten);
      // Lets'do the countdown
      //DebugTln(F("Next State: MG_DO_COUNTDOWN"));
      stateMindergas = MG_DO_COUNTDOWN;
      break;
      
    case MG_DO_COUNTDOWN:
      DebugTln(F("Mindergas State: MG_DO_COUNTDOWN"));
      sprintf(timeLastResponse, "@%02d|%02d:%02d -> ", day(), hour(), minute());
      strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "countdown for sending");
      if (millis() < MGcountdownTimer) 
      {
        DebugTf("MinderGas update in less than [%d] minutes\r\n", ((MGcountdownTimer - millis()) / MINUTES) +1);
        intStatuscodeMindergas = ((MGcountdownTimer - millis()) / MINUTES) +1;
      }
      else 
      {
        // when waitime is done, then it's time to send the POST string
        //DebugTln(F("Next State: MG_SEND_MINDERGAS"));
        intStatuscodeMindergas = 0;
        stateMindergas = MG_SEND_MINDERGAS;
      }
      break;
      
    case MG_SEND_MINDERGAS:
      DebugTln(F("Mindergas State: MG_SEND_MINDERGAS"));
      strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "try to send Mindergas.post");

      // if POST response for Mindergas exists, then send it... btw it should exist by now :)
      if ((validToken) && SPIFFS.exists(MG_FILENAME)) 
      {  
         // start the update of mindergas, when the countdown counter reaches 0
          WiFiClient MGclient;   
          //WiFiClientSecure client;          
          // try to connect to minderGas
          DebugTln(F("Connecting to Mindergas..."));
          //connect over http with mindergas
          if (MGclient.connect((char*)"mindergas.nl",80)) 
          {
            // create a string with the date and the meter value
            minderGasFile = SPIFFS.open(MG_FILENAME, "r");
            String sBuffer;
            sBuffer = "";
            while(minderGasFile.available()) 
            { 
              char ltr = minderGasFile.read();
              sBuffer += ltr;
            }
            minderGasFile.close();
            // then post to mindergas...
            DebugTln(F("Reading POST from file:"));
            Debugln(sBuffer);
            DebugTln(F("Send to Mindergas.nl..."));
            MGclient.println(sBuffer);
            // read response from mindergas.nl
            sprintf(timeLastResponse, "@%02d|%02d:%02d >> ", day(), hour(), minute());
            DebugTf("[%s] Mindergas response: ", timeLastResponse);
            bool bDoneResponse = false;
            while (!bDoneResponse && (MGclient.connected() || MGclient.available())) 
            {
              if (MGclient.available()) 
              {
                  // read the status code the response
                  if (MGclient.find("HTTP/1.1"))
                  {
                    // skip to find HTTP/1.1
                    // then parse response code
                    intStatuscodeMindergas = MGclient.parseInt(); // parse status code
                    //Debugln();
                    Debugf("Statuscode: [%d]\r\n", intStatuscodeMindergas);
                    switch (intStatuscodeMindergas) {
                      case 401:
                        validToken = false;
                        strCopy(settingMindergasAuthtoken, sizeof(settingMindergasAuthtoken), "Invalid token"); 
                        strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Unauthorized, token invalid!"); // report error back to see in settings page
                        DebugTln(F("Invalid Mindergas Authenication Token"));
                        stateMindergas = MG_NO_AUTHTOKEN;
                        break;
                        
                      case 422:
                        validToken = true;
                        strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Unprocessed entity"); // report error back to see in settings page
                        DebugTln(F("Unprocessed entity, goto website mindergas for more information")); 
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;              
                        break;
                        
                      case 201:  
                        validToken = true;
                        strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Created entry"); // report error back to see in settings page
                        DebugTln(F("Succes, the gas delivered has been added to your mindergas.nl account"));
                        DebugTln(F("Next State: MG_WAIT_FOR_MIDNIGHT"));
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;               
                        break;
                        
                      default:
                        validToken = true;
                        strCopy(txtResponseMindergas, sizeof(txtResponseMindergas), "Unknown response code"); // report error back to see in settings page
                        DebugTln(F("Unknown responsecode, goto mindergas for information"));
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;           
                        break;
                    } // end switch-case             
                  }  // end-if find HTTP/1.1
                  
                  //close HTTP connection
                  MGclient.stop();
                  DebugTln(F("Disconnected from mindergas.nl"));
                  // delete POST file from SPIFFS
                  if (SPIFFS.remove(MG_FILENAME)) 
                  {
                    DebugTln(F("POST Mindergas file succesfully deleted!"));
                  } 
                  else 
                  {
                    // help, this should just not happen, but if it does, it will not influence behaviour in a negative way
                    DebugTln(F("Failed to delete POST Mindergas file"));
                  } 
                  bDoneResponse = true;    
              } // end-if client.available() 
              else 
              {
                // wait for connections, just keep trying...
                delay(100); 
              } // end-else
            } // end-while
        } // sending done
      } // end-if file exists
      break;
      
    case MG_NO_AUTHTOKEN:
      DebugTln(F("Mindergas State: MG_NO_AUTHTOKEN"));
      if (validToken)
      {
        stateMindergas = MG_INIT;   
      }
      // Do not update mindergas when a failing token is detected
      break;
      
    case MG_ERROR:
      DebugTln(F("Mindergas State: MG_ERROR"));
      break;
      
    default:
      DebugTln(F("Mindergas State: Impossible, default state!")); 
      break;  
          
  } // switch(..)

  //on exit, allow next handle state event
  handleMindergasSemaphore = false;
  
} // processMindergas()

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
