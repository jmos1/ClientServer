/**
 * @module client.c
 * @author James Morar
 * @date 
 *
 * @brief Main client module for talking to server.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP_ADDR			"127.0.0.1"                                                                       
#define PORT			5000
#define RX_BUF_SIZE		4096                                                                          
#define TX_BUF_SIZE		4096

/**************************************
 *VARIABLES AND STRUCT DEFINITIONS
 *************************************/
typedef struct
{
	int clientSock;
	struct sockaddr_in client_addr;
} conn_handle_t;

/***************************************
 *FUNCTION PROTOTYPES 
 **************************************/
conn_handle_t *client_start();
void client_send(conn_handle_t *conn_handle, char *txBuf, ssize_t bufSize); 
void client_recv(conn_handle_t *conn_handle, char *rxBuf, ssize_t bufSize); 

/***************************************
 *MAIN
 **************************************/
int main()
{
	char txBuf[] = "Client request string.\n";
	char rxBuf[RX_BUF_SIZE];
	memset(rxBuf, '\0', RX_BUF_SIZE);

    conn_handle_t *conn_handle = client_start(); /* Start client, establish connection to server */
	client_send(conn_handle, txBuf, sizeof(txBuf));
	client_recv(conn_handle, rxBuf, sizeof(rxBuf));
	close(conn_handle->clientSock);

	return 0;
}

/**
 * @brief Create client socket and connect to server.
 *  Return connection handler for client. Handler contains
 *  client socket and sockaddr_in struct for client.  
 *
 * @return conn_handle_t* - Pointer of connection handler
 */
conn_handle_t *client_start()
{
	/* Create client socket */
	int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock == -1)
    {
        fprintf(stderr, "Unable to create client socket.\n");
        exit(1);
    }
    printf("Client socket created successfully.\n");

    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
    client_addr.sin_addr.s_addr = inet_addr(IP_ADDR);

    /* Establish client connection */
    int clientConn = connect(clientSock,
                            (struct sockaddr *)&client_addr,
                            sizeof(client_addr));
    if (clientConn == -1)
    {
		fprintf(stderr, "Unable to connect.\n");
        exit(1);
    }
    printf("Client connection successful: %s:%d \n", IP_ADDR, PORT);

	conn_handle_t *ret = (conn_handle_t *)malloc(sizeof(conn_handle_t *));
	ret->clientSock = clientSock;
	ret->client_addr = client_addr;

	return ret;
}

/**
 * @brief
 *
 */
void client_send(conn_handle_t *conn_handle, char *txBuf, ssize_t bufSize)
{
	if (send(conn_handle->clientSock, txBuf, bufSize, 0) == -1)
	{
		fprintf(stderr, "Client SEND ERROR.\n");
		exit(1);
	} 
	printf("Client SEND SUCCESS.\n");
}

/**
 * @brief
 *
 */
void client_recv(conn_handle_t *conn_handle, char *rxBuf, ssize_t bufSize)
{
	if (recv(conn_handle->clientSock, rxBuf, bufSize, 0) == -1)
	{
		fprintf(stderr, "Client RECV ERROR.\n");
		exit(1);
	} 
	printf("Client RECV SUCCESS.\n");
	printf("Server reply: %s\n\n", rxBuf);
}