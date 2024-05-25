/////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//              Vito Giacalone  (546646)                \\
/////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\


//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\

#include <stdio.h>
#include "bstree.h"
#include "mpi.h"
#include <limits.h>
#include "enquiry.h"


//////////////////////// MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\

int main(int argc, char *argv[]) {

    //////////////////////// MPI LAYER INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\
    // Initialize the MPI environment and obtain the rank and size of the processes
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // printf("rank: %d argc %d\n", rank, argc); // Print the rank and number of arguments for debugging

    //////////////////////// PATH-BUILDING FOR IO OPERATIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\
    // Define base paths and extensions for various file operations
    int words = 0;
    char path_ix[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/InvertedIndex/inverted_idx_"; // Base path for reading inverted index
    char path_sw[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/Preprocess/stemwords_";       // Base path for reading stemmed words
    char path_dest_tree[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/output/bst_";                        // Base path for saving the BST
    //char path_res[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/output/res_";                              // Base path for result files
    char path_map[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/Documents/mapping_";         // Base path for document mapping
    char ext[] = ".txt";                                                                                        // File extension for all paths
    char complete_path_ix[NAME_MAX];                                                                            
    char complete_path_sw[NAME_MAX];                                                                            
    char complete_path_bst[NAME_MAX];                                                                           
    //char complete_path_result[NAME_MAX];
    char complete_path_map[NAME_MAX];                                                                         
    sprintf(complete_path_ix, "%s%d%s", path_ix, rank, ext); // Complete path for inverted index reading
    sprintf(complete_path_sw, "%s%d%s", path_sw, rank, ext); // Complete path for reading stemmed words
    sprintf(complete_path_bst, "%s%d%s", path_dest_tree, rank, ext); // Complete path for BST storing
    //sprintf(complete_path_result, "%s%d%s", path_res, rank, ext); // Complete path for result files
    sprintf(complete_path_map, "%s%d%s", path_map, rank, ext); // Complete path for document mapping

    //////////////////////// FILES OPENING \\\\\\\\\\\\\\\\\\\\\\\\\\\
    // Open necessary files for reading and writing
    FILE *fstem = fopen(complete_path_sw, "r");
    FILE *f = fopen(complete_path_bst, "w");

    if (fstem == NULL || f == NULL) {
        perror("Errore nell'apertura dei file"); // Print error if file opening fails
        MPI_Finalize(); // Finalize MPI processes
        return EXIT_FAILURE;
    }

    //////////////////////// BST BUILDING \\\\\\\\\\\\\\\\\\\\\\\\\\\
    // Build the binary search tree (BST) from the stemmed words
    node *root = NULL; // Root of the BST
    char buf[SIZE]; // Temporary buffer for storing a row of the file
    while (fgets(buf, sizeof(buf), fstem)) { // Read each stemmed word
        size_t len_buf = strlen(buf); // Compute the length of the stemmed word
        if (len_buf > 0 && buf[len_buf - 1] == '\n') { // Remove \n characters
            buf[len_buf - 1] = '\0';
        }
        safe_recursive_add_node(&root, create_node(buf)); // Add a node to the BST with the stemmed word
    }
    fclose(fstem); // Close the stemmed word file

    node *node_set = build_bst(complete_path_ix, &words); // Create an array with all nodes to insert in the BST from the inverted index

    for (int i = 0; i < words; ++i) { // For each node to insert
        safe_recursive_add_node(&root, &node_set[i]); // Add the node to the BST
    }
 
    // Write the BST to a file
    safe_recursive_print_tree(root, f); // Print the BST to a file
    fclose(f); // Close the file containing the new BST
    
    ////////////////////////// EXACT SEARCH \\\\\\\\\\\\\\\\\\\\\\\\\\\\
    // Perform exact search on the BST

    int elements = 0;
    int key = 0;
    char **idmap = map_ids(complete_path_map, &elements, &key); // Map document IDs


    lower_string(argv[1]); // Convert the search key to lowercase
    node *result = get_node(root, argv[1]); // Get the node corresponding to the search key
    int result_set_size = 0;

    int type_of_query;

    if (rank == 0)
    {
        printf("Choose the type of query:\n1 Disjunctive\n2 Conjunctive\n");
        scanf("%d", &type_of_query);
    }
    
    MPI_Bcast(&type_of_query, 1, MPI_INT, 0, MPI_COMM_WORLD);

    switch(type_of_query) {

        case 1:
            disjunctive_query(root, idmap, key, &argv[1], argc-1, rank);
        case 2:
            conjunctive_query(root, idmap, key, &argv[1], argc-1, rank);
    }

    // Execute different types of queries based on the search

    // search_query(idmap, key, result, rank);
    // 
    

    // Free the memory allocated for the tree
    // safe_recursive_free_tree(root);

    MPI_Finalize(); // Finalize MPI processes
    return EXIT_SUCCESS;
}
