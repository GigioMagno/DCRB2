//////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\\\\\

#include <stdio.h>
#include "bstree.h"
#include "mpi.h"
#include <limits.h>

//////////////////////// MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\

int main(int argc, char *argv[]) {
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int words = 0;

    // Costruzione dei percorsi per i file
    char path_ix[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/InvertedIndex/inverted_idx_";
    char path_sw[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/Preprocess/stemwords_";
    char path_dest_tree[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/output/bst_";
    char path_debug[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/output/debug_";
    char ext[] = ".txt";
    char complete_path_ix[NAME_MAX];
    char complete_path_sw[NAME_MAX];
    char complete_path_bst[NAME_MAX];
    char complete_path_debug[NAME_MAX];
    sprintf(complete_path_ix, "%s%d%s", path_ix, rank, ext);
    sprintf(complete_path_sw, "%s%d%s", path_sw, rank, ext);
    sprintf(complete_path_bst, "%s%d%s", path_dest_tree, rank, ext);
    sprintf(complete_path_debug, "%s%d%s", path_debug, rank, ext);


    // Apertura dei file
    FILE *fstem = fopen(complete_path_sw, "r");
    FILE *f = fopen(complete_path_bst, "w");
    if (fstem == NULL || f == NULL) {
        perror("Errore nell'apertura dei file");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Costruzione dell'albero binario di ricerca
    node *root = NULL;
    char buf[SIZE];
    while (fgets(buf, sizeof(buf), fstem)) {
        size_t len_buf = strlen(buf);
        if (len_buf > 0 && buf[len_buf - 1] == '\n') {
            buf[len_buf - 1] = '\0';
        }
        safe_recursive_add_node(&root, create_node(buf));
    }
    fclose(fstem);

    node *node_set = build_bst(complete_path_ix, &words);

    FILE *fdeg = fopen(complete_path_debug, "w");
    for (int i = 0; i < words; ++i)
    {
        fprintf(fdeg, "%s\n", node_set[i].searchKey);
    }

    fclose(fdeg);

    for (int i = 0; i < words; ++i) {
        safe_recursive_add_node(&root, &node_set[i]);
    }

    // Scrittura dell'albero su file
   safe_recursive_print_tree(root, f);
   fclose(f);

    // // Libera la memoria allocata per l'albero
    // safe_recursive_free_tree(root);

    MPI_Finalize();
    return EXIT_SUCCESS;
}