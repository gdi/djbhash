#include "djbhash.h"

int main( int argc, char *argv[] )
{
  // Hash table structure.
  struct h_table ht;
  // Return value from finding a key.
  struct h_node *item;

  // Initialize the hash table with 5 linked lists.
  djbhash_init( &ht, 5 );

  // Set hash values with various data types.
  int temp = 10;
  djbhash_set( &ht, "int", &temp, DJBLL_INT );
  double temp2 = 3.14159;
  djbhash_set( &ht, "double", &temp2, DJBLL_DOUBLE );
  char temp3 = 'a';
  djbhash_set( &ht, "char", &temp3, DJBLL_CHAR );
  djbhash_set( &ht, "string", "bar", DJBLL_STRING );
  int temp_arr[] = { 8, 6, 7, 5, 3, 0, 9 };
  djbhash_set( &ht, "array", temp_arr, DJBLL_ARRAY, 7 );

  // Find and print items.
  item = djbhash_find( &ht, "int" );
  djbhash_print( item );
  item = djbhash_find( &ht, "double" );
  djbhash_print( item );
  item = djbhash_find( &ht, "char" );
  djbhash_print( item );
  item = djbhash_find( &ht, "string" );
  djbhash_print( item );
  item = djbhash_find( &ht, "array" );
  djbhash_print( item );

  // Search for an item that doesn't exist:
  char *missing = "missing key";
  if ( ( item = djbhash_find( &ht, missing ) ) == NULL )
    printf( "%s: No such item!\n", missing );

  // Remove all items and free memory.
  djbhash_destroy( &ht );

  return 0;
}
