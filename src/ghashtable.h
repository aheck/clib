/*
 * GHashTable
 *
 * Copyright (c) 2022 Andreas Heck <aheck@gmx.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _GHASHTABLE_H
#define _GHASHTABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <strings.h>

#define GHASHTABLE_MIN_SLOTS 64
#define GHASHTABLE_MAX_LOAD 0.5

typedef uint32_t (*GHashFunc)(void *key);
typedef bool (*GEqualFunc)(void *a, void *b);
typedef void (*GDestroyNotify)(void *data);
bool (*GHRFunc) (void *key, void *value, void *user_data);

uint32_t g_int_hash(void *v)
{
    uint32_t x = *((uint32_t*) v);

    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;

    return x;
}

bool g_int_equal(void *v1, void *v2)
{
    return *((uint32_t*) v1) == *((uint32_t*) v2);
}

uint32_t g_str_hash(void *v)
{
    // djb2 hash function
    uint32_t hash = 5381;
    int c;
    unsigned char *str = (unsigned char*) v;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

bool g_str_equal(void *v1, void *v2)
{
    return strcmp((char*) v1, (char*) v2) == 0;
}

struct GHashTableSlot {
    void *key;
    void *value;
};

typedef struct GHashTable {
    size_t num_slots;
    size_t num_used;
    GHashFunc hash_func;
    GEqualFunc key_equal_func;
    GDestroyNotify key_destroy_func;
    GDestroyNotify value_destroy_func;
    struct GHashTableSlot *slots;
} GHashTable;

GHashTable *g_hash_table_new(GHashFunc hash_func, GEqualFunc key_equal_func)
{
    if (hash_func == NULL) {
        return NULL;
    }

    if (key_equal_func == NULL) {
        return NULL;
    }

    GHashTable *hash_table = (GHashTable*) malloc(sizeof(GHashTable));
    if (hash_table == NULL) {
        return NULL;
    }

    hash_table->num_slots = GHASHTABLE_MIN_SLOTS;
    hash_table->num_used = 0;
    hash_table->hash_func = hash_func;
    hash_table->key_equal_func = key_equal_func;
    hash_table->key_destroy_func = NULL;
    hash_table->value_destroy_func = NULL;

    size_t buf_size = hash_table->num_slots * sizeof(struct GHashTableSlot);
    hash_table->slots = malloc(buf_size);
    if (hash_table->slots == NULL) {
        free(hash_table);
        return NULL;
    }

    memset(hash_table->slots, 0, buf_size);

    return hash_table;
}

GHashTable *g_hash_table_new_full(GHashFunc hash_func, GEqualFunc key_equal_func, GDestroyNotify key_destroy_func, GDestroyNotify value_destroy_func)
{
    GHashTable *hash_table = g_hash_table_new(hash_func, key_equal_func);

    if (hash_table == NULL) {
        return NULL;
    }

    hash_table->key_destroy_func = key_destroy_func;
    hash_table->value_destroy_func = value_destroy_func;

    return hash_table;
}

void g_hash_table_destroy(GHashTable *hash_table)
{
    if (hash_table) {
        if (hash_table->slots) {
            free(hash_table->slots);
        }
        free(hash_table);
    }
}

#endif
