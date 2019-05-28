## Firmware Flashen naar DSMR-logger V4 board

Versie 4 van de DSMR-logger hardware maakt gebruik van een ESP-12
processor. Deze processor zit op de printplaat van de DSMR-logger
gesoldeerd en moet dus, op de printplaat, geflashed worden.

Om de firmware naar de **DSMRlogger Versie 4** te kunnen flashen moet deze eerst 
voor de ESP-12 geschikt worden gemaakt.

Dit doe je door in de ArduinoIDE de `#define`'s in het eerste tab-blad aan te passen.

Heb je géén Oled-display (let op de twee *slashes* voor de tweede, derde en vierde `#define`!):
```
/******************** compiler options  ********************************************/
#define IS_ESP12              // define if it's an ESP-12
//  #define USE_ARDUINO_OTA       // define if there is enough memory (DEPRECIATED)
#define USE_UPDATE_SERVER     // define if updateServer to be used and there is enough memory
// #define HAS_OLED_SSD1306      // define if an OLED display is present
//  #define HAS_NO_METER          // define if No "Slimme Meter" is attached
/******************** don't change anything below this comment **********************/

```
Heb je wel een Oled-display op de DSMR-logger aangesloten dan moeten de compiler options
als volgt worden aangepast (alleen twee *slashes* voor de tweede en vierde `#define`!):
```
/******************** compiler options  ********************************************/
#define IS_ESP12              // define if it's an ESP-12
//  #define USE_ARDUINO_OTA       // define if there is enough memory (DEPRECIATED)
#define USE_UPDATE_SERVER     // define if updateServer to be used and there is enough memory
#define HAS_OLED_SSD1306      // define if an OLED display is present
//  #define HAS_NO_METER          // define if No "Slimme Meter" is attached
/******************** don't change anything below this comment **********************/

```

Vervolgens moeten de `Boards` settings als volgt worden ingevuld:

|   | Instelling        |Waarde|
|:-:|:------------------|:-----|
|   | Board             | "Generic ESP8266 Module" |
|   | Upload Speed      | "115200" |
|   | CPU Frequency     | "80MHz" |
|   | Flash Frequency   | "40MHz" |
|   | Flash Mode        | "DOUT (compatible)" |
|   | Flash Size        | "4M (1M SPIFFS)" |
|   | Crystal Frequency | "26MHz" |
|   | Reset Method      | "None" |
|   | Debug Port        | "Disabled" |
|   | Debug Level       | "None" |
|   | IwIP Variant      | "v2 Lower Memory" |
|   | VTables           | "Flash" |
|   | Exeptions         | "Disabled" |
|   | Builtin Led       | "2" |
|   | Erase Flash       | "Only Sketch"<br>(First Time: "All Flash Contents") |
|   | Port              | Bedraad: "Serial Port" <br> OTA: &nbsp; &nbsp; &nbsp; &nbsp; "Network port" |



<hr>
### Firmware Bedraad Flashen
<div class="admonition note">
<p class="admonition-title">Pas op!</p>
Koppel de DSMR-logger los van de <i>Slimme Meter</i> vóórdat je de DSMR-logger
op de programmer aansluit!!
</div>

Sluit de USB->ESP12 programmer aan op de `Program`-header van de *DSMR-logger v4*. 

1. Druk de `FLASH` knop in en houd deze ingedrukt
2. Druk op de `RESET` knop 
3. Laat de `RESET` knop los
4. Laat de `FLASH` knop los

De *DSMR-logger v4* staat nu in Flash-mode en blijft in die mode tot er gegevens vanaf
de programmer naar de DSMR-logger zijn overgebracht óf tot je nog een keer op de
`RESET` knop drukt.

Vergeet niet in de ArduinoIDE de `Port` te selecteren waarop je de USB->ESP12 
programmer hebt aangesloten en druk op het *Compile and Upload* icoon.

![](img/CompileAndUploadIcon.png)

<hr>
### Firmware *Over The Air* Flashen
Bij het *Over The Air* flashen van de firmware of `data`-map kan de 
DSMR-logger v4 gewoon op de Slimme Meter aangesloten blijven.

Alle instellingen voor de DSMRloggerWS firmware blijven gelijk aan
de bedraade methode van flashen. 

Vervolgens moet je niet op het &nbsp; `Compile & Upload`-Icoon
&nbsp; ![](img/CompileAndUploadIcon.png) &nbsp;
klikken maar op het `Verify`-icoon
&nbsp; ![](img/VerifyIcon.png) &nbsp;

Let op!
Om de zojuist gecompileerde firmware straks terug te kunnen vinden is het handig deze instruktie te volgen!
```
Sketch uses 543550 bytes (52%) of program storage space. Maximum is 1044464 bytes.
Global variables use 55948 bytes (68%) of dynamic memory, leaving 25972 bytes for local variables. Maximum is 81920 bytes.
/Users/(LoginName)/Library/Arduino15/packages/esp8266/tools/python/3.7.2-post1/python /Users/(LoginName)/Library/Arduino15/packages/esp8266/hardware/esp8266/2.5.2/tools/upload.py --chip esp8266 --port /dev/cu.Bluetooth-Incoming-Port --baud 115200 --trace version --end --chip esp8266 --port /dev/cu.Bluetooth-Incoming-Port --baud 115200 --trace write_flash 0x0 /Users/(LoginName)/tmp/arduinoBuild/DSMRloggerWS.ino.bin --end 
esptool.py v2.6
```

Als de firmware gecompileerd is klik je op de DSMR-logger pagina op het 
<img src="../img/FSexplorer.png"> icoontje.

In de FSexplorer klik je op de knop `select Firmware`

<center>![](img/DSMRloggerWS_FSexplorer.png)</center>

Er verschijnt nu een scherm waarin je een firmware bestand (de naam eindigt 
op `.ino.bin`) kunt kiezen door op de bovenste knop `Choose File` te klikken. 


<center>![](img/DSMR-FlashUtility.png)</center>

Selecteer uit het `popUp scherm` dat nu verschijnt het binary file dat je 
wilt uploaden ..

<center>![](img/DSMR-FlashChooseFile.png)</center>

.. en klik op de knop `flash Firmware`

<center>![](img/DSMR-FlashWait4Reboot.png)</center>

Na enige tijd krijg je de melding dat de upload is geslaagd en dat de DSMR-logger
opnieuw opstart. 

Het komt regelmatig voor dat het scherm niet automatisch ge-refreshed wordt (dit
lijkt te maken te hebben met de omvang van de firmware die je upload). Klik
in dat geval, na ongeveer 3 minuten, op de tekst 
```
Als het lijkt of er niets gebeurd, wacht dan ongeveer drie minuten en klik daarna hier.
```
Als de verbinding met de server vóór die tijd verbroken wordt klik dan op de
`back` knop van de browser waarna de tekst alsnog (weer) verschijnt. Klik
nu op **hier** om de DSMRloggerWS hoofd pagina opnieuw te laden.

Kijk ook [hier](uploadOTA-bug.md).




<br>

---
<center  style="font-size: 70%">[DSMR-PCB]</center><br>
<center>![](img/DSMRlogger_v4_Top.png)</center>
