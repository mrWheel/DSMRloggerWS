## Firmware naar DSMR-logger v3 Flashen

<div class="admonition note">
<p class="admonition-title">Let op!</p>
De DSMR-logger Versie 3 maakt gebruik van een ESP-01 bordje met 1MB flash geheugen.
Vanwege de omvang van de DSMRloggerWS firmware is deze
alleen met beperkte functionaliteit geschikt om naar
een ESP-01 te worden ge-upload.   

Met een ESP-01(Black/S) is het niet mogelijk om <b><i>Over The Air</i></b> updates van de
firmware of SPIFFS te doen.<br>
Ook de <i><b>RTS</b></i> hardware en de <i><b>I2C</b></i> interface (oled-scherm) zijn 
voor de ESP-01 niet beschikbaar.
</div>

Om de firmware naar de ESP-01 (Black/S) te kunnen flashen moet deze eerst 
voor de ESP-01 geschikt worden gemaakt.

Dit doe je door vóór de `#define`'s twee *slashes* (**//**) te zetten.
In de ArduinoIDE zien de regels na `/*** compiler options ***/` er dan
zó uit:

```
/******************** compiler options  ********************************************/
//  #define IS_ESP12                  // define if it's an ESP-12
//  #define USE_UPDATE_SERVER         // define if updateServer to be used and there is enough memory
//  #define HAS_OLED_SSD1306          // define if an OLED display is present
//  #define SM_GIVES_NO_TIMESTAMP     // define to generate Timestamp from NTP (Only Winter Time)
//  #define SHOW_PSK_KEY              // well .. show the PSK key, what else?
//  #define HAS_NO_METER              // define if No "Slimme Meter" is attached (*TESTING*)
/******************** don't change anything below this comment **********************/

```

<div class="admonition note">
<p class="admonition-title">Let op!</p>
Mocht je een ESP-01 bordje met een <i><b>PUYA</i></b> flash chip hebben, dan moet je nog wat
doen om SPIFFS met deze PUYA chip te laten werken. Kijk
<a href="../PUYA_patch/">hier</a> wat je moet doen.
</div>

Vervolgens moeten de [`Boards`] settings als volgt worden ingevuld:


|   | Instelling        |Waarde|
|:-:|:------------------|:-----|
|   | Board             | "Generic ESP8266 Module" |
|   | Upload Speed      | "115200" |
|   | CPU Frequency     | "80MHz" |
|   | Flash Frequency   | "40MHz" |
|   | Flash Mode        | "DOUT (compatible)" |
|   | Flash Size        | "1M (256K SPIFFS)" |
|   | Crystal Frequency | "26MHz" |
|   | Reset Method      | "None" (afhankelijk van de gebruikte programmer) |
|   | Debug Port        | "Disabled" |
|   | Debug Level       | "None" |
|   | IwIP Variant      | "v2 Lower Memory" |
|   | VTables           | "Flash" |
|   | Exeptions         | "Disabled" |
|   | Builtin Led       | ESP-01 (Black): "1"<br>ESP-01S: &nbsp; &nbsp; &nbsp; &nbsp; "2" |
|   | Erase Flash       | "Only Sketch"<br>(First Time "All Flash Contents") |
|   | Port              | Bedraad: "Serial Port" |


Stop de ESP-01 in de programmer 
(<a href="https://willem.aandewiel.nl/index.php/2018/08/27/eenvoudige-programmer-voor-de-esp-01-esp8266/" target="_blank">
hier</a>
vind je een post over hoe je van een USB to ESP-01
Adapter zelf eenvoudig een programmer kunt maken) en sluit deze aan op je computer.
Vergeet niet de juiste `Port` te selecteren en druk op het *Compile and Upload icoon*.

![](img/CompileAndUploadIcon.png)



<br>

---
<center  style="font-size: 70%">[DSMR-Editor]</center><br>
<center>![](img/DSMR-Editor_Maanden.png)</center>
