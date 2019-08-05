## #define SHOW_PASSWRDS

Met deze #define geef je aan of je wilt dat de <b>PSK Key</b> van je WiFi netwerk
en het <b>wachtwoord</b> van de gebruikte MQTT Broker zichtbaar worden.

Via telnet het opvragen van de Board Info:

```
 B - Board Info
         .
         .
         .

==================================================================
               Board type [ESP8266_GENERIC]
                     SSID [A@nd@W@F@]
                  PSK key [**********]
               IP Address [192.168.1.108]
                 Hostname [DSMR-WS]
       Last reset reason: [Exception]
                   upTime [32(d):02:01]
==================================================================

```
en de Settings:
```
 S - list Settings
         .
         .
         .

==== MQTT settings ==============================================
          MQTT broker URL/IP : hassio.local
                   MQTT user : hassUser1
               MQTT password : *************
          MQTT send Interval : 120
              MQTT top Topic : DSMR-WS

```

<table>
<tr>
<th align="left">Define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">SHOW_PASSWRDS</td><td>
In de <b>System Info</b> tab wordt, als deze #define actief is
de <b>PSK Key</b> van het WiFi netwerk getoont.
<br>Bij de <b>B - Board Info</b> uitvoer wordt met deze deze #define
de <b>PSK Key</b> zichtbaar en bij <b>S - list Settings</b> het 
wachtwoord van de MQTT Broker.
</td>
</tr>
</table>

<br>

---
<center  style="font-size: 70%">[System Info]</center><br>
<center>![](img/DSMR-logger-PSK_Key.png)</center>

