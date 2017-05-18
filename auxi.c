/********************************************************************************************************************
 *
 * File Name: aux.c
 *
 *
 * COMMENTS
 *		Implements auxiliary functions to handle lists, their contents and memory checks
 *      It contains a structure and functions for type t_lista, a 1st order abstract data type.
 *      Each variable of type t_lista implements a node of that contains items.
 *
 *
 ******************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "auxi.h"

/******************************************************************************
 * VerifyMalloc ()
 *
 * Arguments: test - item to verify
 * Returns:  (void)
 * Side-Effects: check memory allocation
 *
 * Description: verifies if memory was allocated correctly
 *
 *****************************************************************************/

void VerifyMalloc(Item test)
{
	if(test == NULL)
	{
		printf("No memory\n");
		exit(EXIT_FAILURE);
	}
}

/******************************************************************************
 * iniLista ()
 *
 * Arguments: none
 * Returns: t_lista *
 * Side-Effects: list is initialized
 *
 * Description: initializes list
 *
 *****************************************************************************/

t_lista *iniList()
{
    return NULL;
}

/******************************************************************************
 * new_node ()
 *
 * Arguments: this - Item to save in the new node
 *            lp - list that will contain the new node
 * Returns: t_lista  *
 * Side-Effects: none
 *
 * Description: creates and returns a new node that is added to the list
 *
 *****************************************************************************/

t_lista *newNode(t_lista *lp, Item this)
{
    t_lista *new_node;

    new_node = (t_lista*) malloc(sizeof(t_lista));
    VerifyMalloc(new_node);

    new_node->this = this;
    new_node->next = lp;
    lp = new_node;

    return lp;
}

/******************************************************************************
 * newEndNode ()
 *
 * Arguments: lp - pointer to the list
 *            this - content of the new element
 * Returns: t_lista  *
 *
 * Description: creates a new node ate the end of the list
 *
 *****************************************************************************/
t_lista *newEndNode (t_lista *lp, Item this)
{

    t_lista *aux;
    t_lista *new_node = (t_lista*) malloc(sizeof(t_lista));
    VerifyMalloc(new_node);

    /* In case the list is NULL, put it as the head of the list */
    if(lp==NULL)
    {
        new_node->this = this;
        lp=new_node;
        new_node->next = NULL;

    }
    else
    {
        /* Put it at the end of the list */
        aux=lp;

        while(aux->next!=NULL)
        {
            aux=aux->next;
        }

        new_node->this = this;
        aux->next=new_node;
        new_node->next = NULL;
    }

    return lp;
}

/******************************************************************************
 * getItemLista ()
 *
 * Arguments: p - pointer to a node from the list
 * Returns: Item (content) of the node
 * Side-Effects: none
 *
 * Description: returns an Item from the list
 *
 *****************************************************************************/

Item getListItem(t_lista *p)
{
    return p -> this;
}

/******************************************************************************
 * getNextListElement ()
 *
 * Arguments: p - pointer to a node from the list
 * Returns: pointer to next element in list
 * Side-Effects: none
 *
 * Description: returns a pointer to the next element of the list
 *
 *****************************************************************************/

t_lista *getNextListElement(t_lista *p)
{
    return p -> next;
}

/******************************************************************************
 * freeList ()
 *
 * Arguments: lp - pointer to the list
 * Returns:  (void)
 * Side-Effects: frees space occupied by list items
 *
 * Description: free list
 *
 *****************************************************************************/

void freeList(t_lista *lp, void freeItem(Item))
{
	/* auxiliar pointers to iterate the list */
    t_lista *aux, *newhead;

    for(aux = lp; aux != NULL; aux = newhead)
    {
        newhead = aux->next;
        freeItem(aux->this);
        free(aux);
    }
}

/******************************************************************************
 * itemFree ()
 *
 * Arguments: this - item to free
 * Return:  (void)
 * Side-Effects: frees an item
 *
 * Description: free the item from the node before free the node itself
 *
 *****************************************************************************/

void itemFree(Item this)
{
    free(this);
}

/********************************************************************************************
 * freeItemWithMessage ()
 *
 * Arguments: Item - item that will be free
 *
 * Returns: (void)
 * Side-Effects: free items
 *
 * Description: free items from the list that contains a message that has to be free before
 *
 ********************************************************************************************/

void freeItemWithMessage(Item this)
{
    free(((Message*)this)->post);
    free(this);
}

/********************************************************************************************
 * freeItemWithName ()
 *
 * Arguments: Item - item that will be free
 *
 * Returns: (void)
 * Side-Effects: free items
 *
 * Description: free items from the list that contains a name that has to be free before
 *
 ********************************************************************************************/

void freeItemWithName(Item this)
{
    if((((TCP_Session*)this)->name) != NULL)
        free(((TCP_Session*)this)->name);

    free(this);
}

/******************************************************************************
 * deleteFirstNode()
 *
 * Arguments: lp - heads list
 *            type - aux type
 * Return:  (void)
 * Side-Effects: delete the first node
 *
 * Description: delestes the first node of a list, changing the head
 *
 *****************************************************************************/

t_lista *deleteFirstNode(t_lista *lp, int type)
{
    t_lista *aux;

    aux = lp;

    lp = getNextListElement(aux);

    if(type == MSG)
        freeItemWithMessage(aux->this);
    else if(type == NAME)
        freeItemWithName(aux->this);

    free(aux);
    
    return lp;

}

/**********************************************************************************************************
 * deleteMiddleNode()
 *
 * Arguments: lp - heads list
 *            position_in_list - gives th position in list of the element that we want to delete from list
 * Return:  (void)
 * Side-Effects: delete the first node
 *
 * Description: delestes the first node of a list, changing the head
 *
 **********************************************************************************************************/

t_lista *deleteMiddleNode(t_lista *lp, int position_in_list)
{
    t_lista *temp;
    t_lista * aux;
    t_lista *after_temp;
    int i = 0;
    
    aux = lp;
    temp = lp;
    
    for(i = 1; temp != NULL && i < position_in_list - 1; i++)
        temp = temp->next;

    after_temp = temp->next->next;

    freeItemWithName(temp->next->this);
    free(temp->next);

    temp->next = after_temp;
    
    return aux;

}










