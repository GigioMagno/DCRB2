#!/usr/bin/python3

from bs4 import BeautifulSoup
import re
import porter
import math
from mpi4py import MPI
import os

#######################################################################################################################
# Utility function to check and create directories if they do not exist
#######################################################################################################################
def check_directories(path_list):
    for path in path_list:
        if not os.path.exists(path):
            os.makedirs(path)

#######################################################################################################################
# Function to read documents, clean text, and write to output files for each process
#######################################################################################################################
def read_data(filename_list, rank, elements, cores, extras):
    out_path = "./Documents/docs_" + str(rank) + ".txt"
    map_path = "./Documents/mapping_" + str(rank) + ".txt"

    fout = open(out_path, "w")    # Output file for cleaned documents
    fmap = open(map_path, "w")    # Lookup table for document ID and file mapping
    
    if rank < extras:
        ID = rank * (elements + 1) + 1
    else:
        ID = extras * (elements + 1) + (rank - extras) * elements + 1

    for file in filename_list:
        with open(file, "r") as f:
            txt = f.read()
            print("%d\t%s" % (ID, clean_document(txt)), file=fout)
            print("%d\t%s" % (ID, file), file=fmap)
        ID += 1

    fout.close()
    fmap.close()

#######################################################################################################################
# Function to create posting lists from the cleaned documents
#######################################################################################################################
def create_postlist(filename_in, stopwords, dic):
    try:
        fsw = open(stopwords, "r")
        stopwords_list = fsw.read().split()  # Read stopwords and split into a list
        fsw.close()
    except Exception as e:
        stopwords_list = []
        
    with open(filename_in, "r") as f:
        for line in f:
            line = line.strip()
            if '\t' not in line:
                continue  # Skip lines that do not contain a tab
            line = line.split('\t')
            if len(line) < 2:
                continue  # Skip lines that do not have at least two elements
            key = int(line[0])
            words = line[1].split(" ")
            words = [w for w in words if (len(w) > 3 and w not in stopwords_list)]

            for word in words:
                word = word.lower()
                try:
                    dic[word].add(key)  # Add document ID to the posting list
                except KeyError:
                    dic[word] = set()
                    dic[word].add(key)
    return dic

#######################################################################################################################
# Function to save the inverted index and stemmed words to files
#######################################################################################################################
def save_inverted_index(dic, filename_out, stem):
    try:
        with open(stem, "w") as fstem, open(filename_out, "w") as fout:
            for word in dic:
                stemlist = porter.stem(word)
                print(stemlist, file=fstem)
                print(word, file=fout)
                sorted_ids = sorted(dic[word])  # Sort the document IDs for each word
                for id in sorted_ids:
                    print(id, file=fout)
    except Exception as e:
        return

#######################################################################################################################
# Function to clean words and calculate average word length, then save filtered words to file
#######################################################################################################################
def clean_words_average_length(filename, filename_out):
    total = 0
    totlines = 0
    fout = open(filename_out, "w")
    words = set()
    with open(filename, "r") as f:
        for line in f:
            total += len(line)
            totlines += 1
        average_len_word = math.ceil(total / totlines)
        f.seek(0)
        for line in f:
            line = line.strip("\n")
            if len(line) < average_len_word + 2:
                words.add(line)
        for w in words:
            print(w, file=fout)
    fout.close()

#######################################################################################################################
# Function to clean document text by removing special characters, LaTeX tags, HTML tags, and numbers
#######################################################################################################################
def clean_document(txt_document):
    remove_list = "$,./\\*+€&!?ì^=)(%£@ç°#§:;)"  # Characters to be removed

    # Pattern to remove LaTeX tags
    pattern_latex = r'\\[a-zA-Z]+\{[^}]*\}|\[[^\]]*\]|\\[a-zA-Z]+'
    pattern_numbers = r'\d+'

    # Use BeautifulSoup to remove HTML tags
    soup = BeautifulSoup(txt_document, "html.parser")
    raw = soup.get_text(" ")
    clean_text = raw
    
    # Remove LaTeX tags
    clean_text = re.sub(pattern_latex, ' ', clean_text)
    
    # Remove special characters
    for char in remove_list:
        clean_text = clean_text.replace(char, " ")
    
    # Remove non-alphanumeric characters and multiple spaces
    clean_text = re.sub(r'[^a-zA-Z\s]', ' ', clean_text)
    clean_text = re.sub(r'\s+', ' ', clean_text)
    
    # Remove numbers
    clean_text = re.sub(pattern_numbers, ' ', clean_text)
    return clean_text

#######################################################################################################################
# Main script execution
#######################################################################################################################

# Initialize MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

dataset_path = "./Dataset/"

file_list = []

if rank == 0:  # Master process
    # Create necessary directories
    dirs = ["./Documents/", "./Preprocess/", "./InvertedIndex/", "./../output/BST/", "./../output/Results/"]
    check_directories(dirs)

    # Read all the paths of the text files
    for root, _, filenames in os.walk(dataset_path):
        for file in filenames:
            if file.endswith(".txt"):
                file_list.append(os.path.join(root, file))

    # Get file sizes
    file_sizes = [(file, os.path.getsize(file)) for file in file_list]
    
    # Sort files by size in descending order
    file_sizes.sort(key=lambda x: x[1], reverse=True)
    
    # Initialize lists to hold files for each rank
    files_per_rank = [[] for _ in range(size)]
    sizes_per_rank = [0] * size
    
    # Distribute files to ranks to balance the total size
    for file, size in file_sizes:
        min_rank = sizes_per_rank.index(min(sizes_per_rank))
        files_per_rank[min_rank].append(file)
        sizes_per_rank[min_rank] += size
    
    # Send the file lists to each rank
    for c in range(1, size):
        if c < len(files_per_rank):
            comm.send(files_per_rank[c], dest=c, tag=100)
    files = files_per_rank[0]
else:
    files = comm.recv(source=0, tag=100)

# Ensure all processes have the number of files they are processing
elements = comm.bcast(len(files), root=0)
extras = 0  # Extras are no longer needed due to balanced distribution

# Process each rank's files
read_data(files, rank, elements, size, extras)
read_doc_path = "./Documents/docs_" + str(rank) + ".txt"
stopwords_path = "./Preprocess/stopwords.txt"
inverted_index_path = "./InvertedIndex/inverted_idx_" + str(rank) + ".txt"
stemwords_path = "./Preprocess/stemwords_" + str(rank) + ".txt"

# Create and save inverted index
dic = {}
dic = create_postlist(read_doc_path, stopwords_path, dic)
save_inverted_index(dic, inverted_index_path, stemwords_path)

# Finalize MPI
MPI.Finalize()

# Optional function call for cleaning words by average length
# clean_words_average_length("stemwords.txt", "average_stemwords.txt")
