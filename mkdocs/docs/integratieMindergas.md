## Integratie DSMR-logger met Mindergas

De data uit de DSMR-logger kan 1x per dag automatisch geupload worden naar de website Mindergas.nl. 
Deze website is in 2011 ontstaan uit de persoonlijke behoefte van David Hei Lei om te meten wat 
het effect is van woningisolatie op zijn gasverbruik
<a href="https://mindergas.nl/about_us" target="_blank">(lees hier meer)</a>.
De website maakt het eenvoudig mogelijk om je gasverbruik in te voeren en vervolgens te 
vergelijken op diverse manieren. 

De website Mindergas.nl maakt het eenvoudig mogelijk om je jaarverbruik te vergelijken 
met een voorgaande periode (jaren vergelijken). Hierbij zorgt mindergas.nl voor de 
omrekening op basis van 
<a href="https://mindergas.nl/degree_days_calculation/explanation" target="_blank">gewogen graaddagen</a>.
Door het gebruik van graaddagen krijg je een beter beeld van het daadwerkelijke verbruik, 
er wordt namelijk rekening gehouden met het feit of sprake is van een milde winter of een 
strenge winter. Hierdoor wordt het ook mogelijk om door de jaren heen je verbruik te 
vergelijken, er wordt immers rekening gehouden met de verschillen in temperaturen door 
de winters heen.

Doordat je je verbruik het hele jaar elke dag opvoert, kan op basis van voorspelling een 
vrij nauwkeurige jaarverbruik door het jaar heen worden voorspelt. Je kunt het verbruik 
vergelijken met voorgaande jaren. En daarnaast kan je je gasverbruik vergelijken met 
dat van anderen. Een handige functie als je bijvoorbeeld jouw verbruik wilt vergelijk met 
je buren, om te ontdekken of energiebezuinigende maatregelen effect hebben.

Bovendien kent Mindergas.nl een integratie met Pricewise waardoor je snel kunt vergelijken
wat jouw energieverbruik bij andere energieleveranciers aan kosten met zich mee zou brengen. 
Handig als je wilt weten of het zinvol is om van energie leverancier te wisselen.

### #define USE_MINDERGAS

<table>
<tr>
<th align="left">#define</th><th align="left">Functie</th>
</tr><tr>
<td style="vertical-align:top">USE_MINDERGAS</td>
<td>Door deze define wordt de mindergas integratie mee gecompileerd 
bij het maken van de firmware. Kijk ook <a href="../Use_Mindergas/">hier</a>
</td>
</tr>
</table>

### Instellen van Mindergas.nl
Om mindergas in gebruik te nemen moet je allereerst een 
<a href="https://mindergas.nl/users/sign_up" target="_blank">account aanmaken</a>
bij de website van Mindergas.nl. Na het aanmaken van een nieuw account kan je 
<a href="https://mindergas.nl/member/api" target="_blank">hier</a> een zogenaamde
`API key` aanmaken.

Klik daar op de knop *Genereren*, dan zal er een Authenicatietoken worden aangemaakt. 
Er verschijnt dan een API authenicatietoken, zoiets als dit `Ahah%4JgongF7pwH92uN`. 
Dit token moeten je invoeren bij de Instellingen pagina van de DSRM-Logger. 

Via  `FSexplorer -> Edit instellingen -> Settings` kun je het authenicatietoken 
invullen voor gebruik van mindergas:

<center>![img](img/DSMR-USE_MINDERGAS_Settings.png)</center>

