# Welkom bij de DSMRloggerWS Documentatie

Ga voor de volledige project beschrijving naar 
<a href="https://willem.aandewiel.nl/index.php/2019/04/09/dsmr-logger-v4-slimme-meter-uitlezer/" target="_blank">
www.aandewiel.nl</a>

#### Het idee

Zo langzamerhand zijn alle huizen in Nederland uitgerust met een “Slimme Meter”. 
Deze meters beschikken over een "P1" poort, deze poort kan worden uitgelezen. 
Dit project stelt je in staat dit zelf thuis uit te lezen en de gegevens op een 
door jouw gewenst formaat te presenteren dmv een "DSMR logger". Er zijn 
commerciele P1 oplossingen beschikbaar, dit project geeft je aanzienlijk meer 
controle over de kosten en de gegevens. 

<a href="https://nl.wikipedia.org/wiki/Slimme_meter" target="_blank">DSMR</a> 
is een acroniem voor *Dutch Smart Meter Requirements*

De doelstellingen van de DSMR logger:

- Moet eenvoudig en goedkoop gemaakt kunnen worden.
- Moet “Over The Air” ge-update kunnen worden.
- Moet een beperkte historie hebben die intern opgeslagen word.
- Moet via een externe terminal benaderd kunnen worden (telnet).
- Moet via een browser de gegevens kunnen presenteren.
- Moet via een REST API de gegevens kunnen leveren.
- Moet gegevens eenvoudig verder laten verwerken.


# Inhoudsopgave

* [Bouwbeschrijving](bouwbeschrijvingV4.md)

* [Installatie ArduinoIDE](installatieArduinoIDE.md)

* [Installatie Bibliotheken](installatieBibliotheken.md)

* Firmware
    * [Clonen van Github](clonenFirmware.md)
    * [Uploaden Firmware naar ESP-01](uploadenFirmware.md)
    * [Uploaden Data naar ESP-01](uploadenDataMap.md)
    * [Eerste keer opstarten](eersteKeerOpstarten.md)

* [Verschillen ESP-01 modules](verschillenESP01ESP01S.md)

