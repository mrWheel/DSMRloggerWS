## #define HAS_NO_METER

Om te testen of de verwerking van de gegevens, die normaal gesproken uit
de Slimme Meter komen, goed gaat kun je met deze #define de firmware 
zó maken dat hij zélf voor (test) data zorgt. Om de tijd te versnellen
zal de testdata eerst de maanden versneld laten voorbij gaan, daarna
de dagen en tenslotte de uren.

<div class="admonition note">
<p class="admonition-title">Let op! Alleen om te testen!</p>
<p class="admonition-title">Let op! Deze functie schakelt het versturen van berichten naar de MQTT broker uit</p>
</div>

<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">HAS_NO_METER</td>
<td>
	De DSMRloggerWS firmware zorgt zelf voor test-data.
    <br>Als deze functionaliteit actief is moet de DSMR-logger
	  <b>niet</b> op een Slimme Meter worden aangesloten!
    <br>Omdat er geen daadwerkelijke telegrammen worden aangemaakt zullen
        er ook geen berichten naar de MQTT broker worden verstuurd!
</td>
</tr>
</table>

<hr>

