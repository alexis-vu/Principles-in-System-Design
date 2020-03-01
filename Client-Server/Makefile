CC = gcc
CFLAGS = -g -Wall

all: client server

client:
	$(CC) $(CFLAGS) -o client client.c

server:
	$(CC) $(CFLAGS) -o server server.c

test:
	$(CC) $(CFLAGS) -o test test.c

clean:
	$(RM) client server test
