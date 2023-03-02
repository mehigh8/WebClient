// Rosu Mihai Cosmin 323CA
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "helpers.h"
#include "requests.h"
#include "json.h"
#define BUFFLEN 100
#define SERVADDR "34.241.4.235"
#define PORT 8080
#define COOKIE_LEN 500
#define TOKEN_LEN 500

/*
 * Functie care ofera feedback clientului.
 * @response - raspunsul primit de la server
 * @correct_code - codul asteptat
 * @correct_msg - mesajul afisat daca a fost primit codul asteptat
 * return - codul din raspunsul server-ului
 */
int give_feedback(char *response, int correct_code, char *correct_msg)
{
    // Retin linia cu eroarea.
    char *error_line = strstr(response, "{\"error");
    // Preiau prima linie din raspuns.
    char *line = strtok(response, "\r\n");
    // Preiau codul intors de server.
    char *part = strtok(line, " ");
    part = strtok(NULL, " ");
    int code = atoi(part);
    // Setez variabila part sa pointeze la mesajul aflat dupa cod.
    part = part + strlen(part) + 1;
    // Afisez codul si mesajul.
    printf("%d %s\n", code, part);
    // Daca codul este diferit de cel asteptat, afisez eroarea.
    if (code != correct_code) {
        char *error = strtok(error_line, ":");
        error = strtok(NULL, ":");
        // Preiau eroarea si o afisez.
        char *error_msg = strtok(error + 1, "\"");
        printf("%s\n", error_msg);
    } else {
        // Altfel, afisez un feedback corespunzator, daca este cazul.
        if (correct_msg != NULL)
            printf("%s\n", correct_msg);
    }
    return code;
}

/*
 * Functie care se ocupa de comanda register.
 * @socket - socket-ul pe care este conexiunea cu server-ul
 */
void register_cmd(int socket)
{
	char user[BUFFLEN], pass[BUFFLEN];
    // Citesc de la tastatura username-ul si parola contului, oferind un prompt.
	printf("username=");
	fgets(user, BUFFLEN, stdin);
    user[strlen(user) - 1] = '\0';

	printf("password=");
	fgets(pass, BUFFLEN, stdin);
    pass[strlen(pass) - 1] = '\0';

    // Creez json object-ul folosit pentru payload in cerere.
	json_object *auth = auth_json(user, pass);

    // Creez cererea.
    char *message = compute_post_request(SERVADDR, "/api/v1/tema/auth/register",
                                         NULL, auth, NULL, 0);
    // Trimit cererea catre server, si apoi ii eliberez memoria.
    send_to_server(socket, message);
    free(message);

    // Receptionez raspunsul server-ului, si afisez feedback corespuncator.
    char *response = receive_from_server(socket);
    give_feedback(response, 201, "Account was created successfully!");
    // Eliberez memoria raspunsului si a json-ului.
    free(response);
    free_json(auth);
}

/*
 * Functie care se ocupa de comanda login.
 * @socket - socket-ul pe care este conexiunea cu server-ul
 * @old_cookie - vechiul cookie de login, folosit pentru a verifica daca
                 utilizatorul nu este deja logat.
 * return - cookie-ul de login
 */
char *login_cmd(int socket, char *old_cookie)
{
    // Daca utilizatorul este deja logat, il anunt si intorc cookie-ul vechi.
    if (old_cookie != NULL) {
        printf("You are already logged in!\n");
        return old_cookie;
    }

    // Altfel, aloc memorie pentru cookie-ul de login si citesc username-ul si parola
    // de la tastatura, oferind un prompt.
    char *login_cookie = calloc(COOKIE_LEN, sizeof(char));
    char user[BUFFLEN], pass[BUFFLEN];

    printf("username=");
    fgets(user, BUFFLEN, stdin);
    user[strlen(user) - 1] = '\0';

    printf("password=");
    fgets(pass, BUFFLEN, stdin);
    pass[strlen(pass) - 1] = '\0';

    // Creez json object-ul pentru payload-ul cererii.
    json_object *auth = auth_json(user, pass);

    // Creez cererea si o trimit server-ului.
    char *message = compute_post_request(SERVADDR, "/api/v1/tema/auth/login", NULL, auth, NULL, 0);
    send_to_server(socket, message);
    free(message);

    // Receptionez raspunsul server-ului.
    char *response = receive_from_server(socket);
    // Retin linia cu cookie-ul (daca exista), deoarece functia give_feedback foloseste strtok-uri,
    // care introduc newline-uri, ceea ce ar impiedica functia strstr din a gasi linia.
    char *cookie_line = strstr(response, "Set-Cookie:");
    // Afisez feedback corespunzator.
    int err_code = give_feedback(response, 200, "Login successful!");

    // Daca, codul gasit in raspuns nu este cel cautat (200), inseamna ca a existat o eroare, si ca
    // nu a fost intors niciun cookie de login.
    if (err_code != 200) {
        // Eliberez memoria si intorc null.
        free(login_cookie);
        free_json(auth);
        free(response);
        return NULL;
    }

    // Altfel, preiau cookie-ul si il copiez in variabila alocata.
    char *cookie = strtok(cookie_line, " ");
    cookie = strtok(NULL, " ");
    char *aux = strtok(cookie, ";");
    memcpy(login_cookie, aux, strlen(aux));

    // Eliberez memoria si intorc cookie-ul.
    free(response);
    free_json(auth);
    return login_cookie;
}
/*
 * Functie care se ocupa de comanda enter_library.
 * @socket - socket-ul pe care este conexiunea cu server-ul
 * @login_cookie - cookie-ul de login
 * return - token-ul de authorization
 */
char *enter_library_cmd(int socket, char *login_cookie)
{
    // Aloc memorie pentru token.
    char *token = calloc(TOKEN_LEN, sizeof(char));
    // Daca este cazul aloc memorie pentru cookie-uri (cel mult 1, cel de login).
    char **cookies = NULL;
    int cookies_len = 0;
    if (login_cookie != NULL) {
        cookies = calloc(1, sizeof(char *));
        cookies[0] = login_cookie;
        cookies_len = 1;
    }

    // Creez cererea si o trimit catre server.
    char *message = compute_get_request(SERVADDR, "/api/v1/tema/library/access",
                                        NULL, NULL, cookies, cookies_len);
    send_to_server(socket, message);
    // Eliberez memoria cererii, si a cookie-urilor, daca e cazul.
    free(message);
    if (cookies != NULL)
        free(cookies);

    // Receptionez raspunsul server-ului.
    char *response = receive_from_server(socket);
    // Retin linia cu json-ul care contine token-ul.
    char *json = strstr(response, "{\"token");
    // Afisez feedback corespunzator.
    int err_code = give_feedback(response, 200, "You entered the library successfully!");

    // Daca, codul gasit nu este 200, eliberez memoria si intorc null.
    if (err_code != 200) {
        free(response);
        free(token);
        return NULL;
    }

    // Altfel, preiau token-ul si il copiez in variabila alocata.
    char *token_str = strstr(json, ":\"");
    char *aux = strtok(token_str, "\"");
    aux = strtok(NULL, "\"");
    memcpy(token, aux, strlen(aux));

    // Eliberez memoria si intorc token-ul.
    free(response);
    return token;
}

/*
 * Functie care se ocupa de comanda get_books.
 * @socket - socket-ul pe care este conexiunea cu server-ul
 * @library_token - token-ul de authorization
 */
void get_books_cmd(int socket, char *library_token)
{
    // Creez si trimit mesajul catre server.
    char *message = compute_get_request(SERVADDR, "/api/v1/tema/library/books",
                                        NULL, library_token, NULL, 0);
    send_to_server(socket, message);
    free(message);

    // Receptionez raspunsul server-ului.
    char *response = receive_from_server(socket);
    // Preiau lista de carti (daca exista).
    char *book_list = strchr(response, '[');

    // Afisez feedback.
    int err_code = give_feedback(response, 200, NULL);
    // Daca, codul gasit este diferit de 200, eliberez memoria si ies din functie.
    if (err_code != 200) {
        free(response);
        return;
    }

    // Altfel afisez lista, si apoi eliberez memoria.
    printf("%s\n", book_list);
    free(response);
}

/*
 * Functie care verifica daca un sir de caractere are doar cifre.
 * @str - sirul de caractere
 * return - 1, daca are doar cifre, 0 altfel.
 */
int only_digits(char *str)
{
    for (unsigned int i = 0; i < strlen(str); i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

/*
 * Functie care se ocupa de comanda get_book.
 * @socket - socket-ul pe care este conexiunea cu server-ul
 * @library_token - token-ul de authorization
 */
void get_book_cmd(int socket, char *library_token)
{
    char id[BUFFLEN];

    // Citesc id-ul cartii cautate, oferind un prompt, si verific daca este un numar.
    printf("id=");
    fgets(id, BUFFLEN, stdin);
    id[strlen(id) - 1] = '\0';
    if (!only_digits(id)) {
        printf("Id has to be a whole number!\n");
        return;
    }

    // Construiesc url-ul cererii.
    char url[BUFFLEN];
    strcpy(url, "/api/v1/tema/library/books/");
    strcat(url, id);
    // Creez si trimit cererea catre server.
    char *message = compute_get_request(SERVADDR, url, NULL, library_token, NULL, 0);
    send_to_server(socket, message);
    free(message);
    // Receptionez raspunsul server-ului.
    char *response = receive_from_server(socket);
    // Retin linia cu json-ul ce contine cartea.
    char *book = strstr(response, "{\"title");
    book[strlen(book) - 1] = '\0';
    // Afisez feedback.
    int err_code = give_feedback(response, 200, NULL);

    // Daca, codul gasit nu este 200, eliberez memoria raspunsului si ies din functie.
    if (err_code != 200) {
        free(response);
        return;
    }

    // Altfel, afisez cartea si apoi eliberez memoria.
    printf("%s\n", book);
    free(response);
}

/*
 * Functie care se ocupa de comanda add_book.
 * @socket - socket-ul pe care este conexiunea cu server-ul
 * @library_token - token-ul de authorization
 */
void add_book_cmd(int socket, char *library_token)
{
    // Citesc de la tastatura, oferind un prompt, titlul, autorul, genul, editura si
    // numarul de pagini.
    char title[BUFFLEN], author[BUFFLEN], genre[BUFFLEN], publisher[BUFFLEN], page_count[BUFFLEN];

    printf("title=");
    fgets(title, BUFFLEN, stdin);
    title[strlen(title) - 1] = '\0';

    printf("author=");
    fgets(author, BUFFLEN, stdin);
    author[strlen(author) - 1] = '\0';

    printf("genre=");
    fgets(genre, BUFFLEN, stdin);
    genre[strlen(genre) - 1] = '\0';

    printf("publisher=");
    fgets(publisher, BUFFLEN, stdin);
    publisher[strlen(publisher) - 1] = '\0';

    printf("page_count=");
    fgets(page_count, BUFFLEN, stdin);
    page_count[strlen(page_count) - 1] = '\0';
    // Verific daca numarul de pagini este numar.
    if (!only_digits(page_count)) {
        printf("Page count has to be a whole number!\n");
        return;
    }

    // Creez json object-ul cu informatiile cartii, pentru payload-ul cererii.
    json_object *book = book_json(title, author, genre, publisher, page_count);

    // Creez cererea si o trimit catre server.
    char *message = compute_post_request(SERVADDR, "/api/v1/tema/library/books",
                                         library_token, book, NULL, 0);
    send_to_server(socket, message);
    free(message);

    // Receptionez raspunsul.
    char *response = receive_from_server(socket);
    // Afisez feedback.
    give_feedback(response, 200, "Book added successfully!");
    // Eliberez memoria.
    free(response);
    free_json(book);
}

/*
 * Functie care se ocupa de comanda delete_book.
 * @socket - socket-ul pe care este conexiunea cu server-ul
 * @library_token - token-ul de authorization
 */
void delete_book(int socket, char *library_token)
{
    // Citesc de la tastatura id-ul cartii, oferind un prompt, si verific daca este numar.
    char id[BUFFLEN];

    printf("id=");
    fgets(id, BUFFLEN, stdin);
    id[strlen(id) - 1] = '\0';
    if (!only_digits(id)) {
        printf("Id has to be a whole number!\n");
        return;
    }

    // Contruiesc url-ul cererii.
    char url[BUFFLEN];
    strcpy(url, "/api/v1/tema/library/books/");
    strcat(url, id);
    // Creez si trimit cererea catre server.
    char *message = compute_delete_request(SERVADDR, url, library_token);
    send_to_server(socket, message);
    free(message);
    // Receptionez raspunsul.
    char *response = receive_from_server(socket);
    // Afisez feedback.
    give_feedback(response, 200, "Book deleted successfully!");
    // Eliberez memoria.
    free(response);
}

/*
 * Functie care se ocupa de comanda logout.
 * @socket - socket-ul pe care este conexiunea cu server-ul
 * @login_cookie - cookie-ul de login
 */
void logout_cmd(int socket, char *login_cookie)
{
    // Daca este cazul, aloc memorie pentru cookie-uri, si pun in vectorul de cookie-uri,
    // cookie-ul de login. 
    char **cookies = NULL;
    int cookies_len = 0;
    if (login_cookie != NULL) {
        cookies = calloc(1, sizeof(char *));
        cookies[0] = login_cookie;
        cookies_len = 1;
    }

    // Creez si trimit cererea catre server.
    char *message = compute_get_request(SERVADDR, "/api/v1/tema/auth/logout",
                                        NULL, NULL, cookies, cookies_len);
    send_to_server(socket, message);
    // Eliberez memoria.
    free(message);
    if (cookies != NULL)
        free(cookies);

    // Receptionez raspunsul.
    char *response = receive_from_server(socket);
    // Afisez feedback.
    give_feedback(response, 200, "Logout successful!");
    // Eliberez memoria.
    free(response);
}

int main(void)
{
    // Buffer folosit pentru citirea comenzilor.
    char buff[BUFFLEN];
    // Cookie-ul de login si token-ul de authorization.
    char *login_cookie = NULL;
    char *library_token = NULL;
    while (1) {
        // Deschid conexiunea cu server-ul.
        int sockfd = open_connection(SERVADDR, PORT, AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            fprintf(stderr, "open connection\n");
            return 0;
        }
        // Citesc o comanda.
    	fgets(buff, BUFFLEN, stdin);

        // Verific ce fel de comanda este, si apelez functia care ii corespunde.
    	if (strncmp(buff, "register", 8) == 0) {
    		register_cmd(sockfd);
    	} else if (strncmp(buff, "login", 5) == 0) {
            login_cookie = login_cmd(sockfd, login_cookie);
        } else if (strncmp(buff, "enter_library", 13) == 0) {
            library_token = enter_library_cmd(sockfd, login_cookie);
        } else if (strncmp(buff, "get_books", 9) == 0) {
            get_books_cmd(sockfd, library_token);
        } else if (strncmp(buff, "get_book", 8) == 0) {
            get_book_cmd(sockfd, library_token);
        } else if (strncmp(buff, "add_book", 8) == 0) {
            add_book_cmd(sockfd, library_token);
        } else if (strncmp(buff, "delete_book", 11) == 0) {
            delete_book(sockfd, library_token);
        } else if (strncmp(buff, "logout", 6) == 0) {
            logout_cmd(sockfd, login_cookie);
            if (login_cookie != NULL)
                free(login_cookie);
            if (library_token != NULL)
                free(library_token);
            login_cookie = library_token = NULL;
        } else if (strncmp(buff, "exit", 4) == 0) {
            if (login_cookie != NULL)
                free(login_cookie);
            if (library_token != NULL)
                free(library_token);
            close_connection(sockfd);
            return 0;
        } else {
            printf("Unknown command!\n");
        }

        // Inchid conexiunea.
        close_connection(sockfd);
    }
	return 0;
}