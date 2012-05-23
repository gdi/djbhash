#include "djbhash.h"

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

  // Find and print items.
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

  // Search for an item that doesn't exist:
  char *missing = "missing key";
  if ( ( item = djbhash_find( &hash, missing ) ) == NULL )
    printf( "%s: No such item!\n", missing );

  // Remove all items and free memory.
  djbhash_destroy( &hash );

  return 0;
}
