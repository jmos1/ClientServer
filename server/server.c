/**
 * @module server.c
 * @author James Morar
 * @date   12-10-2020
 *
 * @brief  A multithreaded server module which demonstrates the use
 *  	   of thread pools, condition variables, fifo buffers, and sockets
 * 	   among other things.
 *  	     The client transmits a greeting, and the server replies with its
 *  	   own greeting. If all threads are busy and the fifo is full, then the
 *  	   main thread processes the client request and transmits a message
 * 	   indicating that the server is full. Each thread sleeps for 5 seconds
 *  	   to allow the fifo to fill.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h> 	/* Mutex locks and threads */
#include <unistd.h>	/* Sleep */
#include "fifo.h"

#define IP_ADDR				"127.0.0.1"
#define PORT				9000
#define BACKLOG_QUEUE		100
#define RX_BUF_SIZE			4096
#define MAX_THREADS			8

/*************************************
 *VARIABLES AND STRUCT DEFINITIONS
 ************************************/
pthread_mutex_t mutex 		= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition 	= PTHREAD_COND_INITIALIZER;

typedef struct
{
	int servSock;
	struct sockaddr_in serv_addr;
} serv_handle_t;

/*************************************
 *FUNCTION PROTOTYPES
 ************************************/
serv_handle_t *server_start();
void server_send(int *pClientSock, char *txBuf, ssize_t bufSize);
void server_recv(int *pClientSock, char *rxBuf, ssize_t bufSize);
void *thread_handler(void *param);

/*************************************
 *MAIN
 ************************************/
int main()
{
	/* Message buffers used by main thread when fifo is full */
	char maxFifoTx[] = "Server fifo is full.\n";
	char maxFifoRx[RX_BUF_SIZE];

	/* Initialize thread pool */
	pthread_t thread_arr[MAX_THREADS];
	for (int i=0; i<MAX_THREADS; i++)
		pthread_create(&thread_arr[i], NULL, thread_handler, NULL);

	serv_handle_t *serv_handle = server_start(); /* Start the server */
	
	/* Service client requests */
	while (1)
	{
		int clientSock;
		if ((clientSock = accept(serv_handle->servSock, NULL, NULL)) == -1)
		{
			perror("Server accept failed");
		} 
		printf("Client connection accepted!\n");
		fflush(stdout);
		
		if (currentFifoSize < MAX_FIFO_SIZE)
		{
			int *pClientSock 	= (int *)malloc(sizeof(int *));
			*pClientSock 		= clientSock; 
			
			pthread_mutex_lock(&mutex);
			fifo_add(pClientSock);
			pthread_cond_signal(&condition);
			pthread_mutex_unlock(&mutex);
		}
		else /* If fifo is full. */
		{
			memset(maxFifoRx, '\0', sizeof(maxFifoRx));
			server_recv(&clientSock, maxFifoRx, sizeof(maxFifoRx));
			server_send(&clientSock, maxFifoTx, sizeof(maxFifoTx));
			close(clientSock);
		}
	}

	return 0;
}

/*************************************
 *FUNCTION DEFINITIONS
 ************************************/
/**
 * @brief  Handle incoming client connections.
 *  	   8 worker threads wait until main() thread signals that
 *  	   an entry has been added to the fifo.
 *
 * @param  param Ignore
 * @return none
 */
void *thread_handler(void *param)
{
	printf("THREAD CREATED\n");	

	char txBuf[] = "Hello from the server!\n";
	char rxBuf [RX_BUF_SIZE];
	int *pClientSocket = NULL;

	while (1)
	{
		/* CRITICAL SECTION BEGIN */
		pthread_mutex_lock(&mutex);
		if ((pClientSocket = fifo_peek())) /* If client request exists */
		{
			fifo_remove();
		}
		else
		{
			pthread_cond_wait(&condition, &mutex);
			pClientSocket = fifo_peek();
			fifo_remove();
		}
		pthread_mutex_unlock(&mutex);
		/* CRITICAL SECTION END */
		
		/* Service the request */
		if (pClientSocket)
		{
			memset(rxBuf, '\0', sizeof(rxBuf));
			server_recv(pClientSocket, rxBuf, sizeof(rxBuf));
			server_send(pClientSocket, txBuf, sizeof(txBuf));
			close(*pClientSocket);
			sleep(5); /* Give the fifo buffer time to fill */
		}
	}
	return NULL;
}

/**
 * @brief  Create server socket, bind socket, and listen.
 * 
 * @param  none
 * @return serv_handle_t * Pointer to server handle struct. Contains
 *  	   main server socket and config struct.
 */
serv_handle_t *server_start()
{
	/* Create server socket and sockaddr_in struct */
	int servSock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serv_addr;
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_port 			= htons(PORT);
	serv_addr.sin_addr.s_addr 	= inet_addr(IP_ADDR);

	/* Bind server socket */
	if (bind(servSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1 )
	{
        perror("Server port bind failed");
        exit(1);
    }
    printf("Port bind success.\n");

	/* Socket listen */
	if (listen(servSock, BACKLOG_QUEUE) == -1)
	{
        perror("Server socket listen failed");
        exit(1);
    }
    printf("\nListening...\n\n");

	serv_handle_t *ret = (serv_handle_t *)malloc(sizeof(serv_handle_t *));	
	ret->servSock 	= servSock;
	ret->serv_addr 	= serv_addr;

	return ret;
}

/**
 * @brief  Wrapper around send() that handles server failure and success.
 * 
 * @param  socket Pointer to client socket which should be in fifo node.
 * @param  txBuf Pointer to buffer that contains message that will be sent
 * 	   to client.
 * @param  bufSize Size of txBuf.
 * @return none
 */
void server_send(int *socket, char *txBuf, ssize_t bufSize)
{
	if (send(*socket, txBuf, bufSize, 0) == -1)
	{
		perror("Server send error");
	} 
	printf("Server send successful.\n");
	fflush(stdout);
}

/**
 * @brief  Wrapper around recv() that handles server failure and success.
 *
 * @param  socket Pointer to client socket which should be in fifo node.
 * @param  rxBuf Pointer to buffer that contains message that will be 
 * 	   received by the server.
 * @param  bufSize Size of rxBuf.
 * @return none
 */
void server_recv(int *socket, char *rxBuf, ssize_t bufSize)
{
	if (recv(*socket, rxBuf, bufSize, 0) == -1 )
	{
		perror("Server receive error");
	} 
	printf("Client message: %s\n", rxBuf);
	fflush(stdout);
}

