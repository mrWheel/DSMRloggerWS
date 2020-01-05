/* 
***************************************************************************  
**  Program  : MQTTstuff, part of DSMRloggerWS
**  Version  : v1.0.4/a
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*  RB  changed MQTT stuff to FSM 
*/

// Declare some variables within global scope

  static IPAddress  MQTTbrokerIP;
  static char       MQTTbrokerURL[101];
  static uint16_t   MQTTbrokerPort = 1883;
  static char       MQTTbrokerIPchar[20];

// Moved to main global firmware space, since it's used in more than one place now.
//  static  DynamicJsonDocument jsonDoc(4000);  // generic doc to return, clear() before use!  
  
#ifdef USE_MQTT
  #include <PubSubClient.h>           // MQTT client publish and subscribe functionality
  #define MQTT_WAITFORCONNECT 600000  // 10 minutes
  #define MQTT_WAITFORRETRY     3000  // 3 seconden backoff
  
  static            PubSubClient MQTTclient(wifiClient);

  int8_t            reconnectAttempts = 0;
  uint32_t          timeMQTTPublish  = 0;
  String            lastMQTTTimestamp = "";
  uint32_t          timeMQTTLastRetry = 0;
  uint32_t          timeMQTTReconnect = 0;

  enum states_of_MQTT {  MQTT_STATE_INIT, MQTT_STATE_TRY_TO_CONNECT
                       , MQTT_STATE_WAIT_FOR_FIRST_TELEGRAM, MQTT_STATE_IS_CONNECTED
                       , MQTT_STATE_WAIT_CONNECTION_ATTEMPT, MQTT_STATE_WAIT_FOR_RECONNECT
                       , MQTT_STATE_ERROR };
  enum states_of_MQTT stateMQTT = MQTT_STATE_INIT;

  String            MQTTclientId;
#endif

//===========================================================================================
void startMQTT() 
{
#ifdef USE_MQTT
  stateMQTT = MQTT_STATE_INIT;
  handleMQTT();
#endif
}
//===========================================================================================
void handleMQTT() 
{
#ifdef USE_MQTT

  switch(stateMQTT) 
  {
    case MQTT_STATE_INIT:  
      DebugTln(F("MQTT Initializing")); 
      WiFi.hostByName(MQTTbrokerURL, MQTTbrokerIP);  // lookup the MQTTbrokerURL convert to IP
      sprintf(MQTTbrokerIPchar, "%d.%d.%d.%d", MQTTbrokerIP[0], MQTTbrokerIP[1], MQTTbrokerIP[2], MQTTbrokerIP[3]);
      if (isValidIP(MQTTbrokerIP))  
      {
        DebugTf("[%s] => setServer(%s, %d)\r\n", settingMQTTbroker, MQTTbrokerIPchar, MQTTbrokerPort);
        MQTTclient.disconnect();
        MQTTclient.setServer(MQTTbrokerIPchar, MQTTbrokerPort);
        MQTTclientId  = String(_HOSTNAME) + WiFi.macAddress();
        //skip wait for reconnect
        stateMQTT = MQTT_STATE_WAIT_FOR_FIRST_TELEGRAM;     
        //DebugTln(F("Next State: MQTT_STATE_WAIT_FOR_FIRST_TELEGRAM"));
      }
      else
      { // invalid IP, then goto error state
        DebugTf("ERROR: [%s] => is not a valid URL\r\n", MQTTbrokerURL);
        stateMQTT = MQTT_STATE_ERROR;
        //DebugTln(F("Next State: MQTT_STATE_ERROR"));
      }     
      timeMQTTReconnect = millis(); //do setup the next retry window in 10 minutes.
    break;

    case MQTT_STATE_WAIT_FOR_FIRST_TELEGRAM:
      if (Verbose2) DebugTln(F("MQTT_STATE_WAIT_FOR_FIRST_TELEGRAM"));
      // if you received at least one telegram, then try to connect
      if (telegramCount > 0) 
      {
        // Now that there is something to send to MQTT, start with connecting to MQTT.
        stateMQTT = MQTT_STATE_TRY_TO_CONNECT;
        //DebugTln(F("Next State: MQTT_STATE_TRY_TO_CONNECT"));
      }
      break;

    case MQTT_STATE_TRY_TO_CONNECT:
      DebugTln(F("MQTT try to connect"));
      //DebugTf("MQTT server is [%s], IP[%s]\r\n", settingMQTTbroker, MQTTbrokerIPchar);
      
      DebugT(F("Attempting MQTT connection .. "));
      reconnectAttempts++;

      //If no username, then anonymous connection to broker, otherwise assume username/password.
      if (String(settingMQTTuser).length() == 0) 
      {
        Debug(F("without a Username/Password "));
        MQTTclient.connect(MQTTclientId.c_str());
      } 
      else 
      {
        Debugf("Username [%s] ", settingMQTTuser);
        MQTTclient.connect(MQTTclientId.c_str(), settingMQTTuser, settingMQTTpasswd);
      }

      //If connection was made succesful, move on to next state...
      if  (MQTTclient.connected())
      {
        reconnectAttempts = 0;  
        Debugln(F(" .. connected\r"));
        stateMQTT = MQTT_STATE_IS_CONNECTED;
        //DebugTln(F("Next State: MQTT_STATE_IS_CONNECTED"));
      }
      else
      { // no connection, try again, do a non-blocking wait for 3 seconds.
        Debugln(F(" .. \r"));
        DebugTf("failed, retrycount=[%d], rc=[%d] ..  try again in 3 seconds\r\n", reconnectAttempts, MQTTclient.state());
        timeMQTTLastRetry= millis();
        stateMQTT = MQTT_STATE_WAIT_CONNECTION_ATTEMPT;  // if the re-connect did not work, then return to wait for reconnect
        //DebugTln(F("Next State: MQTT_STATE_WAIT_CONNECTION_ATTEMPT"));
      }
      
      //After 5 attempts... go wait for a while.
      if (reconnectAttempts >= 5)
      {
        DebugTln(F("5 attempts have failed. Retry wait for next reconnect in 10 minutes\r"));
        stateMQTT = MQTT_STATE_WAIT_FOR_RECONNECT;  // if the re-connect did not work, then return to wait for reconnect
        //DebugTln(F("Next State: MQTT_STATE_WAIT_FOR_RECONNECT"));
      }   
    break;
    
    case MQTT_STATE_IS_CONNECTED:
      if (Verbose2) DebugTln(F("MQTT is Connected"));
      if (MQTTclient.connected()) 
      { //if the MQTT client is connected, then please do a .loop call...
        MQTTclient.loop();
      }
      else
      { //else go and wait 10 minutes, before trying again.
        stateMQTT = MQTT_STATE_WAIT_FOR_RECONNECT;
        //DebugTln(F("Next State: MQTT_STATE_WAIT_FOR_RECONNECT"));
      }  
    break;

    case MQTT_STATE_WAIT_CONNECTION_ATTEMPT:
      //do non-blocking wait for 3 seconds
      //DebugTln(F("MQTT_WAIT_CONNECTION_ATTEMPT"));
      if ((millis() - timeMQTTLastRetry) > MQTT_WAITFORRETRY) 
      {
        //Try again... after waitforretry non-blocking delay
        stateMQTT = MQTT_STATE_TRY_TO_CONNECT;
        //DebugTln(F("Next State: MQTT_STATE_TRY_TO_CONNECT"));
      }
    break;
    
    case MQTT_STATE_WAIT_FOR_RECONNECT:
      //do non-blocking wait for 10 minutes, then try to connect again. 
      if (Verbose1) DebugTln(F("MQTT wait for reconnect"));
      if ((millis() - timeMQTTReconnect) > MQTT_WAITFORCONNECT) 
      {
        //remember when you tried last time to reconnect
        timeMQTTReconnect = millis();
        reconnectAttempts = 0; 
        stateMQTT = MQTT_STATE_TRY_TO_CONNECT;
        //DebugTln(F("Next State: MQTT_STATE_TRY_TO_CONNECT"));
      }
    break;

    case MQTT_STATE_ERROR:
      DebugTln(F("MQTT ERROR, wait for 10 minutes, before trying again"));
      //next retry in 10 minutes.
      timeMQTTReconnect = millis(); 
      stateMQTT = MQTT_STATE_WAIT_FOR_RECONNECT;
      //DebugTln(F("Next State: MQTT_STATE_WAIT_FOR_RECONNECT"));
    break;

    default:
      DebugTln(F("MQTT State: default, this should NEVER happen!"));
      //do nothing, this state should not happen
      stateMQTT = MQTT_STATE_INIT;
      //DebugTln(F("Next State: MQTT_STATE_INIT"));
    break;
  }
  
#endif
} // handleMQTT()

//===========================================================================================
String trimVal(char *in) 
{
  String Out = in;
  Out.trim();
  return Out;
} // trimVal()

//=======================================================================
struct buildJsonMQTT {
#ifdef USE_MQTT

  String jsonString;
  char topicId[100];
  
    template<typename Item>
    void apply(Item &i) {
      if (i.present()) 
      {
        String Name = Item::name;
        String Unit = Item::unit();

        jsonDoc.clear();
        jsonString = "";
        
        sprintf(topicId, "%s/JSON/", settingMQTTtopTopic);
        strConcat(topicId, sizeof(topicId), Name.c_str());
        //DebugTf("topicId[%s]\r\n", topicId);
        
        JsonArray array = jsonDoc.createNestedArray(Name);
        JsonObject nested = array.createNestedObject();
        
        nested["value"] = typecastValue(i.val());
        
        if (Unit.length() > 0)
        {
          nested["unit"]  = Unit;
        }
        serializeJson(jsonDoc, jsonString); 
        sprintf(cMsg, "%s", jsonString.c_str());
        if (Verbose1) DebugTf("topicId[%s] -> [%s]\r\n", topicId, cMsg);
        MQTTclient.publish(topicId, cMsg); 
      }
  }
#endif
};

//===========================================================================================
void sendMQTTData() 
{
/*  
* The maximum message size, including header, is 128 bytes by default. 
* This is configurable via MQTT_MAX_PACKET_SIZE in PubSubClient.h.
* Als de json string te lang wordt zal de string niet naar de MQTT server
* worden gestuurd. Vandaar de korte namen als ED en PDl1.
* Mocht je langere, meer zinvolle namen willen gebruiken dan moet je de
* MQTT_MAX_PACKET_SIZE dus aanpassen!!!
*/
//===========================================================================================
#ifdef USE_MQTT
  String dateTime, topicId, json;

  // only if the DSMR timestamp is different from last, never sent the same telegram twice.
  if (Verbose1) DebugTf("Timestamp [last:now] compared [%s]:[%s]\r\n", lastMQTTTimestamp.c_str(), pTimestamp.c_str());
  if (lastMQTTTimestamp==pTimestamp) return;

    if (millis() > timeMQTTPublish) 
  {
    timeMQTTPublish = millis() + (settingMQTTinterval * 1000);
    if (settingMQTTinterval==settingInterval) timeMQTTPublish -= 1000; //special case, if DSMR and MQTT interval time are the same, then make sure MQTT is set to shorter loop, this makes sure every telegram will be sent.
  }
  else  return;

  if (!MQTTclient.connected() || !isValidIP(MQTTbrokerIP)) return;

  DebugTf("Sending data to MQTT server [%s]:[%d]\r\n", MQTTbrokerURL, MQTTbrokerPort);

  DSMRdata.applyEach(buildJsonMQTT());

#endif

} // sendMQTTData()
 
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
