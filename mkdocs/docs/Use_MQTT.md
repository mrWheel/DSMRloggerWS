## #define USE_MQTT

Met deze optie wordt de functionaliteit om data naar een MQTT Broker
te sturen geactiveerd.

Via `FSexplorer -> Edit instellingen -> Settings` kun je gegevens
over de te gebruiken MQTT Broker invullen:

![img](img/DSMR-USE_MQTT_Settings.png)

<table>
<tr>
<th align="left" width="30%">Rubriek</th><th align="left" width="69%">Functie</th>
</tr><tr>
<td style="vertical-align:top">MQTT broker URL/IP</td><td>
	Vul hier de URL of het IP adres van de MQTT broker in 
	eventueel gevolgd door een '<b>:</b>' en poort-nummer.
	<br>Voorbeeld: "<b>test.mosquitto.org:1883</b>"
	<br>Hou er wel rekening mee dat de ESP8266 geen
	encryptie aan kan! 
</td>
</tr><tr>
<td style="vertical-align:top">MQTT Username</td><td>
 	Vul hier de naam in van de gebruiker van de MQTT broker.
</td>
</tr><tr>
<td style="vertical-align:top">MQTT Password</td><td>
 	Als de MQTT broker gebruiker een wachtwoord nodig heeft
	vul die dan hier in.
</td>
</tr><tr>
<td style="vertical-align:top">MQTT Interval (sec.)</td><td>
 	De Interval geeft aan om de hoeveel seconden
	de DSMR-logger gegevens naar de MQTT broker
	moet sturen. 
	<br>Maak deze Interval niet te klein (30 seconden
	is een minimale waarde waarbij de DSMR-logger normaal
	blijft functioneren).
</td>
</tr><tr>
<td style="vertical-align:top">MQTT TopTopic</td><td>
 	Dit is het topic waaronder alle andere gegevens
	worden gepubliceerd. Standaard is dit "DSMR-WS".
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


De DSMR-logger zal iedere `MQTT Interval` seconden de volgende berichten naar
de MQTT Broker sturen:
```

DSMR-WS/JSON/identification {"identification":"XMX5LGBBLB2410065887"}
DSMR-WS/JSON/p1_version {"p1_version":"50"}
DSMR-WS/JSON/timestamp {"timestamp":"140223173001S"}
DSMR-WS/JSON/equipment_id {"equipment_id":"4530303336303000000000000000000040"}
DSMR-WS/JSON/energy_delivered_tariff1 {"energy_delivered_tariff1":170.899,"unit":"kWh"}
DSMR-WS/JSON/energy_delivered_tariff2 {"energy_delivered_tariff2":163.798,"unit":"kWh"}
DSMR-WS/JSON/energy_delivered {"energy_delivered":334.697,"unit":"kWh"}
DSMR-WS/JSON/energy_returned_tariff1 {"energy_returned_tariff1":113.378,"unit":"kWh"}
DSMR-WS/JSON/energy_returned_tariff2 {"energy_returned_tariff2":76.650,"unit":"kWh"}
DSMR-WS/JSON/energy_returned {"energy_returned":190.028,"unit":"kWh"}
DSMR-WS/JSON/electricity_tariff {"electricity_tariff":0001}
DSMR-WS/JSON/power_delivered {"power_delivered":1.230,"unit":"kW"}
DSMR-WS/JSON/power_returned {"power_returned":1.770,"unit":"kW"}
DSMR-WS/JSON/voltage_l1 {"voltage_l1":237.0,"unit":"volt"}
DSMR-WS/JSON/voltage_l2 {"voltage_l2":238.0,"unit":"volt"}
DSMR-WS/JSON/voltage_l3 {"voltage_l3":235.0,"unit":"volt"}
DSMR-WS/JSON/current_l1 {"current_l1":2,"unit":"ampere"}
DSMR-WS/JSON/current_l2 {"current_l2":2,"unit":"ampere"}
DSMR-WS/JSON/current_l3 {"current_l3":0,"unit":"ampere"}
DSMR-WS/JSON/power_delivered_l1 {"power_delivered_l1":874,"unit":"Watt"}
DSMR-WS/JSON/power_delivered_l2 {"power_delivered_l2":336,"unit":"Watt"}
DSMR-WS/JSON/power_delivered_l3 {"power_delivered_l3":17,"unit":"Watt"}
DSMR-WS/JSON/power_returned_l1 {"power_returned_l1":1066,"unit":"Watt"}
DSMR-WS/JSON/power_returned_l2 {"power_returned_l2":274,"unit":"Watt"}
DSMR-WS/JSON/power_returned_l3 {"power_returned_l3":427,"unit":"Watt"}
DSMR-WS/JSON/gas_device_type {"gas_device_type":"3"}
DSMR-WS/JSON/gas_equipment_id {"gas_equipment_id":"4730303339303031363532303530323136"}
DSMR-WS/JSON/gas_delivered {"gas_delivered":119.69,"unit":"m3"}

```

Voor de Mosquitto client kun je de berichten volgen met het volgende commando:

```mosquitto_sub -h test.mosquitto.org -v -t 'DSMR-WS/#' ```

of, als je een username/password hebt ingesteld met dit commando:

```mosquitto_sub -h test.mosquitto.org -u <username> -P <password> -v -t 'DSMR-WS/#' ```


<div class="admonition note">
<p class="admonition-title">Let op!</p>
De standaard <b>PubSubClient</b> library kan berichten publiceren van
maximaal 128 characters. Vandaar de (korte) cryptische benaming van de verschillende velden in
de JSON <i>Energy</i> en <i>Power</i> boodschappen.
Mocht je langere, meer betekenisvolle, namen willen gebruiken dan moet je in
<b>PubSubClient.h</b> de <i>#define</i> van <b>MQTT_MAX_PACKET_SIZE</b> aanpassen.
<br>
<br>De <i>Energy</i> en <i>Power</i> publicaties zullen in een volgende versie van de DSMRloggerWS 
firmware vervallen.
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

