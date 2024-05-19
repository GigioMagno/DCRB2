#ifndef _SSTACK_
#define _SSTACK_

#include <stdio.h>
#include <stdlib.h>

//////////// Forward declaration of the struct to avoid cyclic dependance with the file bstree.h \\\\\\\\\\\\\

struct node;

struct record {
    struct node *memaddr;
    struct record *next;
};

typedef struct record record;

record *create_record(struct node *mem);
record *push(record **top, struct node *addr);
record *pop(record **top);
struct node *head_of_stack(record *tos);
int is_empty(record *top);
void print_stack(record *top);
void print_stack2(record *top);

#endif
