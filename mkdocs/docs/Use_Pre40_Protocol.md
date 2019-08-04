## #define USE_PRE40_PROTOCOL

Vanaf versie 0.4.5 kan de DSMRloggerWS firmware ook overweg met Slimme Meters
die nog niet aan de DSMR 4.0 standaard voldoen.

<div class="admonition note">
<p class="admonition-title">Let op!</p>
Deze functionaliteit is slechts beperkt getest!
<br>Met dank aan <i>Jordy</i> voor het testen met een DSMR 2.2 meter!
<br>
<br>Mocht je meer informatie kunnen geven over eventuele
problemen neem dan contact met mij op (plaats een comment op mijn website)
</div>

Om de DSMRloggerWS firmware geschikt te maken voor een pré DSMR 4.0 Slimme 
Meter zijn er een aantal opties mogelijk (zie de #define tabel hieronder) die mogelijk
van toepassing zijn op pré DSMR 4.0 Slimme Meters.

Om deze functionaliteit te kunnen gebruiken moet je ook de speciaal gehackte 
<a href="https://github.com/mrWheel/arduino-dsmr30" target="_blank">
arduino-dsmr30</a> bibliotheek installeren!

<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">USE_PRE40_PROTOCOL</td><td>Deze define zorgt ervoor dat de instellingen 
<br>voor de seriële poort goed worden gezet (9600, SERIAL_7E1).
<br>tevens wordt een speciaal gehackte 'arduino-dsmr' bibliotheek
<br>gebruikt omdat het pré DSMR 4.0 protocol geen checksum heeft.</td>
</tr><tr>
<td style="vertical-align:top">USE_NTP_TIME</td><td>Het pré DSMR 4.0 protocol geeft niet altijd een timestamp (0-0:1.0.0).
<br>Doet jouw Slimme Meter dat ook niet, dan wordt met deze define de tijd
<br>via het NTP protocol gebruikt. Vooralsnog alleen Winter Tijd!</td>
</tr><tr>
<td style="vertical-align:top">SM_HAS_NO_FASE_INFO</td><td>Heb je een Slimme Meter die geen verbruik per fase (1-0:x1.7.0 enz.)
<br>door geeft, dan kun je met deze optie er toch voor zorgen dat de <i>Actuele</i> 
<br>chart informatie geeft over je verbruik</td>
</tr>
</table>

<hr>

