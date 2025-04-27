#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
typedef int bool;
#define false 0
#define true 1

struct llNode_ {
    void *data;
    struct llNode_ *next;
};
typedef struct llNode_ llNode;

struct llLinkedList_ {
    llNode *first;
    llNode *last;
    void (*clear)(void*);
    int (*compare)(const void* , const void*);
};
typedef struct llLinkedList_ llLinkedList;

_Bool ll_is_empty(void *queue)
{
    llLinkedList *q = (llLinkedList*)queue;
    return q->first == NULL;
}

void* create(void)
{
    llLinkedList *q = (llLinkedList*)malloc(sizeof(llLinkedList));
    if (q == NULL) return NULL;

    q->first = NULL;
    q->last = NULL;
    q->clear = NULL;
    q->compare = NULL;

    return q;
}

void clear(void *queue)
{
    llLinkedList *q = (llLinkedList*)queue;

    llNode *n = q->first;
    while (n != NULL) {
        llNode *next = n->next;
        if (q->clear == NULL) free(n->data);
        else q->clear(n->data);
        free(n);
        n = next;
    }
    q->first = q->last = NULL;
}


void setClear(void *queue, void (*clear)(void *))
{
    llLinkedList *q = (llLinkedList*)queue;
    q->clear = clear;
}

_Bool push(void *queue, void *entry)
{
    llLinkedList *q = (llLinkedList*)queue;
    if (entry == NULL) return false;

    llNode *new_node = (llNode*)malloc(sizeof(llNode));
    if (new_node == NULL) return false;

    new_node->data = entry;
    new_node->next = NULL;

    if (ll_is_empty(queue))
    {
        q->first = q->last = new_node;
    } else
    {
        q->last->next = new_node;
        q->last = new_node;
    }

    return true;
}

void* pop(void *queue)
{
    llLinkedList *q = (llLinkedList*)queue;
    if (ll_is_empty(queue)) return NULL;

    void *value = q->first->data;
    llNode *tmp = q->first;
    if (q->first == q->last)
    {
        q->first = q->last = NULL;
    } else{
        q->first = q->first->next;
    }

    free(tmp);

    return value;
}

_Bool insert(void *queue, void *entry)
{
    llLinkedList *q = (llLinkedList*)queue;
    if (!entry || !q->compare) return false;

    llNode *new_node = (llNode*)malloc(sizeof(llNode));
    if (new_node == NULL) return false;
    new_node->data = entry;
    new_node->next = NULL;

    if (ll_is_empty(queue) || 0 <= q->compare(entry, q->first->data)) 
    {
        new_node->next = q->first;
        q->first = new_node;
        if (!q->last) q->last = new_node;
        return true;
    }

    llNode *prev = q->first;
    llNode *n = q->first->next;
    while (n != NULL && 0 < q->compare(n->data, entry))
    {
        prev = n;
        n = n->next;
        //break;
    }

    prev->next = new_node;
    new_node->next = n;
    if (n == NULL) q->last = new_node;
    
    return true;
}

void setCompare(void *queue, int (*compare)(const void *, const void *))
{
    llLinkedList *q = (llLinkedList*)queue;
    q->compare = compare;
}

_Bool erase(void *queue, void *entry)
{
    llLinkedList *q = (llLinkedList*)queue;
    bool success = false;

    if (ll_is_empty(queue)) return false;
    if (q->compare == NULL) return false;

    llNode *previous = NULL;
    llNode *n = q->first;
    while (n != NULL)
    {
        llNode *next_node = n->next;

        if (0 == q->compare(n->data, entry))
        {
            if (previous == NULL)
            {
                q->first = next_node;
                if (n == q->last) q->last = NULL;
            } else
            {
                previous->next = next_node;
                if (n == q->last) q->last = previous;
            }
            if (q->clear == NULL) free(n->data);
            else q->clear(n->data);
            free(n);
            success = true;
        } else
        {
            previous = n;
            n = next_node;
            continue;
        } 
        
        n = next_node;
    }
    
    return success;
}

void* getEntry(const void *queue, int idx)
{
    llLinkedList *q = (llLinkedList*)queue;
    if (idx < 0 || idx >= size(queue)) return NULL;

    int count = 0;
    for (llNode *n = q->first; n != NULL; n = n->next)
    {
        if (count == idx) return n->data;
        count++;
    }

    return NULL;
}

int size(const void *queue)
{
    llLinkedList *q = (llLinkedList*)queue;
    int count = 0;
    for (llNode *n = q->first; n != NULL; n = n->next) count++;
    return count;
}
