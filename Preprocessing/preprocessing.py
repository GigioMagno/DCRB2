#!/usr/bin/python3

import nltk
from bs4 import BeautifulSoup
import re
import porter
import math
from mpi4py import MPI
import os

#######################################################################################################################

def check_directories(path_list):
    for path in path_list:
        if not os.path.exists(path):
            os.makedirs(path)

#######################################################################################################################

## each core -> reads a document and writes in an output file the pairs [doc_ID: posting_list]

#devo togliere il filename in input, leggere da una cartella speicifica tutti i documenti. Filename in realtà deve diventare una lista di file
#per ogni file nella lista leggo e scrivo.

#######################################################################################################################

def read_data(filename_list, rank, elements, cores, extras):
#rank*(elements+1)-extras + (cores-rank)
    out_path = "./Documents/docs_"+str(rank)+".txt"
    map_path = "./Documents/mapping_"+str(rank)+".txt"

    fout = open(out_path, "w")    # assegno l'id al documento. Leggo il documento e lo pulisco da caratteri speciali ecc...
    fmap = open(map_path, "w")    # lookup table ID-DOCUMENT
    
    if rank < extras:
        ID = rank * (elements + 1)
    else:
        ID = extras * (elements + 1) + (rank - extras) * elements

        #ID=rank*(elements+1)-extras+(cores-rank)

                                     #ID incrementale per documento
    for file in filename_list:
        with open(file) as f:
            txt = f.read()
            print("%d\t%s"%(ID, clean_document(txt)), file=fout)
            print("%d\t%s"%(ID, file), file=fmap)
        ID+=1                                   #ID da modificare come ((rank * size lista)  + incremento)

    fout.close()
    fmap.close()

#######################################################################################################################

def create_postlist(filename_in, stopwords, dic):
    
    try:
        fsw = open(stopwords, "r")
        stopwords_list = fsw.read().split()  # Leggi le stopwords e dividile in una lista
        fsw.close()
    except Exception as e:
        stopwords_list = []
        
    with open(filename_in, "r") as f:
        for line in f:
            line = line.strip()
            if '\t' not in line:
                continue  # Salta le righe che non contengono un tab
            line = line.split('\t')
            if len(line) < 2:
                continue  # Salta le righe che non hanno almeno due elementi
            key = int(line[0])
            words = line[1].split(" ")
            words = [w for w in words if (len(w) > 3 and w not in stopwords_list)]    

            for word in words:
                word = word.lower()
                try:
                    dic[word].add(key)      #usa timsort -> mix tra insertion sort e merge sort, best case n if there's an implicit ordering
                except KeyError:
                    dic[word] = set()
                    dic[word].add(key)
    return dic

#######################################################################################################################

def save_inverted_index(dic, filename_out, stem):
    try:
        with open(stem, "w") as fstem, open(filename_out, "w") as fout:
            for word in dic:
                stemlist = porter.stem(word)
                print(stemlist, file=fstem)
                print(word, file=fout)
                sorted_ids = sorted(dic[word])  # Ordina i valori (ID) per ogni parola
                for id in sorted_ids:
                    print(id, file=fout)
    except Exception as e:
        return

#######################################################################################################################

def clean_words_average_length(filename, filename_out):
    total = 0
    totlines = 0
    fout = open(filename_out, "w")
    words = set()
    with open(filename, "r") as f:
        for line in f:
            total+=len(line)
            totlines+=1
        average_len_word = math.ceil(total/totlines)
        f.seek(0)
        for line in f:
            line = line.strip("\n")
            if len(line)<average_len_word+2:
                words.add(line)
        for w in words:
            print(w, file=fout)
    fout.close()

#######################################################################################################################

def clean_document(txt_document):
    # Definisci i caratteri da rimuovere
    remove_list = "$,./\\*+€&!?ì^=)(%£@ç°#§:;)" #### tengo i simboli: - e ""
    
    # Pattern per rimuovere i tag LaTeX
    pattern_latex = r'\\[a-zA-Z]+\{[^}]*\}|\[[^\]]*\]|\\[a-zA-Z]+'
  
    pattern_numbers = r'\d+'

    # Usa BeautifulSoup per rimuovere i tag HTML
    soup = BeautifulSoup(txt_document, "html.parser")
    raw = soup.get_text(" ")
    clean_text = raw
    # Rimuovi i tag LaTeX
    clean_text = re.sub(pattern_latex, ' ', clean_text)
    
    # Rimuovi i caratteri speciali
    for char in remove_list:
        clean_text = clean_text.replace(char, " ")
    
    # Rimuovi eventuali spazi multipli e caratteri non alfanumerici
    clean_text = re.sub(r'[^a-zA-Z0-9\s]', ' ', clean_text)
    clean_text = re.sub(r'\s+', ' ', clean_text)
    
    clean_text = re.sub(pattern_numbers, ' ', clean_text)
    return clean_text

######################################################## MAIN #########################################################

# dataset_path = "./movieReview"

# MPI.Init()

#### EACH PROCESS TAKES IT'S RANK
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

dataset_path = "./Dataset/split/"

files_per_process = 0
remaining_files = 0
file_list=[]

if rank == 0:   #if i'm the master process

    # Create all the necessary directories
    dirs = ["./Documents/", "./Preprocess/", "./InvertedIndex/"]
    check_directories(dirs)

    # read all the paths of the txt files
    for root, _, filenames in os.walk(dataset_path):
        for file in filenames:
            if file.endswith(".txt"):
                file_list.append(os.path.join(root, file))
    file_list = [file for file in file_list if(file.endswith(".txt"))]

    files_per_process = len(file_list)//size;
    remaining_files = len(file_list)%size;
    for c in range(1, size):
        files = file_list[c*files_per_process: (c+1)*files_per_process]
        comm.send(files, dest=c, tag=100)

    files = file_list[0:files_per_process]
else:
    files = comm.recv(source=0, tag=100)

elements = comm.bcast(files_per_process, root=0)    ## receive the integer division between total number of files/numer of processes
extras = comm.bcast(remaining_files, root=0)



if rank == 0 and extras != 0:
    extra_elements = file_list[-extras:]  # Ottieni gli ultimi file rimanenti
    files.append(extra_elements[0])
    for x in range(1, extras):
        comm.send(extra_elements[x - 1], dest=x, tag=101)
elif rank < extras and rank != 0:
    extra_file = comm.recv(source=0, tag=101)
    files.append(extra_file)

del file_list


##################### il master deve mandare a tutti gli altri la lista di file su cui operare, dopodichè gli slave eseguono il calcolo
#filename_list, rank, elements, cores, extras
read_data(files, rank, elements, size, extras)
read_doc_path = "./Documents/docs_" + str(rank) + ".txt"
stopwords_path = "./Preprocess/stopwords.txt"
inverted_index_path = "./InvertedIndex/inverted_idx_"+ str(rank) +".txt"
stemwords_path = "./Preprocess/stemwords_"+ str(rank) +".txt"



dic = {}
dic = create_postlist(read_doc_path, stopwords_path, dic)

save_inverted_index(dic, inverted_index_path, stemwords_path)

MPI.Finalize()
# # clean_words_average_length("stemwords.txt", "average_stemwords.txt")