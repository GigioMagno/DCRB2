#ifndef POSTING_LIST_H
#define POSTING_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct list {
    int doc_ID;
    struct list *next;
};

typedef struct list list;

void add_id(list **l, int ID);
void print_list(list *l, FILE *fdest);
void print_list2(list *l, FILE *fdest, int *result_set);
void free_list(list *l);
char **map_ids_from_paths(char **id_paths, int num_id_paths, int *total_num, int *key);

#endif
