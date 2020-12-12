# ClientServer
This is a client-server system written in C that exercises the use of threads, condition variables, locks, data structures, and network sockets.

The client will simply transmit a message and receive a reply from the server. If the thread pool is busy and the fifo buffer is full, the main server thread
will reply back to indicate this and close the client connection. Otherwise, one of the threads from the thread pool will service the request.

### Follow the steps below to get the program to run:

git clone https://github.com/jmos1/ClientServer

In one terminal, cd to ClientServer/server 

Run make

Run ./server

In another terminal, cd to ClientServer/client 

Run make

Run ./client_test.sh (This will flood the server with 40 client connections)
