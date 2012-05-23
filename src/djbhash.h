#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef true
  #define true 1
#endif
#ifndef false
  #define false 0
#endif

#define DJBHASH_CHUNK_SIZE 16

// Node structure
struct h_node {
  // Key string.
  char *key;
  // Generic pointer to value.
  void *value;
  // Data type for this node.
  int data_type;
  // If it's an array data type, the number of items.
  int count;
  // Pointer to the next node in the list.
  struct h_node *next;
};

// Linked list structure.
struct h_table {
  // Linked lists composing the hash table.
  struct h_node **ll;
  // Number of arrays/linked lists.
  int t_size;
  // Type of table to use - Array or Linked list.
  int t_type;
  // Number of items in the array when using array mode.
  int *count;
};

// Type of hash table (array or linked list)
enum djbhash_table_type {
  DJBHASH_ARRAY_LIST,
  DJBHASH_LINKED_LIST,
};

// Some various return functions.
enum djbhash_data_type {
  DJBHASH_INT,
  DJBHASH_DOUBLE,
  DJBHASH_CHAR,
  DJBHASH_STRING,
  DJBHASH_ARRAY,
};

// Print just the item's value.
void djbhash_print_value( struct h_node *item );

// Print an item in format: key => value
void djbhash_print( struct h_node *item );

// Hash initialization.
void djbhash_init( struct h_table *ht, int t_size, int t_type );

// DJB string hashing function.
unsigned int djb_hash( struct h_table *ht, char *key, int length );

// Find an element in the hash using binary search with the array list method.
int djbhash_bin_search( struct h_table *ht, char *key, int min, int max, int index, int length, int insert_mode );

// Set a hash item using array mode.
int djbhash_set_arr( struct h_table *ht, char *key, void *value, int data_type, int index, int length, int count );

// Set a hash item using linked list mode.
int djbhash_set_ll( struct h_table *ht, char *key, void *value, int data_type, int index, int length, int count );

// Generic driver to set a hash item.
int djbhash_set( struct h_table *ht, char *key, void *value, int data_type, ... );

// Find an element using linked list mode.
struct h_node *djbhash_find_ll( struct h_table *ht, char *key );

// Find an element using array mode.
struct h_node *djbhash_find_arr( struct h_table *ht, char *key );

// Generic driver to find a hash item.
struct h_node *djbhash_find( struct h_table *ht, char *key );

// Print all items in the hash.
void djbhash_dump( struct h_table *ht );

// Remove all items from the hash.
void djbhash_empty( struct h_table *ht );

// Remove all items from the hash and free memory.
void djbhash_destroy( struct h_table *ht );
