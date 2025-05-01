#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum {
    Ok = 0,
    Error_llPush = 100
};

struct llNode_ {
    int             data;
    struct llNode_ *next;
};
typedef struct llNode_ llNode;

struct llLinkedList_ {
    llNode *first;
    llNode *last;
};
typedef struct llLinkedList_ llLinkedList;
#define llEmpty {NULL, NULL}

bool ll_is_empty(llLinkedList *self);
bool ll_push(llLinkedList *self, int value);
bool ll_pop(llLinkedList *self, int *value);
void ll_print(llLinkedList *self);
void ll_free(llLinkedList *self);

int main()
{
    int status = Ok;

    llLinkedList list = llEmpty;

    while (1) {
        int i;
        if(scanf("%d", &i) != 1) break;

        if(!ll_push(&list, i)) {
            status = Error_llPush;
            goto main_return;
        }
    }

    int i;
    while (ll_pop(&list, &i)) {
        printf("%d\n", i);
    }
    
    // ll_print(&list);

    main_return:
    ll_free(&list);
    return status;
}

bool ll_is_empty(llLinkedList *self)
{
    return self->first == NULL;
}

bool ll_push(llLinkedList *self, int value)
{
    llNode *new_node = (llNode*)malloc(sizeof(llNode));
    if(new_node == NULL) return false;

    new_node->data = value;  
    new_node->next = NULL;

    if(ll_is_empty(self)) {
        self->first = self->last = new_node; 
    } else {
        self->last->next = new_node;  
        self->last = new_node;
    }

    return true;
}

bool ll_pop(llLinkedList *self, int *value)
{
    if(ll_is_empty(self)) return false;

    *value = self->first->data;

    llNode *tmp = self->first;
    if(self->first == self->last) {
        self->first = self->last = NULL;
    } else {
        self->first = self->first->next;
    }
    free(tmp);

    return true;
}

void ll_print(llLinkedList *self)
{
    for(llNode *n = self->first; n != NULL; n = n->next) {
        printf("%d", n->data);
        if(n->next != NULL) {
            printf(" -> ");
        }
    }
    putchar('\n');
}

void ll_free(llLinkedList *self)
{
    for(llNode *n = self->first; n != NULL;) {
        llNode *tmp = n;
        n = n->next;
        free(tmp);
    }
}
