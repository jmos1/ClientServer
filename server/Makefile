CC=gcc

all: server

server: server.c fifo.c
	$(CC) -g $^ -o $@ -lpthread

clean:
	rm server
