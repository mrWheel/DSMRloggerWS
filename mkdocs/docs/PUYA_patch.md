## PUYA Flash Chip 
Als je een ESP-01 met PUYA flash chip hebt moet je, om SPIFFS
te kunnen gebruiken, een specifieke compiler optie meegeven.

Dit doe je door in de map:

` /Users/(YourLoginName)/Library/Arduino15/packages/esp8266/hardware/esp8266/2.5.0/`

een extra bestand neer te zetten.  
Er staat al een bestand met de naam `platform.txt`.   
Maak een nieuw bestand aan met de naam `platform.local.txt` en
zet daar de volgende twee regels in:
```
compiler.c.extra_flags=-DPUYA_SUPPORT=1
compiler.cpp.extra_flags={compiler.c.extra_flags}

```
Vanaf nu zal de PUYA patch automatich gebruikt worden. Ook als dat niet nodig is.
Mocht je de optie niet (meer) nodig hebben dan kun je het `platform.local.txt`
bestand weggooien of een andere naam geven. Bijvoorbeeld: `platform.local.NOT`.

Volgens 
<a href="https://github.com/esp8266/Arduino/pull/5504#issuecomment-490131482" target="_blank">
`TD-er`</a>
 is het geen probleem om deze optie altijd mee te geven, ook als je
een ESP8266 gebruikt met een `niet PUYA chip`.

Kijk ook
 <a href="https://github.com/esp8266/Arduino/pull/5504#issuecomment-490097913" target="_blank">
hier naar (`#5504`)</a> voor meer informatie.

<div class="admonition note">
<p class="admonition-title">Let op!</p>
<i><b>Pim van den Berg</b></i> heeft uitvoerig onderzoek gedaan naar de firmware
op een <i>DSMR-logger <b>Versie 3</b></i> bordje en dit zijn de bevindingen:
<p>
<table>
  <tr align="left"><th>Firmware</th><th align="left">PUYA flash Chip</th><th align="left">andere Flash Chip</th></tr>
  <tr><td>DSMRlogger2HTTP</td><td>Werkt</td><td>Werkt</td></tr>
  <tr><td>DSMRloggerWS</td><td>Werkt niet (altijd)</td><td>Werkt</td></tr>
</table>
Voor de PUYA chip is de extra compiler optie in <b>platfom.local.txt</b> zoals op
deze pagina beschreven gebruikt.
<p><br/>
Alles afwegend lijkt het erop dat de DSMRloggerWS firmware <b>niet</b> geschikt
is voor ESP-01 bordjes met een PUYA flash chip!
</div>


