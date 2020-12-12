/**
 * @module client.c
 * @author James Morar
 * @date   12-10-2020
 *
 * @brief  Main client module for talking to server.c. Client transmits
 *  	   a message and waits for a response.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP_ADDR			"127.0.0.1"                                                                       
#define PORT			9000
#define RX_BUF_SIZE		4096                                                                

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
	char txBuf[] = "Hello from the client!\n";
	char rxBuf[RX_BUF_SIZE];
	memset(rxBuf, '\0', sizeof(rxBuf));

    	conn_handle_t *conn_handle = client_start(); /* Start client, establish connection to server */
	client_send(conn_handle, txBuf, sizeof(txBuf));
	client_recv(conn_handle, rxBuf, sizeof(rxBuf));
	close(conn_handle->clientSock);

	return 0;
}

/***************************************
 *FUNCTION DEFINITIONS
 **************************************/
/**
 * @brief  Create client socket and connect to server.
 *  	   Return connection handler for client. Handler contains
 *  	   client socket and sockaddr_in struct for client.  
 * 
 * @param  none
 * @return conn_handle_t * Pointer to client connection handler. Contains
 *  	   main client socket and config struct.
 */
conn_handle_t *client_start()
{
	/* Create client socket */
    	int clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock == -1)
	{
		perror("Client socket creation failed");
		exit(1);
	}
	printf("Client socket created successfully.\n");

	struct sockaddr_in client_addr;
	client_addr.sin_family 		= AF_INET;
	client_addr.sin_port 		= htons(PORT);
	client_addr.sin_addr.s_addr 	= inet_addr(IP_ADDR);

	/* Establish client connection */
	int clientConn = connect(clientSock,
			    (struct sockaddr *)&client_addr,
			    sizeof(client_addr));
	if (clientConn == -1)
	{
		perror("Client connection failed");
		exit(1);
	}
	printf("Client connection successful: %s:%d\n", IP_ADDR, PORT);

	conn_handle_t *ret 	= (conn_handle_t *)malloc(sizeof(conn_handle_t));
	ret->clientSock 	= clientSock;
	ret->client_addr 	= client_addr;

	return ret;
}

/**
 * @brief  Wrapper around send() that handles client failure and success.
 *
 * @param  conn_handle Pointer to client connection handle which contains
 *  	   client socket and configuration struct.
 * @param  txBuf Pointer to buffer that contains message that client will
 *  	   transmit to server.
 * @param  bufSize Size of txBuf.
 * @return none
 */
void client_send(conn_handle_t *conn_handle, char *txBuf, ssize_t bufSize)
{
	if (send(conn_handle->clientSock, txBuf, bufSize, 0) == -1)
	{
		perror("Client send error");
	} 
	printf("Client send Successful.\n");
	fflush(stdout);
}

/**
 * @brief  Wrapper around recv() that handles client failure and success.
 *
 * @param  conn_handle Pointer to client connection handle which contains
 *  	   client socket and configuration struct.
 * @param  rxBuf Pointer to buffer that will receive server message.
 * @param  bufSize Size of rxBuf.
 * @return none
 */
void client_recv(conn_handle_t *conn_handle, char *rxBuf, ssize_t bufSize)
{
	if (recv(conn_handle->clientSock, rxBuf, bufSize, 0) == -1)
	{
		perror("Client receive error");
	} 
	printf("Server message: %s\n", rxBuf);
	fflush(stdout);
}
