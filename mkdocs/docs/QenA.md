## Vragen en Antwoorden

* [Niet alles DSMR 4 is DSMR 4](#niet-alles-dsmr-4-is-dsmr-4)
* [5 Volt Programmer](#5-volt-programmer)
* [Tactile Switches](#tactile-switches)
* [PUYA Chip](#puya-chip)
* [Update of Upgrade](#update-of-upgrade)

<hr>

### Niet alles DSMR 4 is DSMR 4 ..
<div class="admonition note">
<p class="admonition-title">Mijn DSMR-logger doet het niet als ik hem op de Slimme Meter aansluit</p>
Er zijn Slimme Meters die 'zeggen' DSMR Versie 4
te volgen maar toch geen 5 volt met voldoende stroom leveren.
<br>Bouwers van de DSMR-logger hebben problemen gemeld met de <i><b>KAIFA</b> MA105A</i> en 
<i>MA105C</i> en met de <i><b>KAIFA</b> MA304</i>.
<br>Kijk bijvoorbeeld
<a href="https://willem.aandewiel.nl/index.php/2018/08/28/slimme-meter-uitlezer/#comment-368" target="_blank">
hier</a> en
<a href="https://willem.aandewiel.nl/index.php/2018/08/28/slimme-meter-uitlezer/#comment-334" target="_blank">
hier</a>.
<br>
<br>Dit probleem is op te lossen door de DSMR-logger op een externe (bijvoorbeeld USB) voeding
aan te sluiten.

</div>
<hr>

### 5 Volt Programmer
<div class="admonition note">
<p class="admonition-title">Kan ik met een 5 volt USB to TTL adaptor de DSMR-logger flashen?</p>
Als het een type is dat de TX en RX signalen op 3v3 levert (bijvoorbeeld 
<a href="https://opencircuit.nl/Product/12809/USB-to-TTL-Serial-Cable-Debug-Console-Cable-for-Raspberry-Pi" target="_blank">
deze</a>)
dan kán het wel, maar moet je de 5 Volt output op de 5 Volt pin van de DSMR-logger aansluiten.
Je moet hem dan zoals in het plaatje hieronder aansluiten.

</div>

<center> ![](img/5VoltProgrammer.png) </center>

<hr>

### Tactile Switches
<div class="admonition note">
<p class="admonition-title">Waarom heb ik een tacktile switch met een lange en één met een korte stift?</p>
De tacktile switch met de korte stift is de RESET switch. 
Eenmaal ingebouwd in een (project) box moet de FLASH switch bereikbaar blijven omdat hij
een dubbel functie heeft. 
Hij dient bij normaal gebruik om het oled-scherm 'wakker' te maken.
Vandaar de lange stift!
<br />
Deze lange stift maakt de switch wel erg kwetsbaar zolang de DSMR-logger niet in een (project) box
is ingebouwd. Als je er een niet <i><b>recht-naar-beneden</b></i> kracht op uitoefend, dan 
breekt hij makkelijk af!

</div>

<hr>

### PUYA Chip
<div class="admonition note">
<p class="admonition-title">Mijn versie 3 bordje met PUYA chip doet het niet</p>
Het heeft er alle schijn van dat de DSMRloggerWS firmware niet geschikt is voor
een ESP-01 (versie 3 bordje) met een PUYA flash chip.
<p>
Kijk ook <a href="../PUYA_patch/">hier</a>.
</div>

<hr>

### Update of Upgrade?
<div class="admonition note">
<p class="admonition-title">Wanneer moet ik updaten en wanneer upgraden?</p>
We doen een `update` als we de firmware willen vervangen met 
een nieuwere versie van dezelfde firmware (dus we vervangen DSMRlogger2HTTP door 
een nieuwere versie van DSMRlogger2HTTP óf we vervangen de DSMRloggerWS 
firmware door een nieuwere versie van de DSMRloggerWS firmware)

We doen een `upgrade` als we de ene firmware vervangen door een andere (dus
we vervangen bijvoorbeeld DSMRlogger2HTTP door DSMRloggerWS).

#### Update
Bij een `update` zal de lay-out van de data-bestanden niet veranderen en kun 
je volstaan met het opnieuw flashen van de firmware.    
Voor de data-map (SPIFFS) kun je twee wegen bewandelen.

1)
Je haalt m.b.v. de `FSexplorer` (of met DSMRlogger2HTTP `http://DSMR-ESP01.local/onderhoud`)
de `.html` en `.js` bestanden op uit de `data`-map en zet deze in SPIFFS

2)
Je haalt met de `FSexplorer` de `PRDxxxx.csv` bestanden op en zet deze in de `data`-map, 
waarna je met het `Data Upload Tool` de `data`-map naar SPIFFS kunt overzetten.

#### Upgrade
Bij een `upgrade` moet je zowel de firmware als de `data`-map opnieuw naar de
DSMR-logger flashen.   
Uiteraard kun je ook in dit geval éérst de `PRDxxxx.csv` bestanden uit de 
DSMR-logger halen en ergens op je computer bewaren. Met wat moeite kun je
de inhoud van deze bestanden omzetten in het nieuwe formaat en ze dan weer
terug zetten in de DSMR-logger.

</div>

<br>
<hr>
<center style="font-size: 70%;">[RJ12 (6P6C)]</center><br>

<center> ![](img/RJ12.png) </center>
 
