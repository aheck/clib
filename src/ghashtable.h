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
#include <string.h>
#include <stdlib.h>

#define GHASHTABLE_MIN_SLOTS 64
#define GHASHTABLE_MAX_LOAD 0.5

typedef uint32_t (*GHashFunc)(void *key);
typedef bool (*GEqualFunc)(void *a, void *b);
typedef void (*GDestroyNotify)(void *data);
typedef void (*GHFunc) (void *key, void *value, void *user_data);
typedef bool (*GHRFunc) (void *key, void *value, void *user_data);

struct GHashTableSlot {
    void *key;
    void *value;
    bool used : 1;
    bool deleted : 1;
};

typedef struct GHashTable {
    uint32_t num_slots;
    uint32_t num_used;
    uint32_t resize_threshold;
    GHashFunc hash_func;
    GEqualFunc key_equal_func;
    GDestroyNotify key_destroy_func;
    GDestroyNotify value_destroy_func;
    struct GHashTableSlot *slots;
} GHashTable;

uint32_t g_int_hash(void *v);
bool g_int_equal(void *v1, void *v2);
uint32_t g_str_hash(void *v);
bool g_str_equal(void *v1, void *v2);
GHashTable *g_hash_table_new(GHashFunc hash_func, GEqualFunc key_equal_func);
GHashTable *g_hash_table_new_full(GHashFunc hash_func, GEqualFunc key_equal_func, GDestroyNotify key_destroy_func, GDestroyNotify value_destroy_func);
void g_hash_table_insert(GHashTable *hash_table, void *key, void *value);
uint32_t g_hash_table_size(GHashTable *hash_table);
void* g_hash_table_lookup(GHashTable *hash_table, void *key);
void g_hash_table_foreach(GHashTable *hash_table, GHFunc func, void *user_data);
bool g_hash_table_remove(GHashTable *hash_table, void *key);
void g_hash_table_destroy(GHashTable *hash_table);


#ifdef _CLIB_IMPL

uint32_t g_int_hash(void *v)
{
    uint32_t x = (uint32_t) (uint64_t) v; // cast to uint64_t to omit warning

    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;

    return x;
}

bool g_int_equal(void *v1, void *v2)
{
    return (uint32_t*) v1 == (uint32_t*) v2;
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
        fprintf(stderr, "FATAL ERROR: g_hash_table_new: Out of memory");
        exit(1);
    }

    hash_table->num_slots = GHASHTABLE_MIN_SLOTS;
    hash_table->num_used = 0;
    hash_table->resize_threshold = (uint32_t) (hash_table->num_slots * GHASHTABLE_MAX_LOAD);
    hash_table->hash_func = hash_func;
    hash_table->key_equal_func = key_equal_func;
    hash_table->key_destroy_func = NULL;
    hash_table->value_destroy_func = NULL;

    size_t buf_size = hash_table->num_slots * sizeof(struct GHashTableSlot);
    hash_table->slots = malloc(buf_size);
    if (hash_table->slots == NULL) {
        fprintf(stderr, "FATAL ERROR: g_hash_table_new: Out of memory");
        exit(1);
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

uint32_t _g_hash_table_find_free_slot(GHashTable *hash_table, uint32_t start_slot, void *key)
{
    for (uint32_t i = start_slot; i < hash_table->num_slots; i++) {
        if (hash_table->slots[i].used == false) {
            return i;
        }

        if (key && hash_table->key_equal_func(hash_table->slots[i].key, key)) {
            return i;
        }
    }

    for (uint32_t i = 0; i < start_slot; i++) {
        if (hash_table->slots[i].used == false) {
            return i;
        }

        if (key && hash_table->key_equal_func(hash_table->slots[i].key, key)) {
            return i;
        }
    }

    fprintf(stderr, "BUG: _g_hash_table_find_free_slot: Failed to find an empty slot.");
    int *ptr = NULL;
    *ptr = 0;

    return 0;
}

uint32_t _g_hash_table_calc_start_slot(GHashTable *hash_table, void *key)
{
    return hash_table->hash_func(key) % hash_table->num_slots;
}

uint32_t _g_hash_table_find_slot_by_key(GHashTable *hash_table, void *key, uint32_t start_slot, bool *ret_found)
{
    for (uint32_t i = start_slot; i < hash_table->num_slots; i++) {
        if (hash_table->slots[i].used == false && hash_table->slots[i].deleted == false) {
            *ret_found = false;
            return 0;
        }

        if (hash_table->key_equal_func(key, hash_table->slots[i].key)) {
            *ret_found = true;
            return i;
        }
    }

    for (uint32_t i = 0; i < start_slot; i++) {
        if (hash_table->slots[i].used == false) {
            *ret_found = false;
            return 0;
        }

        if (hash_table->key_equal_func(key, hash_table->slots[i].key)) {
            *ret_found = true;
            return i;
        }
    }

    *ret_found = false;
    return 0;
}

void _g_hash_table_resize(GHashTable *hash_table, uint32_t new_num_slots)
{
    uint32_t old_num_slots = hash_table->num_slots;
    struct GHashTableSlot *old_slots = hash_table->slots;
    struct GHashTableSlot *new_slots;

    size_t buf_size = new_num_slots * sizeof(struct GHashTableSlot);
    new_slots = malloc(buf_size);
    if (new_slots == NULL) {
        fprintf(stderr, "FATAL ERROR: _g_hash_table_resize: Out of memory");
        exit(1);
    }

    hash_table->slots = new_slots;
    hash_table->num_used = 0;
    hash_table->num_slots = new_num_slots;
    hash_table->resize_threshold = (uint32_t) (hash_table->num_slots * GHASHTABLE_MAX_LOAD);

    memset(hash_table->slots, 0, buf_size);

    for (uint32_t i = 0; i < old_num_slots; i++) {
        if (old_slots[i].used) {
            g_hash_table_insert(hash_table, old_slots[i].key, old_slots[i].value);
        }
    }

    free(old_slots);
}

void g_hash_table_insert(GHashTable *hash_table, void *key, void *value)
{
    if (hash_table->num_used >= hash_table->resize_threshold) {
        _g_hash_table_resize(hash_table, hash_table->num_slots * 2);
    }

    uint32_t start_slot = _g_hash_table_calc_start_slot(hash_table, key);
    uint32_t slot = 0;

    if (hash_table->slots[start_slot].used == false) {
        slot = start_slot;
    } else {
        // hashed slot is already used
        // search for next free one
        slot = _g_hash_table_find_free_slot(hash_table, start_slot, key);
    }

    if (hash_table->slots[slot].used) {
        // key already exists in the hash table
        if (hash_table->key_destroy_func && key != hash_table->slots[slot].key) {
            hash_table->key_destroy_func(hash_table->slots[slot].key);
        }

        if (hash_table->value_destroy_func && value != hash_table->slots[slot].value) {
            hash_table->value_destroy_func(hash_table->slots[slot].value);
        }

        hash_table->slots[slot].value = value;
    } else {
        hash_table->slots[slot].key = key;
        hash_table->slots[slot].value = value;
        hash_table->slots[slot].used = true;
        hash_table->slots[slot].deleted = false;
        hash_table->num_used++;
    }
}

uint32_t g_hash_table_size(GHashTable *hash_table)
{
    return hash_table->num_used;
}

void* g_hash_table_lookup(GHashTable *hash_table, void *key)
{
    bool found = false;
    uint32_t start_slot = _g_hash_table_calc_start_slot(hash_table, key);
    uint32_t slot = _g_hash_table_find_slot_by_key(hash_table, key, start_slot, &found);

    if (!found) {
        return NULL;
    }

    return hash_table->slots[slot].value;
}

void g_hash_table_foreach(GHashTable *hash_table, GHFunc func, void *user_data)
{
    for (uint32_t i = 0; i < hash_table->num_slots; i++) {
        if (hash_table->slots[i].used == true) {
            func(hash_table->slots[i].key, hash_table->slots[i].value, user_data);
        }
    }
}

bool g_hash_table_remove(GHashTable *hash_table, void *key)
{
    bool found = false;
    uint32_t start_slot = _g_hash_table_calc_start_slot(hash_table, key);
    uint32_t slot = _g_hash_table_find_slot_by_key(hash_table, key, start_slot, &found);

    if (!found) {
        return false;
    }

    if (hash_table->key_destroy_func) {
        hash_table->key_destroy_func(hash_table->slots[slot].key);
    }

    if (hash_table->value_destroy_func) {
        hash_table->value_destroy_func(hash_table->slots[slot].value);
    }

    hash_table->slots[slot].key = 0;
    hash_table->slots[slot].value = 0;
    hash_table->slots[slot].used = false;
    hash_table->slots[slot].deleted = true;
    hash_table->num_used--;

    return true;
}

void g_hash_table_destroy(GHashTable *hash_table)
{
    if (hash_table) {
        if (hash_table->key_destroy_func || hash_table->value_destroy_func) {
            for (uint32_t i = 0; i < hash_table->num_slots; i++) {
                if (hash_table->slots[i].used == false) {
                    continue;
                }

                if (hash_table->key_destroy_func) {
                    hash_table->key_destroy_func(hash_table->slots[i].key);
                }

                if (hash_table->value_destroy_func) {
                    hash_table->value_destroy_func(hash_table->slots[i].value);
                }
            }
        }

        if (hash_table->slots) {
            free(hash_table->slots);
        }
        free(hash_table);
    }
}

#endif
#endif
