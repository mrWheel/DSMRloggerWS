## #define IS_ESP12

Met deze optie zal de DSMR-logger de Data Request pin van de Slimme Meter 
'hoog' maken als hij een Telegram kan ontvangen. Na het ontvangen van
een volledig Telegram zal de Data Request pin weer 'laag' worden gemaakt
waardoor de Slimme Meter stopt met het sturen van Telegrammen.
<div class="admonition note">
<p class="admonition-title">Let op! Alleen voor DSMR-logger V4</p>
Deze functionaliteit werkt alleen in combinatie met een Versie 4
van de DSMR-logger. Met een DSMR-logger V2 of V3 bordje moet je
deze optie niet activeren.
</div>

<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">IS_ESP12</td><td>Om gebruik te kunnen maken van
de Data Request pin van de Slimme Meter.
</tr>
</table>


<br>

---

