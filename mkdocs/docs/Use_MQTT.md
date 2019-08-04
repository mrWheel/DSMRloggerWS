## #define USE_MQTT

Met deze optie wordt de functionaliteit om data naar een MQTT Broker
te sturen geactiveerd.

Via `FSexplorer -> Edit instellingen -> Settings` kun je gegevens
over de te gebruiken MQTT Broker invullen:

![img](img/DSMR-USE_MQTT_Settings.png)

<div class="admonition note">
<p class="admonition-title">Let op!</p>
Het gebruik van de Editor lijkt op een DSMR-logger V3 bordje teveel
van het goede. In veel gevallen crashed de DSMR-logger.
<br><br>
Oplossing 1: upload het <b>DSMRsettings.ini</b> bestand naar je computer, doe de 
aanpassingen die je nodig hebt en upload het bestand weer naar
de DSMR-logger. <br>
Niet vergeten daarna de DSMR-logger te re-booten!
<br><br>
Oplossing 2: Upgrade naar een versie 4 bordje of vervang de 1MB
Flash Chip door een 4MB W25Q32FVSIG 32Mbit flash chip!
</div>


De DSMR-logger zal iedere `MQTT Interval` seconden de volgende berichten naar
de MQTT Broker sturen:
```

DSMR-WS/DateTime 2014-01-05 20:55:01
DSMR-WS/EnergyDel   224.665
DSMR-WS/EnergyRet   186.916
DSMR-WS/GasDel    101.86
DSMR-WS/PowerDel     1.350
DSMR-WS/PowerRet     0.000
DSMR-WS/PowerDelL1        36
DSMR-WS/PowerDelL2       702
DSMR-WS/PowerDelL3       609
DSMR-WS/PowerRetL1         0
DSMR-WS/PowerRetL2         0
DSMR-WS/PowerRetL3         0
DSMR-WS/JSON/Energy {"DT":"2014-01-05 20:55:01","ED":224.665,"ER":186.916,"GD":101.86}
DSMR-WS/JSON/Power {"PDt":1.350,"PRt":0.000,"PDl1":36,"PDl2":702,"PDl3":609,"PRl1":0,"PRl2":0,"PRl3":0}

```
<div class="admonition note">
<p class="admonition-title">Let op!</p>
De standaard <b>PubSubClient</b> library kan berichten publiceren van
maximaal 128 characters. Vandaar de (korte) cryptische benaming van de verschillende velden in
de JSON boodschappen.
Mocht je langere, meer betekenisvolle, namen willen gebruiken dan moet je in
<b>PubSubClient.h</b> de <i>#define</i> van <b>MQTT_MAX_PACKET_SIZE</b> aanpassen.
</div>

<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">USE_MQTT</td><td>Deze define zorgt ervoor dat
de Firmware data uit de Slimme Meter naar een MQTT Broker stuurt.
</tr>
</table>

---

