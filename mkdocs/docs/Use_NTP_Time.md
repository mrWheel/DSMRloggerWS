## #define USE_NTP_TIME

Met deze #define zorg je ervoor dat de Firmware gebruik maakt van het Network Time Protocol 
om te bepalen wanneer een Telegram binnen is gekomen.
Normaal geeft de Slimme Meter de datum en tijd bij ieder Telegram mee 
(in recordtype "0-0:1.0.0"), maar sommige Slimme Meters doen dat niet 
(bijvoorbeeld sommige pré DSMR 4.0 meters).
Deze optie zorgt er dan voor dat alle Telegrammen goed verwerkt worden.
<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">USE_NTP_TIME</td><td>
Ieder Telegram wordt gemarkeerd met een via NTP verkregen Timestamp.
</tr>
</table>

<hr>

