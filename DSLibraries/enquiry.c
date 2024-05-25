#include "enquiry.h"

// Function to convert a string to lowercase
void lower_string(char *input) {
    int len = strlen(input); // Get the length of the input string
    for (int i = 0; i < len; ++i) {
        input[i] = tolower(input[i]); // Convert each character to lowercase
    }
}

// Function to print a document based on its ID
void print_document(char **map, int doc_ID, int key, int rank) {
    if (rank != 0) {
        printf("%s\n", map[doc_ID % key]); // Print the document if rank is not 0
    } else {
        printf("%s\n", map[doc_ID - 1]); // Adjust document ID for rank 0
    }
}

// Function to perform a conjunctive query on the BST
void conjunctive_query(node *root, char **map, int key, char **searchKeys, int num_keys, int rank) {
    if (num_keys < 1) return; // Exit if there are no search keys

    // Retrieve posting lists for each search key
    list *posting_lists[num_keys];
    for (int i = 0; i < num_keys; i++) {
        node *r = get_node(root, searchKeys[i]); // Get the node for the current search key
        if (r == NULL) {
            return; // Exit if any search key is not found
        }
        posting_lists[i] = r->posting; // Store the posting list
    }

    while (1) {
        // Find the minimum and maximum doc_IDs among the current heads of the posting lists
        int min_doc_ID = posting_lists[0]->doc_ID;
        int max_doc_ID = posting_lists[0]->doc_ID;

        for (int i = 1; i < num_keys; i++) {
            if (posting_lists[i]->doc_ID < min_doc_ID) {
                min_doc_ID = posting_lists[i]->doc_ID; // Update minimum doc_ID
            }
            if (posting_lists[i]->doc_ID > max_doc_ID) {
                max_doc_ID = posting_lists[i]->doc_ID; // Update maximum doc_ID
            }
        }

        if (min_doc_ID == max_doc_ID) {
            // All lists point to the same doc_ID
            print_document(map, min_doc_ID, key, rank); // Print the document
            for (int i = 0; i < num_keys; i++) {
                posting_lists[i] = posting_lists[i]->next; // Move to the next element in each list
                if (posting_lists[i] == NULL) return; // Exit if any list is exhausted
            }
        } else {
            // Advance all lists that point to the minimum doc_ID
            for (int i = 0; i < num_keys; i++) {
                while (posting_lists[i] != NULL && posting_lists[i]->doc_ID < max_doc_ID) {
                    posting_lists[i] = posting_lists[i]->next; // Move to the next element
                }
                if (posting_lists[i] == NULL) return; // Exit if any list is exhausted
            }
        }
    }
}

// Function to perform a disjunctive query on the BST
void disjunctive_query(node *root, char **map, int key, char **searchKeys, int num_keys, int rank) {
    if (num_keys < 1) return; // Exit if there are no search keys

    // Retrieve posting lists for each search key
    list *posting_lists[num_keys];
    int num_active_lists = 0;
    
    for (int i = 0; i < num_keys; i++) {
        node *r = get_node(root, searchKeys[i]); // Get the node for the current search key
        if (r != NULL && r->posting != NULL) {
            posting_lists[num_active_lists++] = r->posting; // Store the posting list if it's not NULL
        }
    }

    if (num_active_lists == 0) return; // Exit if no active lists

    while (1) {
        // Find the minimum doc_ID among the current heads of the active posting lists
        int min_doc_ID = posting_lists[0]->doc_ID;
        for (int i = 1; i < num_active_lists; i++) {
            if (posting_lists[i]->doc_ID < min_doc_ID) {
                min_doc_ID = posting_lists[i]->doc_ID; // Update minimum doc_ID
            }
        }

        // Print the document with the minimum doc_ID
        print_document(map, min_doc_ID, key, rank);

        // Advance all lists that point to the minimum doc_ID
        int num_finished_lists = 0;
        for (int i = 0; i < num_active_lists; i++) {
            if (posting_lists[i]->doc_ID == min_doc_ID) {
                posting_lists[i] = posting_lists[i]->next; // Move to the next element
                if (posting_lists[i] == NULL) {
                    // Shift remaining lists to fill the gap
                    for (int j = i; j < num_active_lists - 1; j++) {
                        posting_lists[j] = posting_lists[j + 1];
                    }
                    num_active_lists--; // Decrease the number of active lists
                    i--; // Recheck the new list at this position
                }
            }
        }

        if (num_active_lists == 0) break; // Exit if no active lists remain
    }
}
