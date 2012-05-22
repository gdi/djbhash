##djbll - A simple hash table implementation using the DJB string hashing function.

### Usage:
#### Initializing the hash table.
```c
  // The hash table object.
  struct h_table ht;

  // Initialize the hash table with a set number of linked lists (2 in this case).
  // Typically, the 2 here should be about equal to the sqare root of the number of
  //    items expected for good distribution.
  djbll_init( &ht, 2 );
```

#### Adding an item to the hash.
```c
  /* djbll_set( <hash>, <key>, <value>, <data type>, <count (optional) >.
   *
   * Supported data types
   * =====================
   *    DJBLL_INT => int
   *    DJBLL_DOUBLE => double (or float)
   *    DJBLL_CHAR => char
   *    DJBLL_STRING => char *, const char *
   *    DJBLL_ARRAY => int * (When used, must also pass the count parameter)
  */

  // Item with int value.
  int temp = ( 10 );
  djbll_set( &ht, "int", &temp, DJBLL_INT );

  // Item with a double value.
  double temp2 = 3.14159;
  djbll_set( &ht, "double", &temp2, DJBLL_DOUBLE );

  // Item with a char value.
  char temp3 = 'a';
  djbll_set( &ht, "char", &temp3, DJBLL_CHAR );

  // Item with a string value.
  djbll_set( &ht, "string", "bar", DJBLL_STRING );

  // Item with an array value - notice the additional param.
  int temp_arr[] = { 8, 6, 7, 5, 3, 0, 9 };
  djbll_set( &ht, "array", temp_arr, DJBLL_ARRAY, 7 );
```

#### Finding an item in the hash.
```c
  // Pointer to the hash item.
  struct h_node *item;

  // Add an item to find.
  djbll_set( &ht, "foo", "bar", DJBLL_STRING );

  // Find and print an item.
  item = djbll_find( &ht, "foo" );
  djbll_print( item );
  // Output: "foo => bar"

  // The value item (void * data type) can be accessed with item->value.

  // Search for an item that doesn't exist:
  char *missing = "missing";
  if ( ( item = djbll_find( &ht, missing ) ) == NULL )
    printf( "%s: No such item!\n", missing );
```

#### Cleanup:
  // Remove all items and free memory.
  djbll_destroy( &ht );
```

### For a full example, see test.c.
