/**
 * @module server.c
 * @author James Morar
 * @date 
 *
 * @brief Server module for taking client requests.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define IP_ADDR			"127.0.0.1"
#define PORT			5000
#define BACKLOG_QUEUE	0
#define RX_BUF_SIZE		4096
#define TX_BUF_SIZE		4096

/*************************************
 *VARIABLES AND STRUCT DEFINITIONS
 ************************************/
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
void *thread_handler(void *pClientSock);

/*************************************
 *MAIN
 ************************************/
int main()
{
	char txBuf[] = "Response from the server.\n";
	char rxBuf[RX_BUF_SIZE];
	memset(rxBuf, '\0', RX_BUF_SIZE);	

	serv_handle_t *serv_handle = server_start(); /* Start the server */
	
	while (1)
	{
		memset(rxBuf, '\0', RX_BUF_SIZE);

		int clientSock = accept(serv_handle->servSock, NULL, NULL); /* Accept client connections */
		server_recv(&clientSock, rxBuf, sizeof(rxBuf));
		server_send(&clientSock, txBuf, sizeof(txBuf));
	}

	close(serv_handle->servSock);

	return 0;
}

/*************************************
 *FUNCTION DEFINITIONS
 ************************************/

/**
 * @brief
 *
 */
void *thread_handler(void *pClientSock)
{
	printf("In thread handler\n");	
	free(pClientSock);
	return NULL;
}

/**
 * @brief
 *
 */
serv_handle_t *server_start()
{
	/* Create server socket and sockaddr_in struct */
	int servSock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = inet_addr(IP_ADDR);

	/* Bind server socket */
	if (bind(servSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1 )
	{
        fprintf(stderr, "Port bind fail.\n");
        exit(1);
    }
    printf("Port bind success.\n");

	/* Socket listen */
	if (listen(servSock, BACKLOG_QUEUE) == -1)
	{
        fprintf(stderr, "Error while listening.\n");
        exit(1);
    }
    printf("\nListening...\n\n");

	serv_handle_t *ret = (serv_handle_t *)malloc(sizeof(serv_handle_t *));	
	ret->servSock = servSock;
	ret->serv_addr = serv_addr;

	return ret;
}

/**
 * @brief
 *
 */
void server_send(int *pClientSock, char *txBuf, ssize_t bufSize)
{
	if (send(*pClientSock, txBuf, bufSize, 0) == -1)
	{
		fprintf(stderr, "ERROR on sever SEND.\n");
	}
	printf("Server SEND SUCCESS.\n");
}

/**
 * @brief
 *
 */
void server_recv(int *pClientSock, char *rxBuf, ssize_t bufSize)
{
	if ( (recVal = recv(*pClientSock, rxBuf, bufSize, 0)) == -1 )
	{
		fprintf(stderr, "ERROR on sever RECV.\n");
	} 
	printf("Server RECV SUCCESS.\n");
	printf("Client message: %s", rxBuf);
}

