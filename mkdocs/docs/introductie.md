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
<tr><th align="left">Begrip</th><th align="left">Omschrijving</th></tr>
<tr>
	<td style="vertical-align:top">DSMR-logger</td>
	<td>de Hardware</td>
</tr>
<tr>
	<td style="vertical-align:top">DSMR-logger v2 (of Versie 2)</td>
	<td>Versie 2 van de Hardware
            <br>Dit is de hardware van de DSMR-logger die is uitgerust 
            <br>met een ESP-01 (Black/S) bordje.
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
	<td>Een ESP-01 met 1MB flash geheugen
            <br>Dit bordje heeft een rode Power Led en een blauwe Led op GPIO-01
        </td>
</tr>
<tr>
	<td style="vertical-align:top">ESP-01S</td>
	<td>Een ESP-01 met 1MB flash geheugen
            <br>Dit bordje heeft alleen een blauwe Led op GPIO-02
        </td>
</tr>
<tr>
	<td style="vertical-align:top">ESP-01(Black/S)</td>
	<td>Een ESP-01 Black of ESP-01S
            <br>DSMR-logger v2 en v3 maken gebruik van dit bordje
        </td>
</tr>
<tr>
	<td style="vertical-align:top">ESP-12</td>
	<td>Een bordje met een ESP8266 processor en 4MB Flash Geheugen
	    <br>Dit bordje wordt gebruikt in de DSMR-logger v4
	</td>
</tr>
</table>
</div>
Een volledige beschrijving van dit project kun je
[hier](https://willem.aandewiel.nl/index.php/2019/04/09/dsmr-logger-v4-slimme-meter-uitlezer/)
vinden.



<br>

---
<center  style="font-size: 70%">[DSMRlogger v4]</center><br>
<center>![](img/DSMR_V4_d.JPG)</center>

