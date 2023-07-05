#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashtable.h"

static ht_item HT_REMOVED_ITEM = {NULL, NULL};

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
        if (item)
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

/* Collision handling using open addressing & double hash */
// i for hash attempt
static int get_hash(const char *key, const int m, const int i)
{
    return (hash(key, HT_PRIME_1, m) + i * (hash(key, HT_PRIME_2, m) + 1)) % m;
}

// Hashtable API

// hash_table.h
void insert_item(ht_hashtable *table, const char *key, const char *value)
{
    ht_item *item = new_item(key, value);
    int index = get_hash(item->key, table->size, 0);
    ht_item *current_item = table->items[index];
    int iter = 1;
    while (current_item && current_item != &HT_REMOVED_ITEM)
    {
        if (strcmp(current_item->key, key) == 0)
        {
            remove_item(current_item);
            table->items[index] = item;
            return;
        }
        index = get_hash(item->key, table->size, iter);
        current_item = table->items[index];
        iter++;
    }
    table->items[index] = item;
    table->count++;
}

char *search(ht_hashtable *table, const char *key)
{
    int index = get_hash(key, table->size, 0);
    ht_item *item = table->items[index];
    int iter = 1;
    while (item)
    {
        if (item != &HT_REMOVED_ITEM)
        {
            if (strcmp(item->key, key) == 0)
                return item->value;
            else
            {
                index = get_hash(key, table->size, iter);
                item = table->items[index];
                iter++;
            }
        }
    }
    return NULL;
}

// Deleting causes issue in open addressing, breaks collision chain,
// instead mark as removed (using sntinel value)
void remove_item(ht_hashtable *table, const char *key)
{
    int index = get_hash(key, table->size, 0);
    ht_item *item = table->items[index];
    int iter = 1;
    while (item)
    {
        if (item != &HT_REMOVED_ITEM)
        {
            if (strcmp(item->key, key) == 0)
            {
                remove_item(item);
                table->items[index] = &HT_REMOVED_ITEM;
            }
            index = get_hash(key, table->size, iter);
            item = table->items[index];
            iter++;
        }
    }
    table->count--;
}