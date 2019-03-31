# Github pages onderhouden

Voer, in een command window, de volgende commando's in:

````
cd <waar je de boel wilt hebben>
git clone https://github.com/mrWheel/DSMRloggerHTTP.git

cd DSMRloggerHTTP

git checkout gh-pages

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
mkdocs gh-deploy

git add mkdocs
git commit -m "reden van deze commit"
git push -u origin gh-pages
````

Klaar!
