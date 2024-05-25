//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "stack.h"
#include "bstree.h"

//////////////////////// Print the stack \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function prints the stack with the position and address of each record.
// It is useful for debugging to see the order and addresses of the elements in the stack.

void print_stack2(record *top) {
    if (top == NULL) { // Check if the stack is empty
        return; // If the stack is empty, do nothing
    }
    int i = 0; // Initialize a counter to track the position in the stack
    while (top != NULL) { // Iterate through the stack until the end
        printf("%d-%p\n", i, top->memaddr); // Print the position and address of the current record
        i++; // Increment the counter
        top = top->next; // Move to the next record in the stack
    }
}

///////////////////////////// Print the stack with search keys \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function prints the stack with the position, search key, and address of each record.
// It provides more detailed information by also showing the search key of each node in the stack.

void print_stack(record *top) {
    if (top == NULL) { // Check if the stack is empty
        return; // If the stack is empty, do nothing
    }
    int i = 0; // Initialize a counter to track the position in the stack
    while (top != NULL) { // Iterate through the stack until the end
        printf("%d-%s-%p\n", i, top->memaddr->searchKey, top->memaddr); // Print the position, search key, and address of the current record
        i++; // Increment the counter
        top = top->next; // Move to the next record in the stack
    }
}
