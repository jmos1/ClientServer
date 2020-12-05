all: server

server: server.c fifo.c
	gcc -g $^ -o $@ -lpthread

clean:
	rm server
