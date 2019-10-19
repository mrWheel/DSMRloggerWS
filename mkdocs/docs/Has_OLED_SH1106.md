## #define HAS_OLED_SH1106

Als je een OLED schermpje op <b>J4</b> van de DSMR-logger V4 hebt aangesloten
dan zorgt deze optie ervoor dat het OLED schermpje ook gebruikt wordt door de Firmware.

<div class="admonition note">
<p class="admonition-title">Let op!  Alleen voor DSMR-logger V4</p>
<br>
Met dank aan <i>Dick Spork</i>.
</div>

<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">USE_OLED_SH1106</td><td>Deze define zorgt ervoor dat
het 1.3" OLED schermpje gebruikt wordt om meldingen op te presenteren.
<br>Deze define kan niet in combinatie met <b>HAS_OLED_SSD1306</b>
</td>
</tr>
</table>

---
<center  style="font-size: 70%">[SH1106_Display (foto Dick Spork)]</center><br>
<center>![](img/SH1106_Display.png)</center>
