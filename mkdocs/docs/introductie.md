## Introductie

Je vindt hier de documentatie voor het project DSMR-logger (Versie 4).

De DSMR-logger is een hardware en software systeem waarmee de Slimme Meter 
(vanaf *DSMR* versie 4.0) kan worden uitgelezen. De uitgelezen data (telegrammen) 
worden in de DSMR-logger opgeslagen en kunnen in de vorm van tabellen en 
charts, via WiFi, op een computer of tablet worden weergegeven.

<div class="admonition note">
<p class="admonition-title">Begrippen</p>
In dit document worden de volgende begrippen gebruikt:
<table>
<tr><th align="left" width="35%">Begrip</th><th align="left">Omschrijving</th></tr>
<tr>
	<td style="vertical-align:top">DSMR</td>
	<td><a href="https://nl.wikipedia.org/wiki/Slimme_meter" target="_blank">
               Dutch Smart Meter Requirements</a>
            <br>De 
   <a href="https://www.netbeheernederland.nl/_upload/Files/Slimme_meter_15_a727fce1f1.pdf" target="_blank">
            DSMR-specificatie</a>
            is een afgeleide van de NTA 8130-normering.
	    <br>De DSMR-logger (V3 en V4) is ontworpen voor de DSMR versie 4.0 of
                hoger. Versie 4 geeft aan dat de Slimme Meter op de P1-poort
                5 volt bij 250mA moet kunnen leveren (zie pagina 8 en 9 van
                de specificatie). Deze spanning wordt door
                de DSMR-logger gebruikt om de elektronica te voeden.
        </td>
</tr>
<tr>
	<td style="vertical-align:top">DSMR-logger</td>
	<td>de Hardware</td>
</tr>
<tr>
	<td style="vertical-align:top">DSMR-logger v2 (of Versie&nbsp;2)</td>
	<td>Versie 2 van de Hardware
            <br>Dit is de hardware van de DSMR-logger die is uitgerust 
            <br>met een <b>ESP-01 (Black/S)</b> bordje.
	    <br>Voor deze versie is de firmware DSMRlogger2HTTP bedoeld
	    <br>maar DSMRloggerWS is, met beperkte functionaliteit, ook
	    <br>geschikt te maken voor deze versie van de hardware.
        </td>
</tr>
<tr>
	<td style="vertical-align:top">DSMR-logger v3 (of Versie 3)</td>
	<td>Versie 3 van de Hardware
	    <br>Verder gelijk aan Versie 2.
        </td>
</tr>
<tr>
	<td style="vertical-align:top">DSMR-logger v4 (of Versie 4)</td>
	<td>Versie 4 van de Hardware
	    <br>Deze hardware is de basis voor dit project
        </td>
</tr>
<tr>
	<td style="vertical-align:top">DSMRloggerWS</td>
	<td>De firmware voor de DSMR-logger v4</td></tr>
<tr>
	<td style="vertical-align:top">ESP-01</td>
	<td>Een bordje met een ESP8266 processor
	    <br>en 512KB flash geheugen
	    <br>Dit bordje is <b>niet</b> geschikt voor dit project
	</td>
</tr>
<tr>
	<td style="vertical-align:top">ESP-01 (Black)</td>
	<td>Een <b>ESP-01</b> met <b>1MB</b> flash geheugen
            <br>Dit bordje heeft een rode Power Led en een blauwe Led op <b>GPIO-01</b>
        </td>
</tr>
<tr>
	<td style="vertical-align:top">ESP-01S</td>
	<td>Een <b>ESP-01</b> met <b>1MB</b> flash geheugen
            <br>Dit bordje heeft alleen een blauwe Led op <b>GPIO-02</b>
        </td>
</tr>
<tr>
	<td style="vertical-align:top">ESP-01(Black/S)</td>
	<td>Als een '<b>ESP-01 Black</b>' of '<b>ESP-01S</b>' wordt bedoelt maar niet een '<b>ESP-01</b>'.
            <br>DSMR-logger v2 en v3 maken gebruik van dit bordje
        </td>
</tr>
<tr>
	<td style="vertical-align:top">ESP-12</td>
	<td>Een bordje met een ESP8266 processor en <b>4MB</b> Flash Geheugen
	    <br>Dit bordje wordt gebruikt in de DSMR-logger v4
	</td>
</tr>
</table>
</div>
Een volledige beschrijving van dit project kun je
<a href="https://willem.aandewiel.nl/index.php/2019/04/09/dsmr-logger-v4-slimme-meter-uitlezer/" target="_blank">
hier</a>
vinden.

<br>

---
<center  style="font-size: 70%">[DSMRlogger v4]</center><br>
<center>![](img/DSMR_V4_d.JPG)</center>

