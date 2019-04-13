## Installatie ESP8266 core
In de Arduino IDE moet bij “Instellingen” de volgende URL worden ingevoerd 
achter “*Additional Boards Manager URL’s:*” (zie rood omlijnde kader)
`http://arduino.esp8266.com/stable/package_esp8266com_index.json`

![](img/Preferences.png)

Lees vooral de uitleg in het `README.md` bestand en de uitgebreide 
[documentatie](https://arduino-esp8266.readthedocs.io/en/2.5.0/)!

Er kunnen meer additional board manager URL’s worden ingevuld. Je moet ze dan 
achter elkaar zetten en scheiden door een komma (**,**).

Eventueel kun je ook het pad waar je projecten staan (de Sketchbook Location) 
aanpassen. Standaard verwijst deze naar je “Documenten” map:
`C:\Users\<LogInNaam>\Documents\arduino\`
.. en dat is een prima plek!

De andere instellingen kun je naar behoefte aanpassen. Hierboven staan de 
instellingen die ik prettig vind. 

Na het maken van aanpassingen klik je op [OK].

Er worden regelmatig verbeteringen in de esp8266 core aangebracht. Deze kun je 
installeren via [`Tools`] -> [`Board`] -> [`Boards Manager`]. Voer bij filter “esp8266” 
in. 

De nieuwste versie is op dit moment 2.5.0.


