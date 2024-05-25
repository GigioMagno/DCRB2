//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "stack.h"
#include "bstree.h"

//////////////////////// Record creation \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function creates a record for the stack.
// A record contains a pointer to a tree node and a pointer to the next record in the stack.

record *create_record(struct node *mem) {
    // Allocate memory for a new record
    record *r = (record*)malloc(sizeof(record));
    
    // Initialize the record's fields
    r->memaddr = mem;
    r->next = NULL;
    
    // Return the newly created record
    return r;
}

//////////////////////// Looks at the top of the stack \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function returns the memory address of the node at the top of the stack.

struct node *head_of_stack(record *tos) {
    return tos->memaddr;
}

//////////////////////// Push \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function pushes an element onto the stack.
// If the stack is empty, create the first record.
// Otherwise, create a new record and add it to the top of the stack.

record *push(record **top, struct node *addr) {
    if (*top == NULL) { // Check if the stack is empty
        *top = create_record(addr); // Create the first record
        return *top;
    }

    // Create a new record and add it to the top of the stack
    record *newtop = create_record(addr);
    newtop->next = *top;
    *top = newtop;
    return *top;
}

//////////////////////// Pop \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function removes the first element from the top of the stack and returns it.
// If the stack is empty, return NULL.

record *pop(record **top) {
    if (*top == NULL) { // Check if the stack is empty
        return NULL;
    }

    // Remove the top element and update the top of the stack
    record *head = *top;
    *top = (*top)->next;
    head->next = NULL;
    
    // Return the removed record
    return head;
}

//////////////////////// Check if the stack is empty \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function checks if the stack is empty and returns 1 if it is, otherwise returns 0.

int is_empty(record *top) {
    return (top == NULL);
}
