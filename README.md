# ttk-15 #
ttk-15 on ttk-91 klooni pienillä muutoksilla

##v0.2 valmiina##
 * popr ja pushr menee rikki jos sp != r6                       1 
 * yksimoduuliset ei tarvitse main labelia                      1 
 * tr2 pois ja cu.c kuntoon static!                             1 
 * parametrit paremmiksi masiinalle                             5, 3 : 4
 * parametrit paremmiksi linkkerille                            5, 3 : 4 
 * virheilmoituksien rivinumerot
 * kääntäjän virheilmoitukset kaikki errorcodes.h:n
 * parametrit paremmiksi kääntäjälle                            5, 3 : 4
 * pienet funktiot makroiksi (mmu ja machine loop)              3, 2 : 2.5
 * headereille uudet paikat (jos a.h tarvitaan vain esim. linkkerissä niin siitterään src/linker)  2
 
## v0.2 työn alla ##
 * import tai include kääntäjä direktiivin toteutus kirjastofunktioille     2,5 : 3.5
 * dokumentointi!!!                                             4
 * pitää kääntyä windowsilla (ainakin cygwinillä)
## backlog ##
 * kaikki mytypeks mikä kannattaa
 * aliakset jumpille ja callille ja exitille
 * jumpit ignoraa moden????
 * floating point unit + konekäskyt    (vaatii yhteistä suunnittelua)       5,6 : 5.5
 * testit floateille                                                        4
 * "wrapper" skripti tms joka kapseloi linkkerin ja kääntäjän (ehkä masiinan?)
 * konekäskyt state registerin tutkimiseen
 * overflow tarkistus tehokkaammaksi xor menetelmällä
 * testit overflow yms flageille!
 * state registerin nollaus?
 * moduuleille uusi rakenne. import ja export taulut!
 * GUI 
 * pienin osoitettava muistiyksikkö keskusmuistissa MYTYPE:stä yhteen bittiin
 * konekäskyt 16 ja 8 bitin käsittelyyn
 * n ydintä

