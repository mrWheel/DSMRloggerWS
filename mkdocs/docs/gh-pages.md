# Github pages onderhouden

Als je wil bijdragen aan een project in github moet je dat project eerst 'clonen'.

Ik vond [hier](https://kbroman.org/github_tutorial/pages/fork.html) een aardige post
over de stappen die je dan moet nemen.

In de basis komt het erop neer dat je een kopie (fork) van deze repository maakt en 
dan met dit kopie aan de gang gaat. Uiteindelijk ben je klaar met je aanpassingen en
kun je een `merge request` naar mij sturen. Het onderstaande blijft dan geldig maar 
gebeurd op je ge-forkte repository.

Voer, in een command window, de volgende commando's in:

````
cd <waar je de boel wilt hebben>
git clone https://github.com/<jouw-repository-username>/DSMRlogger2HTTP.git

cd DSMRlogger2HTTP

cd mkdocs
````

In de `mkdocs` map zit een map `docs` met daarin alle `.md` bestanden.
Pas deze naar hartelust aan.

Je kunt vervolgens lokaal een server starten met het commando:
````
mkdocs serve
````

Ga vervolgens met een browser naar `http://127.0.0.1:8000` om
je werk te bekijken.

Als je tevreden bent dan moet je de aangepaste sources naar github 
pushen:
````
mkdocs build

git add mkdocs/<1e file dat je veranderd hebt>
git add mkdocs/<2e file dat je veranderd hebt>
git commit -m "reden van deze commit"
git push 
````

Ga vervolgens naar github en naar *jouw* versie van **DSMRlogger2HTTP** en
klik op de `pull Request` knop boven in het scherm en klik vervolgens op
de groene `Create pull request` knop, voer een duidelijke beschrijving in
van wat je hebt aangepast en klik nogmaals op de groene `Create pull request` 
knop.

Ik moet vervolgens het volgende doen:

* Ik ga naar mijn versie van de repository
* Klik vervolgens op de `Pull requests` knop
* Klik op een request
* Klik op de omschrijving om de aanpassingen te zien
* Geef aan dat er nog andere dingen moeten worden aangepast
* Als de aanpassingen niet goed zijn klik op de `Close` knop
* Klik anders op de `Merge pull request` knop

Klaar!
