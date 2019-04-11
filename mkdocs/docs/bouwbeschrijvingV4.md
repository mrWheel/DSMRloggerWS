# Bouwbeschrijving DSMR-logger V3

Een bouwpakket is [hier verkrijgbaar](https://opencircuit.nl/Product/13859/Slimme-meter-uitlezer-V3-bouwpakket-met-ESP-01) 

### Inhoud Opencircuit bouwpakket 

|Aantal|Item|PCB label|
|---|---|---|
|1x|PCB|-|
|1x|ESP8266-ESP-01 Black edition|-|
|1x|BC547|Q1|
|1x|MCP1700-3v3|U1|
|1x|1K Ω weerstand (bruin,zwart,rood)|R1|
|2x|2K2 Ω weerstand (rood,rood,rood)|R2,R3|
|3x|10K Ω weerstand (bruin,zwart,oranje)|R4,R5,R6|
|1x|Elco 1µF|C1| 
|1x|Elco 2.2µF|C2| 
|1x|Elco 1000µF|C3| 
|1x|Socket connector 4x2|J2|
|1x|RJ12 Socket connector|J1|
|1x|RJ12 kabel|-|



### Solderen

Begin met de laagste componenten: 

- Plaats: R4, R5 en R6, plaats deze plat. Weerstanden hebben geen polariteit, de richting 
maakt dus niet uit.
- Plaats R1,R2,R3 rechtop. 
- Plaats Q1 en U1 - de soldeer pads zijn klein en liggen dicht bij elkaar, wees 
zorgvuldig en neem de tijd. 

De Elco's hebben wel polariteit, de negatieve pool is te zien aan de lichtere streep op de 
zijkant. Op de PCB is deze kant aangegeven met een witte helft. Daar moet de negatieve pool 
op worden aangesloten.

- Plaats C1, C2 en C3 met de juiste negatieve pool en de correcte Elco.
- Plaats J2 en J1 

Als alles goed vast zit, dan is het bordje klaar!

De ESP01 word uiteindelijk in de socket geplaatst, maar moet eerst nog worden geflashed met 
de `DSMRlogger2HTTP` firmware voordat hij kan gaan loggen. Dit doen we via de computer en
de Arduino IDE.


