//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\\\\\

#include <stdio.h>
#include "bstree.h"

#define SIZE 80

//////////////////////// Create Nodes \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function creates all the nodes to insert in the BST
// The Searchkeys are read from a file and also the posting lists
// The function returns an array of nodes

node *create_nodes_bst(char *filename, int *len);
node *create_nodes_bst(char *filename, int *len) {
    char buf[SIZE]; 
    int capacity = SIZE;
    *len = 0;

    node *node_set = (node *)malloc(sizeof(node) * capacity);
    if (node_set == NULL) {
        printf("Error file opening\n");
        return NULL;
    }

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error file opening\n");
        free(node_set);
        return NULL;
    }

    while (fgets(buf, sizeof(buf), f)) {
        size_t len_buf = strlen(buf);
        if (buf[len_buf - 1] == '\n') {
            buf[len_buf - 1] = '\0';
        }

        if (atoll(buf) == 0) {
            if (*len >= capacity) {
                capacity *= 2;
                node *temp = realloc(node_set, capacity * sizeof(node));
                if (temp == NULL) {
                    printf("Not enough memory for realloc\n");
                    free(node_set);
                    fclose(f);
                    return NULL;
                }
                node_set = temp;
            }

            node_set[*len] = *create_node(buf);
            node_set[*len].posting = NULL;
            (*len)++;
        } else {
            if (*len > 0) {
                add_id(&(node_set[*len - 1].posting), atoll(buf));
            } else {
                printf("ID ERROR: NO PAIR (ID-WORD)\n");
                free(node_set);
                fclose(f);
                return NULL;
            }
        }
    }

    fclose(f);

    node *final_set = realloc(node_set, (*len) * sizeof(node));
    if (final_set == NULL && *len > 0) {
        printf("Not enough memory for realloc\n");
        //free(node_set);
        return NULL;
    }

    return final_set;
}


//////////////////////// MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\

int main() {
    FILE *f = fopen("finale.txt", "w");
    FILE *fstem = fopen("stemwords.txt", "r");
    node *root = NULL;
    int words = 0;
    node *node_set = create_nodes_bst("postings.txt", &words);
    char buf[SIZE];

    while (fgets(buf, sizeof(buf), fstem)) {
        size_t len_buf = strlen(buf);
        if (buf[len_buf - 1] == '\n') {
            buf[len_buf - 1] = '\0';
        }
        safe_recursive_add_node(&root, create_node(buf));
    }

    for (int i = 0; i < words; ++i) {
        safe_recursive_add_node(&root, &node_set[i]);
    }
    printf("albero ok\n");

    safe_recursive_print_tree(root, f);
    fclose(f);
    fclose(fstem);
    free(node_set);
    return 0;
}

