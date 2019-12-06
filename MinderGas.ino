/*
**  Copyright (c) 2019 Robert van den Breemen
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
* Inspired by the code from Harold - SolarMeter code
<<<<<<< HEAD
* Created by Robert van den Breemen (16 nov 2019)
*   - RB - parse the HTTP response token
*   - RB - added AuthToken to settings
*   - RB - many more formatting for gas changed to 3 digits
*      
*   - RB - gas delivered should be [.3f] - lots of formatting of gasdelivered changed to 3 digits
*   - RB - changed around the way debug is done in rollover on month, day and hour
*   - RB - fixing the mindergas integration - mindergas.ino
*   - RB - added initial support for mindergas
=======
* Created by Robert van den Breemen (26 nov 2019)
*   - RvdB - changing into a statemachine and survive reboot
*   - RvdB - added AuthToken to settings
*   - RvdB - many more formatting for gas changed to 3 digits in DSMRlogger data
*   - RvdB - gas delivered should be [.3f] - lots of formatting of gasdelivered changed to 3 digits
*   - RvdB - changed around the way debug is done in rollover on month, day and hour
*   - RvdB - fixing the mindergas integration - mindergas.ino
*   - RvdB - added initial support for mindergas
>>>>>>> parent of 88f7aa1... Merge branch 'v1.0.4-dev-robert' into Statemachine-for-mindergas
*
*/

#ifdef USE_MINDERGAS

byte GasCountdown = 0;
float TotalGas;
uint32_t lastMindergas = millis();
boolean failToken = false;

// this function is called at midnight
void updateMindergas(float GasDelivered)
{
    if (failToken || (String(settingMindergasAuthtoken).length()==0)) {
      // token not set (correctly) then do nothing
      DebugTln("Mindergas Authtoken is not set, no update is done.");
    } else {
      // If (valid) authtoken exists, then start countdown
      // select a number between 1 and 59
      // this will be the minutes to wait before uploading
      GasCountdown = random(1,60);
      DebugTf("MinderGas Countdown started... in [%6d] minute(s)\r\n", GasCountdown);
  
      // the actual total-gas value is saved
      TotalGas = GasDelivered;
      DebugTf("GasDelivered = [%.3f]\r\n", GasDelivered);
      //now lets wait until the random waittime has passed
    }
}

// next function is called at every minute
void checkMindergas()
{
   // If the countdown is running, then actually do countdown, else do nothing.
    if(GasCountdown>0)
    {
      //when 1 minute (=60.000 ms)has passed, then countdown to 0
      if (millis() - lastMindergas > 60000) { 
        lastMindergas = millis();
   
        // Countdown to 0, then update the Gas Delivered, and write it with date from yesterday.
        GasCountdown--;
        DebugTf("MinderGas update in [%2d] minute(s)\r\n", GasCountdown);
<<<<<<< HEAD
        if(GasCountdown==0)
        {
            // start the update of mindergas, when the countdown counter reaches 0
            WiFiClient client;      
            //WiFiClientSecure client;                 
            time_t t = now() - SECS_PER_DAY;  // we want to upload the gas usage of yesterday so rewind the clock for 1 day
            // try to connect to minderGas
            DebugTln("Connecting to Mindergas...");
            if (client.connect((char*)"mindergas.nl",80)) {
            //if (client.connect((char*)"mindergas.nl",443)) {
                // create a string with the date and the meter value
                char dataString[80];
                sprintf(dataString,"{ \"date\": \"%04d-%02d-%02d\", \"reading\": \"%.3f\" }", year(t), month(t), day(t), TotalGas);
                DebugTf("Send update to mindergas: [%s]\r\n", dataString);
                DebugTln("Send to server:");
                //print to debug 
                DebugTln("POST /api/gas_meter_readings HTTP/1.1");
                DebugT("AUTH-TOKEN:"); Debugln(settingMindergasAuthtoken);
                DebugTln("Host: mindergas.nl");
                DebugTln("User-Agent: DSMRWS");
                DebugTln("Content-Type: application/json");
                DebugTln("Accept: application/json");
                DebugT("Content-Length: "); Debugln(strlen(dataString));
                DebugTln();
                DebugTln(dataString);
                //print to debug serial    
                client.println("POST /api/gas_meter_readings HTTP/1.1");
                client.print("AUTH-TOKEN:"); client.println(settingMindergasAuthtoken);
                client.println("Host: mindergas.nl");
                client.println("User-Agent: DSMRWS");
                client.println("Content-Type: application/json");
                client.println("Accept: application/json");
                client.print("Content-Length: "); client.println(strlen(dataString));
                client.println();
                client.println(dataString);
                // read response from mindergas.nl
                DebugTln("Mindergas response: ");
                while (client.connected() || client.available()) {
                  if (client.available()) {
                    // read the status code the response
                    if (!client.find("HTTP/1.1")) // skip HTTP/1.1
                      return;
                    int statusCode = client.parseInt(); // parse status code
                    DebugT("Statuscode: "); Debugln(statusCode);
                    if (statusCode==422) {
                      failToken = true; //set to true to flag invalid token!
                      strcpy(settingMindergasAuthtoken, "Invalid token!"); //report error back to see in settings page
                      DebugTln("Invalid Mindergas Authenication Token");
                    }
                  }
                } // while ..
                client.stop(); //stop wifi, free memory?
                DebugTln("Disconnected");
              } // if connected ..
              else
              {
                //no succes with connection, try again tomorrow?
                DebugTln("Failed connect to mindergas");
                return;
              }
          } // if GasCountdown == 0
      } // millis ..
   }  // if GasCountdoen > 0
   
} // checkMindergas()
=======
        if (GasCountdown==0) {
          //when waitime is done, then it's time to send the POST string
          stateMindergas = MG_SENDING_MINDERGAS;
        } // else no-state-change, and keep waiting...
      }
    break;
    case MG_SENDING_MINDERGAS:
      if (Verbose1) DebugTln(F("Mindergas State: MG_SENDING_MINDERGAS"));
      // if POST response for Mindergas exists, then send it... btw it should exist by now :)
      if (SPIFFS.exists(MG_FILENAME)) {  
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
                  if (client.find("HTTP/1.1")) { //HTTP 1.1 found, now parse the response code
                    uint8_t statusCode = client.parseInt(); // parse status code
                    Debugln();
                    DebugT("Statuscode: "); Debugln(statusCode);
                    //we could report last response to user... but an user can go to mindergas to see the post (or error if any).
                    switch (statusCode){
                      case 401 : // 401 Unauthorized = invalid token
                        strcpy(settingMindergasAuthtoken, "Invalid token!"); //report error back to see in settings page
                        stateMindergas = MG_NO_AUTHTOKEN;
                        break;
                      case 422 : //unable to process, goto mindergas.nl to find out
                        break;
                      case 201 : //succes, created new entry, just wait for midnight
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;
                        break;
                      default : // all other responses, then wait for midnight... failsafe
                        stateMindergas = MG_WAIT_FOR_MIDNIGHT;
                      break;
                    }//end-switch
                  } //end-if HTTP/1.1
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
>>>>>>> parent of 88f7aa1... Merge branch 'v1.0.4-dev-robert' into Statemachine-for-mindergas

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
