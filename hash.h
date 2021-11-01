#ifndef INITIAL_HASH_SIZE
#define INITIAL_HASH_SIZE 10
#endif

#ifndef LOAD_FACTOR
#define LOAD_FACTOR 0.75
#endif

typedef struct _hashNode {
  void* data; // data in node
  int count; // # of occurances of data in table
  unsigned long long value; // hash value of data (determined by crc_64())
  struct _hashNode *next; // next node in linked list
} HashNode;

typedef struct _hashTable {
  HashNode** table;// pointer to table array of HashNode pointers
  int rowCount;  // # of rows in table
  int entryCount; // total # of entries in table
  int uniqueCount; // # of unique entries (# of HashNodes) in table
} HashTable;

// ***************************************************************
// ***************************************************************
// ** INTERFACE:
// **  create HashTable: initHashTable(void)
// **  insert into HashTable: insert(HashTable*, void*)
// **  print specified number of data entries in
// **    descending order of occurances: printSortedHashTable(HashTable*,int)
// **  free memory stored in HashTable: destroy(HashTable*)
// **
// ** NOTE: The other functions described in this file should not
// **       be implemented by the user directly.


// createHashTable() creates a HashTable structure with an entryCount
// of 0, uniqueCount of 0, and a rowCount specified by the integer
// passed in as an argument. The rowCount also specifies the length of the
// HashNode array created. All HashNodes in the array are set to NULL.
// The function returns a pointer to the HashTable created.
HashTable* createHashTable(int);

// initHashTable(void) is a wrapper that will call the createHashTable()
// function with the defined argument INITIAL_HASH_SIZE specified above.
// The function returns a pointer to the HashTable initialized.
// NOTE: createHashTable was initially meant to be called every time the
//       HashTable grew/expanded, so this wrapper was only to be used
//       upon the initial creation of the HashTable. That is no longer
//       the case, but this function is still used to initialize a 
//       HashTable. 
HashTable* initHashTable(void);

// insert() is a function that takes a pointer to some data and
// inserts it into a HashTable. The first argument is a pointer
// to the HashTable to insert the data into. The second argument
// is a pointer to the data to be inserted. The data is hashed
// using the crc_64() function. If a node containing the hashed
// value is already present in the HashTable, the data pointed
// to by that node is freed and replaced by the pointer to the new
// data passed as an argument to this function.
void insert(HashTable*, void*);

// newHashNode() creates a new HashNode structure with a count of
// 1 and a next field of NULL. The first argument is a pointer to
// the data that this node will point to, and the second argument
// is the unsigned long long hash value determined by the hashing
// function crc_64(). The function returns a pointer to the new
// HashNode.
HashNode* newHashNode(void*, unsigned long long);

// calcLoadFactor is a function that determines the load factor
// of a HashTable using that table's uniqueCount / rowCount. The
// only argument is a pointer to the HashTable where the load
// factor is to be calculated. The function returns the calculated
// load factor for the HashTable.
double calcLoadFactor(HashTable*);

// destroy() is a function that frees the memory/data present in a 
// HashTable as well as the HashTable itself. The only argument to
// this function is a pointer to the HashTable to be freed from 
// memory.
// NOTE: For this assignment, destroy() will free all of the data
//       pointed to by the HashNodes within the HashTable.
void destroy(HashTable*);

// expansionInsert() is a function that takes an existing HashNode
// pointer and inserts it into an array of HashNode pointers. This
// function is called when growing/expanding a HashTable. The first
// argument is a pointer to the first HashNode pointer in an array
// of HashNodes, the second argument is the rowCount/number of buckets
// in the array, and the third argument is a pointer to the HashNode
// to be inserted.
void expansionInsert(HashNode**, int, HashNode*);

// expand() is a function that grows the number of rows/buckets in an
// existing HashTable. The only argument to the function is a pointer
// to the HashTable to be expanded. The number of rows/buckets in the
// HashTable is multiplied by 3 each time the table is expanded.
void expand(HashTable*);

// arrayDump() is a function that returns an array of all of the
// HashNode pointers present in a HashTable, sorted in descending order
// based on the HashNode's "count" variable. The function takes a pointer
// to the HashTable to dump as the sole argument. qsort() is the function
// used to sort the HashNode pointers in the pointer array.
// NOTE: The array that is created will need to be freed by free() after it's
//       creation.
HashNode** arrayDump(HashTable*);

// compare() is the function that qsort() uses when comparing HashNode pointers
// in the arrayDump() function described above. The function results in qsort()
// sorting the HashNode pointers in descending order based on their "count" variable.
int compare(const void*, const void*);

// printSortedHashTable() is a function specific for the wordpair program, which
// assumes that the data stored in the HashTable are strings. A sorted HashNode 
// pointer array is created by arrayDump(), and a specified number of HashNode
// data entries are printed to stdout. The first argument to the function is a pointer
// to the HashTable to print the data from, the second argument to the function
// is an integer that specifies the number of HashNode data entries to print.
void printSortedHashTable(HashTable*, int);
