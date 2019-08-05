/*
***************************************************************************  
**  Program  : MQTTstuff, part of DSMRloggerWS
**  Version  : v0.4.7
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

#ifdef USE_MQTT
  #include <PubSubClient.h>       // MQTT client publish and subscribe functionality

  static            PubSubClient MQTTclient(wifiClient);

  int8_t            reconnectAttempts = 0;
  uint32_t          lastMQTTPublish   = 0;

  static bool       MQTTisConnected   = false;
  static uint32_t   MQTTretry;
  static IPAddress  MQTTbrokerIP;
  static char       MQTTbrokerIPchar[20];
#endif

//===========================================================================================
void startMQTT() {
//===========================================================================================
#ifdef USE_MQTT
  _dThis = true;
  Debug("set MQTT server .. ");       
  WiFi.hostByName(settingMQTTbroker, MQTTbrokerIP);
  sprintf(MQTTbrokerIPchar, "%d.%d.%d.%d", MQTTbrokerIP[0]
                                         , MQTTbrokerIP[1]
                                         , MQTTbrokerIP[2]
                                         , MQTTbrokerIP[3]);
  if (strcmp(MQTTbrokerIPchar, "0.0.0.0") == 0) {
    Debugf("ERROR: [%s] => is not a valid URL\n", settingMQTTbroker);
    MQTTisConnected = false;
  } else {
    Debugf("[%s] => IP[%s]\n", settingMQTTbroker, MQTTbrokerIPchar);
    MQTTclient.setServer(MQTTbrokerIPchar, 1883);         
  }
  DebugFlush();

#endif
} // startMQTT()


//===========================================================================================
void handleMQTT() {
//===========================================================================================
#ifdef USE_MQTT
  bool doTry = true;

  if (millis() > MQTTretry) {
    MQTTretry = millis() + 600000;  // tien minuten voor re-connect
    _dThis = true;
    Debugf("MQTT server is [%s], IP[%s]\n", settingMQTTbroker, MQTTbrokerIPchar);
    if (String(settingMQTTbroker).length() < 10) {  // not likely a valid server name
      doTry = false;
    }
    if (strcmp(MQTTbrokerIPchar, "0.0.0.0") == 0) {
      MQTTisConnected = false;
      return;
    }
    if (!MQTTclient.connected() && doTry) {
      if (!MQTTreconnect()) {
        doTry = false;
        MQTTisConnected = false;
      } else {
        MQTTisConnected = true;        
      }
    }
  }
  MQTTclient.loop();
#endif
} // handleMQTT()


//===========================================================================================
bool MQTTreconnect() {
//===========================================================================================
#ifdef USE_MQTT
  String    MQTTclientId  = String(_HOSTNAME) + WiFi.macAddress();
  
    if (strcmp(MQTTbrokerIPchar, "0.0.0.0") == 0) return false;

    reconnectAttempts = 0;
    // Loop until we're reconnected
    while (reconnectAttempts < 2) {
      reconnectAttempts++;
      _dThis = true;
      Debug("Attempting MQTT connection ... ");
      // Attempt to connect
      if (String(settingMQTTuser).length() < 1) {
        Debug("without a Username/Password ");
        MQTTisConnected = MQTTclient.connect(MQTTclientId.c_str());
      } else {
        Debugf("Username [%s] ", settingMQTTuser);
        MQTTisConnected = MQTTclient.connect(MQTTclientId.c_str(), settingMQTTuser, settingMQTTpasswd);
      }
      if (MQTTisConnected) {
        Debugln(" .. connected");
        return true;
      } else {
        Debugln(" .. ");
        _dThis = true;
        Debugf("failed, rc=[%d] ..  try again in 3 seconds\n", MQTTclient.state());
        // Wait 3 seconds before retrying
        delay(3000);
      }
    }

    _dThis = true;
    Debugln("5 attempts have failed.");
    return false;

#endif
}


String trimVal(char *in) {
  String Out = in;
  Out.trim();
  return Out;
}

//===========================================================================================
void sendMQTTData() {
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

  if ((millis() - lastMQTTPublish) >= (settingMQTTinterval * 1000))
        lastMQTTPublish = millis();
  else  return;

  if (!MQTTisConnected || (strcmp(MQTTbrokerIPchar, "0.0.0.0")) == 0) return;

  _dThis = true;
  Debugf("Sending data to MQTT server [%s]\n",settingMQTTbroker);

  json = "{";
  
  topicId = String(settingMQTTtopTopic) + "/DateTime";
  dateTime = buildDateTimeString(pTimestamp);
  MQTTclient.publish(topicId.c_str(), dateTime.c_str());
  json += "\"DT\":\"" + dateTime + "\"";

  topicId = String(settingMQTTtopTopic) + "/EnergyDel";
  sprintf(cMsg, "%9.3f", EnergyDelivered);
  MQTTclient.publish(topicId.c_str(), cMsg);
  json += ",\"ED\":" + trimVal(cMsg);

  topicId = String(settingMQTTtopTopic) + "/EnergyRet";
  sprintf(cMsg, "%9.3f", EnergyReturned);
  MQTTclient.publish(topicId.c_str(), cMsg);
  json += ",\"ER\":" + trimVal(cMsg);

  topicId = String(settingMQTTtopTopic) + "/GasDel";
  dtostrf(GasDelivered, 9, 2, fChar);
  MQTTclient.publish(topicId.c_str(), fChar);
  json += ",\"GD\":" + trimVal(fChar);

  json += "}";
  _dThis = true;
  if (Verbose1) Debugf("json[%s], length[%d]\n", json.c_str(), json.length());
  topicId = String(settingMQTTtopTopic) + "/JSON/Energy";
  _dThis = true;
  if (!MQTTclient.publish(topicId.c_str(), json.c_str()))
      Debugf("Error publishing Values! (json [%d]chars is to long?)\n", json.length());

  json = "{";
  topicId = String(settingMQTTtopTopic) + "/PowerDel";
  dtostrf(PowerDelivered, 9, 3, fChar);
  MQTTclient.publish(topicId.c_str(), fChar);
  json += "\"PDt\":" + trimVal(fChar);

  topicId = String(settingMQTTtopTopic) + "/PowerRet";
  dtostrf(PowerReturned, 9, 3, fChar);
  MQTTclient.publish(topicId.c_str(), fChar);
  json += ",\"PRt\":" + trimVal(fChar);

  topicId = String(settingMQTTtopTopic) + "/PowerDelL1";
  dtostrf(PowerDelivered_l1, 9, 0, fChar);
  MQTTclient.publish(topicId.c_str(), fChar);
  json += ",\"PDl1\":" + trimVal(fChar);

  topicId = String(settingMQTTtopTopic) + "/PowerDelL2";
  dtostrf(PowerDelivered_l2, 9, 0, fChar);
  MQTTclient.publish(topicId.c_str(), fChar);
  json += ",\"PDl2\":" + trimVal(fChar);

  topicId = String(settingMQTTtopTopic) + "/PowerDelL3";
  dtostrf(PowerDelivered_l3, 9, 0, fChar);
  MQTTclient.publish(topicId.c_str(), fChar);
  json += ",\"PDl3\":" + trimVal(fChar);

  topicId = String(settingMQTTtopTopic) + "/PowerRetL1";
  dtostrf(PowerReturned_l1, 9, 0, fChar);
  MQTTclient.publish(topicId.c_str(), fChar);
  json += ",\"PRl1\":" + trimVal(fChar);
  
  topicId = String(settingMQTTtopTopic) + "/PowerRetL2";
  dtostrf(PowerReturned_l2, 9, 0, fChar);
  MQTTclient.publish(topicId.c_str(), fChar);
  json += ",\"PRl2\":" + trimVal(fChar);

  topicId = String(settingMQTTtopTopic) + "/PowerRetL3";
  dtostrf(PowerReturned_l3, 9, 0, fChar);
  MQTTclient.publish(topicId.c_str(), fChar);
  json += ",\"PRl3\":" + trimVal(fChar);

  json += "}";
  _dThis = true;
  if (Verbose1) Debugf("json[%s], length[%d]\n", json.c_str(), json.length());
  topicId = String(settingMQTTtopTopic) + "/JSON/Power";
  _dThis = true;
  if (!MQTTclient.publish(topicId.c_str(), json.c_str()))
      Debugf("Error publishing Values! (json [%d]chars is to long?)\n", json.length());

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
