# Github pages onderhouden

Voer, in een command window, de volgende commando's in:

```
cd <waar je de boel wilt hebben>
git clone https://github.com/mrWheel/DSMRloggerWS.git

cd DSMRloggerHTTP

git checkout gh-pages

cd mkdocs
```

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

cd <waar je de boel neergezet hebt>/DSMRloggerWS
git add -A
git commit -am "reden van deze commit"
git push

mkdocs gh-deploy

````

Klaar!
