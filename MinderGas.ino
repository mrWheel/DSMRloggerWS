/*
**  Copyright (c) 2019 Robert van den Breemen
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
* Inspired by the code from Harold - SolarMeter code
* Created by Robert van den Breemen (16 nov 2019)
*/

#ifdef USE_MINDERGAS
//Fetch a auth token from mindergas.nl to enable upload
#define MINDERGAS_TOKEN "<add auth token for mindergas.nl api>"

byte GasCountdown = 0;
float TotalGas;
uint32_t lastMindergas = millis();

// this function is called at midnight
void updateMindergas(float GasDelivered)
{
    // select a number between 1 and 59
    // this will be the minutes to wait before uploading
    GasCountdown = random(1,60);
    DebugTf("Minderdag Countdown started... in [%6d] minute(s)\r\n", GasCountdown);
    
    // the actual total-gas value is saved
    TotalGas = GasDelivered;
    DebugTf("GasDelivered = [%.3f]\r\n", GasDelivered);
    //now lets wait until the random waittime has passed
}

// next function is called at every minute
void checkMindergas()
{
   // If the countdown is running, then actually do countdown, else do nothing.
    if(GasCountdown>0)
    {
      //every minute, do a countdown
      if (millis() - lastMindergas > 60000) { 
        lastMindergas = millis();
   
        // Countdown to 0, then update the Gas Delivered, and write it with date from yesterday.
        GasCountdown--;
        DebugTf("Minderdag update in [%2d] minute(s)\r\n", GasCountdown);
        if(GasCountdown==0)
        {
            // start the upload when the counter reaches 0
            WiFiClient client;              //setup a TCP connection over wifi
            time_t t = now() - SECS_PER_DAY;  // we want to upload the gas usage of yesterday so rewind the clock for 1 day
            // try to connect to minderGas
            DebugTln("Connecting to Mindergas...");
            if (client.connect((char*)"mindergas.nl",80)) {
                // create a string with the date and the meter value
                char dataString[80];
                sprintf(dataString,"{ \"date\": \"%04d-%02d-%02d\", \"reading\": \"%.3f\" }", year(t), month(t), day(t), TotalGas);
                DebugTf("Send update to mindergas: [%s]\r\n", dataString);
                DebugTln("Send to server:");
                //print to debug serial
                DebugTln("POST /api/gas_meter_readings HTTP/1.1");
                DebugT("AUTH-TOKEN:"); DebugTln(MINDERGAS_TOKEN);
                DebugTln("Host: mindergas.nl");
                DebugTln("User-Agent: DSMRWS");
                DebugTln("Content-Type: application/json");
                DebugTln("Accept: application/json");
                DebugT("Content-Length: "); Debugln(strlen(dataString));
                DebugTln();
                DebugTln(dataString);
                //print to debug serial    
                client.println("POST /api/gas_meter_readings HTTP/1.1");
                client.print("AUTH-TOKEN:"); client.println(MINDERGAS_TOKEN);
                client.println("Host: mindergas.nl");
                client.println("User-Agent: DSMRWS");
                client.println("Content-Type: application/json");
                client.println("Accept: application/json");
                client.print("Content-Length: "); client.println(strlen(dataString));
                client.println();
                client.println(dataString);
                // read response from mindergas.nl
                while (client.connected() || client.available()){
                    if (client.available())
                    {
                      String line = client.readStringUntil('\n');
                      DebugT("Mindergas response: ["); DebugT(line); DebugTln("]");
                    } 
                 }
                // close connection
                client.stop();
                DebugTln("[Disconnected]");
            }
            else
            {
                //no succes with connection, try again tomorrow?
                DebugTln("Failed to mindergas");
                return;
            }
        }
    }
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
