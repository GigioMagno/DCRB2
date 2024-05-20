# DCRB2
GH repository for project delivery DCRB2 part 2

# HOW TO RUN THE PROGRAM
Configure your DBMS using the script to build the DB in the DCRB1 repository.
* Run the preprocessing file. This file read from the DB the raw html files, clean them and split each text in token. The token are "cleaned" according to some logic $\rightarrow$ all the numbers and non alphanumerical characters are removed. The only symbols kept are: "" and -, in order to consider "complex terms" (for example: "Vapnik-Chervonenkis"). Thanks to this cleaning, it's possible to obtain the terms from the tokens. The porter algorithm is used to produce other words that will be used to guide the searching and the various operations on the tree. The output of this script are 3 files: "docs.txt", "posting.txt" and "stemwords.txt". The input are the "stopwords.txt" file (in order to remove the stopwords from the corpus) and the file "out.txt" (file produced from the searching script in the DCRB1 repository).
* To run the main are needed 2 files, "posting.txt" and "stemwords.txt". The firsst file contains the posting lists that must be built and the second file contains the stemmed words of the previous step in order to populate the tree and guide correctly the operations in the BST.

# COMPILE AND RUN THE MAIN.C FILE
* ```gcc -I./DSLibraries -o main main.c ./DSLibraries/*.c```
* ```./main```
