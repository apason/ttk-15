## ttk-15 ##
ttk-15 on ttk-91 klooni pienillä muutoksilla

# työn alla #
 * kääntäjä ja linkkeri + testaus (testauksen voisi tehdä esim niin että lataa originalin titokoneen
kääntämiä tiedostoja machinen loader moduulilla ja vertaa data-aluetta compilerilla tehtyyn?)
 * testit kaikille konekäskyille
# tulevia #
 * formaatti voi kusta: r1,        =x
 * muistin vapautus!
 * bitshiftit kuntoon
 * ylivuoto yms bitit kuntoon
 * riippuvuuksien lisääminen makefileen jotta headerien päivitys aiheuttaa uudelleenkäännöksen
 * pienin osoitettava muistiyksikkö keskusmuistissa MYTYPE:stä yhteen bittiin
 * n ydintä
 * linkkerin muutos, jotta external labelien kohdalla, konekäskyssä oleva osoite muutetaan
negatiiviseksi int32 tyypiksi, jolloin sitä voidaan verrata symbolitauluun. External label
ensin tunnistetaan merkkibitin avulla, jolloin vain external labelien osoitteet
tulkitaan negatiivisina.

# vasta tehtyjä #

 * speksin muutos kääntäjään, ulkoiset moduuliosoitteet objektitiedostoissa positiivinen (1,2,3,4..), mutta
symbolitaulussa negatiivinen (-1,-2,-3,-4..), koska negatiivisia lukuja ei voi käyttää objekti-
tiedoston koodiosiossa osoiteosan koosta johtuen (16bit)
