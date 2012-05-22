#include "djbll.h"

// Print an items' data.
void djbll_print_value( struct h_node *item )
{
  // Loop iterator for array values.
  int i;
  // Pointer to integer array.
  int *arr_ptr;

  // Print the value depending on data type.
  switch ( item->data_type )
  {
    case DJBLL_INT:
      printf( "%d", *( int * )item->value );
      break;
    case DJBLL_DOUBLE:
      printf( "%f", *( double * )item->value );
      break;
    case DJBLL_CHAR:
      printf( "%c", *( char * )item->value );
      break;
    case DJBLL_STRING:
      printf( "%s", ( char * )item->value );
      break;
    case DJBLL_ARRAY:
      arr_ptr = item->value;
      for ( i = 0; i < item->count - 1; i++ )
        printf( "%d, ", arr_ptr[i] );
      printf( "%d", arr_ptr[item->count - 1] );
      break;
    default:
      printf( "%s", ( char * )item->value );
  }
}

// Print the key value pair.
void djbll_print( struct h_node *item )
{
  printf( "%s => ", item->key );
  djbll_print_value( item );
  printf( "\n" );
}

// Initialize the hash table.
void djbll_init( struct h_table *ht, int t_size )
{
  int i;

  // Set the table size and allocate some memory.
  ht->t_size = t_size;
  ht->ll = malloc( sizeof( struct h_node * ) * t_size );

  // Initialize the lists to be NULL.
  for ( i = 0; i < ht->t_size; i++ )
    ht->ll[i] = NULL;
}

// DJB Hash function.
unsigned int djb_hash( struct h_table *ht, char *key, int length )
{
  unsigned int i;
  unsigned int hash;

  hash = 5381;
  for ( i = 0; i < length; key++, i++ )
    hash = ( ( hash << 5 ) + hash ) + ( *key );
  return hash % ht->t_size;
}

// Set the value for an item in the hash table.
int djbll_set( struct h_table *ht, char *key, void * value, int data_type, ... )
{
  int length;
  int index;
  struct h_node *searcher;
  struct h_node *temp;
  int count;
  va_list arg_ptr;

  // Default invalid data types.
  if ( data_type < DJBLL_INT || data_type > DJBLL_ARRAY )
    data_type = DJBLL_STRING;

  // If the data type is an array, track how many items the array has.
  if ( data_type == DJBLL_ARRAY )
  {
    va_start( arg_ptr, 1 );
    count = va_arg( arg_ptr, int );
    va_end( arg_ptr );
  }

  // Create our hash item.
  temp = malloc( sizeof( struct h_node ) );
  temp->key = key;
  temp->value = value;
  temp->data_type = data_type;
  temp->count = count;
  temp->next = NULL;

  // Set up the linked list iterator.
  length = strlen( key );
  index = djb_hash( ht, key, length );
  searcher = ht->ll[index];

  // See if this is the first item or not.
  if ( searcher == NULL )
  {
    ht->ll[index] = temp;
    return true;
  }

  // Go to the end of the linked list and check if the key already exists while doing so.
  while ( searcher->next != NULL )
  {
    // Make sure this item doesn't already exist while we're at it.
    if ( strncmp( searcher->key, key, length ) == 0 )
    {
      // Already exists, update the value.
      searcher->value = value;
      // Cleanup the temp variable.
      free( temp );
      temp = NULL;
      return true;
    }
    searcher = searcher->next;
  }

  // Set the next item to be the new item.
  searcher->next = temp;
  return true;
}

// Find an item in the hash table.
struct h_node *djbll_find( struct h_table *ht, char *key )
{
  int length;
  struct h_node *searcher;

  length = strlen( key );
  searcher = ht->ll[djb_hash( ht, key, length )];

  // Make sure this list isn't empty first.
  if ( searcher == NULL )
    return NULL;

  // Iterate through the list and try to find the item.
  while ( searcher )
  {
    if ( strncmp( searcher->key, key, length ) == 0 )
      return searcher;
    searcher = searcher->next;
  }
  return NULL;
}

// Dump all data in the hash table.
void djbll_dump( struct h_table *ht )
{
  int i;
  struct h_node *iter;
  for ( i = 0; i < ht->t_size; i++ )
  {
    iter = ht->ll[i];
    while ( iter )
      djbll_print( iter );
  }
}

// Remove all elements from the hash table.
void djbll_empty( struct h_table *ht )
{
  int i;
  struct h_node *iter;
  struct h_node *next;
  for ( i = 0; i < ht->t_size; i++ )
  {
    iter = ht->ll[i];
    while ( iter )
    {
      next = iter->next;
      free( iter );
      iter = next;
    }
  }
}

// Remove all elements and frees memory used by the hash table.
void djbll_destroy( struct h_table *ht )
{
  djbll_empty( ht );
  free( ht->ll );
  ht->ll = NULL;
}
