# ttk-15 #
ttk-15 on ttk-91 klooni pienillä muutoksilla

##v0.4 valmiina##
  * objekti- ja ajomoduuleille uusi rakenne.
    + ajomoduuliin headeri
    + debug moodissa compileriin kaikkien labelien paikat
    + debug moodissa linkkeri lisää data alueiden alut ja loput ajomoduulin headeriin
    + linkkeri päivittää moduulivakiot usage tauluun
    + neljäs sana objektimoduulin headeriin
 * "wrapper" skripti tms joka kapseloi linkkerin ja kääntäjän (ehkä masiinan?)

##v0.4 backlog##
 * breakpointit compileriin ja debuggeriin.
 * GUI 

## backlog ##
 * testaa titokoneessa nollalla jakaminen ym. keskeytykset
 * konekäskyt state registerin tutkimiseen
 * keskeytykset!
 * overflow tarkistus tehokkaammaksi xor menetelmällä
 * testit overflow yms flageille!
 * pienin osoitettava muistiyksikkö keskusmuistissa MYTYPE:stä yhteen tavuun
 * konekäskyt 16 ja 8 bitin käsittelyyn?
 * Windows compatibility
 * PHP + cli web interface

##known issues##
 * linkkeri ei toimi moduuleilla, joissa on pelkkä data-alue
 * masiinan ncurses ei suljeta oikein virhetilanteissa => terminaali hyytyy
