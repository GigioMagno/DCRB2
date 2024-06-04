    /////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
    //              Vito Giacalone  (546646)                \\
    /////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\


    //////////////////////// Libraries \\\\\\\\\\\\\\\\\\\\\\\

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <limits.h>
#include "posting_list.h"
#include "enquiry.h"
#include "bstree.h"
#include <stdbool.h>

#define NAME_MAX 255

bool *printed_docs;

char** distribute_paths(const char* base_path, const char* ext, int total_paths, int rank, int size, int* my_num_paths) {
    int num_paths_per_proc = total_paths / size;
    int remainder = total_paths % size;

    *my_num_paths = (rank < remainder) ? num_paths_per_proc + 1 : num_paths_per_proc;

    char** core_paths = (char**)malloc(*my_num_paths * sizeof(char*));
    for (int i = 0; i < *my_num_paths; i++) {
        core_paths[i] = (char*)malloc(NAME_MAX * sizeof(char));
    }

    if (rank == 0) {
        char total_paths_arr[total_paths][NAME_MAX];

        // Creazione dei path completi
        for (int i = 0; i < total_paths; ++i) {
            snprintf(total_paths_arr[i], NAME_MAX, "%s%d%s", base_path, i, ext);
        }

        int offset = 0;
        for (int i = 0; i < size; i++) {
            int send_count = (i < remainder) ? num_paths_per_proc + 1 : num_paths_per_proc;
            if (i == 0) {
                for (int j = 0; j < send_count; j++) {
                    strcpy(core_paths[j], total_paths_arr[offset + j]);
                }
            } else {
                // Corretto il modo di inviare i path
                for (int j = 0; j < send_count; j++) {
                    MPI_Send(total_paths_arr[offset + j], NAME_MAX, MPI_CHAR, i, 0, MPI_COMM_WORLD);
                }
            }
            offset += send_count;
        }
    } else {
        for (int i = 0; i < *my_num_paths; i++) {
            MPI_Recv(core_paths[i], NAME_MAX, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    return core_paths;
}

void initialize_printed_docs(int size) {
    printed_docs = (bool *)calloc(size, sizeof(bool));
}

void free_printed_docs() {
    free(printed_docs);
}

void print_document(char **map, int doc_ID, int key, int rank) {
    int index = (rank != 0) ? doc_ID % key : doc_ID - 1;
    if (index < 0 || index >= key) return;

    if (!printed_docs[index]) {
        printf("%s\n", map[index]);
        printed_docs[index] = true;
    }
}


int main(int argc, char *argv[]) {
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char base_path_inverted_indexes[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/InvertedIndex/inverted_idx_";
    char base_path_sw[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/Preprocess/stemwords_";
    char base_path_dest_tree[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/output/bst_";                        
    char base_path_map[] = "/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/Documents/mapping_"; 
    char ext[] = ".txt";
    char writeout_tree[NAME_MAX];

    int paths_num;
    char** paths_rank_inverted_index = distribute_paths(base_path_inverted_indexes, ext, 12, rank, size, &paths_num);
    char** paths_rank_stemwords = distribute_paths(base_path_sw, ext, 12, rank, size, &paths_num);
    char** paths_rank_map = distribute_paths(base_path_map, ext, 12, rank, size, &paths_num);

    node *root = NULL;

    // Ogni processo stampa i path che ha ricevuto
    //printf("Processo %d ha ricevuto %d path:\n", rank, paths_num);
    // for (int i = 0; i < paths_num; i++) {
    //     printf("%s\n", paths_rank_inverted_index[i]);
    //     printf("%s\n", paths_rank_stemwords[i]);
    //     printf("%s\n", paths_rank_map[i]);
    // }

    char buf[512]; 

    for (int i = 0; i < paths_num; ++i) {
        printf("Processing file: %s\n", paths_rank_stemwords[i]);
        FILE *fstem = fopen(paths_rank_stemwords[i], "r");
        if (fstem == NULL) {
            printf("Null pointer exception\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        while (fgets(buf, sizeof(buf), fstem)) {
            size_t len_buf = strlen(buf); 
            if (len_buf > 0 && buf[len_buf - 1] == '\n') { 
                buf[len_buf - 1] = '\0';
            }
            safe_recursive_add_node(&root, create_node(buf)); 
        }
        fclose(fstem);
    }

    int words = 0;
    for (int i = 0; i < paths_num; ++i) {
        node *node_set = build_bst(paths_rank_inverted_index[i], &words);
        for (int j = 0; j < words; ++j) {
            safe_recursive_add_node(&root, &node_set[j]);
        }
    }

    snprintf(writeout_tree, NAME_MAX, "%s%d%s", base_path_dest_tree, rank, ext);
    FILE *ftree = fopen(writeout_tree, "w");
    safe_recursive_print_tree(root, ftree);
    fclose(ftree);

    int elements = 0;
    int key = 0;
    // for (int i = 0; i < paths_num; ++i)
    // {
    //     printf("kkkkkk:%s\n", paths_rank_map[i]);
    // }
    char **idmap = map_ids_from_paths(paths_rank_map, paths_num, &elements, &key);
    if (idmap == NULL) {
            printf("Error in creating ID map\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        initialize_printed_docs(elements);

        int result_set_size = 0;
        int type_of_query;

        if (rank == 0) {
            printf("Choose the type of query:\n1 Disjunctive\n2 Conjunctive\n");
            scanf("%d", &type_of_query);
        }
        
        for (int i = 1; i < argc; ++i) {
            lower_string(argv[i]);
        }
        
        MPI_Bcast(&type_of_query, 1, MPI_INT, 0, MPI_COMM_WORLD);

        switch (type_of_query) {
            case 1:
                disjunctive_query(root, idmap, key, &argv[1], argc-1, rank);
                break;
            case 2:
                conjunctive_query(root, idmap, key, &argv[1], argc-1, rank);
                break;
            default:
                printf("Invalid query type\n");
                break;
        }

        free_printed_docs();

    MPI_Finalize();
    return 0;
}

        // lower_string(argv[1]); // Convert the search key to lowercase
        // node *result = get_node(root, argv[1]); // Get the node corresponding to the search key
        // int result_set_size = 0;

        // int type_of_query;

        // if (rank == 0)
        // {
        //     printf("Choose the type of query:\n1 Disjunctive\n2 Conjunctive\n");
        //     scanf("%d", &type_of_query);
        // }
        
        // MPI_Bcast(&type_of_query, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // switch(type_of_query) {

        //     case 1:
        //         disjunctive_query(root, idmap, key, &argv[1], argc-1, rank);
        //     case 2:
        //         conjunctive_query(root, idmap, key, &argv[1], argc-1, rank);
        // }

        // // Execute different types of queries based on the search

        // // search_query(idmap, key, result, rank);
        // // 
        

        // // Free the memory allocated for the tree
        // // safe_recursive_free_tree(root);

        // MPI_Finalize(); // Finalize MPI processes
        // return EXIT_SUCCESS;
    // //}