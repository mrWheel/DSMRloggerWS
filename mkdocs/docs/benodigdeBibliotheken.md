## Benodigde Bibliotheken

Voor de **DSMRloggerWS** firmware zijn de volgende bibliotheken nodig:

### dsmr
Deze library is ontwikkeld door *Matthijs Kooijman* en vormt het hart van de
DSMR-logger. Je kunt de bibliotheek hier
<a href="https://github.com/matthijskooijman/arduino-dsmr" target="_blank">
https://github.com/matthijskooijman/arduino-dsmr</a>
downloaden.   
De firmware is getest met `Version 0.1 - Commit f79c906 on 18 Sep 2018`.

### TimeLib
Deze is door *Paul Stoffregen* ontwikkeld. Je kunt hem hier
<a href="https://github.com/PaulStoffregen/Time" target="_blank">
https://github.com/PaulStoffregen/Time</a>
downloaden.

### WiFiManager
Je kunt de, door *Tzapu* ontwikkelde, bibliotheek hier
<a href="https://github.com/tzapu/WiFiManager" target="_blank">
https://github.com/tzapu/WiFiManager</a>
downloaden.   
De DSMR-logger firmware is getest met `version 0.14.0` van deze bibliotheek.

### TelnetStream
Deze bibliotheek is door *Jan Drassy* ontwikkeld.
Je kunt deze bibliotheek hier
<a href="https://github.com/jandrassy/TelnetStream" target="_blank">
https://github.com/jandrassy/TelnetStream</a>
downloaden.   
De firmware is getest met `version 0.0.1`.

**Let op:** De installatie van deze bibliotheek gaat net als de andere bibliotheken. Een update 
kan echter pas geïnstalleerd worden als éérst de map `TelnetStream-master` uit de
map `Libraries` wordt verwijderd! 

### WebSocketsServer
Deze bibliotheek is ontwikkeld door *Markus Sattler* en je kunt hem hier
<a href="https://github.com/Links2004/arduinoWebSockets" target="_blank">
https://github.com/Links2004/arduinoWebSockets</a>
downloaden.   
De DSMR-logger firmware is getest met `Version 20.05.2015 - commit 72731be on 16 Jan 2019`.

### SSD1306Ascii
*William Greiman* heeft deze bibliotheek ontwikkeld met in het achterhoofd minimaal 
gebruik van resources (dus: een bibliotheek die weinig geheugen gebruikt).
Je kunt de bibliotheek hier
<a href="https://github.com/greiman/SSD1306Ascii" target="_blank">
https://github.com/greiman/SSD1306Ascii</a>
downloaden.     
De DSMR-logger Firmware is getest met `Version 1.2.x - Commit 97a05cd on 24 Mar 2019`.

  
### Overige libraries
Onderstaande libraries zijn onderdeel van de `ESP8266 Core` **en moeten dus niet handmatig
geïnstalleerd worden**!

	* ESP8266WiFi    
	* ESP8266WebServer
	* WiFiUdp        
	* ESP8266mDNS   
	* FS           
	* ArduinoOTA  

<br>

---
<center style="font-size: 70%">[DSMR-Chart Dag]</center><br>
![](img/ChartDag.png)

