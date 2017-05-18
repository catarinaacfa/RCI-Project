#ifndef AUX_H
#define AUX_H

#include "servers.h"
#include "defs.h"

#define RMB 0
#define MSG 1

#define NAME 2

/* type definition for structure to hold list item */
typedef struct _t_lista
{
	Item this;
    struct _t_lista *next;

}t_lista;

void VerifyMalloc(Item test);
t_lista *iniList();
t_lista *newNode (t_lista *lp, Item this);
t_lista *newEndNode (t_lista *lp, Item this);
Item getListItem(t_lista *p);
t_lista *getNextListElement(t_lista *p);
void freeList(t_lista *lp, void freeItem(Item));
void itemFree(Item this);
void freeItemWithMessage(Item this);
void freeItemWithName(Item this);
t_lista *deleteFirstNode(t_lista *lp, int type);
t_lista *deleteMiddleNode(t_lista *lp, int position_in_list);

#endif
