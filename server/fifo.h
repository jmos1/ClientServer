/**
 * @module fifo.h
 * @author James Morar
 * @date 
 *
 * @brief Header file for fifo data structure
 *
 */
#ifndef _FIFO_
#define _FIFO_

/*************************************
 *VARIABLES AND STRUCT DEFINITIONS
 ************************************/
typedef struct fifo_node_t
{
    int *pClientSocket;
    struct fifo_node_t *next;
} fifo_node_t;

extern fifo_node_t *g_fifo_front; /* Defined in fifo.c */
extern fifo_node_t *g_fifo_back;  /* Defined in fifo.c */

/*************************************
 *FUNCTION PROTOTYPES
 ************************************/
void fifo_add(int *pClientSocket);
void fifo_remove();
int *fifo_peek();

#endif /* _FIFO_ */