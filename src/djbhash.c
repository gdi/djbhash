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
  int i;
  hash->buckets = malloc( sizeof( struct djbhash_bucket ) * DJBHASH_MAX_BUCKETS );
  hash->active = malloc( sizeof( int ) * DJBHASH_MAX_BUCKETS );
  hash->active_count = 0;
  hash->iter.node = NULL;
  hash->iter.last = NULL;
  hash->iter.id = 0;
  for ( i = 0; i < DJBHASH_MAX_BUCKETS; i++ )
  {
    hash->buckets[i].id = i;
    hash->buckets[i].list = NULL;
  }
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
  pos.item = NULL;
  pos.parent = parent;
  return pos;
}

// Create our own memory for the item value so we don't have to worry about local values and such.
void *djbhash_value( void *value, int data_type, int count )
{
  int i;
  long *temp, *iter;
  double *temp2;
  unsigned char *temp3;
  void *ptr;

  switch( data_type )
  {
    case DJBHASH_INT:
      temp = malloc( sizeof( long ) );
      *temp = *( long * )value;
      ptr = temp;
      break;
    case DJBHASH_DOUBLE:
      temp2 = malloc( sizeof( double ) );
      *temp2 = *( double * )value;
      ptr = temp2;
      break;
    case DJBHASH_CHAR:
      temp3 = malloc( sizeof( unsigned char ) );
      *temp3 = *( unsigned char * )value;
      ptr = temp3;
      break;
    case DJBHASH_STRING:
      ptr = strdup( ( unsigned char * )value );
      break;
    case DJBHASH_ARRAY:
      temp = malloc( sizeof( long ) * count );
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
  search = djbhash_bin_search( hash, 0, DJBHASH_MAX_BUCKETS - 1, bucket_id, key, length );

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

  if ( search.parent == NULL )
  {
    hash->buckets[search.bucket_id].list = temp;
    hash->active_count++;
    hash->active[hash->active_count - 1] = search.bucket_id;
  } else
  {
    search.parent->next = temp;
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
  search = djbhash_bin_search( hash, 0, DJBHASH_MAX_BUCKETS - 1, bucket_id, key, length );
  return search.item;
}

// Remove an item from the hash.
int djbhash_remove( struct djbhash *hash, char *key )
{
  int i, offset;
  int length;
  int bucket_id;
  struct djbhash_search search;
  struct djbhash_node *item, *parent, *next;

  length = strlen( key );
  bucket_id = djb_hash( key, length );
  search = djbhash_bin_search( hash, 0, DJBHASH_MAX_BUCKETS - 1, bucket_id, key, length );

  // If we don't find the item, we obviously can't remove it.
  if ( !search.found )
    return false;

  // Otherwise, free the item, and set the parent node's next to the item's next.
  item = search.item;
  parent = search.parent;
  next = search.item->next;

  if ( parent == item )
  {
    hash->buckets[search.bucket_id].list = next;
    if ( hash->buckets[search.bucket_id].list == NULL )
    {
      offset = 0;
      // Remove this from active buckets.
      for ( i = 0; i < hash->active_count; i++ )
      {
        if ( hash->active[i] == search.bucket_id )
          offset = 1;
        else
          hash->active[i - offset] = hash->active[i];
      }
      hash->active_count--;
    }
  } else
  {
    parent->next = next;
  }

  djbhash_free_node( search.item );
  return true;
}

// Dump all data in the hash table using linked lists.
void djbhash_dump( struct djbhash *hash )
{
  int i;
  struct djbhash_node *iter;

  for ( i = 0; i < hash->active_count; i++ )
  {
    iter = hash->buckets[hash->active[i]].list;
    while ( iter )
    {
      djbhash_print( iter );
      iter = iter->next;
    }
  }
}

// Iterate through all hash items one at a time.
struct djbhash_node *djbhash_iterate( struct djbhash *hash )
{
  if ( hash->iter.node == NULL && hash->iter.last == NULL )
  {
    if ( hash->active_count > 0 )
    {
      hash->iter.node = hash->buckets[hash->active[0]].list;
      return hash->iter.node;
    }
    return NULL;
  } else if ( hash->iter.node == NULL )
    return NULL;

  hash->iter.last = hash->iter.node;
  hash->iter.node = hash->iter.node->next;
  if ( hash->iter.node == NULL )
  {
    if ( hash->iter.id == hash->active_count - 1 )
      return NULL;
    hash->iter.id++;
    hash->iter.node = hash->buckets[hash->active[hash->iter.id]].list;
  }
  return hash->iter.node;
}

// Reset iterator.
void djbhash_reset_iterator( struct djbhash *hash )
{
  hash->iter.id = 0;
  hash->iter.node = NULL;
  hash->iter.last = NULL;
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
  for ( i = 0; i < hash->active_count; i++ )
  {
    iter = hash->buckets[hash->active[i]].list;
    while ( iter != NULL )
    {
      next = iter->next;
      djbhash_free_node( iter );
      iter = next;
    }
  }
  hash->active_count = 0;
}

// Remove all elements and frees memory used by the hash table.
void djbhash_destroy( struct djbhash *hash )
{
  djbhash_empty( hash );
  free( hash->buckets );
  hash->buckets = NULL;
  free( hash->active );
  hash->active = NULL;
}
