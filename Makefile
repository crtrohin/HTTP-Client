CC=gcc
CFLAGS=-I.

client: client.c client_utils.c requests.c helpers.c buffer.c parson.c
	$(CC) -o client client.c client_utils.c requests.c helpers.c buffer.c parson.c -Wall

run: client
	./client

clean:
	rm -f *.o client
