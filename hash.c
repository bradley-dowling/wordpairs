#include "hash.h"
#include "crc64.h"
#include <stdlib.h>
#include <stdio.h>

// ************************************************
// ************************************************
// ** Bradley Dowling - wordpairs
// ** FALL 2021 - CS360 (Assignment 1)
// **
// ** hash.c represents the functions that control
// ** the hash table implemented for the wordpairs
// ** assigment in FALL 2021 - CS360. The hash
// ** table implemented uses separate chaining and
// ** grows dynamically by checking a load factor
// ** specified in hash.h. The data stored in this
// ** assignment happens to be a char*, but this
// ** hash table is generalized and can handle
// ** other types of data as well. The hash function
// ** used is specified in crc64.h.
// **
// ** See hash.h for more information on each
// ** individual function and the data structures
// ** that comprise the hash table.

// create a new hash table:
HashTable* createHashTable(int rows) {
  HashTable* hashTable = malloc(sizeof(HashTable));
  hashTable->table = malloc(sizeof(HashNode*) * rows);
  hashTable->rowCount = rows; // total number of rows (buckets) in hash table
  hashTable->entryCount = 0; // total number of inserts into the hash table
  hashTable->uniqueCount = 0; // total number of unique data entries in hash table
                              // (also the total number of individual hash nodes)

  // initialize all rows (buckets) in the hash table to NULL to start
  for (int i = 0; i < hashTable->rowCount; i++) {
    hashTable->table[i] = NULL;
  }

  return hashTable;
}

// initialize a new HashTable:
HashTable* initHashTable(void) {
  // wrapper for the creation of a hash table
  // NOTE: createHashTable() was initially meant to be called
  //       each time the hash table grew, so this wrapper was
  //       only to be used during the initial creation of the
  //       hash table. This is no longer the case.
  HashTable* initialHashTable = createHashTable(INITIAL_HASH_SIZE);
  return initialHashTable;
}

// insert a new HashNode into the HashTable
void insert(HashTable* hashTable, void* dataEntry) {
  // hash the data entry
  unsigned long long hashValue = crc64(dataEntry); // get hash value
  int index = hashValue % hashTable->rowCount; // get index
  
  // crawl through linked list at row (bucket)
  HashNode* previousNode;
  HashNode* currentNode = hashTable->table[index];
  if (currentNode == NULL) {
    // empty list, create new hash node
    hashTable->table[index] = newHashNode(dataEntry, hashValue);
    hashTable->uniqueCount++; // increase the number of unique HashNodes in table
  } else {
    // start the crawl through the linked list
    while (currentNode != NULL) {
      // check if currentNode value equals hashValue
      if (currentNode->value == hashValue) {
        // a copy of the inserted data is already present in the
        // HashTable, so free the data currently in the hash table
        // and replace it to avoid a memory leak.
        free(currentNode->data);
        currentNode->data = dataEntry;
        currentNode->count++; // increase this nodes count
        hashTable->entryCount++; // increase total number of insertions
        return; // insertion is complete. number of total nodes hasn't
                // increased so no need to expand the HashTable
      } else {
        // continue crawling through the linked list
        previousNode = currentNode;
        currentNode = currentNode->next;
      }
    }
    // HashNode not in linked list, create a new one following the
    // previous node in the list
    previousNode->next = newHashNode(dataEntry, hashValue);
    hashTable->uniqueCount++;
  }

  hashTable->entryCount++; // increase entryCount

  // check if load factor exceeded and expand table if needed
  if (calcLoadFactor(hashTable) > LOAD_FACTOR) {
    expand(hashTable);
  }
}

// create new HashNode function
HashNode* newHashNode(void* dataEntry, unsigned long long hashValue) {
  HashNode* new = malloc(sizeof(HashNode));
  new->data = dataEntry; // data pointed to by this node
  new->count = 1; // initial count of data
  new->value = hashValue; // hash value determined by crc_64() call
  new->next = NULL;
  return new;
}

// insertion of a HashNode that has already been created into
// a new hash table (used during expand() call)
void expansionInsert(HashNode** table, int rowCount ,HashNode* hashNode) {
  int index = hashNode->value % rowCount; // row (bucket) to insert into
  HashNode* currentNode = table[index]; // node at head of row (bucket)
  if (currentNode == NULL) {
    // empty row (bucket), so point to inserted HashNode
    table[index] = hashNode;
  } else {
    // move to the end of the linked list
    while (currentNode->next != NULL) {
      currentNode = currentNode->next;
    }
    currentNode->next = hashNode; // add the HashNode at the end of the linked list
  }

  // HashNode is in a new position compared to where it was before
  // It is now a tail node in this linked list so make sure ->next is NULL
  hashNode->next = NULL;
}

// calculate load factor of HashTable
double calcLoadFactor(HashTable* hashTable) {
  return (double) hashTable->uniqueCount / hashTable->rowCount;
}

// destroy hashTable (free HashTable and associated HashNodes from memory)
// NOTE: For this assignment, destroy() also frees the data that has been
//       inserted into the HashTable.
void destroy(HashTable* hashTable) {
  int i; // iterator to move through rows (buckets) of HashTable

  // variables to crawl through HashNode linked lists:
  HashNode* previousNode;
  HashNode* currentNode;

  // loop through hashTable rows
  for (i = 0; i < hashTable->rowCount; i++) {
    if (hashTable->table[i] != NULL) {
      // crawl through linked list, free nodes as we go
      previousNode = hashTable->table[i];
      currentNode = previousNode->next;
      while (currentNode != NULL) {
        free(previousNode->data); // free the data pointed to by the node
        free(previousNode); // free the pointer to the node
        previousNode = currentNode; // move to next node
        currentNode = currentNode->next;
      }
      free(previousNode->data); // free data pointed to by final node in linked list
      free(previousNode); // free the pointer to the final node in linked list
    }
  }
  // free the pointer to the HashNode row (bucket) array
  free(hashTable->table);
  // free the pointer to the hashTable
  free(hashTable);
}

// expand hashTable function (used when load factor exceeded)
void expand(HashTable* hashTable) {
  int i; // iterator

  // create new table (array of HashNode rows/buckets)
  int newRowCount = hashTable->rowCount * 3; // number of new rows/buckets
  HashNode** newTable = malloc(sizeof(HashNode*) * newRowCount);

  for (i = 0; i < newRowCount; i++) {
    newTable[i] = NULL; // nullify the head HashNodes in each row/bucket
  }
  
  // set the HashTable to point to the new table, but
  // retain the old table to reinsert the existing HashNodes
  HashNode** previousTable = hashTable->table;
  hashTable->table = newTable;

  // loop through the old hashTable and move over hashed HashNodes
  HashNode* previousNode;
  HashNode* currentNode;

  for (i = 0; i < hashTable->rowCount; i++) {
    if (previousTable[i] != NULL) {
      // crawl through linked list at this non-empty row/bucket
       previousNode = previousTable[i];
       currentNode = previousNode->next;
      while (currentNode != NULL) {
        expansionInsert(newTable, newRowCount, previousNode);
        previousNode = currentNode;
        currentNode = currentNode->next;
      }
      // insert last node in the linked list at this row/bucket
      expansionInsert(newTable, newRowCount, previousNode);
    }
  }
  free(previousTable); // all nodes moved over, previousTable no longer needed
  hashTable->rowCount = newRowCount; // correct the row/bucket count in the new HashTable
}

// dump out an array of HashNodes sorted in descending
// order of their frequency:
HashNode** arrayDump(HashTable* hashTable) {
  int arraySize = hashTable->uniqueCount; // size of dumped HashNode array
  int rows = hashTable->rowCount; // number of rows/buckets in HashTable
  HashNode** hashArray = malloc(sizeof(HashNode*) * arraySize); // create hashArray

  // iterator variables:
  int i = 0; // iterator for HashNodes in hashArray
  int r; // iterator for rows in hashTable
  
  // variables for crawling through linked lists:
  HashNode* currentNode;
  HashNode* previousNode;

  // iterator through rows, adding pointers to HashNodes 
  // to the hashArray as we go...
  for (r = 0; r < rows; r++) {
    if (hashTable->table[r] != NULL) {
      // start crawling through this row's linked list
      previousNode = hashTable->table[r];
      currentNode = previousNode->next;
      while (currentNode != NULL) {
        hashArray[i++] = previousNode; // add node to hashArray

        // move to next node in linked list
        previousNode = currentNode;
        currentNode = currentNode->next;
      }

      hashArray[i++] = previousNode; // add last node in linked list
    }
  }

  // sort the dumped array in descending order by count and then return
  qsort(hashArray, hashTable->uniqueCount, sizeof(HashNode*), compare);
  return hashArray;
}

// qsort's compare function to sort HashNodes in descending order
// by their entry counts:
int compare(const void* n1, const void* n2) {
  HashNode* node1 = *(HashNode**) n1;
  HashNode* node2 = *(HashNode**) n2;

  return(node2->count - node1->count);
}

// print a specified number of HashNode data entries from a sorted 
// array of HashNode pointers from a HashTable:
void printSortedHashTable(HashTable* hashTable, int displayCount) {
  HashNode** array = arrayDump(hashTable); // create sorted HashNode* array

  if (displayCount > hashTable->uniqueCount || displayCount == -1) {
    displayCount = hashTable->uniqueCount; // displayCount is either larger than uniqueCount
                                           // or is -1. If so, set the display count to uniqueCount
  }

  // output the wordpairs
  for (int i = 0; i < displayCount; i++) {
    printf("%10d %s\n", array[i]->count, (char*) array[i]->data);
  }

  free(array);
}
