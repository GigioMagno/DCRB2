#ifndef _BINARY_SEARCH_TREE_
#define _BINARY_SEARCH_TREE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "posting_list.h"

#define SIZE 80

struct node {
    char *searchKey;
    struct node *left;
    struct node *right;
    list *posting;
};

typedef struct node node;

node *create_node(char *searchKey);
void safe_recursive_add_node(node **root, node *newnode);
void safe_recursive_print_tree(node *root, FILE *f);
void safe_recursive_delete_node(node **root, char* searchKey);
void print_tree(node *root, FILE *f);
node *get_minimum(node **root);
node *get_maximum(node **root);
void delete_node(node **root, char* searchKey);
node *build_bst(char *filename, int *len);
node* get_node(node *root, char *searchKey);

#endif
