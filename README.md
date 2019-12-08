# Documentatie DSMRloggerWS (laatste Versie 1.0.4)

<p>Voor de project beschrijving ga naar <a href="https://willem.aandewiel.nl/index.php/2018/08/28/slimme-meter-uitlezer/">www.aandewiel.nl</a>.</p>
<br>
<table>
<tr><th>Versie</th><th>Opmerking</th></tr>
<tr>
   <td valign="top">1.0.4</td>
   <td>[RvdB] Koppeling met MinderGas.nl
      <br>Mogelijkheid dagelijks het gasverbruik te versturen naar mindergas.nl
      <br>Zodat je eenvoudig je gasverbruik door de jaren heen kan bewaken en 
      <br>vergelijken met anderen.
      <br>
      <br>Nieuwe versie FSexplorer
      <br>  * Twee nieuwe SPIFFS files:
      <br> &nbsp; &nbsp; - FSexplorer.html
      <br> &nbsp; &nbsp; - FSexplorer.css
      <br>  * De volgende files zijn aangepast en moeten dus ook worden ge-upload
      <br> &nbsp; &nbsp; - DSMRlogger.html
      <br> &nbsp; &nbsp; - DSMRlogger.js
      <br> &nbsp; &nbsp; - DSMReditor.html
      <br> &nbsp; &nbsp; - DSMReditor.js
      <br>
      <br>[RvdB] Bug Fix m.b.t. validate MQTT broker IP address  (dank Marco voor het melden)
      <br>Memory footprint (iets) verkleint.
   </td>
</tr>
<tr>
   <td valign="top">1.0.3c</td>
   <td>Bug fix on v1.0.3b
      <br>Memory footprint (iets) verkleint.
   </td>
</tr>
<tr>
   <td valign="top">1.0.3b</td>
   <td>Tussen Release
      <br>Bij het opstarten checkt de firmware of alle (html/js) bestanden
   	  die noodzakelijk zijn voor een goede werking ook daadwerkelijk
	    aanwezig zijn. Zoniet dan wordt automatisch de FSexplorer gestart
	    waarna deze bestanden handmatig, alsnog, naar SPIFFS kunnen worden
	    gekopieerd.
   </td>
</tr>
<tr>
   <td valign="top">1.0.3</td>
   <td>Tweede Officiële Release
      <br>Mogelijkheid om een iets groter OLED scherm (1.3" - SH1106) te gebruiken.
      <br>Bij de MQTT broker kan nu ook een poort-nummer worden opgegeven:
	<br> &nbsp; &nbsp; &lt;brokerURL&gt;:&lt;Poort&gt; (test.mosquitto.org:1883)
	<br>of:
	<br> &nbsp; &nbsp; &lt;brokerIP&gt;:&lt;Poort&gt; (5.196.95.208:1883)
      <br>De, naar de MQTT broker, gepubliceerde berichten zijn aangepast (zie 
	  <a href="https://mrwheel.github.io/DSMRloggerWS/Use_MQTT/">documentatie</a>)
      <br>Diverse cosmetische verbeteringen.
   </td>
</tr>
<tr>
   <td valign="top">1.0.2</td>
   <td>Eerste Officiële Release
      <br>Debug efficiënter gemaakt
      <br>Display status OLED scherm is nu onafhankelijk van binnenkomend Telegram
      <br>Firmware Update voortgang zichtbaar in telnet
      <br>Diverse cosmetische verbeteringen.
      <br>Het is verstandig om de <b>.html</b> en <b>.js</b> files uit de data map via de FSexplorer
	  te uploaden (kan ook met de Update Server maar dan verlies je de historische
          gegevens)
   </td>
</tr>
<tr>
   <td valign="top">0.4.7</td>
   <td>Splitsen settings.ini file in DSMRsettings.ini en DSMRchartColors.ini
      <br><b>P - No Parsing (show RAW data from Smart Meter)</b> maakt nu de DTR pin hoog
          zodat de Slimme Meter de Telegrammen ook daadwerkelijk verstuurd!!
      <br>Diverse cosmetische verbeteringen.
   </td>
</tr>
<tr>
   <td valign="top">0.4.6</td>
   <td>Intergratie MQTT (#define USE_MQTT)
      <br>Via FSexplorer -> [Edit instellingen] -> [Settings]
      <br>Kunnen de volgende gegevens worden ingevuld:
      <br> &nbsp; &nbsp; &nbsp; MQTT server (URL of IP)
      <br> &nbsp; &nbsp; &nbsp; MQTT username
      <br> &nbsp; &nbsp; &nbsp; MQTT password
      <br> &nbsp; &nbsp; &nbsp; MQTT send interval
      <br> &nbsp; &nbsp; &nbsp; MQTT top Topic
      <br>Diverse cosmetische verbeteringen.
   </td>
</tr>
<tr>
   <td valign="top">0.4.5</td>
   <td>Mogelijkheid om pré DSMR 4.0 Slimme Meters uit te lezen
          #define&nbsp;USE_PRE40_PROTOCOL
      <br>Diverse cosmetische verbeteringen.
      <br><hr>
          Als de Slimme Meter geen fase informatie geeft kan
          de PowerDelivered_l1 gevuld worden met PowerDeliverd
          (#define&nbsp;SM_HAS_NO_FASE_INFO)
   </td>
</tr>
<tr>
   <td valign="top">0.4.4</td>
   <td>Mogelijkheid om de Timestamp niet uit de Slimme Meter te halen
      <br>maar uit een NTP server (#define&nbsp;USE_NTP_TIME).
      <br>Betere meldingen op het Oled-scherm (o.a. start AP mode).
      <br>Directe reactie als op de Wake-Up knop (FLASH) wordt gedrukt.
      <br>Diverse cosmetische verbeteringen.
   </td>
</tr>
<tr><td valign="top">0.4.3</td><td>Update meterstanden voor tab [per Uur], [per Dag] en [per Maand]</td></tr>
<tr><td valign="top">0.4.0</td><td>Eerste publieke versie</td></tr>
</table>
<br>
<h2 id="inhoudsopgave">Inhoudsopgave</h2>
<ul>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/introductie/">Introductie</a></li>
<li>Voorbereiding<ul>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/installatieArduinoIDE/">ArduinoIDE</a></li>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/installatieESP8266core/">ESP8266 core</a></li>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/installatieDataUploadTool/">Data Upload Tool</a></li>
</ul>
</li>
<li>Firmware<ul>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/benodigdeBibliotheken/">Benodigd Bibliotheken</a></li>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/installatieBibliotheken/">Installeren Bibliotheken</a></li>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/clonenFirmware/">DSMR Firmware van Github</a></li>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/uploadFirmware_V4/">Firmware Flashen DSMR-logger V4 board</a></li>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/uploadDataMap_V4/">Data Flashen naar DSMR-logger V4 board</a></li>
</ul>
</li>
<li>Hardware<ul>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/hardware_V4/">DSMR-logger Versie 4</a></li>
<li><a href="https://mrwheel.github.io/DSMRloggerWS/hardware_V4_Programmer/">Programmer voor de DSMR-logger</a></li>
</ul>
</li>
</ul>

<center><img src="mkdocs/docs/img/DSMR_V4_d.JPG"></center>
