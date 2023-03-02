// Rosu Mihai Cosmin 323CA
#ifndef JSON_H_
#define JSON_H_

// Pereche de tip (id, valoare)
typedef struct {
	char *id; // id-ul perechii
	int id_len; // lungimea id-ului
	char *value; // valoarea perechii
	int value_len; // lungimea valorii
	char type; // tipul valorii: 0 = string, 1 = numar
} pair_t;

// Structura folosita pentru a reprezenta un json object
typedef struct {
	int size; // Numarul de elemente din json
	pair_t *elems; // Vectorul de elemente (perechi)
} json_object;

/*
 * Functie care seteaza campurile unei perechi.
 * @pair - perechea ce trebuie setata
 * @id - id-ul perechii
 * @id_len - lungimea id-ului
 * @value - valoarea perechii
 * @value_len - lungimea valorii
 * @type - tipul valorii
 */
void populate_pair(pair_t* pair, char *id, int id_len, char *value, int value_len, char type);

/*
 * Functie care creeaza json-ul folosit pentru comenzile register si login.
 * @user - username-ul contului
 * @pass - parola contului
 * return - json-ul
 */
json_object* auth_json(char *user, char *pass);

/*
 * Functie care intoarce lungimea unui json, afisat in mod compact.
 * @obj - json object-ul
 * return - lungimea
 */
int json_len(json_object *obj);

/*
 * Functie care scrie continutul unui json object intr-un string.
 * @obj - json object-ul
 * return - string-ul care contine afisarea json-ului
 */
char* json_string(json_object *obj);

/*
 * Functie care creeaza json-ul folosit pentru comanda add_book.
 * @title - titlul cartii
 * @author - autorul cartii
 * @genre - genul cartii
 * @publisher - editura cartii
 * @page_count - numarul de pagini
 * return - json-ul
 */
json_object* book_json(char *title, char *author, char *genre, char *publisher, char *page_count);

/*
 * Functie care elibereaza memoria folosita de un json object.
 * @obj - json object-ul
 */
void free_json(json_object *obj);

#endif  /* JSON_H_ */