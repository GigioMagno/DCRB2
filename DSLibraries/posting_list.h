#ifndef POSTING_LIST_H
#define POSTING_LIST_H

#include <stdio.h>
#include <stdlib.h>

struct list {
    long long int doc_ID;
    struct list *next;
};

typedef struct list list;

void add_id(list **l, long long int ID);
void print_list(const list *l, FILE *fdest);
void free_list(list *l);

#endif
