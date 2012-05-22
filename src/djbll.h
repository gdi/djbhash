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

// Hash table linked list structure.
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
  int t_size;
};

// Some various return functions.
enum djbll_data_type {
  DJBLL_INT,
  DJBLL_UINT,
  DJBLL_DOUBLE,
  DJBLL_CHAR,
  DJBLL_STRING,
  DJBLL_USTRING,
  DJBLL_ARRAY,
};

// Hash function.
unsigned int djb_hash( struct h_table *ht, char *ptr, int length );
// Function to find an item.
struct h_node *djbll_find( struct h_table *ht, char *key );
// Function to remove an item.
int djbll_remove( struct h_table *ht, char *key );
// Function to add/update an item.
int djbll_set( struct h_table *ht, char *key, void *value, int data_type, ... );
// Function to print an element of the hash (based on data type).
void djbll_print( struct h_node *item );
