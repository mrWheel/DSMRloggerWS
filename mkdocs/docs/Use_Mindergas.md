## #define USE_MINDERGAS

Met deze optie wordt de functionaliteit om gasverbruik naar
<a href="https://mindergas.nl/ target="_blank">mindergas.nl</a>
te sturen geactiveerd.

Via `FSexplorer -> Edit instellingen -> Settings` kun je het
jouw toegekende authorisatie token invoeren.

![img](img/DSMR-USE_MINDERGAS_Settings.png)

<table>
<tr>
<th align="left" width="30%">Rubriek</th><th align="left" width="69%">Functie</th>
</tr><tr>
<td style="vertical-align:top">Authorisatie Token</td><td>
</td>
</tr>
</table>

<div class="admonition note">
<p class="admonition-title">Let op! &nbsp; &nbsp; (DSMR-logger V3)</p>
Het gebruik van de Editor lijkt op een DSMR-logger V3 bordje teveel
van het goede. In veel gevallen crashed de DSMR-logger.
<br><br>
Oplossing 1: download het <b>DSMRsettings.ini</b> bestand naar je computer, doe de 
aanpassingen die je nodig hebt en upload het bestand weer naar
de DSMR-logger. <br>
Niet vergeten daarna de DSMR-logger te re-booten!
<br><br>
Oplossing 2: Upgrade naar een versie 4 bordje of vervang de 1MB
Flash Chip door een 4MB W25Q32FVSIG 32Mbit flash chip!
</div>

<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">USE_MINDERGAS</td><td>Deze define zorgt ervoor dat
de Firmware één maal per dag het gasverbruik uit de Slimme Meter naar *mindergas.nl* zal sturen.
</tr>
</table>

---

