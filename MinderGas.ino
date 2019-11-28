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

enum states_of_MG { MG_INIT, MG_WAIT_FOR_MIDNIGHT, MG_START_COUNTDOWN, MG_DO_COUNTDOWN, MG_SENDING_MINDERGAS, MG_NO_AUTHTOKEN, MG_ERROR };
enum states_of_MG stateMindergas = MG_INIT;

int8_t Today = -1;
byte GasCountdown = 0;
uint32_t const WAIT_TIME = 60000; //milliseconds = 60 seconds
uint32_t lastTime = millis();

//force mindergas update, by skipping states
void forceMindergasUpdate(){
  //Skip countdown state, this will force an update.
  switch (stateMindergas){
    case MG_WAIT_FOR_MIDNIGHT: //skip to countdown mode
      stateMindergas = MG_START_COUNTDOWN;
      DebugTln(F("Skip wait for midnight, start countdown now..."));
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
}

// statemachine of mindergas
void handleMindergas(){
  yield(); 
  switch(stateMindergas){
    case MG_INIT:  //after reboot
      if (Verbose1) DebugTln(F("Mindergas State: MG_INIT"));
      // check to see if there is a authtoke
      if (String(settingMindergasAuthtoken).length()==0) {
        //No AuthToken
        DebugTln(F("MinderGas Authtoken is not set, no update can be done."));
        stateMindergas = MG_NO_AUTHTOKEN; //no token, no mindergas
        break;
      }
      //check to see if update in progress
      SPIFFS.begin();
      if (SPIFFS.exists(MG_FILENAME)) 
      {
        //restart countdown
        GasCountdown = random(1,60); //within one hour   
        DebugTf("MinderGas Countdown re-started... in [%6d] minute(s)\r\n", GasCountdown);
        stateMindergas = MG_DO_COUNTDOWN;
      }
    break;
    case MG_WAIT_FOR_MIDNIGHT:
      if (Verbose1) DebugTln(F("Mindergas State: MG_WAIT_FOR_MIDNIGHT"));
      if (thisDay!=Today) { //Detect day change at midnight, then...
        Today = thisDay; //this can only be triggered once a day.
        stateMindergas = MG_START_COUNTDOWN;
      }
      break;
    case MG_START_COUNTDOWN: {
      if (Verbose1) DebugTln(F("Mindergas State: MG_START_COUNTDOWN"));
      //start countdown
      GasCountdown = random(1,60); //within one hour
      //create POST and write to file, so it will survive a reset within the countdown period
      SPIFFS.begin();
      yield();
      Debug(F("Writing to")); Debug(MG_FILENAME); Debugln(F("..."));
      File file = SPIFFS.open(MG_FILENAME, "w"); // open for reading and writing
      if (!file) {
         //cannot create file, thus error
        DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", MG_FILENAME);
        stateMindergas = MG_ERROR;
        //no state change, stay in failure mode
        break;
      } 
        else {
        //write POST respons into file
        yield();
        DebugT(F("\r\nStart writing setting data "));
        time_t t = now() - SECS_PER_DAY;  // we want to upload the gas usage of yesterday so rewind the clock for 1 day
        char dataString[80];
        sprintf(dataString,"{ \"date\": \"%04d-%02d-%02d\", \"reading\": \"%.3f\" }", year(t), month(t), day(t), GasDelivered);
        //write the POST to a file...
        file.println(F("POST /api/gas_meter_readings HTTP/1.1"));
        file.print(F("AUTH-TOKEN:")); file.println(settingMindergasAuthtoken);
        file.println(F("Host: mindergas.nl"));
        file.println(F("User-Agent: DSMRWS"));
        file.println(F("Content-Type: application/json"));
        file.println(F("Accept: application/json"));
        file.print(F("Content-Length: ")); file.println(strlen(dataString));
        file.println();
        file.println(dataString);
        //Lets'do the countdown
        stateMindergas = MG_DO_COUNTDOWN;
      }
    }
      
    break;
    case MG_DO_COUNTDOWN:
      if (Verbose1) DebugTln(F("Mindergas State: MG_DO_COUNTDOWN"));
      if (millis() - lastTime > WAIT_TIME) {
        //waittime has passed, countdown one minute
        lastTime = millis();
   
        // Countdown to 0, then update the Gas Delivered, and write it with date from yesterday.
        GasCountdown--;
        DebugTf("MinderGas update in [%2d] minute(s)\r\n", GasCountdown);
        if (GasCountdown==0) {
          //when waitime is done, then it's time to send the POST string
          stateMindergas = MG_SENDING_MINDERGAS;
        }
      }
      break;
    case MG_SENDING_MINDERGAS:
      if (Verbose1) DebugTln(F("Mindergas State: MG_SENDING_MINDERGAS"));
      // if POST response for Mindergas exists, then send it... btw it should exist by now :)
      if (SPIFFS.exists(MG_FILENAME)) 
      {  
         // start the update of mindergas, when the countdown counter reaches 0
          //WiFiClient client;   
          WiFiClientSecure client;          
          // try to connect to minderGas
          DebugTln(F("Connecting to Mindergas..."));
          //connect over https with mindergas
          if (client.connect((char*)"mindergas.nl",443)) {
            // create a string with the date and the meter value
            File file = SPIFFS.open(MG_FILENAME, "r");
            String sBuffer;
            while(file.available()) { 
              sBuffer = file.read();
            }
            file.close();
            DebugTln(F("Reading POST from file:"));
            Debugln(sBuffer);
            DebugTln(F("Send to Mindergas.nl..."));
            client.println(sBuffer);
            // read response from mindergas.nl
            DebugTln(F("Mindergas response: "));
            while (client.connected() || client.available())
            {
              if (client.available())
                {
                  // read the status code the response
                  if (!client.find("HTTP/1.1")) // skip HTTP/1.1
                    return;
                  int statusCode = client.parseInt(); // parse status code
                  Debug("Statuscode: "); Debugln(statusCode);
                  //String response = client.read();
                  //Debug("Response: "); Debugln(response);
                  //we could report last response to user... but an user can go to mindergas to see the post (or error if any).
                  switch (statusCode){
                    case 401: 
                      strcpy(settingMindergasAuthtoken, "Invalid token!"); //report error back to see in settings page
                      stateMindergas = MG_NO_AUTHTOKEN;
                      break;
                    case 200:
                      //succes
                      stateMindergas = MG_WAIT_FOR_MIDNIGHT;
                      break;
                    default:
                      stateMindergas = MG_WAIT_FOR_MIDNIGHT;
                      break;
                  }//end-switch
                  client.stop(); //closing secure connection to mindergas.nl
                  //delete POST file from SPIFFS
                  if (SPIFFS.remove(MG_FILENAME)) {
                    DebugTln(F("POST Mindergas file succesfully deleted!"));
                  } else {
                    //help, this should just not happen, but if it does, it will not influence behaviour in a negative way
                    DebugTln(F("Failed to delete POST Mindergas file"));
                  }
                  DebugTln(F("Disconnected"));
              }//end-if client.available() 
              else
              {
                //no succes with connection, try again tomorrow?
                DebugTln(F("Failed connect to mindergas"));
                delay(100); //
                return;
              }// end-else
            }//end-while
        }//sending done
      }//end-file-exists
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
