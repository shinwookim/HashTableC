#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashtable.h"

// Tester
int main(void)
{
    ht_hashtable *ht = ht_new();
    remove_table();
}

// Private (static) function is only called internally
static ht_item *new_item(const char *key, const char *value)
{
    ht_item *item = malloc(sizeof(ht_item));
    item->key = strdup(key);
    item->value = strdup(value);
    return item;
}

ht_hashtable *new_table()
{
    ht_hashtable *table = malloc(sizeof(ht_hashtable));
    table->size = 53; // To be resized later
    table->count = 0; // Table is initially empty
    table->items = calloc((size_t)table->size, sizeof(ht_item *));
    return table;
}

static void remove_item(ht_item *item)
{
    free(item->key);
    free(item->value);
    free(item);
}

void remove_table(ht_hashtable *table)
{
    for (int i = 0; i < table->size; i++)
    {
        ht_item *item = table->items[i];
        if (!item)
            remove_item(item);
    }
    free(table->items);
    free(table);
}

/* A hash function must take in a string input and
 * return a number in [0, m] where m is the desired table size
 */

// Prime must be a prime number that is larger than the size of the alphabet (> 128 for ASCII)
int hash(const char *key, const int prime, const int m)
{
    long hash = 0;
    const int key_length = strlen(key);
    for (int i = 0; i < key_length; i++)
    {
        hash += (long)pow(prime, (key_length - (i + 1))) * key[i];
        hash = hash % m;
    }
    return (int)hash;
}
