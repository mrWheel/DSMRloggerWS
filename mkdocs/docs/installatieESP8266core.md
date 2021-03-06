## Installatie ESP8266 core
In de Arduino IDE moet bij “Instellingen” de volgende URL worden ingevoerd 
achter “*Additional Boards Manager URL’s:*” (zie rood omlijnde kader)

`https://arduino.esp8266.com/stable/package_esp8266com_index.json`

![](img/Preferences.png)

Lees vooral de uitleg in het 
<a href="https://github.com/esp8266/Arduino/blob/master/README.md" target="_blank">
`README.md`</a>
bestand en de uitgebreide 
<a href="https://arduino-esp8266.readthedocs.io/en/latest/" target="_blank">
`documentatie`</a>
op hun website!

Er kunnen meer additional board manager URL’s worden ingevuld. Je moet ze dan 
achter elkaar zetten en scheiden door een komma (**,**).

Eventueel kun je ook het pad waar je projecten staan (de `Sketchbook Location`) 
aanpassen. Standaard verwijst deze naar je `Documenten` map:

`C:\Users\<YourLoginName>\Documents\arduino\`

.. en dat is een prima plek!

De andere instellingen kun je naar behoefte aanpassen. Hierboven staan de 
instellingen die ik prettig vind. 

Na het maken van aanpassingen klik je op [OK].

Ga nu via de ArduinoIDE menu-balk naar
[`Tools`] -> [`Board`] -> [`Boards Manager`]. 

![](img/IDE_BoardsManager.png)

Voer bij filter “esp8266” in. 

![](img/IDE_UpdateInstallESP8266core.png)

Selecteer de versie die je wilt gaan gebruiken en klik op [`Install`].

<div class="admonition note">
<p class="admonition-title">Let op!</p>
De DSMRloggerWS firmware v1.0.3 en v1.0.4 zijn getest met <b>versie 2.5.0</b>
en met <b>versie 2.5.2</b> van de Arduino/ESP8266 core.
<br><br>
Ondertussen is versie (1.0.5) geschikt gemaakt voor
<b>versie 2.6.3</b> van de ESP8266 core. De oudere versies
van de core zijn <i>niet</i> compatible met de DSMRloggerWS v1.0.5 firmware!
</div>



<br>

---
<center  style="font-size: 70%">[DSMR-Uur]</center><br>
![](img/TabelUur.png)
