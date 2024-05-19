//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "stack.h"
#include "bstree.h"

//////////////////////// Record creation \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function create a record for the stack

record *create_record(struct node *mem) {
    record *r = (record*)malloc(sizeof(record));
    r->memaddr = mem;
    r->next = NULL;
    return r;
}

//////////////////////// Looks at the top of the stack \\\\\\\\\\\\\\\\\\\\\\\\\\\

struct node *head_of_stack(record *tos) {
    return tos->memaddr;
}

//////////////////////// Push \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function push an element on the stack

record *push(record **top, struct node *addr) {
    if (*top == NULL) { // stack is empty
        *top = create_record(addr);
        return *top;
    }

    record *newtop = create_record(addr);
    newtop->next = *top;
    *top = newtop;
    return *top;
}

//////////////////////// Pop \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function remove the first element from the top 
// of the stack.

record *pop(record **top) {
    if (*top == NULL) {
        return NULL;
    }

    record *head = *top;
    *top = (*top)->next;
    head->next = NULL;
    return head;
}

//////////////////////// Check if the stack is empty \\\\\\\\\\\\\\\\\\\\\\\\\\\

int is_empty(record *top) {
    return (top == NULL);
}
