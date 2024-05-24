//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "bstree.h"

//////////////////////// Create node \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function create a new node for the binary search tree

node *create_node(char *searchKey) {

    node *n = (node*)malloc(sizeof(node));
    n->searchKey = strdup(searchKey);
    n->left = NULL;
    n->right = NULL;
    n->posting = NULL;
    return n;
}

//////////////////////// Adding an element \\\\\\\\\\\\\\\\\\\\\\\\\
// This function adds a new element to the binary search tree
// If the bst is empty, add directly the node. The stack
// is an auxiliary data structure used to keep track
// of the visited nodes. Once that the correct place where to insert
// the node has been found, add the element.
// If there's a duplicate, the posting list is updated

void safe_recursive_add_node(node **root, node *newnode) {
    if (*root == NULL) {

        *root = newnode;
        return;
    }

    record *stack = NULL;
    push(&stack, *root);

    while (!is_empty(stack)) {
        
        record *current_record = pop(&stack);
        node *current = current_record->memaddr;
        free(current_record);

        int cmp = strcmp(newnode->searchKey, current->searchKey);

        if (cmp < 0) {

            if (current->left == NULL) {

                current->left = newnode;
                return;
            } else {

                push(&stack, current->left);
            }
        } else if (cmp > 0) {

            if (current->right == NULL) {

                current->right = newnode;
                return;
            } else {

                push(&stack, current->right);
            }
        } else {

            list *new_posting = newnode->posting;
            while (new_posting != NULL) {

                add_id(&current->posting, new_posting->doc_ID);
                new_posting = new_posting->next;
            }
            free(newnode->searchKey);
            return;
        }
    }
}
    
////////////////////////// Print the bst \\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// if the tree is empty return. Otherwise create a stack and insert
// each visited node on top of the stack. For each node on the stack
// check its right and left children and print them and the
// associated posting lists

void safe_recursive_print_tree(node *root, FILE* f) {
    if (root == NULL) {
  
        return;
    }

    record *stack = NULL;
    node *current = root;

    while (current != NULL || !is_empty(stack)) {
  
        while (current != NULL) {
  
            push(&stack, current);
            current = current->left;
        }

        record *top_record = pop(&stack);
        current = top_record->memaddr;
        free(top_record);

        fprintf(f, "%s : ", current->searchKey);
        print_list(current->posting, f);

        current = current->right;
    }
}

//////////////////////// BST node deletion \\\\\\\\\\\\\\\\\\\\\\\\\\\
// If the tree is empty, return. Otherwise, traverse the tree and keep
// track of the visited nodes using a stack. Once the node has been
// removed, fill the "hole" with the smallest node of the right
// child of the removed node.

void safe_recursive_delete_node(node **root, char* searchKey) {
    if (*root == NULL) {
        
        return;
    }

    record *stack = NULL;
    push(&stack, *root);
    node *parent = NULL;

    while (!is_empty(stack)) {
        
        record *current_record = pop(&stack);
        node *current = current_record->memaddr;
        free(current_record);

        if (strcmp(searchKey, current->searchKey) < 0) {
            
            if (current->left != NULL) {
            
                push(&stack, current->left);
                parent = current;
            }
        } else if (strcmp(searchKey, current->searchKey) > 0) {
            
            if (current->right != NULL) {
            
                push(&stack, current->right);
                parent = current;
            }
        } else {
            
            if (current->left == NULL && current->right == NULL) {
            
                if (parent == NULL) {
                    *root = NULL;
                } else if (parent->left == current) {
            
                    parent->left = NULL;
                } else {
            
                    parent->right = NULL;
                }
                free(current->searchKey);
                free(current);
                return;
            } else if (current->left == NULL) {
            
                if (parent == NULL) {
            
                    *root = current->right;
                } else if (parent->left == current) {
            
                    parent->left = current->right;
                } else {
            
                    parent->right = current->right;
                }
                free(current->searchKey);
                free(current);
                return;
            } else if (current->right == NULL) {
            
                if (parent == NULL) {
            
                    *root = current->left;
                } else if (parent->left == current) {
            
                    parent->left = current->left;
                } else {
            
                    parent->right = current->left;
                }
                free(current->searchKey);
                free(current);
                return;
            } else {
                node *successor_parent = current;
                node *successor = current->right;
            
                while (successor->left != NULL) {
            
                    successor_parent = successor;
                    successor = successor->left;
                }

                if (successor_parent != current) {
            
                    successor_parent->left = successor->right;
                } else {
            
                    successor_parent->right = successor->right;
                }

                free(current->searchKey);
                current->searchKey = strdup(successor->searchKey);
                free(successor->searchKey);
                free(successor);
                return;
            }
        }
    }
}


//////////////////////// Create Nodes \\\\\\\\\\\\\\\\\\\\\\\\\\\
// This function creates all the nodes to insert in the BST
// The Searchkeys are read from a file and also the posting lists
// The function returns an array of nodes

node *build_bst(char *filename, int *len) {
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
            (*len)++;           //increments the counter of words in the tree
        } else {
            if (*len > 0) {
                add_id(&(node_set[*len - 1].posting), atoll(buf));          // i have to use *len -1 because above I already incremented
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
