#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hash.h"
#include "getWord.h"

// ********************************************************
// ********************************************************
// **  Bradley Dowling - wordpairs
// **  FALL 2021 - CS360 (Assignment 1)
// **
// **  This program ingests 1 or more files and uses a 
// **  hash table to store consecutive word pairs. The 
// **  qsort() function is used to sort the word pairs by
// **  the number of times they appear in the file(s). 
// **  An optional argument specifying the number of word 
// **  pairs to display to stdout may be given. If no optional 
// **  argument specifying the display count is given, all 
// **  word pairs present in the file(s) will be displayed.
// **  See hash.h for more information on the hash table
// **  implementation.
// **

int main(int argc, char** argv) {
  
  int displayWordpairCount = -1; // number of wordpairs to show, -1 indicates it is not set
  int fileCount = 0; // number of files ingested
  
  if (argc == 1) {
   // Arguments not provided, need at least a filename argument
   fprintf(stderr, "Expected at least 1 file as argument, none provided...\n");
   return 1;
  }

  // variables needed for argument checking:
  char argBuffer[10]; // used for sscanf read (during display count arg check)
  int tempInt; // temporary storage for display count arg scanned
  int argIterator; // for iterating through argv
  
  // variables needed for constructing wordpairs
  char* word1;
  char* word2;
  char wordpair[514]; // maximum space for combined wordpairs
  char* space = " "; // space concatenated between words in wordpair[]
  
  HashTable* myHashTable = initHashTable(); // initialize the HashTable

  FILE* fp; // pointer to file to ingest from

  // iterate through arguments provided, check for optional display count
  // arg, otherwise try to open a file with filename provided in arg  
  for (argIterator = 1; argIterator < argc; argIterator++) {

    // handle optional integer argument first
    if (sscanf(argv[argIterator], "-%d%s", &tempInt, argBuffer) == 1) {
      // valid optional display count argument provided
      // assign and then move on to next arg
      displayWordpairCount = tempInt;
      continue;
    }
 
    // this argument is not the optional display count arg
    // try to open the file specified
    fp = fopen(argv[argIterator], "r");
    if (fp == NULL) {
      // unable to open file specified, print to stderr
      // and exit...
      fprintf(stderr, "Unable to open file: %s\n", argv[argIterator]);
      destroy(myHashTable);
      return 1;
    }
 
    if (feof(fp) != 0) {
      // File exists but is empty. Close the file and then
      // move on to the next argument.
      fclose(fp);
      continue;
    }

    fileCount++; // valid file read
    
    // get first two words in file
    word1 = getNextWord(fp);
    word2 = getNextWord(fp);

    while (word2 != NULL) {
      // wordpair is the temp storage for each wordpair;
      // start copying words into worpair and insert into
      // hash table as we go.
      strcpy(wordpair, word1);
      strcat(wordpair, space);
      strcat(wordpair, word2);
      // insert the new wordpair into the hashTable
      insert(myHashTable, strdup(wordpair));    
      // get ready for next word pair
      free(word1);
      word1 = word2;
      word2 = getNextWord(fp);
    }
  
    // free up final word and close the file
    free(word1);
    fclose(fp);
  }

  if (fileCount != 0) {
    // Hash table has ingested at least 1 viable file. We will now
    // print the entries of the hash table into an array and sort
    // them in descending order of appearance count prior to outputting to stdout.
 
    printSortedHashTable(myHashTable, displayWordpairCount);

  } else {
    // User didn't specify a valid filename as an argument.
    // This check is here in case change is made to allow a
    // non-viable filename as an argument without immediately
    // exiting the program.
    fprintf(stderr, "Did not receive valid filename as argument...\n");
  }

  // free all hash nodes as well as the hash table and return!
  // (See hash.h for more info on destroy())
  destroy(myHashTable);
  return 0;
}
