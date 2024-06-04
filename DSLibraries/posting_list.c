//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "posting_list.h"

//////////////////////// Append an element \\\\\\\\\\\\\\\\\\\\\\\\\\\
// Create a node with the specified ID number and append it to the list.
// If the list is empty, the new node is the first node of the list.
// Otherwise, navigate to the end of the list and append the new node.
void add_id(list **l, int ID) {
    // Allocate memory for the new node
    list *new_node = (list *)malloc(sizeof(list));

    // Check if memory allocation was successful
    if (new_node == NULL) {
        printf("Memoria insufficiente\n");
        return;
    }

    // Initialize the new node
    new_node->doc_ID = ID;
    new_node->next = NULL;

    // If the list is empty, set the new node as the first node
    if (*l == NULL) {
        *l = new_node;
        return;
    }

    // Traverse the list to find the last node
    list *current = *l;
    while (current->next != NULL) {
        current = current->next;
    }

    // Append the new node at the end of the list
    current->next = new_node;
}

////////////////////// Print the posting list \\\\\\\\\\\\\\\\\\\\\\\\
// Navigate through the list and print each element's ID to a file.
void print_list(list *l, FILE *fdest) {
    // Traverse the list
    while (l != NULL) {
        // Print the current node's ID
        fprintf(fdest, "%d ---> ", l->doc_ID);
        l = l->next;
    }
    // Print the end of the list
    fprintf(fdest, "NULL\n");
}

////////////////////// Print the posting list with results count \\\\\\\\\\\\\\\\\\\\\\\\
// Navigate through the list, print each element's ID to a file,
// and count the number of elements in the list.
void print_list2(list *l, FILE *fdest, int *result_set) {
    // Traverse the list
    while (l != NULL) {
        // Print the current node's ID
        fprintf(fdest, "%d", l->doc_ID);
        // Increment the result set counter
        (*result_set)++;
        l = l->next;
    }
}

//////////////////////// Free the posting list \\\\\\\\\\\\\\\\\\\\\\\\\\\
// Free all the nodes in the list to prevent memory leaks.
void free_list(list *l) {
    list *temp;
    // Traverse the list
    while (l != NULL) {
        // Save the current node
        temp = l;
        // Move to the next node
        l = l->next;
        // Free the saved node
        free(temp);
    }
}


char **map_ids_from_paths(char **paths, int num_paths, int *total_num, int *key) {
    int elements = 8;  // Initial capacity for the map
    char buf[NAME_MAX];
    char **map = (char**)malloc(elements * sizeof(char*));
    char discarded[NAME_MAX];
    // Allocate memory for each string in the map
    for (int i = 0; i < elements; ++i) {
        map[i] = (char*)malloc(NAME_MAX * sizeof(char));
    }

    int i = 0;  // Counter for the number of elements
    int id;  // Variable to hold the ID

    for (int p = 0; p < num_paths; ++p) {
        FILE *f = fopen(paths[p], "r");
        if (!f) {
            fprintf(stderr, "Error opening file: %s\n", paths[p]);
            continue;
        }

        while (fgets(map[i], NAME_MAX, f)) {
            sscanf(map[i], "%d\t%s", &id, discarded);
            if (i == 0) {
                *key = id;
            }
            i++;
            if (i == elements) {
                elements *= 2;
                map = (char**)realloc(map, elements * sizeof(char*));
                for (int j = elements / 2; j < elements; ++j) {
                    map[j] = (char*)malloc(NAME_MAX * sizeof(char));
                }
            }
        }

        fclose(f);
    }
    *key = id - *key + 1;
    //printf("KEY:%d\n", *key);
    *total_num = i;
    return (char**)realloc(map, i * sizeof(char*));
}