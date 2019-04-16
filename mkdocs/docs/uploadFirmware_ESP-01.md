## Firmware naar ESP-01 Flashen

Om de firmware naar de ESP-01 (black Edition) te kunnen flashen moet deze eerst 
voor deze ESP-01 geschikt worden gemaakt.

In de ArduinoIDE moet in het eerste tab-blad de volgende regels
zó worden ingesteld:

```
/******************** compiler options  ********************************************/
// #define IS_ESP12              // define if it's an ESP-12
// #define USE_ARDUINO_OTA       // define if there is enough memory
// #define HAS_OLED_SSD1306      // define if an OLED display is present
// #define HAS_NO_METER          // define if No "Slimme Meter" is attached
/******************** don't change anything below this line ************************/
```

Vervolgens moeten de [`Boards`] settings als volgt worden ingevuld:

![](img/ESP01_BoardSettings.png)

Voor een ESP-01S moeten dezelfde instellingen worden gebruikt, alleen zit bij dit board
de "Builtin Led:" op `GPIO2`!

Stop de ESP-01 in de programmer en sluit deze aan op je computer.
Vergeet niet de juiste `Port` te selecteren en druk op het Compile and Upload icoon.

![](img/CompileAndUploadIcon.png)


