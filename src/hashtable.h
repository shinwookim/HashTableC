
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

void insert(ht_hashtable *table, const char *key, const char *value);
char *search(ht_hashtable *table, const char *key);
void remove(ht_hashtable *table, const char *key);