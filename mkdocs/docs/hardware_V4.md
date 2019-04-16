## DSMR-logger Versie 4 Bouwen

Het bouwen van de DSMR-logger Versie 4 is vrij *Recht Toe, Recht Aan*. 

### Bill Of Material
Je hebt de volgende componenten nodig:

|Reference    |Value|Footprint|
|:------------|:----|:--------|
|C1  | 1uF    | Elco Radial D5.0mm P2.00mm  (max. 7mm height) |
|C2  | 2.2uF  | Elco Radial D5.0mm P2.00mm|
|C3  | 470-1000uF | Elco Radial D8.0mm P5.00mm|
|C4  | 100nF  | Cap. Disc D5.0mm W2.5mm P5.00mm|
|C5  | 100nF  | Cap. Disc D5.0mm W2.5mm P5.00mm|
|R5  | 10R    | DIN0207 L6.3mm D2.5mm P7.62mm |
|R1  | 220R   | DIN0207 L6.3mm D2.5mm P7.62mm |
|R4  | 220R   | DIN0207 L6.3mm D2.5mm P7.62mm |
|R7  | 1k     | DIN0207 L6.3mm D2.5mm P7.62mm |
|R14 | 4k7    | DIN0207 L6.3mm D2.5mm P7.62mm |
|R15 | 4k7    | DIN0207 L6.3mm D2.5mm P7.62mm |
|R6  | 4k7    | DIN0207 L6.3mm D2.5mm P7.62mm |
|R10 | 10k    | DIN0207 L6.3mm D2.5mm P7.62mm |
|R11 | 10k    | DIN0207 L6.3mm D2.5mm P7.62mm |
|R2  | 10k    | DIN0207 L6.3mm D2.5mm P7.62mm |
|R3  | 10k    | DIN0207 L6.3mm D2.5mm P7.62mm |
|R8  | 10k    | DIN0207 L6.3mm D2.5mm P7.62mm |
|R9  | 10k    |DIN0207 L6.3mm D2.5mm P7.62mm |
|    |        |
|D1  | PWR LED|LED D3.0mm THT TO-92 Inline|
|    |        |
|U1  | LM2937xMP | SMD SOT-223-3 TabPin2 |
|U2  | 4N35   | DIP-6 W7.62mm|
|U3  | ESP-12 | ESP-12E/F|
|    |    |
|Q1  | BC547  | TO-92 Inline|
|    |    |
|J1  | RJ12 female | Amphenol 54601 |
|J4  | I2C conn | PinSocket 1x04 P2.54mm Vertical |
|J5  | Programmer | PinHeader 1x05 P2.54mm Vertical |
|    |    |
|SW1 | RESET  | THT SW PUSH 6mm H7.3mm|
|SW2 | FLASH  | THT SW PUSH 6mm H20mm-25mm (long shaft)|
| |  | |
|Optioneel|||
|R12 | 12k    | DIN0207 L6.3mm D2.5mm P7.62mm  (E24 – 5% or better) |
|R13 | 3k     | DIN0207 L6.3mm D2.5mm P7.62mm  (E24 – 5% or better) |
|J9  | ADC 0-5v | PinHeader 1x02 P2.54mm Vertical|
|| oled display |[11171](https://opencircuit.nl/Product/11171/OLED-display-blauw-128-x-64-I2C) |

### Spanningsdeler ADC
R12 en R13 zijn optioneel en afhankelijk van welke spanning je wilt kunnen meten.

| R12 | R14 | Maximale spanning op J9 |
|:---:|:---:|:------------------------|
| 4k7 | 4k7 | 0 - 2 volt              |
| 8k2 | 3k3 | 0 - 3.3 volt            |
| 12k | 3k  | 0 - 5 volt              |
| 10k | 1k2 | 0 - 10 volt             |

### Bouwen
Zoals altijd is het het eenvoudigst om eerst de laagste componenten op
de printplaat te solderen en dan de steeds hogere componenten.

De laagste componenten zijn de SMD voltage regulator (U1, LM2937-3v3) en
de processor (U3, ESP-12).

Beide zijn zgn. SMD componenten die eigenlijk niet bedoeld zijn om *met de hand*
te solderen, maar het is toch vrij eenvoudig en goed te doen.

Begin met het aanbrengen van een druppeltje soldeer op één van de pads van de LM2937-3v3 (U1):

<center>![](img/solder_SMD-a.png)</center>

Plaats nu U1 op zijn plek en verwarm de pad plus U1  met de soldeer druppel nogmaals
met de soldeerbout ..

<center>![](img/solder_SMD-b.png)</center>

Als het soldeer is uitgehard zit U1 op zijn plaats
en kun je deze aan de overige pads solderen.

<center>![](img/solder_SMD-c.png)</center>

Voor U3 (ESP-12) geldt hetzelfde princiepe ..

<center>![](img/solder_ESP12-a.png)
![](img/solder_ESP12-b.png)</center>

.. alleen zijn er véél meer pads te solderen.

<center>![](img/solder_ESP12-c.png)</center>

Hieronder staat een video waarin je het hele bouw-proces kunt volgen:

<center><iframe width="560" height="315" src="https://www.youtube.com/embed/zA5apEOGtU4" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe></center>

<br>

---
<center style="font-size: 70%">[DSMR-logger Versie 4]</center><br>
![](img/DSMRlogger_v4_3D.png)
