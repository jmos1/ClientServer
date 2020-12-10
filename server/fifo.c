/**
 * @module fifo.c
 * @author James Morar
 * @date   12-10-2020
 *
 * @brief  Fifo module for servicing threads. This is just a 
 *         linked list with a head and tail pointer.
 */
#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

fifo_node_t *g_fifo_front   = NULL; /* Global */
fifo_node_t *g_fifo_back    = NULL; /* Global */
int currentFifoSize         = 0;    /* Global */

/*************************************
 *FUNCTION DEFINITIONS
 ************************************/
/**
 * @brief  Add a client socket node to the back.
 * 
 * @param  pClientSock Pointer the accepted client socket.
 * @return none
 */
void fifo_add(int *pClientSocket)
{
    fifo_node_t *fifo_node      = malloc(sizeof(fifo_node_t *));
    fifo_node->pClientSocket    = pClientSocket;
    fifo_node->next             = NULL;
    
    if (g_fifo_back == NULL) /* If fifo is empty */
    {
        /* Set front and back global fifo pointer to fifo_node */
        g_fifo_back     = fifo_node;
        g_fifo_front    = fifo_node;
    }
    else /* Else fifo is not empty */
    {
        /* Add a node to the back */
        g_fifo_back->next   = fifo_node;
        g_fifo_back         = g_fifo_back->next;
    }
    currentFifoSize++;
}

/**
 * @brief  Remove client socket node from the front.
 * 
 * @param  none
 * @return none
 */
void fifo_remove()
{
    fifo_node_t *temp = g_fifo_front;

    if (g_fifo_front == g_fifo_back) /* If there is <= 1 node */
    {
        g_fifo_back     = NULL;
        g_fifo_front    = NULL;
    }
    else /* Else there is more than one node */
    {    
        g_fifo_front = g_fifo_front->next;
    }
    free(temp);
    currentFifoSize--;
}

/**
 * @brief  Return client socket pointer from front.
 * 
 * @param  none
 * @return int * Pointer to the client socket.
 */
int *fifo_peek()
{
    if (g_fifo_front != NULL)
    {
        return g_fifo_front->pClientSocket;
    }
    return NULL;
}
