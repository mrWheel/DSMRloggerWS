## telnet

telnet is een programma/protocol dat veelal op Unix-achtige operating
systems te vinden is. Het is een eenvoudige, niet beveiligde, manier
om op de DSMR-logger in te loggen.

Onder Windows kun je hier het uitstekende 
<a href="https://www.putty.org/" target="_blank">Putty</a>
voor gebruiken.

telnet maakt gebruik van poort 23.
Je start het vanaf een command window op de volgende manier op:
```
    telnet dsmr-ws.local
```
of met het IP adres van de DSMR-logger:
```
    telnet <IP-addres>
```

Hieronder volgt een stukje logging zoals die dan te zien is:
```
    telnet 192.168.9.97   <<<-- dit toetst de gebruiker in

Trying 192.168.9.97...
Connected to 192.168.9.97.
Escape character is '^]'.

[00:00:04][  16536| 16192] setup       ( 630): SPIFFS Mount succesfull
[00:00:05][  16536| 16192] DSMRfileExis( 565): check if [/DSMRlogger.html] exists .. Yes! OK!
[00:00:07][  16536| 16192] DSMRfileExis( 565): check if [/DSMRlogger.js] exists .. Yes! OK!
[00:00:09][  16536| 16192] DSMRfileExis( 565): check if [/DSMRgraphics.js] exists .. Yes! OK!
[00:00:11][  16032| 15688] DSMRfileExis( 565): check if [/DSMRlogger.css] exists .. Yes! OK!
[00:00:13][  16032| 15688] DSMRfileExis( 565): check if [/DSMReditor.html] exists .. Yes! OK!
[00:00:15][  16032| 15688] DSMRfileExis( 565): check if [/DSMReditor.js] exists .. Yes! OK!
[00:00:16][  16032| 15688] DSMRfileExis( 565): check if [/FSexplorer.html] exists .. Yes! OK!
[00:00:18][  16032| 15688] DSMRfileExis( 565): check if [/FSexplorer.css] exists .. Yes! OK!
[00:00:21][  15992| 15688] startWiFi   (  93): Connected with IP-address [192.168.9.97]

Connected to yourWiFi
IP address: 192.168.9.97
[00:00:25][  15880| 15688] startMDNS   ( 120): [1] mDNS setup as [DSMR-WS.local]
[00:00:25][  13992| 13944] startMDNS   ( 122): [2] mDNS responder started as [DSMR-WS.local]
[00:00:26][  15168| 13824] setup       ( 708): Last reset reason: [Software/System restart]
[00:00:26][  15072| 13824] setup       ( 742): Time is set to [180410020101W] from hourData
[02:01:01][  15072| 13824] readSettings(  86):  /DSMRsettings.ini ..
[02:01:01][  14984| 13824] readSettings( 120): Reading settings:
[02:01:01][  13600| 13176] readSettings( 124): [EnergyDeliveredT1 = 0.10000] (27)
[02:01:01][  13520| 13176] readSettings( 124): [EnergyDeliveredT2 = 0.20000] (27)
[02:01:01][  13520| 13176] readSettings( 124): [EnergyReturnedT1 = 0.30000] (26)
[02:01:01][  13520| 13176] readSettings( 124): [EnergyReturnedT2 = 0.40000] (26)
[02:01:01][  13520| 13176] readSettings( 124): [GASDeliveredT = 0.50000] (23)
[02:01:01][  13520| 13176] readSettings( 124): [EnergyVasteKosten = 15.15] (25)
[02:01:01][  12848| 12528] readSettings( 124): [GasVasteKosten = 11.11] (22)
[02:01:01][  12848| 12528] readSettings( 124): [SleepTime = 0] (13)
[02:01:01][  12848| 12528] readSettings( 124): [TelegramInterval = 10] (21)
[02:01:01][  12848| 12528] readSettings( 124): [BackGroundColor = deepskyblue] (29)
[02:01:01][  12824| 12528] readSettings( 124): [FontColor = white] (17)
[02:01:01][  12808| 12528] readSettings( 124): [MQTTbroker = 192.168.9.136:1883] (32)
[02:01:01][  12752| 12528] readSettings( 154): ->Port[1883]
[02:01:01][  12752| 12528] readSettings( 161): => MQTTbrokerURL[192.168.9.136], port[1883]
[02:01:01][  12752| 12528] readSettings( 124): [MQTTinterval = 50] (17)
[02:01:01][  12752| 12528] readSettings( 124): [MQTTtopTopic = DSMR104] (22)
[02:01:01][  12752| 12528] readSettings( 124): [MindergasAuthtoken = MIJNMINDRGASTOKEN] (38)
[02:01:01][  12840| 12528] readSettings( 172):  .. done
[02:01:01][  13056| 12528] readColors  ( 260):  /DSMRchartColors.ini .. .. done
[02:01:01][  13728| 12528] startMQTT   (  33): Set MQTT broker..
[02:01:01][  13728| 12528] startMQTT   (  36): settingMQTTbroker[192.168.9.136:1883] => found[:] @[14]
[02:01:01][  13728| 12528] startMQTT   (  39): ->Port[1883]
[02:01:01][  13728| 12528] startMQTT   (  46): MQTTbrokerURL [192.168.9.136], port[1883]
[02:01:01][  13728| 12528] isValidIP   (  48): 0.168.9.136 = Valid IP
[02:01:01][  13728| 12528] startMQTT   (  57): [192.168.9.136:1883] => setServer(192.168.12.136, 1883)
[02:01:02][  15072| 13824] setup       ( 781): SPIFFS correct populated -> normal operation!
[02:01:05][  13048| 12832] setup       ( 822): HTTP server gestart
[02:01:08][  13048| 12832] setup       ( 835): Enable slimmeMeter..
[02:01:08][  13048| 12832] setup       ( 841): Startup complete! pTimestamp[180410020101W]
[02:01:08][  11704| 11536] setup       ( 850): Last reset reason: [Software/System restart]

Commands are:

   B - Board Info
   C - list GUI Colors
   S - list Settings
   D - Display Day table from SPIFFS
   H - Display Hour table from SPIFFS
   M - Display Month table from SPIFFS
   I - Identify by blinking LED on GPIO[02]
   P - No Parsing (show RAW data from Smart Meter)
  *W - Force Re-Config WiFi
  *R - Reboot
   F - File info on SPIFFS
  *U - Update SPIFFS (save Data-files)
   V - Toggle Verbose 1
   T - Force update mindergas.nl

[09:25:19][   9856|  6392] loop        ( 951):
[Time----][FreeHeap/mBlck][Function----(line):
[09:25:19][   8512|  6392] loop        ( 954): read telegram [21677] => [180409092501S]
[09:40:01][   8312|  6392] sendMQTTData( 163): Sending data to MQTT server [192.168.9.136]:[1883]
[09:40:07][   9856|  6392] loop        ( 951):
[Time----][FreeHeap/mBlck][Function----(line):
[09:40:07][   8512|  6392] loop        ( 954): read telegram [21681] => [180409094001S]

    b <<<--- gebruiker toetst een 'B' in (show Board Info)

==================================================================

            (c)2019 by [Willem Aandewiel]
      Firmware Version [v1.0.4 (06-12-2019)]
              Compiled [Dec  7 2019  12:55:02]
         compiled with [dsmr.h]
              #defines [IS_ESP12][USE_UPDATE_SERVER][USE_MQTT][USE_MINDERGAS][HAS_OLED_SSD1306]
   Telegrams Processed [21690]
           With Errors [6]
              FreeHeap [8432]
             max.Block [6392]
               Chip ID [C2FD70]
          Core Version [2_5_2]
           SDK Version [2.2.1(cfd48f3)]
        CPU Freq (MHz) [80]
      Sketch Size (kB) [530.75]
Free Sketch Space (kB) [2540.00]
         Flash Chip ID [001640EF]
  Flash Chip Size (kB) [4096]
   Chip Real Size (kB) [4096]
      SPIFFS Size (kB) [934]
      Flash Chip Speed [40]
       Flash Chip Mode [DOUT]
==================================================================

            Board type [ESP8266_GENERIC]
                  SSID [yourWiFi]
               PSK key [**********]
            IP Address [192.168.9.97]
              Hostname [DSMR-WS]
     Last reset reason [Software/System restart]
                upTime [8(d):02(h):04]
==================================================================

           MQTT broker [192.168.9.136:1883]
             MQTT User [pi]
         MQTT PassWord [**********]
             Top Topic [DSMR104]
       Update Interval [50]
==================================================================

[09:45:07][   9856|  6392] loop        ( 951):
[Time----][FreeHeap/mBlck][Function----(line):
[09:45:07][   8512|  6392] loop        ( 954): read telegram [21691] => [180409094501S]

    p <<<--- gebruiker toetst de letter 'P' in (stop parsing, show raw telegram)

/XMX5LGBBLB2410065887

1-3:0.2.8(50)
0-0:1.0.0(180409095501S)
0-0:96.1.1(4530303336303000000000000000000040)
1-0:1.8.1(045182.559*kWh)
1-0:1.8.2(170491.991*kWh)
1-0:2.8.1(004428.981*kWh)
1-0:2.8.2(002189.490*kWh)
0-0:96.14.0(0001)
1-0:1.7.0(001.84*kW)
1-0:2.7.0(002.18*kW)
0-0:96.7.21(00010)
0-0:96.7.9(00000)
1-0:99.97.0(0)(0-0:96.7.19)
1-0:32.32.0(00002)
1-0:52.32.0(00003)
1-0:72.32.0(00003)
1-0:32.36.0(00000)
1-0:52.36.0(00000)
1-0:72.36.0(00000)
0-0:96.13.0()
1-0:32.7.0(242.0*V)
1-0:52.7.0(237.0*V)
1-0:72.7.0(236.0*V)
1-0:31.7.0(000*A)
1-0:51.7.0(000*A)
1-0:71.7.0(000*A)
1-0:21.7.0(01.193*kW)
1-0:41.7.0(00.292*kW)
1-0:61.7.0(00.358*kW)
1-0:22.7.0(00.920*kW)
1-0:42.7.0(00.697*kW)
1-0:62.7.0(00.562*kW)
0-1:24.1.0(003)
0-1:96.1.0(4730303339303031363532303530323136)
0-1:24.2.1(180409095501S)(17685.314*m3)
!425D

    p <<<--- gebruiker toetst weer een letter 'P' in (restart parsing)

[09:50:13][   9856|  6392] loop        ( 951):
[Time----][FreeHeap/mBlck][Function----(line):
[09:50:13][   8512|  6392] loop        ( 954): read telegram [21692] => [180409095001S]
[10:00:01][   8072|  6392] processData ( 561): Saving data for thisHourKey[18040909]
[10:00:01][   8072|  6392] fileWriteDat( 187): newDat.label is [18040909]
[10:00:01][   8072|  6392] fileLabel2Re(  78): RecKey is [18040909]
[10:00:01][   8072|  6392] fileWriteDat( 205): ----> write recNo[1]
[10:00:01][   7960|  6392] fileWriteDat( 247): checkRecordsInFile [/PRDhours.csv] ...
[10:00:01][   8072|  6392] fileWriteDat( 187): newDat.label is [18040910]
[10:00:01][   8072|  6392] fileLabel2Re(  78): RecKey is [18040910]
[10:00:01][   7984|  6392] fileLabel2Re( 115): NotFound: Label [18040909] < [18040910]
[10:00:01][   7400|  6392] fileWriteDat( 205): ----> write recNo[-1]
[10:00:01][   7288|  6392] fileWriteDat( 247): checkRecordsInFile [/PRDhours.csv] ...
[10:00:01][   7288|  6392] fileWriteDat( 270): Need to shift down!
[10:00:02][   9416|  6392] processData ( 570): Rollover on the Hour: thisHourKey is [18040910]

    ^] <<<--- gebruiker toetst '<Cntr>+]' in

telnet> q   <<<--- gebruiker toetst de letter 'Q' in

Connection closed.
```
