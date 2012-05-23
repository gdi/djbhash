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

#define DJBHASH_CHUNK_SIZE 512
#define DJBHASH_MAX_BUCKETS 65536

// Node structure
struct djbhash_node {
  // Key string.
  char *key;
  // Generic pointer to value.
  void *value;
  // Data type for this node.
  int data_type;
  // If it's an array data type, the number of items.
  int count;
  // Pointer to the next node in the list.
  struct djbhash_node *next;
};

// Linked list bucket structure.
struct djbhash_bucket {
  // Bucket ID.
  unsigned int id;
  // Linked list containing items.
  struct djbhash_node *list;
};

// Linked list structure.
struct djbhash {
  // Buckets.
  struct djbhash_bucket *buckets;
  // Number of buckets.
  int count;
};

// Some various return functions.
enum djbhash_data_type {
  DJBHASH_INT,
  DJBHASH_DOUBLE,
  DJBHASH_CHAR,
  DJBHASH_STRING,
  DJBHASH_ARRAY,
  DJBHASH_OTHER,
};

// Method to use when setting a hash item.
enum djbhash_method {
  DJBHASH_APPEND,
  DJBHASH_UPDATE,
  DJBHASH_INSERT,
};

void djbhash_print_value( struct djbhash_node *item );
void djbhash_print( struct djbhash_node *item );
void djbhash_init( struct djbhash *hash );
unsigned int djb_hash( char *key, int length );
int djbhash_bin_search( struct djbhash *hash, int min, int max, int bucket_id, char *key, int length, int insert_mode );
void *djbhash_value( void *value, int data_type, int count );
int djbhash_set( struct djbhash *hash, char *key, void *value, int data_type, ... );
struct djbhash_node *djbhash_find( struct djbhash *hash, char *key );
void djbhash_dump( struct djbhash *hash );
void djbhash_free_node( struct djbhash_node *item );
void djbhash_empty( struct djbhash *hash );
void djbhash_destroy( struct djbhash *hash );
