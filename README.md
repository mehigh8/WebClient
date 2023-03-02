# Rosu Mihai Cosmin 323CA

Mentiuni:
-Am ales sa folosesc o implementare proprie pentru json object-uri, aflata in
json.h/json.c, deoarece biblioteca parson avea foarte multe functionalitati de
care nu aveam nevoie si mi s-a parut mai simplu sa imi creez propria
implementare decat sa invat cum functioneaza parson-ul.
-Mereu am folosit pe post de mesaje de eroare, erorile din raspunsurile de la
server, cu exceptia cazului in care utilizatorul, deja logat, dadea din nou
login, caz in care server-ul intoarce doar No Content, fara o anumita eroare.
-Am folosit scheletul de laborator de la laboratorul 10.

client.c:
-Clientul retine cookie-ul de login si token-ul de authorization si citeste,
intr-un while loop, comenzile de la tastatura. De asemenea, redeschide la
fiecare iteratie conexiunea cu server-ul, deoarece, daca nu face asta, dupa
prima comanda, celelalte nu se mai trimit.

Comenzi:
-register:
 -Clientul ofera prompt pentru username si parola, apoi trimite o cerere POST
 catre server.
-login:
 -Clientul mai intai verifica daca utilizatorul este deja logat, caz in care il
 anunta si trece la urmatoarea comanda. Altfel, ofera prompt pentru username si
 parola, si trimite o cerere POST catre server. Dupa ce receptioneaza raspunsul
 server-ului, il parseaza si obtine cookie-ul de login.
-enter_library:
 -Clientul trimite o cerere de tip GET catre server, cu cookie-ul de login
 detinut la momentul respectiv. Daca acesta era null, atunci server-ul va
 intoarce eroare, altfel, utilizatorul a intrat cu succes in biblioteca. In
 acest caz, clientul parseaza raspunsul primit de la server si obtine token-ul
 de authorization.
-get_books:
 -Clientul trimite o cerere de tip GET catre server, cu token-ul detinut la
 momentul respectiv. Daca este null, server-ul va intoarce o eroare, altfel
 clientul va parsa raspunsul server-ului si va afisa lista de carti (in format
 specific json-urilor).
-get_book:
 -Clientul ofera prompt pentru id, si se asigura ca valoarea introdusa este un
 numar. Apoi, trimite o cerere de tip GET catre server si, in functie de token,
 raspunsul server-ului poate fi o eroare (sau daca id-ul cautat nu exista), sau
 poate contine detaliile cartii cautate, care sunt afisate in format specific
 json-urilor.
-add_book:
 -Clientul ofera prompt pentru titlu, autor, gen, editura si numar de pagini si
 verica daca numarul de pagini este un numar. Apoi trimite o cerere de tip POST
 catre server. Acesta poate intoarce eroare daca token-ul este invalid.
-delete_book:
 -Clientul ofera prompt pentru id, si se asigura ca valoarea introdusa este un
 numar. Apoi, trimite o cerere de tip DELETE catre server. Daca token-ul sau
 id-ul sunt incorecte, acesta va intoarce o eroare.
-logout:
 -Clientul trimite o cerere de tip GET catre server. Acesta va intoarce eroare
 daca utilizatorul nu era logat.

