#include "djbhash.h"

// Print an items' data.
void djbhash_print_value( struct h_node *item )
{
  // Loop iterator for array values.
  int i;
  // Pointer to integer array.
  int *arr_ptr;

  // Print the value depending on data type.
  switch ( item->data_type )
  {
    case DJBHASH_INT:
      printf( "%d", *( int * )item->value );
      break;
    case DJBHASH_DOUBLE:
      printf( "%f", *( double * )item->value );
      break;
    case DJBHASH_CHAR:
      printf( "%c", *( char * )item->value );
      break;
    case DJBHASH_STRING:
      printf( "%s", ( char * )item->value );
      break;
    case DJBHASH_ARRAY:
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
void djbhash_print( struct h_node *item )
{
  printf( "%s => ", item->key );
  djbhash_print_value( item );
  printf( "\n" );
}

// Initialize the hash table.
void djbhash_init( struct h_table *ht, int t_size, int t_type )
{
  // Iterator.
  int i;

  // Set the table size and allocate some memory.
  ht->t_size = t_size;
  ht->ll = malloc( sizeof( struct h_node * ) * t_size );

  // Default the table type.
  if ( t_type != DJBHASH_ARRAY_LIST && t_type != DJBHASH_LINKED_LIST )
    t_type = DJBHASH_LINKED_LIST;
  ht->t_type = t_type;

  if ( t_type == DJBHASH_LINKED_LIST )
  {
    // Initialize the lists to be NULL.
    for ( i = 0; i < ht->t_size; i++ )
      ht->ll[i] = NULL;
  } else
  {
    ht->count = malloc( sizeof( int ) * t_size );
    for ( i = 0; i < ht->t_size; i++ )
    {
      ht->ll[i] = ( struct h_node * )malloc( sizeof( struct h_node ) * DJBHASH_CHUNK_SIZE );
      ht->count[i] = 0;
    }
  }
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

// Find the position of an element, (or the index where it should be inserted).
int djbhash_bin_search( struct h_table *ht, char *key, int min, int max, int index, int length, int insert_mode )
{
  int mid;
  int cmp;

  if ( max < min )
  {
    if ( insert_mode )
      return min;
    else
      return -1;
  }

  mid = ( min + max ) / 2;
//printf( "min: %d, max: %d, mid: %d, size: %d, index: %d, key: '%s', cmp: '%s'\n", min, max, mid, ht->count[index], index, key, ht->ll[index][mid].key );

  cmp = strncmp( ht->ll[index][mid].key, key, length );

  if ( cmp > 0 )
    return djbhash_bin_search( ht, key, min, mid - 1, index, length, insert_mode );
  else if ( cmp < 0 )
    return djbhash_bin_search( ht, key, mid + 1, max, index, length, insert_mode );
  else
    return mid;
}

// Set the value for an item in the hash table using array hash table.
int djbhash_set_arr( struct h_table *ht, char *key, void *value, int data_type, int index, int length, int count )
{
  int i;
  int chunks;
  int insert_pos;
  struct h_node *item;
  int insert;

  insert_pos = djbhash_bin_search( ht, key, 0, ht->count[index] - 1, index, length, true );
  insert = true;
  if ( insert_pos < ht->count[index] )
  {
    item = &ht->ll[index][insert_pos];
    // Check if we're doing an insert or an update.
    if ( strncmp( item->key, key, length ) == 0 )
    {
      insert = false;
      item->value = value;
    }
  }
  if ( insert )
  {
    // See if we need more memory.
    chunks = ht->count[index] / DJBHASH_CHUNK_SIZE + 1;
    if ( ht->count[index] > 0 && ht->count[index] % DJBHASH_CHUNK_SIZE == 0 )
      ht->ll[index] = realloc( ht->ll[index], sizeof( struct h_node ) * ( ( chunks + 1 ) * DJBHASH_CHUNK_SIZE ) );

    // Move all of the bigger elements over.
    for ( i = ht->count[index]; i > insert_pos; i-- )
    {
      // Move the key.
      ht->ll[index][i].key = ht->ll[index][i - 1].key;

      // Move the value.
      ht->ll[index][i].value = ht->ll[index][i - 1].value;

      // Move the data type.
      ht->ll[index][i].data_type = ht->ll[index][i - 1].data_type;

      // Move the array count.
      ht->ll[index][i].count = ht->ll[index][i - 1].count;
    }
    // Add the new element.
    ht->ll[index][insert_pos].key = strdup( key );
    ht->ll[index][insert_pos].value = value;
    ht->ll[index][insert_pos].count = count;
    ht->ll[index][insert_pos].data_type = data_type;
    ht->count[index]++;
  }
}

// Set the value for an item in the hash table using linked list.
int djbhash_set_ll( struct h_table *ht, char *key, void *value, int data_type, int index, int length, int count )
{
  struct h_node *searcher;
  struct h_node *temp;

  // Create our hash item.
  temp = malloc( sizeof( struct h_node ) );
  temp->key = strdup( key );
  temp->value = value;
  temp->data_type = data_type;
  temp->count = count;
  temp->next = NULL;

  // Set up the linked list iterator.
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

int djbhash_set( struct h_table *ht, char *key, void *value, int data_type, ... )
{
  int count;
  int index;
  int length;
  va_list arg_ptr;

  // Default invalid data types.
  if ( data_type < DJBHASH_INT || data_type > DJBHASH_ARRAY )
    data_type = DJBHASH_STRING;

  // If the data type is an array, track how many items the array has.
  if ( data_type == DJBHASH_ARRAY )
  {
    va_start( arg_ptr, 1 );
    count = va_arg( arg_ptr, int );
    va_end( arg_ptr );
  }

  length = strlen( key );
  index = djb_hash( ht, key, length );

  if ( ht->t_type == DJBHASH_ARRAY_LIST )
    return djbhash_set_arr( ht, key, value, data_type, index, length, count );
  else
    return djbhash_set_ll( ht, key, value, data_type, index, length, count );
}

// Find an item in the hash table using linked lists.
struct h_node *djbhash_find_ll( struct h_table *ht, char *key )
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

// Find an item in the hash table using array mode.
struct h_node *djbhash_find_arr( struct h_table *ht, char *key )
{
  int length;
  int index;
  int pos;

  length = strlen( key );
  index = djb_hash( ht, key, length );
  pos = djbhash_bin_search( ht, key, 0, ht->count[index] - 1, index, length, false );

  if ( pos >= 0 )
  {
    return &ht->ll[index][pos];
  } else
    return NULL;
}

// Find an item in the hash.
struct h_node *djbhash_find( struct h_table *ht, char *key )
{
  if ( ht->t_type == DJBHASH_LINKED_LIST )
    return djbhash_find_ll( ht, key );
  else
    return djbhash_find_arr( ht, key );
}

// Dump all data in the hash table using linked lists.
void djbhash_dump( struct h_table *ht )
{
  int i, j;
  struct h_node *iter;

  if ( ht->t_type == DJBHASH_LINKED_LIST )
  {
    for ( i = 0; i < ht->t_size; i++ )
    {
      iter = ht->ll[i];
      while ( iter )
        djbhash_print( iter );
    }
  } else
  {
    for ( i = 0; i < ht->t_size; i++ )
    {
      for ( j = 0; j < ht->count[i]; j++ )
        djbhash_print( &ht->ll[i][j] );
    }
  }
}

// Remove all elements from the hash table.
void djbhash_empty( struct h_table *ht )
{
  int i, j;
  struct h_node *iter;
  struct h_node *next;
  for ( i = 0; i < ht->t_size; i++ )
  {
    if ( ht->t_type == DJBHASH_LINKED_LIST )
    {
      iter = ht->ll[i];
      while ( iter )
      {
        if ( iter->key != NULL )
        {
          free( iter->key );
          iter->key = NULL;
        }
        next = iter->next;
        free( iter );
        iter = next;
      }
    } else
    {
      for ( j = 0; j < ht->count[i]; j++ )
      {
        if ( ht->ll[i][j].key != NULL )
        {
          free( ht->ll[i][j].key );
          ht->ll[i][j].key = NULL;
        }
      }
      ht->count[i] = 0;
      free( ht->ll[i] );
      ht->ll[i] = NULL;
    }
  }
}

// Remove all elements and frees memory used by the hash table.
void djbhash_destroy( struct h_table *ht )
{
  djbhash_empty( ht );
  free( ht->ll );
  ht->ll = NULL;
}
