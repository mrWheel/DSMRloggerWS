## upload Data map naar DSMR-logger V4 board

Zorg dat de `Serial Monitor` is afgesloten.

- Sluit de programmer aan op de DSMR-logger. 
- Druk nu de **[Flash]** knop in en houdt deze ingedrukt. 
- Druk vervolgens de **[Reset]** knop in en laat deze weer los. 
- Laat nu ook de **[Flash]** knop los.

De DSMR-logger staat nu in "Flash-mode" en wacht (geduldig) tot de inhoud van de `data` map
wordt opgestuurd.

- Ga in de Arduino IDE naar `Tools` ** -> ** `ESP8266 Sketch Data Upload`

Alle bestanden in de `data` map worden nu naar het SPIFFS overgezet.

Hierna zal de DSMR-logger weer normaal opstarten, maar met de nieuw ge-flash-de firmware.
