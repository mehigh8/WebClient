#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "json.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char *authorization, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Scriu tipul cererii, url-ul, parametrii cererii, daca e cazul, si protocolul.
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Scriu host-ul.
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Scriu header-ul pentru authorization, daca este cazul.
    if (authorization != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", authorization);
        compute_message(message, line);
    }
    // Daca exista cookie-uri, le scriu in cerere.
    if (cookies != NULL) {
       memset(line, 0, LINELEN);
       strcpy(line, "Cookie: ");
       for (int i = 0; i < cookies_count - 1; i++) {
            strcat(line, cookies[i]);
            strcat(line, "; ");
       }
       strcat(line, cookies[cookies_count - 1]);
       compute_message(message, line);
    }
    // Adaug o linie goala la finalul cererii, si intorc cererea.
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char *authorization,
                            json_object* obj, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Scriu tipul cererii, url-ul si protocolul.
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Scriu host-ul.
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    // Scriu header-ele de authorization, daca e cazul, de content type si de
    // content length.
    if (authorization != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", authorization);
        compute_message(message, line);
    }

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", "application/json");
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %d", json_len(obj));
    compute_message(message, line);

    // Daca exista cookie-uri, le scriu in cerere.
    if (cookies != NULL) {
       memset(line, 0, LINELEN);
       strcpy(line, "Cookie: ");
       for (int i = 0; i < cookies_count - 1; i++) {
            strcat(line, cookies[i]);
            strcat(line, "; ");
       }
       strcat(line, cookies[cookies_count - 1]);
       compute_message(message, line);
    }
    // Adaug o linie goala la finalul cererii.
    compute_message(message, "");
    // Scriu payload-ul.
    char *json_str = json_string(obj);
    strcat(message, json_str);

    free(line);
    free(json_str);
    return message;
}

char *compute_delete_request(char *host, char *url, char *authorization)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    // Scriu tipul cererii, url-ul si protocolul.
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);
    // Scriu host-ul.
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Scriu header-ul pentru authorization, daca este cazul.
    if (authorization != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", authorization);
        compute_message(message, line);
    }
    // Adaug o linie goala la finalul cererii.
    compute_message(message, "");

    free(line);
    return message;
}
