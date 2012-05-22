#include "djbll.h"

int main( int argc, char *argv[] )
{
  // Hash table structure.
  struct h_table ht;
  // Return value from finding a key.
  struct h_node *item;

  // Initialize the hash table with 5 linked lists.
  djbll_init( &ht, 5 );

  // Set hash values with various data types.
  int temp = 10;
  djbll_set( &ht, "int", &temp, DJBLL_INT );
  double temp2 = 3.14159;
  djbll_set( &ht, "double", &temp2, DJBLL_DOUBLE );
  char temp3 = 'a';
  djbll_set( &ht, "char", &temp3, DJBLL_CHAR );
  djbll_set( &ht, "string", "bar", DJBLL_STRING );
  int temp_arr[] = { 8, 6, 7, 5, 3, 0, 9 };
  djbll_set( &ht, "array", temp_arr, DJBLL_ARRAY, 7 );

  // Find and print items.
  item = djbll_find( &ht, "int" );
  djbll_print( item );
  item = djbll_find( &ht, "double" );
  djbll_print( item );
  item = djbll_find( &ht, "char" );
  djbll_print( item );
  item = djbll_find( &ht, "string" );
  djbll_print( item );
  item = djbll_find( &ht, "array" );
  djbll_print( item );

  // Search for an item that doesn't exist:
  char *missing = "missing key";
  if ( ( item = djbll_find( &ht, missing ) ) == NULL )
    printf( "%s: No such item!\n", missing );

  // Remove all items and free memory.
  djbll_destroy( &ht );

  return 0;
}
