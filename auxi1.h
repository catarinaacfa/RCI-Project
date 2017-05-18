#ifndef AUX1_H
#define AUX1_H

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"

#define MSG_SIZE 141

/* Linked list  */
typedef struct _t_lista
{
    Item this;
    struct _t_lista *prox;
}t_lista;

typedef struct _messages
{
	char post[MSG_SIZE];

}Messages;

void VerifyMalloc(Item test);
t_lista *iniList();
t_lista *new_Node (t_lista *lp, Item this);
t_lista *newEndNode (t_lista *lp, Item this);
Item getListItem(t_lista *p);
t_lista *getNextListElement(t_lista *p);
void freeList(t_lista *lp, void freeItem(Item));
void itemFree(Item this);
t_lista *deleteFirstNode(t_lista *lp);

#endif