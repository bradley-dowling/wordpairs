# Wordpairs
#### Bradley Dowling
#### FALL 2021 - CS360

Wordpairs is a program that ingests 1 or more files and stores the pairs of words present in those files in a hash table. The word pairs are then sorted in descending order by the number of times they appear in the file(s). The number of wordpairs displayed can be specified by an optional argument. The program can be called as follows:

wordpairs <-count> fileName1 <fileName2> <fileName3> ...

Where: count is the integer number of word pairs to print out and fileNameN are pathnames from which to read words. If no count argument is specified, ALL word pairs are printed to stdout. (tokens enclosed in angular brackets are optional).

Compile the programing using the command:
make
