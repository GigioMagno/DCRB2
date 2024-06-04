#ifndef _INQUERY_
#define _INQUERY_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "posting_list.h"
#include "bstree.h"
#include <stdbool.h>

void lower_string(char *input);
void initialize_printed_docs(int size);
void free_printed_docs();
void print_document(char **map, int doc_ID, int key, int rank);
void conjunctive_query(node *root, char **map, int key, char **searchKeys, int num_keys, int rank);
void disjunctive_query(node *root, char **map, int key, char **searchKeys, int num_keys, int rank);

#endif