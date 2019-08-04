## #define SM_HAS_NO_FASE_INFO

Sommige (vaak pré DSMR 4.0) meters die geschikt zijn voor één fase,
geven geen Fase Informatie via recordtype "1-0:x1.7.0" en "1-0:x2.7.0"
door, waardoor de DSMRloggerWS firmware geen data voor de "Actueel" chart 
ontvangt.
Met deze optie wordt de informatie uit de "1-0:1.7.0" en "1-0:2.7.0" records
in de fase records gestopt.
<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">SM_HAS_NO_FASE_INFO</td><td>
De velden <b>PowerDelivered_l1</b> en <b>PowereReceived_l1</b> worden, respectievelijk, 
gevuld met de waarden van de <b>PowerDelivered</b> en <b>PowerReceived</b> velden.
</td>
</tr>
</table>

<hr>

