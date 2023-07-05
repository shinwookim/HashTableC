
typedef struct
{
    char *key;
    char *value;
} ht_item;

typedef struct
{
    int size;
    int count;       // Number of populated slots in array
    ht_item **items; // Array of pointers to Key-Value pairs
} ht_hashtable;