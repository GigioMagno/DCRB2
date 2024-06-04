#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <limits.h>
#include "posting_list.h"
#include "enquiry.h"
#include "bstree.h"
#include <stdbool.h>


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

    char buf[512]; 

    for (int i = 0; i < paths_num; ++i) {
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
    char **idmap = map_ids_from_paths(paths_rank_map, paths_num, &elements, &key);

    // if (rank == 0)
    // {

    //     //print_doc(idmap, elements);
    //     for (int i = 0; i < elements; ++i)
    //         {
    //             printf("%s\n", idmap[i]);
    //             char *tokens = strtok(idmap[i], "\t");
    //             printf("%s\n", tokens);
    //             tokens = strtok(NULL, "\t");
    //             printf("%s\n", tokens);
    //         }
    // }

    if (idmap == NULL) {
        printf("Error in creating ID map\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

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
            if (rank == 0) {
                printf("Invalid query type\n");
            }
            MPI_Finalize();
            return 0;
            break;
    }

    MPI_Finalize();
    return 0;
}
