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
#define MG_FILENAME      "/Mindergas.post"

enum states_of_MG { MG_INIT, MG_WAIT_FOR_FIRST_TELEGRAM, MG_WAIT_FOR_MIDNIGHT, MG_WRITE_TO_FILE, MG_START_COUNTDOWN, MG_DO_COUNTDOWN, MG_SENDING_MINDERGAS, MG_NO_AUTHTOKEN, MG_ERROR };
enum states_of_MG stateMindergas = MG_INIT;

int8_t Today = -1;
uint32_t const WAIT_TIME = 60000; //milliseconds = 60 seconds
uint32_t lastTime = millis();
bool validToken = false;
File dataFile;
bool bHandleMindergas = false;

//force mindergas update, by skipping states
void forceMindergasUpdate()
{
  //Skip countdown state, this will force an update.
  switch (stateMindergas){
    case MG_WAIT_FOR_MIDNIGHT: //skip to countdown mode
      stateMindergas = MG_WRITE_TO_FILE;
      DebugTln(F("Skip wait for midnight, write file and start countdown now..."));
      break;
      
    case MG_DO_COUNTDOWN: //skip to sending mode
      byteUpdateMindergasCountdown=0; //Skip countdown to 0
      stateMindergas = MG_SENDING_MINDERGAS;
      DebugTln(F("Skip do the countdown, send to mindergas now..."));
      break;
      
    default:
      DebugTln(F("Nothing to skip..."));
      //in all other situation do nothing
      break;
  } 
}

// handle finite state machine of mindergas
void handleMindergas()
{
  if (bHandleMindergas) //if busy, then return...
    return; // you may only enter this once
  //signal that we are busy...
  bHandleMindergas=true;
  
  yield(); 
  
  switch(stateMindergas) {
    case MG_INIT :  //after reboot
      lastTime = millis(); // reset time counter
      if (Verbose1) DebugTln(F("Mindergas State: MG_INIT"));
      //Next state is wait for first telegram
      stateMindergas = MG_WAIT_FOR_FIRST_TELEGRAM; 
      sprintf(txtLastUpdateMindergas, "Wait for first telegram...");
      // check to see if there is a authtoken
      validToken = (String(settingMindergasAuthtoken).length()>0); //Assume there is a valid token, if there is a string. To be proven later.
      if  (!validToken) 
      {
        //No AuthToken
        DebugTln(F("MinderGas Authtoken is not set, no update can be done."));
        stateMindergas = MG_NO_AUTHTOKEN; //no token, no mindergas
        sprintf(txtLastUpdateMindergas, "No authenication token."); 
      } else 
      { 
        //check to see if update in progress
        SPIFFS.begin();
        if (SPIFFS.exists(MG_FILENAME))
        {
          //restart countdown
          byteUpdateMindergasCountdown = random(1,60); //within one hour   
          sprintf(txtLastUpdateMindergas, "Restart countdown...");
          DebugTf("MinderGas Countdown re-started... in [%6d] minute(s)\r\n", byteUpdateMindergasCountdown);
          stateMindergas = MG_DO_COUNTDOWN;
        } //if Reboot File exists
      }// end-if 
      break;
      
    case MG_WAIT_FOR_FIRST_TELEGRAM:
      if (Verbose1) DebugTln(F("Mindergas State: MG_WAIT_FOR_FIRST_TELEGRAM"));
      //if you received at least one telegram, then wait for midnight
      if (telegramCount>0) 
      {
        stateMindergas = MG_WAIT_FOR_MIDNIGHT;
        sprintf(txtLastUpdateMindergas, "Wait for midnight...");
        //Now you know what day it is, do setup today. This to enable day change detection.
        Today = thisDay; 
      }
      break;
      
    case MG_WAIT_FOR_MIDNIGHT:
      if (Verbose1) DebugTln(F("Mindergas State: MG_WAIT_FOR_MIDNIGHT"));
      //Detect day change at midnight, then...
      if (thisDay!=Today)   //It is no longer the same day, so it must be midnight
      {
        Today = thisDay;                    //make it today...
        stateMindergas = MG_WRITE_TO_FILE;  //write file is next state
      }
      break;
      
    case MG_WRITE_TO_FILE:
      if (Verbose1) DebugTln(F("Mindergas State: MG_WRITE_TO_FILE"));
      //create POST and write to file, so it will survive a reset within the countdown period
      if (!SPIFFS.begin())
      {
        DebugTln("Serious problem with SPIFFS, not mounted");
      };
      //yield();
      DebugT(F("Writing to [")); Debug(MG_FILENAME); Debugln(F("] ..."));
      dataFile = SPIFFS.open(MG_FILENAME, "a"); //  create File
      if (!dataFile) 
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
        sprintf(txtLastUpdateMindergas, "Writing POST for mindergas.");
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
      if (dataFile) 
      {
        //if you can open the file, then goto next state
        stateMindergas = MG_START_COUNTDOWN;
      } else 
      {
          DebugTf("Something is very wrong writing to [%s]\r\n", MG_FILENAME);
      }
      dataFile.close();
      break;
      
    case MG_START_COUNTDOWN:
      if (Verbose1) DebugTln(F("Mindergas State: MG_START_COUNTDOWN"));
      //start countdown
      sprintf(txtLastUpdateMindergas, "Starting countdown...");
      byteUpdateMindergasCountdown = random(1,60); //within one hour
      DebugTf("MinderGas update in [%2d] minute(s)\r\n", byteUpdateMindergasCountdown);
      //Lets'do the countdown
      stateMindergas = MG_DO_COUNTDOWN;
      break;
      
    case MG_DO_COUNTDOWN:
      if (Verbose1) DebugTln(F("Mindergas State: MG_DO_COUNTDOWN"));
      sprintf(txtLastUpdateMindergas, "Countdown running...");
      if (millis() - lastTime > WAIT_TIME) 
      {
        // wait time has passed, countdown by 1 minute
        lastTime = millis();
        // Countdown to 0, then update the Gas Delivered, and write it with date from yesterday.
        byteUpdateMindergasCountdown--;
        DebugTf("MinderGas update in [%2d] minute(s)\r\n", byteUpdateMindergasCountdown);
        if (byteUpdateMindergasCountdown==0) 
        {
          //when waitime is done, then it's time to send the POST string
          stateMindergas = MG_SENDING_MINDERGAS;
        } // else no-state-change, and keep waiting...
      }
      break;
      
    case MG_SENDING_MINDERGAS:
      if (Verbose1) DebugTln(F("Mindergas State: MG_SENDING_MINDERGAS"));
      sprintf(txtLastUpdateMindergas, "Sending to mindergas...");
      // if POST response for Mindergas exists, then send it... btw it should exist by now :)
      if ((validToken) && SPIFFS.exists(MG_FILENAME)) 
      {  
         // start the update of mindergas, when the countdown counter reaches 0
          WiFiClient client;   
          //WiFiClientSecure client;          
          // try to connect to minderGas
          DebugTln(F("Connecting to Mindergas..."));
          //connect over https with mindergas
          if (client.connect((char*)"mindergas.nl",80)) 
          {
            // create a string with the date and the meter value
            dataFile = SPIFFS.open(MG_FILENAME, "r");
            String sBuffer;
            sBuffer = "";
            while(dataFile.available()) 
            { 
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
            while (!bDoneResponse && (client.connected() || client.available())) 
            {
              if (client.available()) 
              {
                  // read the status code the response
                  if (client.find("HTTP/1.1"))
                  {
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
                  if (SPIFFS.remove(MG_FILENAME)) 
                  {
                    DebugTln(F("POST Mindergas file succesfully deleted!"));
                  } else 
                  {
                    //help, this should just not happen, but if it does, it will not influence behaviour in a negative way
                    DebugTln(F("Failed to delete POST Mindergas file"));
                  }
                  time_t t = now();  // Note last update (today!)
                  sprintf(txtLastUpdateMindergas, "%02d-%02d-%04d", day(t), month(t), year(t)); 
                  bDoneResponse = true;    
              } //end-if client.available() 
              else 
              {
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
      if (millis() - lastTime > WAIT_TIME) 
      {
        lastTime = millis();
        // wait 1 minute, maybe user will enter token, retry once a minute.
        sprintf(txtLastUpdateMindergas, "Setup authtoken in settings");
        stateMindergas = MG_INIT;   
      }
      //Do not update mindergas when a failing token is detected
      break;
      
    case MG_ERROR:
      if (Verbose1) DebugTln(F("Mindergas State: MG_ERROR"));
      sprintf(txtLastUpdateMindergas, "This should not happen! Error."); 
      break;
      
    default:
      if (Verbose1) DebugTln(F("Mindergas State: Impossible, default state!")); 
      sprintf(txtLastUpdateMindergas, "Impossible state! Error.");      
      break;  
          
  } // switch(..)

  //on exit, allow next handle state event
  bHandleMindergas = false;
  
} // handleMindergas()

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
