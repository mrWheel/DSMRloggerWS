# uploaden Firmware naar ESP8266

De ESP01 heeft geen ingebouwde flash logica. Om toch aan de ESP01 duidelijk te maken
dat je er firmware naar toe wilt sturen (flashen) moet je de ESP01 bij het opstarten 
in deze flash mode zetten door een paar GPIO-pinnen Hoog (3v3) of juist Laag (GND)
te maken.

[Hier](ESP01FlashMode.md) staat beschreven hoe je dat kunt doen.

Als de ESP01 eenmaal in flash mode staat kun je de firmware er eenvoudig naartoe sturen door
op deze &nbsp; ![](img/ArduinoCompileAndUpload.png) &nbsp; knop te klikken.


Sluit de ESP01 aan op je computer en zet deze in `flash mode` dmv een ESP programmer 
of FTDI adapter. 

Meer informatie hierover kun je <a href="https://willem.aandewiel.nl/index.php/2018/09/27/aan-de-slag-met-de-esp8266/" target=_"blank">hier</a>
vinden.


Gebruik deze instellingen:

```
   - Board: "Generic ESP8266 Module"
   - Flash mode: "DOUT"
   - Flash size: "1M (128K SPIFFS)"
   - Debug port: "Disabled"
   - Debug Level: "None"
   - IwIP Variant: "v2 Lower Memory"
   - Reset Method: "nodemcu"   // <-- op basis van de programmer die je gebruikt
   - Crystal Frequency: "26 MHz"
   - Flash Frequency: "40MHz"
   - CPU Frequency: "80 MHz"
   - Buildin Led: "1"  // GPIO01 - Pin 2
   - Upload Speed: "115200"
   - Erase Flash: "Only Sketch"
   - Port: "Serial Port > COMx" // <- de poort waar je programmer op zit
```

