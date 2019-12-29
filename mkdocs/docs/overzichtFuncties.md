## Overzicht te selecteren functies

Tijdens het compileren van de firmware kun je bepaalde functionaliteit
in- en uit-schakelen door de #defines wél of níet door twee
slashes ("//") vooraf te laten gaan.

In onderstaande tabel kun je zien of een bepaalde functionaliteit beschikbaar is voor
de verschillende versies van de DSMR-logger.

<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th><th align="center">Versie&nbsp;4</th><th align="center">Versie&nbsp;3</th>
</tr><tr>
  <td style="vertical-align:top"><a href="../Is_ESP12/">IS_ESP12</a></td>
  <td>Geeft aan of je de firmware voor een ESP12 of andere (ESP-01) processor compileren
  </td><td align="center">
  JA (moet)
  </td><td align="center">
  NEE
  </td>
</tr><tr>
  <td style="vertical-align:top"><a href="../Use_Update_Server/">USE_UPDATE_SERVER</a></td>
  <td>Met de Update Server kun je vanuit de FSexplorer updates van de firmware installeren
  </td><td align="center">
  JA
  </td><td align="center">
  NEE
  </td>
</tr><tr>
  <td style="vertical-align:top"><a href="../Has_OLED_SSD1306/">HAS_OLED_SSD1306</a></td>
  <td>Functionaliteit om (status) meldingen naar een OLED scherm (0.96" type SSD1306) te sturen
  </td><td align="center">
  JA
  </td><td align="center">
  NEE
  </td>
</tr><tr>
  <td style="vertical-align:top"><a href="../Has_OLED_SH1106/">HAS_OLED_SH1106</a></td>
  <td>Functionaliteit om (status) meldingen naar een OLED scherm (1.3" type SH1106) te sturen
  </td><td align="center">
  JA
  </td><td align="center">
  NEE
  </td>
</tr><tr>
  <td style="vertical-align:top"><a href="../Use_Pre40_Protocol/">USE_PRE40_PROTOCOL</a></td>
  <td>Voor "oude" Slimme Meters
  </td><td align="center">
  JA
  </td><td align="center">
  JA
  </td>
</tr><tr>
  <td style="vertical-align:top"><a href="../Use_NTP_Time/">USE_NTP_TIME</a></td>
  <td>Gebruik de tijd van een NTP server i.p.v. die van de Slimme Meter
  </td><td align="center">
  JA
  </td><td align="center">
  JA
  </td>
</tr><tr>
  <td style="vertical-align:top"><a href="../SM_Has_No_Fase_Info/">SM_HAS_NO_FASE_INFO</a></td>
  <td>Sommige één fase Slimme Meter's geven geen info per fase af
  </td><td align="center">
  JA
  </td><td align="center">
  JA
  </td>
</tr><tr>
  <td style="vertical-align:top"><a href="../Use_MQTT/">USE_MQTT</a></td>
  <td>Deze optie zorgt ervoor dat de functionaliteit voor het versturen van gegevens naar een MQTT broker wordt ingebouwd
  </td><td align="center">
  JA
  </td><td align="center">
  JA
  </td>
</tr><tr>
  <td style="vertical-align:top"><a href="../Use_Mindergas/">USE_MINDERGAS</a></td>
  <td>Deze optie zorgt ervoor dat de functionaliteit voor het versturen van gegevens naar 
  <a href=https://mindergas.nl/ target="_blank">mindergas.nl</a> waar je huidige gasverbruik kunt vergelijken
  met vorig jaar.
  </td><td align="center">
  JA
  </td><td align="center">
  NEE
  </td>
</tr><tr>
  <td style="vertical-align:top"><a href="../Show_Passwrds/">SHOW_PASSWORDS</a></td>
  <td>Of je de gebruikte passwords in het Systeem Info scherm en via telnet wilt tonen
  </td><td align="center">
  JA
  </td><td align="center">
  JA
  </td>
</tr><tr>
  <td style="vertical-align:top"><a href="../Has_No_Meter/">HAS_NO_METER</a></td>
  <td>Als je geen Slimme Meter op de DSMR-logger hebt aangesloten
  </td><td align="center">
  JA
  </td><td align="center">
  JA
  </td>
</tr>
</table>

<hr>
```

/******************** compiler options  ********************************************/
#define IS_ESP12                  // define if it's a 'bare' ESP-12 (no reset/flash functionality on board)
#define USE_UPDATE_SERVER         // define if there is enough memory and updateServer to be used
#define HAS_OLED_SSD1306          // define if an OLED display is present
//  #define HAS_OLED_SH1106           // define if an OLED display is present
//  #define USE_PRE40_PROTOCOL        // define if Slimme Meter is pre DSMR 4.0 (2.2 .. 3.0)
//  #define USE_NTP_TIME              // define to generate Timestamp from NTP (Only Winter Time for now)
//  #define SM_HAS_NO_FASE_INFO       // if your SM does not give fase info use total delevered/returned
#define USE_MQTT                  // define if you want to use MQTT
//  #define USE_MINDERGAS             // define if you want to update mindergas (also add token down below)
//  #define SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
//  #define HAS_NO_METER              // define if No "Slimme Meter" is attached (*TESTING*)
/******************** don't change anything below this comment **********************/

```
<hr>
