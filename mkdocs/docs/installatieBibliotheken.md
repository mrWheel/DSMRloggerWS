## Installatie Bibliotheken
Nu je een ArduinoIDE hebt waarmee je ESP8266’s kunt programmeren (flashen) zul je 
ontdekken dat er door briljante mensen software is ontwikkeld die je kunt 
gebruiken om complexe projecten te realiseren, zonder dat je zelf het wiel 
hoeft uit te vinden.

Deze software wordt veelal in de vorm van een bibliotheek aangeboden en je 
hoeft zo’n bibliotheek alleen maar te installeren om er gebruik van te kunnen maken.

<a href="https://www.arduino.cc/en/guide/libraries" target="_blank">
Hier</a>
vind je de officiële instructies
voor het installeren van bibliotheken met de ArduinoIDE.

Stel je wilt je ESP8266 benaderen met een `telnet client` zodat je vanaf
je Desktop of Laptop op de ESP8266 kunt inloggen.
Je hebt op de ESP8266 dan een `telnet server` nodig.
Met wat *Googelen* naar "*ESP8266 telnet*" blijken hier een aantal 
bibliotheken voor te bestaan. 

Als voorbeeld nemen we *TelnetStream* van *Juraj Andrassy*. Zijn telnet implementatie
kun je vanaf deze
<a href="https://github.com/jandrassy/TelnetStream/" target="_blanl">
https://github.com/jandrassy/TelnetStream/</a>
github pagina als bibliotheek downloaden. 

![](img/DownloadTelnet.png)

Om de bibliotheek te installeren klik je op de groene 
<span style="background: green; color: white;">[Clone or download]</span>
knop en selecteer je 
<span style="background: blue; color: white;">[Download ZIP]</span>.

Onthou waar je het zip-file bewaard!

Ga nu naar de ArduinoIDE en selecteer:

[`Sketch`] => [`Include Library`] => [`Add .ZIP Library`]

![](img/IDE_Add_Lib_Zip.png)

Er verschijnt een selectie window waar je het zojuist ge-download-de bestand selecteert.

![](img/IDE_Install_Lib_Zip.png)

Klik op <span style="background: blue; color: white;">[Choose]</span>.

De bibliotheek is nu geïnstalleerd en klaar om gebruikt te worden. De meeste 
bibliotheken komen met een aantal voorbeeld programma’s waarmee je kunt leren 
hoe je de bibliotheek kunt gebruiken.

*Juraj Andrassy* is erg summier met zijn uitleg maar gelukkig is 
er een map met voorbeelden (nou ja, één voorbeeld). 

![](img/TelnetExample.png)
(sorry, het plaatje is een beetje verknipt)

Klik je nu op [`TelnetStreamTest`] dan wordt dit voorbeeld programma 
in de ArduinoIDE geladen.

![](img/TelnetTestProg.png)

---
Installeer op dezelfde manier de bibliotheken die voor de
`DSMRloggerWS` firmware nodig zijn
(zie het [volgende hoofdstuk](benodigdeBibliotheken.md)).

<br>

---
<center style="font-size: 70%">[DSMR-Dag]</center><br>
![](img/TabelDag.png)
