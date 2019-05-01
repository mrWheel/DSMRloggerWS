## Firmware naar ESP-01 Flashen

<div class="admonition note">
<p class="admonition-title">Let op!</p>
De DSMRloggerWS firmware is niet geschikt om, met alle functionaliteit, op
een ESP-01 te worden ge-upload. Daarvoor is de omvang van de firmware
eenvoudig te groot.<br>
Met een ESP-01(Black/S) is het niet mogelijk om <b><i>Over The Air</i></b> updates van de
firmware of SPIFFS te doen.<br>
Ook de <i>RTS</i> hardware en de <i>I2C</i> interface (oled-scherm) zijn voor de ESP-01 
niet beschikbaar.
</div>

Om de firmware naar de ESP-01 (Black/S) te kunnen flashen moet deze eerst 
voor deze ESP-01 geschikt worden gemaakt.

Dit doen we door vóór de `#define`'s twee *slashes* (**//**) te zetten.
In de ArduinoIDE zien de regels na `/*** compiler options ***/` er dan
zó uit:

```
/******************** compiler options  ********************************************/
// #define IS_ESP12              // define if it's an ESP-12
// #define USE_ARDUINO_OTA       // define if there is enough memory
// #define HAS_OLED_SSD1306      // define if an OLED display is present
// #define HAS_NO_METER          // define if No "Slimme Meter" is attached
/******************** don't change anything below this line ************************/
```

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
|   | Reset Method      | "None" |
|   | Debug Port        | "Disabled" |
|   | Debug Level       | "None" |
|   | IwIP Variant      | "v2 Lower Memory" |
|   | VTables           | "Flash" |
|   | Exeptions         | "Disabled" |
|   | Builtin Led       | ESP-01 (Black) "1" <br> ESP-01S "2" |
|   | Erase Flash       | "Only Sketch" <br> First Time "All Flash Contents" |
|   | Port              | Bedraad: "Serial Port" |


Stop de ESP-01 in de programmer 
([hier](https://willem.aandewiel.nl/index.php/2018/08/27/eenvoudige-programmer-voor-de-esp-01-esp8266/) 
vind je een post over hoe je van een USB to ESP-01
Adapter zelf eenvoudig een programmer kunt maken) en sluit deze aan op je computer.
Vergeet niet de juiste `Port` te selecteren en druk op het *Compile and Upload icoon*.

![](img/CompileAndUploadIcon.png)



<br>

---
<center  style="font-size: 70%">[ScreenShot DSMR-FSexplorer]</center><br>
![](img/DSMR_FSexplorer.png)
