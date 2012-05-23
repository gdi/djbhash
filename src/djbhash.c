#include "djbhash.h"

// Print an items' data.
void djbhash_print_value( struct djbhash_node *item )
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
void djbhash_print( struct djbhash_node *item )
{
  printf( "%s => ", item->key );
  djbhash_print_value( item );
  printf( "\n" );
}

// Initialize the hash table.
void djbhash_init( struct djbhash *hash )
{
  hash->buckets = malloc( sizeof( struct djbhash_bucket ) * DJBHASH_CHUNK_SIZE );
  hash->count = 0;
}

// DJB Hash function.
unsigned int djb_hash( char *key, int length )
{
  unsigned int i;
  unsigned int hash;

  hash = 5381;
  for ( i = 0; i < length; key++, i++ )
    hash = ( ( hash << 5 ) + hash ) + ( *key );
  return hash % DJBHASH_MAX_BUCKETS;
}

// Find the bucket for the element.
int djbhash_bin_search( struct djbhash *hash, int min, int max, int bucket_id, char *key, int length, int insert_mode )
{
  // Mid-point for search.
  int mid;
  // Linked list iterator.
  struct djbhash_node *iter;

  if ( max < min )
  {
    if ( insert_mode )
      return min;
    else
      return -1;
  }

  mid = ( min + max ) / 2;
  if ( hash->buckets[mid].id > bucket_id )
    return djbhash_bin_search( hash, min, mid - 1, bucket_id, key, length, insert_mode );
  else if ( hash->buckets[mid].id < bucket_id )
    return djbhash_bin_search( hash, mid + 1, max, bucket_id, key, length, insert_mode );
  else
  {
    // Point our iterator to the first element in this bucket.
    iter = hash->buckets[mid].list;
    while ( iter )
    {
      // We want to return if the key in the linked list actually matches.
      if ( strncmp( iter->key, key, length ) == 0 )
        return mid;
      iter = iter->next;
    }

    // If we got here, there the item doesn't actually exist, it's just a hash collision.
    if ( insert_mode )
      return mid;
    else
      return -1;
  }
}

// Set the value for an item in the hash table using array hash table.
int djbhash_set( struct djbhash *hash, char *key, void *value, int data_type, ... )
{
  int i;
  int chunks;
  int insert_pos;
  unsigned int bucket_id;
  int length;
  va_list arg_ptr;
  struct djbhash_node *iter, *temp;
  int method;
  int count;

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

  // Calculate the key length and bucket ID.
  length = strlen( key );
  bucket_id = djb_hash( key, length );

  // Find out which bucket this item belongs to.
  insert_pos = djbhash_bin_search( hash, 0, hash->count - 1, bucket_id, key, length, true );

  // Create our hash item.
  temp = malloc( sizeof( struct djbhash_node ) );
  temp->key = key;
  temp->value = value;
  temp->data_type = data_type;
  temp->count = count;
  temp->next = NULL;

  // Find out if we are inserting, updating, or creating this item.
  method = DJBHASH_INSERT;
  if ( insert_pos < hash->count )
  {
    // Check if we're doing an insert or an update.
    if ( bucket_id == hash->buckets[insert_pos].id )
    {
      method = DJBHASH_APPEND;
      iter = hash->buckets[insert_pos].list;
      while ( iter->next != NULL )
      {
        if ( strncmp( iter->key, key, length ) == 0 )
        {
          method = DJBHASH_UPDATE;
          break;
        }
        iter = iter->next;
      }
    }
  }

  switch ( method )
  {
    case DJBHASH_UPDATE:
      free( temp );
      temp = NULL;
      iter->value = value;
      break; 
    case DJBHASH_APPEND:
      iter->next = temp;
      break;
    case DJBHASH_INSERT:
      // See if we need more memory.
      chunks = hash->count / DJBHASH_CHUNK_SIZE + 1;
      if ( hash->count > 0 && hash->count % DJBHASH_CHUNK_SIZE == 0 )
        hash->buckets = realloc( hash->buckets, sizeof( struct djbhash_bucket ) * ( ( chunks + 1 ) * DJBHASH_CHUNK_SIZE ) );

      // Move all of the larger elements over.
      for ( i = hash->count; i > insert_pos; i-- )
        hash->buckets[i] = hash->buckets[i - 1];

      hash->buckets[insert_pos].id = bucket_id;
      hash->buckets[insert_pos].list = temp;
      hash->count++;
      break;
  }
}

// Find an item in the hash table using linked lists.
struct djbhash_node *djbhash_find( struct djbhash *hash, char *key )
{
  int length;
  int bucket_id;
  int position;
  struct djbhash_node *searcher;

  length = strlen( key );
  bucket_id = djb_hash( key, length );
  position = djbhash_bin_search( hash, 0, hash->count - 1, bucket_id, key, length, false );

  // If we don't find the bucket ID, obviously it doesn't exist.
  if ( position == -1 )
    return NULL;

  // Otherwise, search through the linked list to find the item.
  searcher = hash->buckets[position].list;

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

// Dump all data in the hash table using linked lists.
void djbhash_dump( struct djbhash *hash )
{
  int i;
  struct djbhash_node *iter;

  for ( i = 0; i < hash->count; i++ )
  {
    iter = hash->buckets[i].list;
    while ( iter )
    {
      djbhash_print( iter );
      iter = iter->next;
    }
  }
}

// Remove all elements from the hash table.
void djbhash_empty( struct djbhash *hash )
{
  int i;
  struct djbhash_node *iter;
  struct djbhash_node *next;
  for ( i = 0; i < hash->count; i++ )
  {
    iter = hash->buckets[i].list;
    while ( iter )
    {
      next = iter->next;
      free( iter );
      iter = next;
    }
  }
  free( hash->buckets );
  hash->buckets = malloc( sizeof( struct djbhash_bucket ) );
  hash->count = 0;
}

// Remove all elements and frees memory used by the hash table.
void djbhash_destroy( struct djbhash *hash )
{
  djbhash_empty( hash );
  free( hash->buckets );
  hash->buckets = NULL;
}
