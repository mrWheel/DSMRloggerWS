## upload Data map naar DSMR-logger v3
De `data`-map van de DSMRloggerWS firmware kan alleen via een programmer naar 
de ESP-01 worden overgezet (omdat de DSMRloggerWS firmware op de ESP-01 geen 
OTA functionaliteit heeft).


Je kunt 
<a href="https://willem.aandewiel.nl/index.php/2018/08/27/eenvoudige-programmer-voor-de-esp-01-esp8266/" target="_blank">
hier</a>
een uitvoerige post vinden over hoe je zelf een eenvoudige programmer voor de
ESP-01 kunt maken en hoe je die vervolgens moet gebruiken om de firmware en de
*DSMRloggerWS* `data`-map naar de ESP-01 te uploaden.

Uiteraard kun je hiervoor ook iedere andere geschikte programmer gebruiken!

<center>![](img/USB2ESP01-PrgrmrHack.png)</center>

Als je de ESP-01 op de programmer hebt aangesloten en deze staat in "Flash-Mode"
ga dan in de Arduino IDE naar `Tools`** -> **`ESP8266 Sketch Data Upload` <br>

<div class="admonition note">
<p class="admonition-title">Let op!</p>
De lay-out van de <b>.csv</b> bestanden van de <i>DSMRloggerWS</i> firmware is niet
compatibel met die van de <i>DSMRlogger2HTTP</i> firmware.<br>
Mocht je de, met de <i>DSMRlogger2HTTP</i> firmware opgebouwde, history willen
bewaren, dan moet je deze bestanden éérst naar je computer uploaden (dat kan
via de <b>/onderhoud</b> pagina).
</div>

SPIFFS wordt nu leeg gemaakt en alle bestanden in de `data`-map worden naar het SPIFFS overgezet.

Hierna zal de DSMR-logger normaal opstarten, maar met de nieuw SPIFFS inhoud.

<div class="admonition note">
<p class="admonition-title">Let op!</p>
Hou er rekening mee dat eventuele data-bestanden die al op SPIFFS stonden nu weg zijn! 
Als je ze niet kwijt wil moet je er eerst een kopie van maken op je computer en deze, 
na het flashen van SPIFFS weer terug zetten (dat kan met behulp van de 
<b>/onderhoud</b> pagina)!
</div>


<br>

---
<center style="font-size: 70%;">[ChartJaar]</center><br>

![](img/ChartJaar.png)



