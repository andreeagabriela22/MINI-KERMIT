# MINI-KERMIT
Protocolul KERMIT este un protocol ce face parte din clasa protocoalelor ARQ (Automatic Repeat Request),
in care un pachet eronat sau neconfirmat este automat retransmis. Datele utile sunt impachetate, fiind
inconjurate cu unele campuri de control. In timpul transmiterii unui pachet nu se face controlul fluxului.
Fiecare pachet trebuie confirmat.

Avantajul protocolului KERMIT fata de altele asemanatoare este simplitatea de implementare, precum si:
• Negocierea anumitor parametrii de comunicatie intre emitator si receptor prin intermediul primelor
pachete
• Posibilitatea de transfera mai multe fisiere in cadrul unei sesiuni
• Transmiterea numelor fisierelor
• Posibilitatea ca pachetele sa aiba tipuri si lungimi variabile
Transferul unui fisier decurge ca la orice protocol ARQ. Receptorul primeste pachetul, si dupa ce verifica
numarul sau de secventa fata de precedentul pachet, calculeaza o suma de control locala pentru partea de
date. Daca suma de control calculata coincide cu cea sosita se emite o confirmare pozitiva ACK (caracter
sau pachet); in caz contrar se emite confirmare negativa NAK. In final se transmite un pachet de tipul EOF.
Daca mai exista fisiere de transmis, se transmite header-ul urmatorului, iar in final un pachet de tipul EOT.
Structura pachetului KERMIT in forma redusa, numit MINI-KERMIT, precum si tipurile de pachete vor fi
detaliate in sectiunea urmatoare.
