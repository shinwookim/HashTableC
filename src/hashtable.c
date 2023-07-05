#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashtable.h"

#define HT_PRIME_1 2
#define HT_PRIME_2 3
#define HT_INIT_BASE_SIZE 53

static ht_item HT_REMOVED_ITEM = {NULL, NULL};

// Private (static) function is only called internally
static ht_item *ht_new_item(const char *key, const char *value)
{
    ht_item *item = malloc(sizeof(ht_item));
    item->key = strdup(key);
    item->value = strdup(value);
    return item;
}

ht_hashtable *ht_new_table()
{
    return ht_new_table_with_size(HT_INIT_BASE_SIZE);
}
ht_hashtable *ht_new_table_with_size(const int base_size)
{
    ht_hashtable *table = malloc(sizeof(ht_hashtable));
    table->base_size = base_size;
    table->size = next_prime(table->base_size);
    table->count = 0; // Table is initially empty
    table->items = calloc((size_t)table->size, sizeof(ht_item *));
    return table;
}

void ht_resize(ht_hashtable *table, const int base_size)
{
    if (base_size < HT_INIT_BASE_SIZE) // check to makesure we don't dpwnsie below min
        return;
    ht_hashtable *new_table = ht_new_table_with_size(base_size);
    for (int i = 0; i < table->size; i++)
    {
        ht_item *item = table->items[i];
        if (item != NULL && item != &HT_REMOVED_ITEM)
            ht_insert(new_table, item->key, item->value);
    }

    table->base_size = new_table->base_size;
    table->count = new_table->count;

    // To delete new_ht, we give it ht's size and items
    const int tmp_size = table->size;
    table->size = new_table->size;
    new_table->size = tmp_size;

    ht_item **tmp_items = table->items;
    table->items = new_table->items;
    new_table->items = tmp_items;

    ht_del_hash_table(new_table);
}

static void ht_remove_item(ht_item *item)
{
    free(item->key);
    free(item->value);
    free(item);
}

static void ht_resize_up(ht_hashtable *table)
{
    const int new_size = table->base_size * 2;
    ht_resize(table, new_size);
}

static void ht_resize_down(ht_hashtable *table)
{
    const int new_size = table->base_size / 2;
    ht_resize(table, new_size);
}

void ht_remove_table(ht_hashtable *table)
{
    for (int i = 0; i < table->size; i++)
    {
        ht_item *item = table->items[i];
        if (item)
            ht_remove_item(item);
    }
    free(table->items);
    free(table);
}

/* A hash function must take in a string input and
 * return a number in [0, m] where m is the desired table size
 */

// Prime must be a prime number that is larger than the size of the alphabet (> 128 for ASCII)
int ht_hash(const char *key, const int prime, const int m)
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
static int ht_get_hash(const char *key, const int m, const int i)
{
    return (hash(key, HT_PRIME_1, m) + i * (hash(key, HT_PRIME_2, m) + 1)) % m;
}

// Hashtable API

// hash_table.h
void insert(ht_hashtable *table, const char *key, const char *value)
{
    const int load = table->count * 100 / table->size;
    if (load > 70)
    {
        ht_resize_up(table);
    }
    ht_item *item = new_item(key, value);
    int index = get_hash(item->key, table->size, 0);
    ht_item *current_item = table->items[index];
    int iter = 1;
    while (current_item && current_item != &HT_REMOVED_ITEM)
    {
        if (strcmp(current_item->key, key) == 0)
        {
            ht_remove_item(current_item);
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
void remove(ht_hashtable *table, const char *key)
{
    const int load = table->count * 100 / table->size;
    if (load < 10)
    {
        ht_resize_down(table);
    }
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