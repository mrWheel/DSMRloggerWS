/* 
***************************************************************************  
**  Program  : MQTTstuff, part of DSMRloggerWS
**  Version  : v1.0.4
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*  RB  changed MQTT stuff to FSM 
*/

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

  String            MQTTclientId;
  static IPAddress  MQTTbrokerIP;
  static char       MQTTbrokerURL[101];
  static uint16_t   MQTTbrokerPort = 1883;
  static char       MQTTbrokerIPchar[20];

  enum states_of_MQTT { MQTTstuff_INIT, MQTTstuff_TRY_TO_CONNECT, MQTTstuff_IS_CONNECTED, MQTTstuff_WAIT_CONNECTION_ATTEMPT, MQTTstuff_WAIT_FOR_RECONNECT, MQTTstuff_ERROR };
  enum states_of_MQTT stateMQTT = MQTTstuff_INIT;
  
#endif

//===========================================================================================
void startMQTT() 
{
#ifdef USE_MQTT
  stateMQTT = MQTTstuff_INIT;
  handleMQTT();
#endif
}
//===========================================================================================
void handleMQTT() 
{
#ifdef USE_MQTT

  switch(stateMQTT) 
  {
    case MQTTstuff_INIT:  
      DebugTln(F("MQTT State: MQTT Initializing")); 
      WiFi.hostByName(MQTTbrokerURL, MQTTbrokerIP);  // lookup the MQTTbrokerURL convert to IP
      sprintf(MQTTbrokerIPchar, "%d.%d.%d.%d", MQTTbrokerIP[0], MQTTbrokerIP[1], MQTTbrokerIP[2], MQTTbrokerIP[3]);
      if (isValidIP(MQTTbrokerIP))  
      {
        DebugTf("[%s] => setServer(%s, %d)\r\n", settingMQTTbroker, MQTTbrokerIPchar, MQTTbrokerPort);
        MQTTclient.disconnect();
        MQTTclient.setServer(MQTTbrokerIPchar, MQTTbrokerPort);
        MQTTclientId  = String(_HOSTNAME) + WiFi.macAddress();
        //skip wait for reconnect
        stateMQTT = MQTTstuff_TRY_TO_CONNECT;     
        DebugTln(F("Next State: MQTTstuff_TRY_TO_CONNECT"));
      }
      else
      { // invalid IP, then goto error state
        DebugTf("ERROR: [%s] => is not a valid URL\r\n", MQTTbrokerURL);
        stateMQTT = MQTTstuff_ERROR;
        DebugTln(F("Next State: MQTTstuff_ERROR"));
      }     
      timeMQTTReconnect = millis(); //do setup the next retry window in 10 minutes.
    break;

    case MQTTstuff_TRY_TO_CONNECT:
      DebugTln(F("MQTT State: MQTT try to connect"));
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
        stateMQTT = MQTTstuff_IS_CONNECTED;
        DebugTln(F("Next State: MQTTstuff_IS_CONNECTED"));
      }
      else
      { // no connection, try again, do a non-blocking wait for 3 seconds.
        Debugln(F(" .. \r"));
        DebugTf("failed, retrycount=[%d], rc=[%d] ..  try again in 3 seconds\r\n", reconnectAttempts, MQTTclient.state());
        timeMQTTLastRetry= millis();
        stateMQTT = MQTTstuff_WAIT_CONNECTION_ATTEMPT;  // if the re-connect did not work, then return to wait for reconnect
        DebugTln(F("Next State: MQTTstuff_WAIT_CONNECTION_ATTEMPT"));
      }
      
      //After 5 attempts... go wait for a while.
      if (reconnectAttempts > 5)
      {
        DebugTln(F("5 attempts have failed. Retry wait for next reconnect in 10 minutes\r"));
        stateMQTT = MQTTstuff_WAIT_FOR_RECONNECT;  // if the re-connect did not work, then return to wait for reconnect
        DebugTln(F("Next State: MQTTstuff_WAIT_FOR_RECONNECT"));
      }   
    break;
    
    case MQTTstuff_IS_CONNECTED:
      if (Verbose1) DebugTln(F("MQTT State: MQTT is Connected"));
      if (MQTTclient.connected()) 
      { //if the MQTT client is connected, then please do a .loop call...
        MQTTclient.loop();
      }
      else
      { //else go and wait 10 minutes, before trying again.
        stateMQTT = MQTTstuff_WAIT_FOR_RECONNECT;
        DebugTln(F("Next State: MQTTstuff_WAIT_FOR_RECONNECT"));
      }  
    break;

    case MQTTstuff_WAIT_CONNECTION_ATTEMPT:
      //do non-blocking wait for 3 seconds
       if (Verbose1)  DebugTln(F("MQTT State: MQTT_WAIT_CONNECTION_ATTEMPT"));
      if ((millis() - timeMQTTLastRetry) > MQTT_WAITFORRETRY) 
      {
        //Try again... after waitforretry non-blocking delay
        stateMQTT = MQTTstuff_TRY_TO_CONNECT;
        DebugTln(F("Next State: MQTTstuff_TRY_TO_CONNECT"));
      }
    break;
    
    case MQTTstuff_WAIT_FOR_RECONNECT:
      if (Verbose1) DebugTln(F("MQTT State: MQTT wait for reconnect"));
      if ((millis() - timeMQTTReconnect) > MQTT_WAITFORCONNECT) 
      {
        //remember when you tried last time to reconnect
        timeMQTTReconnect = millis();
        reconnectAttempts = 0; 
        stateMQTT = MQTTstuff_TRY_TO_CONNECT;
        DebugTln(F("Next State: MQTTstuff_TRY_TO_CONNECT"));
      }
    break;

    case MQTTstuff_ERROR:
      DebugTln(F("MQTT State: MQTT ERROR, wait for 10 minutes, before trying again"));
      //next retry in 10 minutes.
      timeMQTTReconnect = millis(); 
      stateMQTT = MQTTstuff_WAIT_FOR_RECONNECT;
      DebugTln(F("Next State: MQTTstuff_WAIT_FOR_RECONNECT"));
    break;

    default:
      DebugTln(F("MQTT State: default, this should NEVER happen!"));
      //do nothing, this state should not happen
      stateMQTT = MQTTstuff_INIT;
      DebugTln(F("Next State: MQTTstuff_INIT"));
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

//===========================================================================================
void sendMQTTData() 
{
/*  
* The maximum message size, including header, is 128 bytes by default. 
* This is configurable via MQTTstuff_MAX_PACKET_SIZE in PubSubClient.h.
* Als de json string te lang wordt zal de string niet naar de MQTT server
* worden gestuurd. Vandaar de korte namen als ED en PDl1.
* Mocht je langere, meer zinvolle namen willen gebruiken dan moet je de
* MQTTstuff_MAX_PACKET_SIZE dus aanpassen!!!
*/
//===========================================================================================
#ifdef USE_MQTT
  String dateTime, topicId, json;

  // only if the DSMR timestamp is different from last, never sent the same telegram twice.
  if (lastMQTTTimestamp==pTimestamp) return;

  if (millis() > timeMQTTPublish) 
  {
    timeMQTTPublish = millis() + (settingMQTTinterval * 1000);
    if (settingMQTTinterval==settingInterval) timeMQTTPublish -= 1000; //special case, if DSMR and MQTT interval time are the same, then make sure MQTT is set to shorter loop, this makes sure every telegram will be sent.
  }
  else  return;

  if (!MQTTclient.connected() || !isValidIP(MQTTbrokerIP)) return;

  DebugTf("Sending data to MQTT server [%s]:[%d]\r\n", MQTTbrokerURL, MQTTbrokerPort);

  //identification
  sprintf(cMsg, "{\"identification\":\"%s\"}", Identification);
  topicId = String(settingMQTTtopTopic) + "/JSON/identification";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //p1_version
  sprintf(cMsg, "{\"p1_version\":\"%s\"}", P1_Version.c_str());
  topicId = String(settingMQTTtopTopic) + "/JSON/p1_version";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //timestamp
  sprintf(cMsg, "{\"timestamp\":\"%s\"}", pTimestamp.c_str());
  topicId = String(settingMQTTtopTopic) + "/JSON/timestamp";
  MQTTclient.publish(topicId.c_str(), cMsg);
  lastMQTTTimestamp=pTimestamp;             //make sure the lastMQTTTimestamp is updated
  
  //equipment_id
  sprintf(cMsg, "{\"equipment_id\":\"%s\"}", Equipment_Id.c_str());
  topicId = String(settingMQTTtopTopic) + "/JSON/equipment_id";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //energy_delivered_tariff1
  sprintf(cMsg, "{\"energy_delivered_tariff1\":%.3f,\"unit\":\"kWh\"}", EnergyDeliveredTariff1);
  topicId = String(settingMQTTtopTopic) + "/JSON/energy_delivered_tariff1";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //energy_delivered_tariff2
  sprintf(cMsg, "{\"energy_delivered_tariff2\":%.3f,\"unit\":\"kWh\"}", EnergyDeliveredTariff2);
  topicId = String(settingMQTTtopTopic) + "/JSON/energy_delivered_tariff2";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //energy_delivered_tariff1 + energy_delivered_tariff2
  sprintf(cMsg, "{\"energy_delivered\":%.3f,\"unit\":\"kWh\"}", EnergyDeliveredTariff1 + EnergyDeliveredTariff2);
  topicId = String(settingMQTTtopTopic) + "/JSON/energy_delivered";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //energy_returned_tariff1
  sprintf(cMsg, "{\"energy_returned_tariff1\":%.3f,\"unit\":\"kWh\"}", EnergyReturnedTariff1);
  topicId = String(settingMQTTtopTopic) + "/JSON/energy_returned_tariff1";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //energy_returned_tariff2
  sprintf(cMsg, "{\"energy_returned_tariff2\":%.3f,\"unit\":\"kWh\"}", EnergyReturnedTariff2);
  topicId = String(settingMQTTtopTopic) + "/JSON/energy_returned_tariff2";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //energy_returned_tariff1 + energy_returned_tariff2
  sprintf(cMsg, "{\"energy_returned\":%.3f,\"unit\":\"kWh\"}", EnergyReturnedTariff1 + EnergyReturnedTariff2);
  topicId = String(settingMQTTtopTopic) + "/JSON/energy_returned";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //electricity_tariff
  // 20191101 bug uit reacties gevonden.
  // sprintf(cMsg, "{\"electricity_tariff\":%04d}", String(ElectricityTariff).toInt());
  sprintf(cMsg, "{\"electricity_tariff\":\"%s\"}", String(ElectricityTariff).c_str());
  topicId = String(settingMQTTtopTopic) + "/JSON/electricity_tariff";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //power_delivered
  sprintf(cMsg, "{\"power_delivered\":%.3f,\"unit\":\"kW\"}", PowerDelivered);
  topicId = String(settingMQTTtopTopic) + "/JSON/power_delivered";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //power_returned
  sprintf(cMsg, "{\"power_returned\":%.3f,\"unit\":\"kW\"}", PowerReturned);
  topicId = String(settingMQTTtopTopic) + "/JSON/power_returned";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //voltage_l1
  sprintf(cMsg, "{\"voltage_l1\":%.1f,\"unit\":\"volt\"}", Voltage_l1);
  topicId = String(settingMQTTtopTopic) + "/JSON/voltage_l1";
  MQTTclient.publish(topicId.c_str(), cMsg);

  //voltage_l2
  sprintf(cMsg, "{\"voltage_l2\":%.1f,\"unit\":\"volt\"}", Voltage_l2);
  topicId = String(settingMQTTtopTopic) + "/JSON/voltage_l2";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //voltage_l3
  sprintf(cMsg, "{\"voltage_l3\":%.1f,\"unit\":\"volt\"}", Voltage_l3);
  topicId = String(settingMQTTtopTopic) + "/JSON/voltage_l3";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //current_l1
  sprintf(cMsg, "{\"current_l1\":%d,\"unit\":\"ampere\"}", Current_l1);
  topicId = String(settingMQTTtopTopic) + "/JSON/current_l1";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //current_l2
  sprintf(cMsg, "{\"current_l2\":%d,\"unit\":\"ampere\"}", Current_l2);
  topicId = String(settingMQTTtopTopic) + "/JSON/current_l2";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //current_l3
  sprintf(cMsg, "{\"current_l3\":%d,\"unit\":\"ampere\"}", Current_l3);
  topicId = String(settingMQTTtopTopic) + "/JSON/current_l3";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //power_delivered_l1
  sprintf(cMsg, "{\"power_delivered_l1\":%.0f,\"unit\":\"Watt\"}", (float)PowerDelivered_l1 * 1.0);
  topicId = String(settingMQTTtopTopic) + "/JSON/power_delivered_l1";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //power_delivered_l2
  sprintf(cMsg, "{\"power_delivered_l2\":%.0f,\"unit\":\"Watt\"}", (float)PowerDelivered_l2 * 1.0);
  topicId = String(settingMQTTtopTopic) + "/JSON/power_delivered_l2";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //power_delivered_l3
  sprintf(cMsg, "{\"power_delivered_l3\":%.0f,\"unit\":\"Watt\"}", (float)PowerDelivered_l3 * 1.0);
  topicId = String(settingMQTTtopTopic) + "/JSON/power_delivered_l3";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //power_returned_l1
  sprintf(cMsg, "{\"power_returned_l1\":%.0f,\"unit\":\"Watt\"}", (float)PowerReturned_l1 * 1.0);
  topicId = String(settingMQTTtopTopic) + "/JSON/power_returned_l1";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //power_returned_l2
  sprintf(cMsg, "{\"power_returned_l2\":%.0f,\"unit\":\"Watt\"}", (float)PowerReturned_l2 * 1.0);
  topicId = String(settingMQTTtopTopic) + "/JSON/power_returned_l2";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //power_returned_l3
  sprintf(cMsg, "{\"power_returned_l3\":%.0f,\"unit\":\"Watt\"}", (float)PowerReturned_l3 * 1.0);
  topicId = String(settingMQTTtopTopic) + "/JSON/power_returned_l3";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //gas_device_type
  sprintf(cMsg, "{\"gas_device_type\":\"%s\"}", String(GasDeviceType).c_str());
  topicId = String(settingMQTTtopTopic) + "/JSON/gas_device_type";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //gas_equipment_id
  sprintf(cMsg, "{\"gas_equipment_id\":\"%s\"}", String(GasEquipment_Id).c_str());
  topicId = String(settingMQTTtopTopic) + "/JSON/gas_equipment_id";
  MQTTclient.publish(topicId.c_str(), cMsg);
  
  //gas_delivered
  sprintf(cMsg, "{\"gas_delivered\":%.3f,\"unit\":\"m3\"}",GasDelivered);
  topicId = String(settingMQTTtopTopic) + "/JSON/gas_delivered";
  MQTTclient.publish(topicId.c_str(), cMsg);  

/************ niet meer vanaf versie V1.0.4 *****
//========== combined data =========================================
  json = "{";
  
  dateTime = buildDateTimeString(pTimestamp);
  json += "\"DT\":\"" + dateTime + "\"";

  sprintf(cMsg, "%9.3f", EnergyDelivered);
  json += ",\"ED\":" + trimVal(cMsg);

  sprintf(cMsg, "%9.3f", EnergyReturned);
  json += ",\"ER\":" + trimVal(cMsg);

  dtostrf(GasDelivered, 9, 3, fChar);
  json += ",\"GD\":" + trimVal(fChar);

  json += "}";
  if (Verbose1) DebugTf("json[%s], length[%d]\r\n", json.c_str(), json.length());
  topicId = String(settingMQTTtopTopic) + "/JSON/Energy";
  if (!MQTTclient.publish(topicId.c_str(), json.c_str())) {
    DebugTf("Error publishing Values! JSON [%s]([%d]chars is to long?)\r\n", json.c_str(), json.length());
  }
  json = "{";
  dtostrf(PowerDelivered, 9, 3, fChar);
  json += "\"PDt\":" + trimVal(fChar);

  dtostrf(PowerReturned, 9, 3, fChar);
  json += ",\"PRt\":" + trimVal(fChar);

  dtostrf(PowerDelivered_l1, 9, 0, fChar);
  json += ",\"PDl1\":" + trimVal(fChar);

  dtostrf(PowerDelivered_l2, 9, 0, fChar);
  json += ",\"PDl2\":" + trimVal(fChar);

  dtostrf(PowerDelivered_l3, 9, 0, fChar);
  json += ",\"PDl3\":" + trimVal(fChar);

  dtostrf(PowerReturned_l1, 9, 0, fChar);
  json += ",\"PRl1\":" + trimVal(fChar);
  
  dtostrf(PowerReturned_l2, 9, 0, fChar);
  json += ",\"PRl2\":" + trimVal(fChar);

  dtostrf(PowerReturned_l3, 9, 0, fChar);
  json += ",\"PRl3\":" + trimVal(fChar);

  json += "}";
  if (Verbose1) DebugTf("json[%s], length[%d]\r\n", json.c_str(), json.length());
  topicId = String(settingMQTTtopTopic) + "/JSON/Power";
  if (!MQTTclient.publish(topicId.c_str(), json.c_str())) 
  {
    DebugTf("Error publishing Values! JSON [%s] ([%d]chars is to long?)\r\n", json.c_str(), json.length());
  }
************/
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
