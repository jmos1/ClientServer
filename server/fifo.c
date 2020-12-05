/**
 * @module fifo.c
 * @author James Morar
 * @date 
 *
 * @brief Fifo module for servicing threads.
 * 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

fifo_node_t *g_fifo_front = NULL; /* Global */
fifo_node_t *g_fifo_back = NULL;  /* Global */

/*************************************
 *FUNCTION DEFINITIONS
 ************************************/
/**
 * @brief Add a node to the back of the list
 */
void fifo_add(int *pClientSocket)
{
    fifo_node_t *fifo_node = malloc(sizeof(fifo_node_t *));
    fifo_node->pClientSocket = pClientSocket;
    fifo_node->next = NULL;
    
    if (g_fifo_back == NULL) /* If fifo is empty */
    {
        /* Set front and back global fifo pointer to fifo_node */
        g_fifo_back = fifo_node;
        g_fifo_front = fifo_node;
    }
    else /* Else fifo is not empty */
    {
        /* Add a node to the back */
        g_fifo_back->next = fifo_node;
        g_fifo_back = g_fifo_back->next;
    }
}

/**
 * @brief Remove node from the front
 */
void fifo_remove()
{
    fifo_node_t *temp = g_fifo_front;

    if (g_fifo_front == g_fifo_back) /* If there is <= 1 node */
    {
        g_fifo_back = NULL;
        g_fifo_front = NULL;
    }
    else /* Else there is more than one node */
        g_fifo_front = g_fifo_front->next;

    free(temp);
}

/**
 * @brief Return client socket pointer of front node
 */
int *fifo_peek()
{
    if (g_fifo_front != NULL)
        return g_fifo_front->pClientSocket;
    return NULL;
}