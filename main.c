/////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////// Vito Giacalone  (546646) \\\\\\\\\\\\\\\\
/////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\


//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\

#include <stdio.h>
#include "bstree.h"
#include "mpi.h"
#include <limits.h>

//////////////////////// MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\

int main(int argc, char *argv[]) {


    //////////////////////// MPI LAYER INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    //////////////////////// PATH-BUILDING FOR IO OPERATIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\

    int words = 0;
    char path_ix[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/InvertedIndex/inverted_idx_"; // BASE-PATH FOR INVERTED INDEX READING
    char path_sw[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/Preprocess/stemwords_";       // BASE-PATH FOR STEMMED WORDS READING
    char path_dest_tree[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/output/bst_";                        // BASE-PATH FOR SAVING INDEX ON VIRTUAL MEMORY
    char path_debug[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/output/debug_";                          // BASE-PATH FOR DEBUG FILES
    char ext[] = ".txt";                                                                                        // EXTENSION OF THE FILES FOR IO
    char complete_path_ix[NAME_MAX];                                                                            
    char complete_path_sw[NAME_MAX];                                                                            
    char complete_path_bst[NAME_MAX];                                                                           
    char complete_path_debug[NAME_MAX];                                                                         
    sprintf(complete_path_ix, "%s%d%s", path_ix, rank, ext);                                                    // FULL PATH FOR INVERTED INDEX READING
    sprintf(complete_path_sw, "%s%d%s", path_sw, rank, ext);                                                    // FULL PATH FOR READING STEMMED WORDS ASSOCIATED TO THE SET OF FILES OF EACH PROCESS
    sprintf(complete_path_bst, "%s%d%s", path_dest_tree, rank, ext);                                            // FULL PATH BST STORING
    sprintf(complete_path_debug, "%s%d%s", path_debug, rank, ext);                                              // FULL PATH FOR DEBUG FILE


    //////////////////////// FILES OPENING \\\\\\\\\\\\\\\\\\\\\\\\\\\
    
    FILE *fstem = fopen(complete_path_sw, "r");
    FILE *f = fopen(complete_path_bst, "w");
    FILE *fdeg = fopen(complete_path_debug, "w");

    if (fstem == NULL || f == NULL) {
        perror("Errore nell'apertura dei file");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    //////////////////////// BST BUILDING \\\\\\\\\\\\\\\\\\\\\\\\\\\

    node *root = NULL;                                                                                          // ROOT OF BST
    char buf[SIZE];                                                                                             // TEMPORARY BUFFER FOR STORING A ROW OF THE FILE
    while (fgets(buf, sizeof(buf), fstem)) {                                                                    // FOR EACH STEMMED WORD
        size_t len_buf = strlen(buf);                                                                           // COMPUTE THE LENGTH OF THE STEMMED WORD
        if (len_buf > 0 && buf[len_buf - 1] == '\n') {                                                          // REMOVE \n CHARACTERS AND REPLACE WITH \0
            buf[len_buf - 1] = '\0';
        }
        safe_recursive_add_node(&root, create_node(buf));                                                       // ADD TO THE BST A NODE WITH THE STEMMED WORD IN ORDER TO POPULATE THE TREE WITH ELEMENTS TO GUIDE THE SEARCH
    }
    fclose(fstem);                                                                                              // CLOSE THE STEMWORD FILE

    node *node_set = build_bst(complete_path_ix, &words);                                                       // CREATE AN ARRAY WITH ALL THE NODES TO INSERT IN THE BST. THE NODES ARE READ FROM THE INVERTED INDEX PREVIOUSLY BUILT

    
    for (int i = 0; i < words; ++i)                                                                             // FOR EACH NODE TO INSERT
    {
        fprintf(fdeg, "%s\n", node_set[i].searchKey);                                                           // PRINT THE DEBUG FILE (DEBUG FILE CONTAINS ALL THE SEARCH KEYS OF THE TREE)
    }
    fclose(fdeg);                                                                                               // CLOSE THE DEBUG FILE
    

    for (int i = 0; i < words; ++i) {                                                                           // FOR EACH NODE TO INSERT
        safe_recursive_add_node(&root, &node_set[i]);                                                           // ADD THE NODE TO THE BST
    }

    // Scrittura dell'albero su file
    safe_recursive_print_tree(root, f);                                                                         // PRINT THE BST ON A FILE
    fclose(f);                                                                                                  // CLOSE THE FILE THAT CONTAINS THE NEW BST
    

    // // Libera la memoria allocata per l'albero
    // safe_recursive_free_tree(root);

    MPI_Finalize();                                                                                             // CLOSE MPI PROCESSES
    return EXIT_SUCCESS;
}