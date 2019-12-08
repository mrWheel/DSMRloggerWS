/*
**  Copyright (c) 2019 Robert van den Breemen
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
* Inspired by the code from Harold - SolarMeter code
* Created by Robert van den Breemen (26 nov 2019)
*   - RvdB - changing into a statemachine and survive reboot
*   - RvdB - added AuthToken to settings
*   - RvdB - many more formatting for gas changed to 3 digits in DSMRlogger data
*   - RvdB - gas delivered should be [.3f] - lots of formatting of gasdelivered changed to 3 digits
*   - RvdB - changed around the way debug is done in rollover on month, day and hour
*   - RvdB - fixing the mindergas integration - mindergas.ino
*   - RvdB - added initial support for mindergas
*
*/

#ifdef USE_MINDERGAS

#define MG_FILENAME         "/Mindergas.post"
#define WAIT_TIME           60000       // 60 seconden
#define MINDERGAS_INTERVAL  2*60*1000   // 2 minuten -> mag ook 5 zijn .. toch?

enum states_of_MG { MG_INIT, MG_WAIT_FOR_FIRST_TELEGRAM, MG_WAIT_FOR_MIDNIGHT, MG_WRITE_TO_FILE, MG_START_COUNTDOWN, MG_DO_COUNTDOWN, MG_SENDING_MINDERGAS, MG_NO_AUTHTOKEN, MG_ERROR };
enum states_of_MG stateMindergas = MG_INIT;

int8_t    Today                         = -1;
uint16_t  intStatuscodeMindergas        = 0; 
uint32_t  mindergasTime                 = millis();
uint32_t  lastTime                      = millis();
bool      validToken                    = false;
bool      handleMindergasSemaphore      = false;
char      settingMindergasAuthtoken[21] = "";
char      txtResponseMindergas[30]      = "";
char      dateLastResponse[15]          = "@--|hh:mm -> ";  
//?char   txtLastUpdateMindergas[30]    = "";
byte      byteUpdateMindergasCountdown  = 0;
byte      GasCountdown                  = 0;
File      minderGasFile;


//=======================================================================
//force mindergas update, by skipping states
void forceMindergasUpdate()
{
/***
  //Skip countdown state, this will force an update.
  switch (stateMindergas) {
    case MG_WAIT_FOR_MIDNIGHT: //skip to countdown mode
      stateMindergas = MG_WRITE_TO_FILE;
      DebugTln(F("Skip wait for midnight, write file and start countdown now..."));
      break;
      
    case MG_DO_COUNTDOWN: //skip to sending mode
      stateMindergas = MG_SENDING_MINDERGAS;
      DebugTln(F("Skip do the countdown, send to mindergas now..."));
      break;
    
    default:
      DebugTln(F("Nothing to skip..."));
      //in all other situation do nothing
      break;
  } 
***/
  sprintf(dateLastResponse, "@%02d|%02d:%02d -> ", day(), hour(), minute());

  if (!SPIFFS.exists(MG_FILENAME))
  {
    validToken = true;
    stateMindergas = MG_WRITE_TO_FILE;  // write file is next state
    DebugTln(F("Force Write data to post file"));
    processMindergas();
  }

  validToken = true;
  stateMindergas = MG_SENDING_MINDERGAS;
  DebugTln(F("Force send data to mindergas.nl"));
  processMindergas();
  
} // forceMindergasUpdate()

//=======================================================================
void handleMindergas()
{
  if ((millis() - mindergasTime) > MINDERGAS_INTERVAL) 
  {
    mindergasTime = millis();
    processMindergas();
  }

} // handleMindergas()

//=======================================================================
// handle finite state machine of mindergas
void processMindergas()
{
  if (handleMindergasSemaphore) // if already running ? then return...
  {
    DebugTln(F("already running .. bailing out!"));
    return; // you may only enter this once
  } 
  //signal that we are busy...
  handleMindergasSemaphore = true;
  
  yield(); 
  
  switch(stateMindergas){
    case MG_INIT:  // only after reboot
      if (Verbose1) DebugTln(F("Mindergas State: MG_INIT"));
      if (SPIFFS.exists(MG_FILENAME))
      {
        stateMindergas = MG_START_COUNTDOWN;
        break;
      } 
      //Next state is wait for first telegram
      stateMindergas = MG_WAIT_FOR_FIRST_TELEGRAM; 
      // check to see if there is a authtoken
      validToken = (String(settingMindergasAuthtoken).length()>0); //Assume there is a valid token, if there is a string. To be proven later.
      if  (!validToken) 
      {
        //No AuthToken
        DebugTln(F("MinderGas Authtoken is not set, no update can be done."));
        // ? sprintf(txtLastUpdateMindergas, " [%02d/%02d:%02d] No authentication token.", day(),hour(), minute()); 
        stateMindergas = MG_NO_AUTHTOKEN; //no token, no mindergas
      } else 
      { 
        //check to see if update in progress
        //SPIFFS.begin(); <- wordt toch al in setup() gedaan?
        if (SPIFFS.exists(MG_FILENAME))
        {
          //restart countdown
          GasCountdown = random(1,60); //within one hour   
          DebugTf("MinderGas Countdown re-started... in [%6d] minute(s)\r\n", GasCountdown);
          // ? sprintf(txtLastUpdateMindergas, " [%02d/%02d:%02d] Restart countdown...", day(), hour(), minute());
          stateMindergas = MG_DO_COUNTDOWN;
        } //if Reboot File exists
      }// end-if 
      break;
      
    case MG_WAIT_FOR_FIRST_TELEGRAM:
      if (Verbose1) DebugTln(F("Mindergas State: MG_WAIT_FOR_FIRST_TELEGRAM"));
      //if you received at least one telegram, then wait for midnight
      if (telegramCount>0) 
      {
        // ? sprintf(txtLastUpdateMindergas, " [%02d/%02d:%02d] Wait for midnight...", day(), hour(), minute());
        //Now you know what day it is, do setup today. This to enable day change detection.
        Today = thisDay; 
        stateMindergas = MG_WAIT_FOR_MIDNIGHT;
      }
      break;
      
    case MG_WAIT_FOR_MIDNIGHT:
      if (Verbose1) DebugTln(F("Mindergas State: MG_WAIT_FOR_MIDNIGHT"));
      //Detect day change at midnight, then...
      if (thisDay!=Today)    //It is no longer the same day, so it must be midnight
      {
        Today = thisDay;  //make it today...
        stateMindergas = MG_WRITE_TO_FILE;  //write file is next state
      }
      break;
      
    case MG_WRITE_TO_FILE:
      if (Verbose1) DebugTln(F("Mindergas State: MG_WRITE_TO_FILE"));
      //create POST and write to file, so it will survive a reset within the countdown period
      //if (!SPIFFS.begin())
      //{
      //  DebugTln("Serious problem with SPIFFS, not mounted");
      //};
      //yield();
      DebugT(F("Writing to [")); Debug(MG_FILENAME); Debugln(F("] ..."));
      minderGasFile = SPIFFS.open(MG_FILENAME, "a"); //  create File
      if (!minderGasFile) 
      {
        //cannot create file, thus error
        DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", MG_FILENAME);
        //no state change, stay in failure mode
        stateMindergas = MG_ERROR;
      } else 
      {
        //write POST respons into file
        yield();
        DebugTln(F("Start writing POST data "));
        time_t t = now() - SECS_PER_DAY;  // we want to upload the gas usage of yesterday so rewind the clock for 1 day
        char dataString[80];
        sprintf(dataString,"{ \"date\": \"%04d-%02d-%02d\", \"reading\": \"%.3f\" }", year(t), month(t), day(t), GasDelivered);
        sprintf(dateLastResponse, "@%02d|%02d:%02d -> ", day(), hour(), minute());
        //write the POST to a file...
        minderGasFile.println(F("POST /api/gas_meter_readings HTTP/1.1"));
        minderGasFile.print(F("AUTH-TOKEN:")); minderGasFile.println(settingMindergasAuthtoken);
        minderGasFile.println(F("Host: mindergas.nl"));
        minderGasFile.println(F("User-Agent: DSMRWS"));
        minderGasFile.println(F("Content-Type: application/json"));
        minderGasFile.println(F("Accept: application/json"));
        minderGasFile.print(F("Content-Length: ")); minderGasFile.println(strlen(dataString));
        minderGasFile.println();
        minderGasFile.println(dataString);        
      }
      minderGasFile.close();
      //check to see if there is now a file that can be opened
      minderGasFile  = SPIFFS.open(MG_FILENAME, "r+");       // open for Read & writing
      if (minderGasFile) 
      {
        minderGasFile.close();
        //if you can open the file, then goto next state
        stateMindergas = MG_START_COUNTDOWN;
      } else 
      {
          DebugTf("Something is very wrong writing to [%s]\r\n", MG_FILENAME);
      }
      break;
      
    case MG_START_COUNTDOWN:
      if (Verbose1) DebugTln(F("Mindergas State: MG_START_COUNTDOWN"));
      //start countdown
      GasCountdown = random(1,60); //within one hour
      DebugTf("MinderGas update in [%2d] minute(s)\r\n", GasCountdown);
      // ? sprintf(txtLastUpdateMindergas, " [%02d/%02d:%02d] Starting countdown...", day(), hour(), minute());
      //Lets'do the countdown
      stateMindergas = MG_DO_COUNTDOWN;
      break;
      
    case MG_DO_COUNTDOWN:
      if (Verbose1) DebugTln(F("Mindergas State: MG_DO_COUNTDOWN"));
      if (millis() - lastTime > WAIT_TIME) 
      {
        // wait time has passed, countdown by 1 minute
        lastTime = millis();
        // Countdown to 0, then update the Gas Delivered, and write it with date from yesterday.
        GasCountdown--;
        DebugTf("MinderGas update in [%2d] minute(s)\r\n", GasCountdown);
        // ? sprintf(txtLastUpdateMindergas, " [%02d/%02d:%02d] Countdown running...", day(), hour(), minute());
        if (GasCountdown==0) {
          //when waitime is done, then it's time to send the POST string
          stateMindergas = MG_SENDING_MINDERGAS;
        } // else no-state-change, and keep waiting...
      }
      break;
      
    case MG_SENDING_MINDERGAS:
      if (Verbose1) DebugTln(F("Mindergas State: MG_SENDING_MINDERGAS"));
      // ? sprintf(txtLastUpdateMindergas, " [%02d/%02d:%02d] Sending to mindergas...", day(), hour(), minute());
      // if POST response for Mindergas exists, then send it... btw it should exist by now :)
      if ((validToken) && SPIFFS.exists(MG_FILENAME)) 
      {  
         // start the update of mindergas, when the countdown counter reaches 0
          //WiFiClient client;   
          //WiFiClientSecure client;          
          // try to connect to minderGas
          DebugTln(F("Connecting to Mindergas..."));
          //connect over https with mindergas
          if (wifiClient.connect((char*)"mindergas.nl",80)) 
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
            //then post to mindergas...
            DebugTln(F("Reading POST from file:"));
            Debugln(sBuffer);
            DebugTln(F("Send to Mindergas.nl..."));
            wifiClient.println(sBuffer);
            // read response from mindergas.nl
            DebugT(F("Mindergas response: "));
            bool bDoneResponse = false;
            while (!bDoneResponse && (wifiClient.connected() || wifiClient.available())) 
            {
              if (wifiClient.available()) 
              {
                  // read the status code the response
                  if (wifiClient.find("HTTP/1.1"))
                  {
                    // skip to find HTTP/1.1
                    //then parse response code
                    intStatuscodeMindergas = wifiClient.parseInt(); // parse status code
                    Debugln();
                    DebugT("Statuscode: "); Debugln(intStatuscodeMindergas);
                    switch (intStatuscodeMindergas) {
                      case 401:
                        validToken = false;
                        strcpy(settingMindergasAuthtoken, "Invalid token"); 
                        strcpy(txtResponseMindergas, "Unauthorized, token invalid!"); //report error back to see in settings page
                        DebugTln(F("Invalid Mindergas Authenication Token"));
                        stateMindergas = MG_NO_AUTHTOKEN;
                        break;
                        
                      case 422:
                        validToken = true;
                        strcpy(txtResponseMindergas, "Unprocessed entity"); //report error back to see in settings page
                        DebugTln(F("Unprocessed entity, goto website mindergas for more information")); 
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;              
                        break;
                        
                      case 201:  
                        validToken = true;
                        strcpy(txtResponseMindergas, "Created entry"); //report error back to see in settings page
                        DebugTln(F("Succes, the gas delivered has been added to your mindergas.nl account"));
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;               
                        break;
                        
                      default:
                        validToken = true;
                        strcpy(txtResponseMindergas, "Unknown response code"); //report error back to see in settings page
                        DebugTln(F("Unknown responsecode, goto mindergas for information"));
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;           
                        break;
                    } //end switch-case             
                  }  //end-if find HTTP/1.1
                  
                  //close HTTP connection
                  //??? client.stop();
                  DebugTln(F("Disconnected from mindergas.nl"));
                  //delete POST file from SPIFFS
                  if (SPIFFS.remove(MG_FILENAME)) 
                  {
                    DebugTln(F("POST Mindergas file succesfully deleted!"));
                  } else 
                  {
                    //help, this should just not happen, but if it does, it will not influence behaviour in a negative way
                    DebugTln(F("Failed to delete POST Mindergas file"));
                  } 
                  bDoneResponse = true;    
              } //end-if client.available() 
              else 
              {
                //wait for connections, just keep trying...
                Debug(F("."));
                delay(100); 
              } // end-else
            } //end-while
        } //sending done
      } //end-if file exists
      break;
      
    case MG_NO_AUTHTOKEN:
      if (Verbose1) DebugTln(F("Mindergas State: MG_NO_AUTHTOKEN"));
      if (millis() - lastTime > WAIT_TIME) 
      {
        lastTime = millis();
        // wait 1 minute, maybe user will enter token, retry once a minute.
        // ? sprintf(txtLastUpdateMindergas, " [%02d/%02d:%02d] Setup authtoken in settings", day(), hour(), minute());
        stateMindergas = MG_INIT;   
      }
      //Do not update mindergas when a failing token is detected
      break;
      
    case MG_ERROR:
      if (Verbose1) DebugTln(F("Mindergas State: MG_ERROR"));
      // ? sprintf(txtLastUpdateMindergas, " [%02d/%02d:%02d] This should not happen! Error.", day(), hour(), minute()); 
      break;
      
    default:
      if (Verbose1) DebugTln(F("Mindergas State: Impossible, default state!")); 
      // ? sprintf(txtLastUpdateMindergas, " [%02d/%02d:%02d] Impossible state! Contact developer..", day(), hour(), minute());           
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
