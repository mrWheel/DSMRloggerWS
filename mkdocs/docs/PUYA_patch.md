## PUYA Flash Chip (ESP-01)
<div class="admonition note">
<p class="admonition-title">Let op! (alleen voor DSMR-logger Versie 2 of 3)</p>
<i><b>Pim van den Berg</b></i> heeft uitvoerig onderzoek gedaan naar de firmware
op een <i>DSMR-logger <b>Versie 3</b></i> bordje met de <b>ESP-01</b> en dit zijn de 
bevindingen:
</div>

| Firmware        | PUYA flash Chip | andere Flash Chip |
|:----------------|:----------------|:------------------|
| DSMRlogger2HTTP | Werkt           | Werkt |
| DSMRloggerWS    | Werkt <b>NIET</b>  | Werkt (niet altijd) |

<hr>
In het <b>Systeem Info</b> scherm (`DSMRloggerWS`) of het <b>Device Info</b> 
scherm (`DSMRlogger2HTTP`) kun je bij `Chip ID` zien of jouw ESP-01 een PUYA Flash chip 
heeft:

<center> ![](img/DSMR-logger_PUYAchip.png) </center>

.. maar je kunt ook (met een vergrootglas) op de chip zélf kijken natuurlijk:
<center> ![](img/PUYAflashChip.jpg) </center>
