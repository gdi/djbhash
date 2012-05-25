##djbhash - A simple c hash implementation using the DJB string hashing function.

### Usage:
#### Initializing the hash table.
```c
  // The hash table object.
  struct djbhash hash;

  // Initialize the hash.
  djbhash_init( &hash );
```

#### Adding an item to the hash.
```c
  /* djbhash_set( &<hash>, <key>, <value>, <data type>, (<count> optional).
   *
   * Supported data types
   * =====================
   *    DJBHASH_INT => int
   *    DJBHASH_DOUBLE => double (or float)
   *    DJBHASH_CHAR => char
   *    DJBHASH_STRING => char *, const char *
   *    DJBHASH_ARRAY => int * (When used, must also pass the count parameter)
   *    DJBHASH_HASH => another djbhash object.
   *    DJBHASH_OTHER => generic void *, use with caution!.
  */

  // Item with int value.
  int temp = ( 10 );
  djbhash_set( &hash, "int", &temp, DJBHASH_INT );

  // Item with a double value.
  double temp2 = 3.14159;
  djbhash_set( &hash, "double", &temp2, DJBHASH_DOUBLE );

  // Item with a char value.
  char temp3 = 'a';
  djbhash_set( &hash, "char", &temp3, DJBHASH_CHAR );

  // Item with a string value.
  djbhash_set( &hash, "string", "bar", DJBHASH_STRING );

  // Item with an array value - notice the additional param.
  int temp_arr[] = { 8, 6, 7, 5, 3, 0, 9 };
  djbhash_set( &hash, "array", temp_arr, DJBHASH_ARRAY, 7 );

  // An embedded hash within this hash.
  struct djbhash temp_hash;
  djbhash_init( &temp_hash );
  djbhash_set( &temp_hash, "foo", "bar", DJBHASH_STRING );
  djbhash_set( &temp_hash, "baz", temp_arr, DJBHASH_ARRAY, 7 );
  djbhash_set( &hash, "hash", &temp_hash, DJBHASH_HASH );

  // Item with a different data type.
  //   suppose you declared: struct test_struct { int a; int b; };
  struct test_struct test;
  test.a = 10;
  test.b = 5;
  djbhash_set( &hash, "other", test, DJBHASH_OTHER );
```

#### Finding an item in the hash.
```c
  // Pointer to the hash item.
  struct djbhash_node *item;
  void *value;

  // Add an item to find.
  djbhash_set( &hash, "foo", "bar", DJBHASH_STRING );

  // Find and print the item.
  item = djbhash_find( &hash, "foo" );
  value = item->value;
  djbhash_print( item );

  // Search for an item that doesn't exist:
  char *missing = "missing";
  if ( ( item = djbhash_find( &hash, missing ) ) == NULL )
    printf( "%s: No such item!\n", missing );
```

### Removing an item in the hash.
```c
  djbhash_remove( &hash, "int" );
  if ( djbhash_find( &hash, "int" ) == NULL )
    printf( "item removedn" );
```

### Print all items in the hash.
```c
  djbhash_dump( &hash );
```

### Iterate through all items in the hash.
```c
  item = djbhash_iterate( &hash );
  while ( item )
  {
    djbhash_print( item );
    item = djbhash_iterate( &hash );
  }
```

### Getting a JSON formated string of the hash.
```c
  char *json = djbhash_to_json( &hash );
  printf( "JSON: %s\n", json );
  free( json );
  json = NULL;
```

### Reset the iterator to the first item.
```c
  djbhash_reset_iterator( &hash );
```

#### Cleanup:
```c
  // Remove all items and free memory.
  djbhash_destroy( &hash );
```

### For a full example, see test.c.
