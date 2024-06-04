#include "enquiry.h"

// Function to convert a string to lowercase
void lower_string(char *input) {
    int len = strlen(input); // Get the length of the input string
    for (int i = 0; i < len; ++i) {
        input[i] = tolower(input[i]); // Convert each character to lowercase
    }
}


void conjunctive_query(node *root, char **map, int key, char **searchKeys, int num_keys, int rank) {
    if (num_keys < 1) return;

    list *posting_lists[num_keys];
    for (int i = 0; i < num_keys; i++) {
        node *r = get_node(root, searchKeys[i]);
        if (r == NULL) return;
        posting_lists[i] = r->posting;
    }

    while (1) {
        int min_doc_ID = posting_lists[0]->doc_ID;
        int max_doc_ID = posting_lists[0]->doc_ID;

        for (int i = 1; i < num_keys; i++) {
            if (posting_lists[i]->doc_ID < min_doc_ID) {
                min_doc_ID = posting_lists[i]->doc_ID;
            }
            if (posting_lists[i]->doc_ID > max_doc_ID) {
                max_doc_ID = posting_lists[i]->doc_ID;
            }
        }

        if (min_doc_ID == max_doc_ID) {
            print_document(map, min_doc_ID, key, rank);
            for (int i = 0; i < num_keys; i++) {
                posting_lists[i] = posting_lists[i]->next;
                if (posting_lists[i] == NULL) return;
            }
        } else {
            for (int i = 0; i < num_keys; i++) {
                while (posting_lists[i] != NULL && posting_lists[i]->doc_ID < max_doc_ID) {
                    posting_lists[i] = posting_lists[i]->next;
                }
                if (posting_lists[i] == NULL) return;
            }
        }
    }
}

void disjunctive_query(node *root, char **map, int key, char **searchKeys, int num_keys, int rank) {
    if (num_keys < 1) return;

    list *posting_lists[num_keys];
    int num_active_lists = 0;
    
    for (int i = 0; i < num_keys; i++) {
        node *r = get_node(root, searchKeys[i]);
        if (r != NULL && r->posting != NULL) {
            posting_lists[num_active_lists++] = r->posting;
        }
    }

    if (num_active_lists == 0) return;

    while (1) {
        int min_doc_ID = posting_lists[0]->doc_ID;
        for (int i = 1; i < num_active_lists; i++) {
            if (posting_lists[i]->doc_ID < min_doc_ID) {
                min_doc_ID = posting_lists[i]->doc_ID;
            }
        }

        print_document(map, min_doc_ID, key, rank);

        int num_finished_lists = 0;
        for (int i = 0; i < num_active_lists; i++) {
            if (posting_lists[i]->doc_ID == min_doc_ID) {
                posting_lists[i] = posting_lists[i]->next;
                if (posting_lists[i] == NULL) {
                    for (int j = i; j < num_active_lists - 1; j++) {
                        posting_lists[j] = posting_lists[j + 1];
                    }
                    num_active_lists--;
                    i--;
                }
            }
        }

        if (num_active_lists == 0) break;
    }
}
