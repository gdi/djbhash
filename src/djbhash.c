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
      printf( "Unknown data type.%s" );
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
struct djbhash_search djbhash_bin_search( struct djbhash *hash, int min, int max, int bucket_id, char *key, int length )
{
  // Mid-point for search.
  int mid;
  // Linked list iterator and parent node.
  struct djbhash_node *iter, *parent;
  // Return variable.
  struct djbhash_search pos;

  // If max is less than min, we didn't find it.
  if ( max < min )
  {
    pos.bucket_id = min;
    pos.found = false;
    pos.collision = false;
    pos.item = NULL;
    pos.parent = NULL;
    return pos;
  }

  mid = ( min + max ) / 2;
  if ( hash->buckets[mid].id > bucket_id )
    return djbhash_bin_search( hash, min, mid - 1, bucket_id, key, length );
  else if ( hash->buckets[mid].id < bucket_id )
    return djbhash_bin_search( hash, mid + 1, max, bucket_id, key, length );

  // Point our iterator to the first element in this bucket.
  iter = hash->buckets[mid].list;
  parent = iter;
  while ( iter )
  {
    // We want to return if the key in the linked list actually matches.
    if ( strncmp( iter->key, key, length ) == 0 )
    {
      pos.bucket_id = mid;
      pos.found = true;
      pos.collision = true;
      pos.item = iter;
      pos.parent = parent;
      return pos;
    }
    parent = iter;
    iter = iter->next;
  }

  // If we got here, there the item doesn't actually exist, it's just a hash collision.
  pos.bucket_id = mid;
  pos.found = false;
  pos.collision = true;
  pos.item = NULL;
  pos.parent = parent;
  return pos;
}

// Create our own memory for the item value so we don't have to worry about local values and such.
void *djbhash_value( void *value, int data_type, int count )
{
  int i;
  int *temp, *iter;
  double *temp2;
  void *ptr;

  switch( data_type )
  {
    case DJBHASH_INT:
      temp = malloc( sizeof( int ) );
      *temp = *( int * )value;
      ptr = temp;
      break;
    case DJBHASH_DOUBLE:
      temp2 = malloc( sizeof( double ) );
      *temp2 = *( double * )value;
      ptr = temp2;
      break;
    case DJBHASH_CHAR:
      temp = malloc( sizeof( char ) );
      *temp = *( char * )value;
      ptr = temp;
      break;
    case DJBHASH_STRING:
      ptr = strdup( ( char * )value );
      break;
    case DJBHASH_ARRAY:
      temp = malloc( sizeof( int ) * count );
      iter = value;
      for ( i = 0; i < count; i++ )
        temp[i] = iter[i];
      ptr = temp;
      break;
    default:
      ptr = value;
  }
  return ptr;
}

// Set the value for an item in the hash table using array hash table.
int djbhash_set( struct djbhash *hash, char *key, void *value, int data_type, ... )
{
  int i;
  int chunks;
  struct djbhash_search search;
  unsigned int bucket_id;
  int length;
  va_list arg_ptr;
  struct djbhash_node *iter, *temp;
  int method;
  int count;

  // Default invalid data types.
  if ( data_type < DJBHASH_INT || data_type > DJBHASH_OTHER )
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

  // Find our insert/update/append position.
  search = djbhash_bin_search( hash, 0, hash->count - 1, bucket_id, key, length );

  // If we found the item with this key, we need to just update it.
  if ( search.found )
  {
    free( search.item->value );
    search.item->value = djbhash_value( value, data_type, count );
    return true;
  }

  // Create our hash item.
  temp = malloc( sizeof( struct djbhash_node ) );
  temp->key = strdup( key );
  temp->value = djbhash_value( value, data_type, count );
  temp->data_type = data_type;
  temp->count = count;
  temp->next = NULL;

  // If it's a hash collision, append it to the list.
  if ( search.collision )
  {
    if ( search.parent == NULL )
      hash->buckets[search.bucket_id].list = temp;
    else
      search.parent->next = temp;
  } else
  {
    // See if we need more memory.
    chunks = hash->count / DJBHASH_CHUNK_SIZE + 1;
    if ( hash->count > 0 && hash->count % DJBHASH_CHUNK_SIZE == 0 )
      hash->buckets = realloc( hash->buckets, sizeof( struct djbhash_bucket ) * ( ( chunks + 1 ) * DJBHASH_CHUNK_SIZE ) );

    // Move all of the larger elements over.
    for ( i = hash->count; i > search.bucket_id; i-- )
      hash->buckets[i] = hash->buckets[i - 1];

    // Finally, add this linked list and increment bucket count.
    hash->buckets[search.bucket_id].id = bucket_id;
    hash->buckets[search.bucket_id].list = temp;
    hash->count++;
  }
}

// Find an item in the hash table using linked lists.
struct djbhash_node *djbhash_find( struct djbhash *hash, char *key )
{
  int length;
  int bucket_id;
  struct djbhash_search search;
  struct djbhash_node *searcher;

  length = strlen( key );
  bucket_id = djb_hash( key, length );
  search = djbhash_bin_search( hash, 0, hash->count - 1, bucket_id, key, length );
  return search.item;
}

// Remove an item from the hash.
int djbhash_remove( struct djbhash *hash, char *key )
{
  int length;
  int bucket_id;
  struct djbhash_search search;
  struct djbhash_node *item, *parent, *next;

  length = strlen( key );
  bucket_id = djb_hash( key, length );
  search = djbhash_bin_search( hash, 0, hash->count - 1, bucket_id, key, length );

  // If we don't find the item, we obviously can't remove it.
  if ( !search.found )
    return false;

  // Otherwise, free the item, and set the parent node's next to the item's next.
  item = search.item;
  parent = search.parent;
  next = search.item->next;

  if ( parent == item )
    hash->buckets[search.bucket_id].list = next;
  else
    parent->next = next;

  djbhash_free_node( search.item );
  return true;
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

// Free memory used by a node.
void djbhash_free_node( struct djbhash_node *item )
{
  if ( item->key != NULL )
  {
    free( item->key );
    item->key = NULL;
  }
  if ( item->value != NULL && item->data_type != DJBHASH_OTHER )
  {
    free( item->value );
    item->value = NULL;
  }
  free( item );
  item = NULL;
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
    while ( iter != NULL )
    {
      next = iter->next;
      djbhash_free_node( iter );
      iter = next;
    }
  }
  free( hash->buckets );
  hash->buckets = malloc( sizeof( struct djbhash_bucket ) * DJBHASH_CHUNK_SIZE );
  hash->count = 0;
}

// Remove all elements and frees memory used by the hash table.
void djbhash_destroy( struct djbhash *hash )
{
  djbhash_empty( hash );
  free( hash->buckets );
  hash->buckets = NULL;
}
