
# HashTable (C)
HashTableC is an implementation of a fully-featured, [open-addressed](https://en.wikipedia.org/wiki/Open_addressing), [double hashed](https://en.wikipedia.org/wiki/Double_hashing), and auto-resizing hash table in pure C.


## Usage
```c
#include "hashtable.h"

int main(void)
{
    // Create a new hashtable
    ht_hashtable *table = ht_new_table();
    
    // Insert Key-Value Pair
    ht_insert(table, "k", "v");
    
    // Search for Key (Returns NULL if not found)
    char *value = ht_search(ht, "search_key");
    
    // Remove Key-Value Pair
    ht_remove(ht, "key");
    
    // Delete Table
    ht_delete_table(table);
}

```
## License

This project is released under the GPL 3. For more information, see the `LICENSE` file.