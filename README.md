# DCRB2
GH repository for project delivery DCRB2 part 2

# Preprocessing of the huge amount of text (This step requires time)
Using ```mpi4py``` it's possible to process multiple files in parallel. There are 2 types of preprocessing. The script ```preprocesing.py``` is optimized if the files to process are more or less all of the same size. The file ```preprocessing_loadbalance_size.py``` is recommended when the file to process have different dimension. In this second case the master process is in charge of assign to each process the file to process.
The preprocessing of the various corupuses consists in: 
* create a map ```ID:path_of_the_file```, the ID of the file is assigned by each process to a particular file and is unique, then this map is saved into a ```.txt``` file. Each process creates and saves its own map.
* According to the set of corpuses that have been assigned to that particular process, the process computes a preliminary phase of tokenization in order to get raw terms, after that a list of special characters is removed and each term is stemmed using the Porter's algorithm. These words are stored into a file and will be used in a future phase to build the binary search tree.
* Lastly, each process produces a file. This file is an inverted index $\rightarrow$ ```Term:List_of_socuments_where_the_term_belongs_to```. During the building of the inverted index, all the words that belong to a list of stopwords are dropped off.

According to some tests, to process ```4GB``` of files with 10 cores, are required on average 40 seconds.

The command to run the script is: ```mpiexec -n x python preprocessing.py``` (```x``` in the command is the number of cores that will run the program)

# COMPILE AND RUN THE MAIN.C FILE
* ```mpicc .n x -I./DSLibraries -o main main.c ./DSLibraries/*.c```
* ```mpirun main [list of words to search in the docuements]```
(The words must be separated by a space)
Once the program runs it will ask to you whether you want to compute a disjunctive query or a conjunctive query.
