//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "posting_list.h"

//////////////////////// Append an element \\\\\\\\\\\\\\\\\\\\\\\\\\\
// Create a node with content the ID number specified in the prototype
// If the node points to null, it's possible to append the new node
// directly because the list is empty. Otherwise, if the list contains
// some values, navigate through the list and reach the end, then
// append the new element

void add_id(list **l, long long int ID) {

    list *new_node = (list *)malloc(sizeof(list));

    if (new_node == NULL) {

        printf("Memoria insufficiente\n");
        return;
    }

    new_node->doc_ID = ID;
    new_node->next = NULL;

    if (*l == NULL) {

        *l = new_node;
        return;
    }

    list *current = *l;
    
    while (current->next != NULL) {
        
        current = current->next;
    }
    current->next = new_node;
}

////////////////////// Print the posting lsit \\\\\\\\\\\\\\\\\\\\\\\\
// Navigate through the list and print on a file 
// each element of the list

void print_list(const list *l, FILE *fdest) {
    while (l != NULL) {
        fprintf(fdest, "%lld ---> ", l->doc_ID);
        l = l->next;
    }
    fprintf(fdest, "NULL\n");
}

void free_list(list *l) {
    list *temp;
    while (l != NULL) {
        temp = l;
        l = l->next;
        free(temp);
    }
}
