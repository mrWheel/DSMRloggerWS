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
<td style="vertical-align:top">Mindergas Authenticatie Token</td>
<td>Bij aanmelding bij mindergas.nl kun je een zgn. <b>Authenticatie Token</b> opvragen.
Dit token heb je nodig om data naar mindergas.nl te kunnen uploaden. Voer dit
token hier in.
</td>
</tr>
</table>

<b>Lees ook <a href="../integratieMindergas/">dit</a></b>.

<div class="admonition note">
<p class="admonition-title">Let op! &nbsp; &nbsp; (DSMR-logger V3)</p>
Deze functie is niet beschikbaar op een DSMR-logger versie 2 of 3 bordje!
</div>

<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">USE_MINDERGAS</td><td>Deze define zorgt ervoor dat
de Firmware één maal per dag het gasverbruik uit de Slimme Meter naar <b>mindergas.nl</b> zal sturen.
</tr>
</table>

---

