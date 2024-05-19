#!/usr/bin/python3

################################################
########### Vito Giacalone (546646) ############
####### Digital content retrival project #######
################################################

################################################
############# Preprocessing texts ##############
################################################

################################################
################# Libraries ####################
################################################

import mysql.connector
import nltk
from bs4 import BeautifulSoup
import re
import porter
import math

############ Database connection ###############

def connect_db(host, user, pwd, db, auth):
    db_connection = mysql.connector.connect(
        host=host, 
        user=user, 
        password=pwd, 
        database=db, 
        auth_plugin=auth
    )
    return db_connection

############################### Reading the texts from the database #################################
# Reading of the corpus associated to each ID in the file used for reading
# From each corpus all the html tags are removed
# In the file "docs.txt" are stored the ID of each document and the corpus
#####################################################################################################

def read_data(filename, MAX_BATCH_SIZE, host, user, pwd, db, auth):

    db_conn = connect_db(host, user, pwd, db, auth)
    db_cursor = db_conn.cursor()

    query_retrieval = """SELECT file_info.ID, file_info.Txt
                         FROM file_info
                         WHERE file_info.ID = %s"""
    IDs = []
    out = []
    fout = open("docs.txt", "w")

    with open(filename) as f:
        for line in f:
            elements = line.split("\t")
            if elements[2] == "html" and len(IDs) < MAX_BATCH_SIZE:
                IDs.append((elements[0],))  # Tuple containing the ID

            if len(IDs) == MAX_BATCH_SIZE:
                for id_tuple in IDs:
                    db_cursor.execute(query_retrieval, id_tuple)
                    out.append(db_cursor.fetchone())
                for record in out:
                    print("%d\t%s"%(int(record[0]), clean_html_reverse_posting_lists(record[1])), file=fout)
                IDs = []
                out = []
    
    if IDs:
        for id_tuple in IDs:
            db_cursor.execute(query_retrieval, id_tuple)
            tup = db_cursor.fetchone()
            line = tup[1].replace("\n", " ")
            out.append((tup[0], line))

        for record in out:
            print("%d\t%s"%(int(record[0]), clean_html_reverse_posting_lists(record[1])), file=fout)
    db_conn.close()


############################### Create posting lists from documents #################################
# Read a document with some stopwords
# Read a pair: ID-Corpus and produce a pair ID-LIST_OF_WORDS
# Short words are typically stopwords, so I remove these words from the set of relevant words
# For each relevant word in the document, the stemming is performed. The stemming produce a list
# of words used in the next phase to help the searching, insertion, deletion operations.
# Create the posting lists -> produce a pair: WORD:LIST_OF_DOC_IDS
# Save the stemmed words in a file, save the posting lists
#####################################################################################################

def create_postlist(filename_in, filename_out, stopwords, stem):
    fstem = open(stem, "a")
    fout = open(filename_out, "w")
    fsw = open(stopwords, "r")
    stopwords_list = fsw.read().split()  # Leggi le stopwords e dividile in una lista
    fsw.close()
    pl = {}
    with open(filename_in, "r") as f:
        for line in f:
            line = line.strip()
            if '\t' not in line:
                continue  # Salta le righe che non contengono un tab
            line = line.split('\t')
            if len(line) < 2:
                continue  # Salta le righe che non hanno almeno due elementi
            key = line[0]
            words = line[1].split(" ")
            words = [w for w in words if (len(w) > 3 and w not in stopwords_list)]
            for word in words:
                word = word.lower()
                try:
                    pl[word].add(key)
                except KeyError:
                    pl[word] = set()
                    pl[word].add(key)
    for word in pl:
        stemlist = porter.stem(word)
        print(stemlist, file=fstem)
        print(word, file=fout)
        for id in sorted(pl[word]):
            print(id, file=fout)
    fstem.close()
    fout.close()
 

############################ Clean the input file from all small words ##############################
# This function cleans the input file from all the words whose length is greater than the average 
# length of the words + 2 (It's an empirical rule in order to obtain the words that will guide
# the searching, insertion, deletion operations)
#####################################################################################################

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

################################ Clean the corpus containing tags ###################################
# This function removes the html tags, LaTeX tags (thanks to the regex) and non-alphanumerical
# characters. Furthermore it removes numbers. It takes into account the quotations and the words
# separated by dash
#####################################################################################################

def clean_html_reverse_posting_lists(html_string):
    # Definisci i caratteri da rimuovere
    remove_list = "$,./\\*+€&!?ì^=)(%£@ç°#§:;)" #### tengo i simboli: - e ""
    
    # Pattern per rimuovere i tag LaTeX
    pattern_latex = r'\\[a-zA-Z]+\{[^}]*\}|\[[^\]]*\]|\\[a-zA-Z]+'
  
    pattern_numbers = r'\d+'

    # Usa BeautifulSoup per rimuovere i tag HTML
    soup = BeautifulSoup(html_string, "html.parser")
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

#####################################################################################################
############################################# Main ##################################################

read_data("out.txt", 10, "localhost", "root", "GCLVTI99P27F061Y", "dcr", "mysql_native_password")
create_postlist("docs.txt", "postings.txt", "stopwords.txt", "stemwords.txt")
clean_words_average_length("stemwords.txt", "average_stemwords.txt")