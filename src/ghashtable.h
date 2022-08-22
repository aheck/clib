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

#define GHASHTABLE_MIN_SLOTS 64
#define GHASHTABLE_MAX_LOAD 0.5

typedef unsigned int (*GHashFunc)(void *key);
typedef bool (*GEqualFunc)(void *a, void *b);
typedef void (*GDestroyNotify)(void *data);

typedef struct GHashTable {
    size_t num_slots;
    size_t num_used;
    GHashFunc hash_func;
    GEqualFunc key_equal_func;
    GDestroyNotify key_destroy_func;
    GDestroyNotify value_destroy_func;
} GHashTable;

GHashTable *g_hash_table_new(GHashFunc hash_func, GEqualFunc key_equal_func)
{
    if (hash_func == NULL) {
        return NULL;
    }

    if (key_equal_func == NULL) {
        return NULL;
    }

    GHashTable *htable = (GHashTable*) malloc(sizeof(GHashTable));
    if (htable == NULL) {
        return NULL;
    }

    htable->num_slots = 0;
    htable->num_used = 0;
    htable->hash_func = hash_func;
    htable->key_equal_func = key_equal_func;
    htable->key_destroy_func = NULL;
    htable->value_destroy_func = NULL;

    return NULL;
}

GHashTable *g_hash_table_new_full(GHashFunc hash_func, GEqualFunc key_equal_func, GDestroyNotify key_destroy_func, GDestroyNotify value_destroy_func)
{
    return NULL;
}

void g_hash_table_destroy(GHashTable *hash_table)
{
    if (hash_table) {
        free(hash_table);
    }
}

#endif
