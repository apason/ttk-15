# ttk-15 #
ttk-15 on ttk-91 klooni pienillä muutoksilla

##v0.3 valmiina##
 * jumpit ignoraa moden???? not cool
 * aliakset jumpille ja callille ja exitille 
 * moduuleille uusi rakenne. import ja export taulut!
 * floating point unit + konekäskyt    (vaatii yhteistä suunnittelua)       5,6 : 5.5
 * float hyppykäskyissä ei saa olla floattia osoitteena
 * valgrindin memleakit
 * 16b floatit kääntäjään
 * testit floateille                                                        4


## backlog ##
 * testaa titokoneessa nollalla jakaminen ym. keskeytykset
 * "wrapper" skripti tms joka kapseloi linkkerin ja kääntäjän (ehkä masiinan?)
 * konekäskyt state registerin tutkimiseen
 * breakpointit compileriin ja debuggeriin
 * keskeytykset!
 * overflow tarkistus tehokkaammaksi xor menetelmällä
 * testit overflow yms flageille!
 * GUI 
 * pienin osoitettava muistiyksikkö keskusmuistissa MYTYPE:stä yhteen tavuun
 * konekäskyt 16 ja 8 bitin käsittelyyn?

##known issues##

machine brokes up if one tries string input to fread
