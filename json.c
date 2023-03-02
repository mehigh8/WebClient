// Rosu Mihai Cosmin 323CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"

void populate_pair(pair_t* pair, char *id, int id_len, char *value, int value_len, char type)
{
	// Aloc id-ul perechii si setez id-ul si lungimea id-ului.
	pair->id = calloc(id_len + 1, sizeof(char));
	pair->id_len = id_len;
	memcpy(pair->id, id, id_len);
	// Aloc valoarea perechii si setez valoarea si lungimea valorii.
	pair->value = calloc(value_len + 1, sizeof(char));
	pair->value_len = value_len;
	memcpy(pair->value, value, value_len);
	// Setez tipul valorii.
	pair->type = type;
}

json_object* auth_json(char *user, char *pass)
{
	// Aloc memorie pentru un json object.
	json_object *obj = calloc(1, sizeof(json_object));
	// Setez dimensiunea json object-ului, si aloc memorie pentru elemente.
	obj->size = 2;
	obj->elems = calloc(obj->size, sizeof(pair_t));
	// Populez fiecare element (pereche) cu valorile corespunzatoare.
	populate_pair(&(obj->elems[0]), "username", strlen("username"), user, strlen(user), 0);
	populate_pair(&(obj->elems[1]), "password", strlen("password"), pass, strlen(pass), 0);

	return obj;
}

int json_len(json_object *obj)
{
	// Incep prin a initializa lungimea cu 2 (acoladele '{' si '}') la care adaug numarul de
	// elemente ale json-ului (pentru fiecare ',' aflata intre elemente) si scad 1 (ultimul)
	// element nu este urmat de ','.
	int len = 2 + obj->size - 1;
	for (int i = 0; i < obj->size; i++) {
		// Pentru fiecare element adaug la lungimea finala lungimea id-ului + 2 (ghilimelele de
		// inainte si de dupa) + 1 (':' aflat intre id si valoare) + lungimea valorii + 0 sau 2
		// (daca valoarea este numar nu mai are ghilimele, deci 0, altfel 2 pentru cele doua
		// ghilimele).
		len += obj->elems[i].id_len + 2 + 1 + obj->elems[i].value_len +
			   (obj->elems[i].type == 0 ? 2 : 0);
	}

	return len;
}

char* json_string(json_object *obj)
{
	// Aloc memorie pentru string-ul in care va fi scris json-ul.
	char *json_str = calloc(json_len(obj) + 1, sizeof(char));
	// Incep prin a pune '{' la inceput.
	strcpy(json_str, "{");
	// Apoi afisez fiecare element.
	for (int i = 0; i < obj->size; i++) {
		// Deschid ghilimelele.
		strcat(json_str, "\"");
		// Scriu id-ul elementului.
		strcat(json_str, obj->elems[i].id);
		// Inchid ghilimelele si scriu ':'.
		strcat(json_str, "\":");

		// Daca valoarea este de tip string, deschid iar ghilimelele.
		if (obj->elems[i].type == 0)
			strcat(json_str, "\"");
		// Scriu valoarea.
		strcat(json_str, obj->elems[i].value);
		// Daca este cazul inchid ghilimelele.
		if (obj->elems[i].type == 0)
			strcat(json_str, "\"");

		// Daca elementul curent nu este ultimul, scriu ','.
		if (i != obj->size - 1)
			strcat(json_str, ",");
	}
	// Inchid acoladele.
	json_str[strlen(json_str)] = '}';

	return json_str;
}

json_object* book_json(char *title, char *author, char *genre, char *publisher, char *page_count)
{
	// Aloc memorie pentru un json object.
	json_object *obj = calloc(1, sizeof(json_object));
	// Setez dimensiunea json object-ului, si aloc memorie pentru elemente.
	obj->size = 5;
	obj->elems = calloc(obj->size, sizeof(pair_t));
	// Populez fiecare element (pereche) cu valorile corespunzatoare.
	populate_pair(&(obj->elems[0]), "title", strlen("title"), title, strlen(title), 0);
	populate_pair(&(obj->elems[1]), "author", strlen("author"), author, strlen(author), 0);
	populate_pair(&(obj->elems[2]), "genre", strlen("genre"), genre, strlen(genre), 0);
	populate_pair(&(obj->elems[3]), "page_count", strlen("page_count"), page_count, strlen(page_count), 1);
	populate_pair(&(obj->elems[4]), "publisher", strlen("publisher"), publisher, strlen(publisher), 0);

	return obj;
}

void free_json(json_object *obj)
{
	// Pentru fiecare element eliberez memoria id-ului si a valorii.
	for (int i = 0; i < obj->size; i++) {
		free(obj->elems[i].id);
		free(obj->elems[i].value);
	}
	// Eliberez memoria vectorului de elemente si a json-ului.
	free(obj->elems);
	free(obj);
}
