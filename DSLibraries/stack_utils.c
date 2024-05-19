//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "stack.h"
#include "bstree.h"

//////////////////////// Print the stack \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function print the stack (position - address in the record)

void print_stack2(record *top) {
    if (top == NULL) {
        return;
    }
    int i = 0;
    while (top != NULL) {
        printf("%d-%p\n", i, top->memaddr);
        i++;
        top = top->next;
    }
}

///////////////////////////// Print the stack \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function print the stack (position - searchkey - address in the record)

void print_stack(record *top) {
    if (top == NULL) {
        return;
    }
    int i = 0;
    while (top != NULL) {
        printf("%d-%s-%p\n", i, top->memaddr->searchKey, top->memaddr);
        i++;
        top = top->next;
    }
}
