#include "djbhash.h"

struct test_struct {
  int a;
  int b;
};

int main( int argc, char *argv[] )
{
  // Hash table structure.
  struct djbhash hash;
  // Return value from finding a key.
  struct djbhash_node *item;

  // Initialize the hash table.
  djbhash_init( &hash );

  // Set hash values with various data types.
  int temp = 10;
  djbhash_set( &hash, "int", &temp, DJBHASH_INT );
  double temp2 = 3.14159;
  djbhash_set( &hash, "double", &temp2, DJBHASH_DOUBLE );
  char temp3 = 'a';
  djbhash_set( &hash, "char", &temp3, DJBHASH_CHAR );
  djbhash_set( &hash, "string", "bar", DJBHASH_STRING );
  int temp_arr[] = { 8, 6, 7, 5, 3, 0, 9 };
  djbhash_set( &hash, "array", temp_arr, DJBHASH_ARRAY, 7 );
  struct djbhash temp_hash;
  djbhash_init( &temp_hash );
  djbhash_set( &temp_hash, "foo", "bar", DJBHASH_STRING );
  djbhash_set( &temp_hash, "baz", temp_arr, DJBHASH_ARRAY, 7 );
  djbhash_set( &hash, "hash", &temp_hash, DJBHASH_HASH );
  struct test_struct test;
  test.a = 10;
  test.b = 11;
  djbhash_set( &hash, "other", &test, DJBHASH_OTHER );

  // Find and print items.
  printf( "Finding single items...\n" );
  item = djbhash_find( &hash, "int" );
  djbhash_print( item );
  item = djbhash_find( &hash, "double" );
  djbhash_print( item );
  item = djbhash_find( &hash, "char" );
  djbhash_print( item );
  item = djbhash_find( &hash, "string" );
  djbhash_print( item );
  item = djbhash_find( &hash, "array" );
  djbhash_print( item );
  item = djbhash_find( &hash, "hash" );
  djbhash_print( item );
  item = djbhash_find( &hash, "other" );
  djbhash_print( item );

  // Remove an item.
  printf( "\nRemoving key \"int\"...\n" );
  djbhash_remove( &hash, "int" );
  if ( djbhash_find( &hash, "int" ) == NULL )
    printf( "int removed\n" );

  // Print all items.
  printf( "\nDumping all items...\n" );
  djbhash_dump( &hash );

  // Iterate through items.
  printf( "\nIterating...\n" );
  item = djbhash_iterate( &hash );
  while ( item )
  {
    djbhash_print( item );
    item = djbhash_iterate( &hash );
  }
  // Reset the iterator.
  djbhash_reset_iterator( &hash );

  // Search for an item that doesn't exist:
  char *missing = "missing key";
  printf( "\nSearching for a missing key \"%s\"...\n", missing );
  if ( ( item = djbhash_find( &hash, missing ) ) == NULL )
    printf( "%s: No such item!\n", missing );

  // Printing a hash as JSON.
  char *json = djbhash_to_json( &hash );
  printf( "\nHash to json: %s\n", json );
  free( json );
  json = NULL;

  // Remove all items and free memory.
  djbhash_destroy( &temp_hash );
  djbhash_destroy( &hash );

  return 0;
}
