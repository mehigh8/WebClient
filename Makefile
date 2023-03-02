# Rosu Mihai Cosmin 323CA
CC=gcc
CFLAGS=-Wall -Wextra

client: client.c requests.c helpers.c buffer.c json.c
	$(CC) $(CFLAGS) -o client client.c requests.c helpers.c buffer.c json.c

run: client
	./client

clean:
	rm -f *.o client
