## Integratie DSMR-logger met Mindergas

De data uit de DSMR-logger kan ook automatisch 1x per dag geupload worden naar de website Mindergas.nl. Deze website is in 2011 ontstaan uit de persoonlijke behoefte van David Hei Lei om te meten wat het effect is van woningisolatie op zijn gasverbruik, [lees meer](https://mindergas.nl/about_us). De website maakt het eenvoudig mogelijk om je gasverbruik in te voeren en vervolgens te vergelijken op diverse manieren. 

De website Mindergas.nl maakt het eenvoudig mogelijk om je jaarverbruik te vergelijken met een voorgaande periode (jaren vergelijken). Hierbij zorgt mindergas.nl voor de omrekening op basis van [gewogen graaddagen](https://mindergas.nl/degree_days_calculation/explanation). Door het gebruik van graaddagen krijg je een beter beeld van het daadwerkelijke verbruik, er wordt namelijk rekening gehouden met het feit of sprake is van een milde winter of een strenge winter. Hierdoor wordt het ook mogelijk om door de jaren heen je verbruik te vergelijken, er wordt immers rekening gehouden met de verschillen in temperaturen door de winters heen.

Doordat je je verbruik het hele jaar elke dag opvoert, kan op basis van voorspelling een vrij nauwkeurige jaarverbruik door het jaar heen worden voorspelt. Je kan het verbruik vergelijken met voorgaande jaren. En daarnaast kan je je gasverbruik vergelijken met dat van anderen. Een handige functie als je bijvoorbeeld jouw verbruik wilt vergelijk met je buren, om te ontdekken of energiebezuinigende maatregelen effect hebben.

Bovendien kent Mindergas.nl een integratie met Pricewise kan je snel vergelijk wat jouw energieverbruik bij andere energieleveranciers aan kosten met zich mee zou brengen. Handig als je wilt weten of het zinvol is om van energie leverancier te wisselen.

### #define USE_MINDERGAS
De integratie met mindergas kan je eenvoudig inschakelen door het statement #USE_MINDERGAS in de DSRMLoggerWS.ino.

<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">USE_MINDERGAS</td><td>Door deze define wordt de mindergas integratie mee gecompileerd bij het maken van de firmware.
</td>
</tr>
</table>

### Instellen van Mindergas.nl
Om mindergas in gebruik te nemen moet je allereerst een [account registeren](https://mindergas.nl/users/sign_up) bij de website van Mindergas.nl. Na het aanmaken van een nieuw account kan je een zogenaamde API key aanmaken, ga naar de [API pagina](https://mindergas.nl/member/api). 

Klik daar op de knop *Genereren*, dan zal er een Authenicatietoken worden aangemaakt. Er verschijnt dan een API authenicatietoken, zoiets als dit `Ahah%4JgongF7pwH92uN`. Dit token zal moeten worden ingevoerd in de Instellingen pagina van de DSRM-Logger. 

Via  `FSexplorer -> Edit instellingen -> Settings` kun je het authenicatietoken invullen voor gebruik van mindergas:

![img](img/DSMR-USE_MQTT_Settings.png)
