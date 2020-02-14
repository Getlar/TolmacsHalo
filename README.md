# TolmacsHalo
Számitógépes hálózatok - Projektmunka (Kétszemélyes Tolmács Program)  

# Kétszemélyes tolmács program  

A feladat rövid leírása:  

A feladat megvalósítása egy szerver és egy
kliens program írásából áll. Az eredményként bemutatott programok
segítségével lehetővé kell tenni, hogy két ember konferenciabeszélgetést
tudjon tartani, mindenki a saját nyelvén.  

Szabályok: 

A konferenciabeszélgetés során a résztvevők egymás után
szólalhatnak fel maximum 1023 karakter terjedelemben. A felszólalás után
a nem felszólaló résztvevőnek lehetősége van 255 karakterben röviden
reagálni a leírtakra. ezután az imént felszólalt utáni résztvevőé a jog, hogy
felszólaljon és így tovább. Miután midkét résztvevő felszólalt és reagált,
szavzás következi. Ha mindketten elfogadják a javaslatot, akkor vége a
beszélgetésnek, egyébként újra kell kezdeni.
A program tolmácsolási funkciója abból áll, hogy belépéskor a résztvevők
megadhatják, hogy az alábbi két nyelv közül melyiket beszélik: magyar és
MAGYAR. Mindkét nyelv szerkezete és szabályrendszere azonos,
szókészletük pedig csak abban különbözik, hogy a magyar nyelv beszélői
csupa kisbetűs szavakat, a MAGYAR nyelv beszélői csupa nagybetűs
szavakat használnak. A programnak biztosítania kell, hogy minden
résztvevő a saját nyelvén írhasson üzenetet, illetve a másik üzenetét a
saját nyelvén olvashassa.  

A programok működése:  

Indítás után a két résztvevő jelentkezésére
vár. A résztvevők jelentkezési sorrendje egyúttal a felszólalási sorrend is
lesz. A szerver feladata, hogy a fentiekben megadott szabályok szerint
lehetőséget adjon a klienseknek az üzenetküldésre, fogadja azokat és a
megfelelő nyelvre fordítva továbbítsa őket a többi kliens felé. A kliensek
szavazáskor az „igen”, „nem” és a „tartozkodom” szavakat küldhetik el.
Érvénytelen szavazat leadása tartózkodásnak minősül.
A játékmenettel kapcsolatos minimális elvárások: A feladat
megoldása során a szervernek le kell tudnia fordítani az angol ábécé
betűiből álló szavakat mindkét nyelvre (feltételezhetjük, hogy a kliensek
ezen karaktereken kívülieket nem használnak). Az üzenetek hosszát kliens
oldalon korlátozni kell a szabályok szerint. Sikeres szavazás után minden
program fejeződjön be.
