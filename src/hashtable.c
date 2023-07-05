#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashtable.h"
#include "prime.h"

/**
 * Hashtable Parameters
 */
static int HT_PRIME_1 = 151;                   // Parameter to hashing algorithm
static int HT_PRIME_2 = 163;                   // Parameter to hashing algorithm
static int HT_INIT_BASE_SIZE = 53;             // Minimum size of hashtable
static ht_item HT_REMOVED_ITEM = {NULL, NULL}; // Used to mark a bucket containing a removed item

/* Internal Functions */

/**
 * ht_new_item - Create a new key-value pair
 */
static ht_item *ht_new_item(const char *key, const char *value)
{
    ht_item *item = malloc(sizeof(ht_item));
    item->key = strdup(key);
    item->value = strdup(value);
    return item;
}

/**
 * ht_new_table_with_size - Creates a new table using parameterized base_size
 */
ht_hashtable *ht_new_table_with_size(const int base_size)
{
    ht_hashtable *table = malloc(sizeof(ht_hashtable));
    table->base_size = base_size;
    table->size = next_prime(table->base_size); // Size is always a prime number (for easier resizing)
    table->count = 0;                           // Table initially empty
    table->items = calloc((size_t)table->size, sizeof(ht_item *));
    return table;
}

/**
 * ht_new_table - Create new table with default table size
 */
ht_hashtable *ht_new_table()
{
    return ht_new_table_with_size(HT_INIT_BASE_SIZE);
}

/**
 * ht_delete_item - Deletes key-value pair
 */
static void ht_delete_item(ht_item *item)
{
    free(item->key);
    free(item->value);
    free(item);
}

/**
 * ht_delete_table - Deletes the entire hashtable
 */
void ht_delete_table(ht_hashtable *table)
{
    for (int i = 0; i < table->size; i++)
    {
        ht_item *item = table->items[i];
        if (item && item != &HT_REMOVED_ITEM)
            ht_delete_item(item);
    }
    free(table->items);
    free(table);
}

/**
 * ht_resize - Resizes (up/down) the table by creating new table and copying data
 */
void ht_resize(ht_hashtable *table, const int base_size)
{
    if (base_size < HT_INIT_BASE_SIZE) // To ensure we do not resize down below the minimum
        return;
    ht_hashtable *temp_table = ht_new_table_with_size(base_size);
    for (int i = 0; i < table->size; i++)
    {
        ht_item *item = table->items[i];
        if (item && item != &HT_REMOVED_ITEM)
            ht_insert(temp_table, item->key, item->value);
    }

    table->base_size = temp_table->base_size; // Swap values
    table->count = temp_table->count;         // temp_table's values set by ht_new_table_with_size

    // Put old values in temp_table and call ht_delete_table
    const int old_size = table->size;
    table->size = temp_table->size;
    temp_table->size = old_size;

    ht_item **old_items = table->items;
    table->items = temp_table->items;
    temp_table->items = old_items;

    ht_delete_table(temp_table); // Remove temp_table which holds old values
}

/**
 * ht_hash - Produces a integer in [0, m] using parameterized key input where m is the table size.
 * @param prime is a prime number that is larger than the size of the character set (128 for ASCII)
 */
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

/**
 * ht_get_hash - Wrapper for double-hashing scheme
 * @paragraph iter is the number of hash attempts
 */
static int ht_get_hash(const char *key, const int m, const int iter)
{
    return (ht_hash(key, HT_PRIME_1, m) + iter * (ht_hash(key, HT_PRIME_2, m) + 1)) % m;
}

/* Hashtable API */

/**
 * insert - Insert a Key-Value pair into the hashtable
 */
void ht_insert(ht_hashtable *table, const char *key, const char *value)
{
    if ((table->count * 100 / table->size) > 70)
    {                                           // If load is greater than 70%
        ht_resize(table, table->base_size * 2); // Resize (up) the hashtable for improved performance
    }
    ht_item *item = ht_new_item(key, value);
    int index = ht_get_hash(item->key, table->size, 0);
    ht_item *current_item = table->items[index];
    int iter = 1;
    while (current_item && current_item != &HT_REMOVED_ITEM)
    {
        if (strcmp(current_item->key, key) == 0) // If an item with same key already exists
        {                                        // Update the value of the item
            ht_delete_item(current_item);
            table->items[index] = item;
            return;
        }                                                  // If a collision occurred (with a different key)
        index = ht_get_hash(item->key, table->size, iter); // Handle using double-hashing
        current_item = table->items[index];
        iter++;
    }
    table->items[index] = item;
    table->count++;
}

/**
 * search - Search for a matching item in the table using the key
 */
char *ht_search(ht_hashtable *table, const char *key)
{
    int index = ht_get_hash(key, table->size, 0);
    ht_item *item = table->items[index];
    int iter = 1;
    while (item && item != &HT_REMOVED_ITEM)
    {
        if (strcmp(item->key, key) == 0) // If key matches
            return item->value;          // Return value
        else
        {
            index = ht_get_hash(key, table->size, iter); // Attempt to find using double-hash
            item = table->items[index];
            iter++;
        }
    }
    return NULL;
}

// Deleting causes issue in open addressing, breaks collision chain,
// instead mark as removed (using sntinel value)

/**
 * remove - Remove item from table
 * @details Actually deleting the item from the table can cause issue (in open addressing) as it breaks the collision chain.
 * Instead, we will simply mark the slot as removed using a sentinel value
 */
void ht_remove(ht_hashtable *table, const char *key)
{
    if ((table->count * 100 / table->size) < 10)
    {                                           // If the load is under 10%
        ht_resize(table, table->base_size / 2); // Resize (down) to save memory usage
    }
    int index = ht_get_hash(key, table->size, 0);
    ht_item *item = table->items[index];
    int iter = 1;
    while (item)
    {
        if (item != &HT_REMOVED_ITEM)
        {
            if (strcmp(item->key, key) == 0)
            {
                ht_delete_item(item);
                table->items[index] = &HT_REMOVED_ITEM;
            }
            index = ht_get_hash(key, table->size, iter);
            item = table->items[index];
            iter++;
        }
    }
    table->count--;
}