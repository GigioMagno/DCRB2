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

//////////////////////// Map IDs \\\\\\\\\\\\\\\\\\\\\\\\\\\
// Read a file and create a mapping of IDs to names.
// The function returns an array of strings containing the names,
// and updates the number of elements and the key of the first element.
char **map_ids(char *path, int *num, int *key) {
    int elements = 8;  // Initial capacity for the map
    FILE *f = fopen(path, "r");  // Open the file for reading
    char buf[NAME_MAX + sizeof(long long int)];
    char **map = (char**)malloc(elements * sizeof(char*));  // Allocate memory for the map

    // Allocate memory for each string in the map
    for (int i = 0; i < elements; ++i) {
        map[i] = (char*)malloc(sizeof(char) * NAME_MAX);
    }

    int i = 0;  // Counter for the number of elements
    int id;  // Variable to hold the ID

    // Read the file line by line
    while (fgets(buf, NAME_MAX + sizeof(long long int), f)) {
        // Parse the line to extract the ID and the name
        sscanf(buf, "%d\t%s", &id, map[i]);
        i++;

        // Set the key to the first ID encountered
        if (i == 1) {
            *key = id;
        }

        // If the array is full, double its capacity
        if (i == elements) {
            elements *= 2;
            map = (char**)realloc(map, sizeof(char*) * elements);
            for (int j = 0; j < elements; ++j) {
                map[j] = realloc(map[j], sizeof(char) * NAME_MAX);
            }
        }
    }

    // Update the number of elements
    *num = i;

    // Reallocate the map to fit the number of elements exactly
    return realloc(map, i * sizeof(char*));
}
