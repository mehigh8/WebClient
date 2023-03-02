#ifndef _REQUESTS_
#define _REQUESTS_

#include "json.h"
/*
 * Functie care creeaza cererea de tip GET.
 * @host - host-ul
 * @url - adresa la care este trimisa cererea
 * @query_params - parametrii care pot fi pusi la url
 * @authorization - token-ul care demonstreaza accesul la biclioteca
 * @cookies - lista de cookie-uri
 * @cookies_count - numarul de cookie-uri
 * return - cererea
 */
char *compute_get_request(char *host, char *url, char *query_params,
							char *authorization, char **cookies, int cookies_count);

/*
 * Functie care creeaza cererea de tip POST.
 * @host - host-ul
 * @url - adresa la care este trimisa cererea
 * @authorization - token-ul care demonstreaza accesul la biclioteca
 * @obj - json object-ul folosit pentru payload
 * @cookies - lista de cookie-uri
 * @cookies_count - numarul de cookie-uri
 * return - cererea
 */
char *compute_post_request(char *host, char *url, char *authorization,
							json_object* obj, char** cookies, int cookies_count);

/*
 * Functie care creeaza cererea de tip DELETE.
 * @host - host-ul
 * @url - adresa la care este trimisa cererea
 * @authorization - token-ul care demonstreaza accesul la biclioteca
 * return - cererea
 */
char *compute_delete_request(char *host, char *url, char *authorization);

#endif
