
typedef struct
{
    char *key;
    char *value;
} ht_item;

typedef struct
{
    int base_size;
    int size;
    int count;       // Number of populated slots in array
    ht_item **items; // Array of pointers to Key-Value pairs
} ht_hashtable;

void ht_insert(ht_hashtable *table, const char *key, const char *value);
char *ht_search(ht_hashtable *table, const char *key);
void ht_remove(ht_hashtable *table, const char *key);

ht_hashtable *ht_new_table();
void ht_delete_table(ht_hashtable *table);