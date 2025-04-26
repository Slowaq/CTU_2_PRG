// Code inspired by linked_list.c by David Valouch

#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
typedef int bool;
#define false 0
#define true 1

struct llNode_ {
    int data;
    struct llNode_ *next;
};
typedef struct llNode_ llNode;

struct llLinkedList_ {
    llNode *first;
    llNode *last;
};
typedef struct llLinkedList_ llLinkedList;

#define llEmpty {NULL, NULL}

llLinkedList list = llEmpty;   // Linked list initialization

_Bool ll_is_empty(void)
{
    return list.first == NULL;
}

_Bool push(int entry)
{
    if (entry < 0) return false;

    llNode *new_node = (llNode*)malloc(sizeof(llNode));
    if (new_node == NULL) return false;

    new_node->data = entry;
    new_node->next = NULL;

    if (ll_is_empty())
    {
        list.first = list.last = new_node;
    } else
    {
        list.last->next = new_node;
        list.last = new_node;
    }

    return true;
}

int pop(void)
{
    if (ll_is_empty()) return -1;

    int value = list.first->data;
    llNode *tmp = list.first;
    if (list.first == list.last)
    {
        list.first = list.last = NULL;
    } else{
        list.first = list.first->next;
    }
    free(tmp);

    return value;
}

_Bool insert(int entry)
{
    if (entry < 0) return false;

    llNode *new_node = (llNode*)malloc(sizeof(llNode));
    if (new_node == NULL) return false;

    new_node->data = entry;
    new_node->next = NULL;

    if (ll_is_empty() || entry > list.first->data) {
        new_node->next = list.first;
        list.first = new_node;
        if (!list.last) list.last = new_node;
        return true;
    }

    llNode *prev = list.first;
    llNode *n = list.first->next;
    while (n != NULL && n->data > entry) {
        prev = n;
        n = n->next;
    }

    prev->next = new_node;
    new_node->next = n;

    if (n == NULL) list.last = new_node;
    
    return true;
}

_Bool erase(int entry)
{
    bool success = false;

    if (ll_is_empty()) return false;

    llNode *previous = NULL;
    llNode *n = list.first;
    while (n != NULL)
    {
        llNode *next_node = n->next;

        if (n->data != entry)
        {
            previous = n;
            n = next_node;
            continue;
        } if (n->data == entry)
        {
            if (previous == NULL)
            {
                list.first = next_node;
                if (n == list.last) list.last = NULL;
            } else
            {
                previous->next = next_node;
                if (n == list.last) list.last = previous;
            }
            free(n);
            success = true;
        }

        n = next_node;
    }

    return success;
}

int getEntry(int idx)
{
    if (idx < 0 || idx >= size()) return -1;

    int count = 0;
    for (llNode *n = list.first; n != NULL; n = n->next)
    {
        if (count == idx) return n->data;
        count++;
    }

    return -1;
}

int size(void)
{
    int count = 0;
    for (llNode *n = list.first; n != NULL; n = n->next) count++;
    return count;
}

void clear(void)
{
    llNode *n = list.first;
    while (n != NULL) {
        llNode *next = n->next;
        free(n);
        n = next;
    }
    list.first = list.last = NULL;
}
