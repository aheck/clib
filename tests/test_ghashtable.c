#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <check.h>

#define _CLIB_IMPL 1
#include "ghashtable.h"

// Fake hash function for testing
//
// The static hash value makes the location of the data in the hash table
// predictable
uint32_t fake_int_hash_0(void *v)
{
    return 0;
}

int keys_freed = 0;
void* last_freed_key = NULL;
int values_freed = 0;
void* last_freed_value = NULL;

void free_key_dummy(void *data)
{
    keys_freed++;
    last_freed_key = data;
}

void free_value_dummy(void *data)
{
    values_freed++;
    last_freed_value = data;
}

START_TEST(test_ghashtable_new)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(g_int_hash, g_int_equal);

    ck_assert_ptr_nonnull(htable);
    ck_assert_int_eq(htable->num_slots, GHASHTABLE_MIN_SLOTS);
    ck_assert_int_eq(htable->num_used, 0);
    ck_assert_ptr_eq(htable->hash_func, g_int_hash);
    ck_assert_ptr_eq(htable->key_equal_func, g_int_equal);
    ck_assert_ptr_eq(htable->key_destroy_func, NULL);
    ck_assert_ptr_eq(htable->value_destroy_func, NULL);
    ck_assert_ptr_nonnull(htable->slots);

    g_hash_table_destroy(htable);
}
END_TEST

START_TEST(test_ghashtable_new_full)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new_full(g_int_hash, g_int_equal, free_key_dummy, free_value_dummy);

    ck_assert_ptr_nonnull(htable);
    ck_assert_int_eq(htable->num_slots, GHASHTABLE_MIN_SLOTS);
    ck_assert_int_eq(htable->num_used, 0);
    ck_assert_ptr_eq(htable->hash_func, g_int_hash);
    ck_assert_ptr_eq(htable->key_equal_func, g_int_equal);
    ck_assert_ptr_eq(htable->key_destroy_func, free_key_dummy);
    ck_assert_ptr_eq(htable->value_destroy_func, free_value_dummy);
    ck_assert_ptr_nonnull(htable->slots);

    g_hash_table_destroy(htable);
}
END_TEST

START_TEST(test_ghashtable_find_free_slot)
{
    GHashTable *htable = NULL;
    uint32_t slot = 0;

    htable = g_hash_table_new(g_int_hash, g_int_equal);

    slot = _g_hash_table_find_free_slot(htable, 10, NULL);
    ck_assert_int_eq(slot, 10);

    // mark slot 10 as used
    htable->slots[10].used = true;

    slot = _g_hash_table_find_free_slot(htable, 10, NULL);
    ck_assert_int_eq(slot, 11);

    // mark slot 11 as used
    htable->slots[11].used = true;

    slot = _g_hash_table_find_free_slot(htable, 10, NULL);
    ck_assert_int_eq(slot, 12);

    // if we reach the end of the hash table the search should restart from the
    // beginning

    slot = _g_hash_table_find_free_slot(htable, htable->num_slots - 1, NULL);
    ck_assert_int_eq(slot, htable->num_slots - 1);

    // mark last slot as used
    htable->slots[htable->num_slots - 1].used = true;

    slot = _g_hash_table_find_free_slot(htable, htable->num_slots - 1, NULL);
    ck_assert_int_eq(slot, 0);

    htable->slots[0].used = true;

    slot = _g_hash_table_find_free_slot(htable, htable->num_slots - 1, NULL);
    ck_assert_int_eq(slot, 1);

    g_hash_table_destroy(htable);
}
END_TEST

START_TEST(test_ghashtable_insert_lookup)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(g_int_hash, g_int_equal);

    ck_assert_int_eq(htable->num_used, 0);

    g_hash_table_insert(htable, (void*) 42, (void*) "Answer to everything");
    ck_assert_int_eq(htable->num_used, 1);
    char *result = g_hash_table_lookup(htable, (void*) 42);
    ck_assert_ptr_nonnull(result);
    ck_assert_str_eq(result, "Answer to everything");

    result = NULL;
    g_hash_table_insert(htable, (void*) 56, (void*) "Fifty Six");
    g_hash_table_insert(htable, (void*) 67, (void*) "Sixty Seven");
    g_hash_table_insert(htable, (void*) 23, (void*) "Twenty Three");

    ck_assert_int_eq(htable->num_used, 4);

    result = g_hash_table_lookup(htable, (void*) 42);
    ck_assert_str_eq(result, "Answer to everything");

    result = g_hash_table_lookup(htable, (void*) 23);
    ck_assert_str_eq(result, "Twenty Three");

    result = g_hash_table_lookup(htable, (void*) 67);
    ck_assert_str_eq(result, "Sixty Seven");

    result = g_hash_table_lookup(htable, (void*) 56);
    ck_assert_str_eq(result, "Fifty Six");

    g_hash_table_destroy(htable);
}
END_TEST

uint32_t foreach_num_values = 3;

struct KeyValuePair {
    char *key;
    char *value;
    bool seen;
};

#define FOREACH_NUM_VALUES 3

void test_foreach(void *key, void *value, void *user_data)
{
    static bool first = true;
    struct KeyValuePair expected_values[FOREACH_NUM_VALUES];

    if (first) {
        expected_values[0].key = (void*) 56;
        expected_values[0].value = (void*) "Fifty Six";
        expected_values[0].seen = false;

        expected_values[1].key = (void*) 67;
        expected_values[1].value = (void*) "Sixty Seven";
        expected_values[1].seen = false;

        expected_values[2].key = (void*) 23;
        expected_values[2].value = (void*) "Twenty Three";
        expected_values[2].seen = false;

        first = false;
    }

    ck_assert_str_eq("USER DATA", user_data);
    for (int i = 0; i < FOREACH_NUM_VALUES; i++) {
        if (key != expected_values[i].key) {
            continue;
        }

        ck_assert_str_eq(value, expected_values[i].value);
        ck_assert_int_eq(expected_values[i].seen, false);
        expected_values[i].seen = true;
        foreach_num_values--;
    }
}

START_TEST(test_ghashtable_foreach)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(g_int_hash, g_int_equal);

    g_hash_table_insert(htable, (void*) 56, (void*) "Fifty Six");
    g_hash_table_insert(htable, (void*) 67, (void*) "Sixty Seven");
    g_hash_table_insert(htable, (void*) 23, (void*) "Twenty Three");

    char *user_data = "USER DATA";
    ck_assert_int_eq(foreach_num_values, 3);
    g_hash_table_foreach(htable, test_foreach, user_data);
    ck_assert_int_eq(foreach_num_values, 0);

    g_hash_table_destroy(htable);
}
END_TEST

START_TEST(test_ghashtable_remove)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(g_int_hash, g_int_equal);

    ck_assert_int_eq(htable->num_used, 0);

    g_hash_table_insert(htable, (void*) 56, (void*) "Fifty Six");
    g_hash_table_insert(htable, (void*) 67, (void*) "Sixty Seven");
    g_hash_table_insert(htable, (void*) 23, (void*) "Twenty Three");

    ck_assert_int_eq(htable->num_used, 3);

    g_hash_table_remove(htable, (void*) 56);
    ck_assert_int_eq(htable->num_used, 2);

    char *result = g_hash_table_lookup(htable, (void*) 56);
    ck_assert_ptr_null(result);

    result = g_hash_table_lookup(htable, (void*) 67);
    ck_assert_str_eq(result, "Sixty Seven");

    result = g_hash_table_lookup(htable, (void*) 23);
    ck_assert_str_eq(result, "Twenty Three");

    g_hash_table_destroy(htable);
}
END_TEST

START_TEST(test_ghashtable_lookup_after_remove)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(fake_int_hash_0, g_int_equal);

    ck_assert_int_eq(htable->num_used, 0);
    g_hash_table_insert(htable, (void*) 5, (void*) "Five");
    g_hash_table_insert(htable, (void*) 6, (void*) "Six");
    g_hash_table_insert(htable, (void*) 7, (void*) "Seven");
    ck_assert_int_eq(htable->num_used, 3);

    g_hash_table_remove(htable, (void*) 6);
    ck_assert_int_eq(htable->num_used, 2);

    char *result = g_hash_table_lookup(htable, (void*) 7);
    ck_assert_str_eq(result, "Seven");

    g_hash_table_destroy(htable);
}
END_TEST

START_TEST(test_ghashtable_insert_after_remove)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(fake_int_hash_0, g_int_equal);

    ck_assert_int_eq(htable->num_used, 0);
    g_hash_table_insert(htable, (void*) 5, (void*) "Five");
    g_hash_table_insert(htable, (void*) 6, (void*) "Six");
    g_hash_table_insert(htable, (void*) 7, (void*) "Seven");
    ck_assert_int_eq(htable->num_used, 3);

    g_hash_table_remove(htable, (void*) 6);
    ck_assert_int_eq(htable->num_used, 2);

    g_hash_table_insert(htable, (void*) 8, (void*) "Eight");
    ck_assert_int_eq(htable->num_used, 3);

    // old entry for 6 should be reused
    ck_assert_int_eq((uint32_t) (uint64_t) htable->slots[1].key, 8);
    ck_assert_int_eq(htable->slots[1].used, true);

    // entry after 7 should be unused
    ck_assert_int_eq((uint32_t) (uint64_t) htable->slots[3].key, 0);
    ck_assert_int_eq((uint32_t) (uint64_t) htable->slots[3].value, 0);
    ck_assert_int_eq(htable->slots[3].used, false);

    g_hash_table_destroy(htable);
}
END_TEST

START_TEST(test_ghashtable_double_insert)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(fake_int_hash_0, g_int_equal);

    ck_assert_int_eq(htable->num_used, 0);

    // first insert
    g_hash_table_insert(htable, (void*) 42, (void*) "Answer to everything");
    ck_assert_int_eq(htable->num_used, 1);
    ck_assert_int_eq((uint32_t) (uint64_t) htable->slots[0].key, 42);
    ck_assert_int_eq(htable->slots[0].used, true);
    ck_assert_int_eq((uint32_t) (uint64_t) htable->slots[1].key, 0);
    ck_assert_int_eq(htable->slots[1].used, false);

    // second insert
    g_hash_table_insert(htable, (void*) 42, (void*) "Answer to everything");
    ck_assert_int_eq(htable->num_used, 1);
    ck_assert_int_eq((uint32_t) (uint64_t) htable->slots[0].key, 42);
    ck_assert_int_eq(htable->slots[0].used, true);
    ck_assert_int_eq((uint32_t) (uint64_t) htable->slots[1].key, 0);
    ck_assert_int_eq(htable->slots[1].used, false);

    g_hash_table_destroy(htable);
}
END_TEST

START_TEST(test_ghashtable_resize)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(g_int_hash, g_int_equal);

    uint32_t init_num_slots = htable->num_slots;
    uint32_t tipping_point = ceil(GHASHTABLE_MAX_LOAD * htable->num_slots);

    for (uint32_t i = 0; i <= tipping_point; i++) {
        g_hash_table_insert(htable, (void*) (uint64_t) i, (void*) "Dummy String");
    }

    ck_assert_int_eq(htable->num_slots, init_num_slots * 2);

    for (uint32_t i = 0; i <= tipping_point; i++) {
        void *result = g_hash_table_lookup(htable, (void*) (uint64_t) i);
        ck_assert_ptr_nonnull(result);
    }

    g_hash_table_destroy(htable);
}
END_TEST

START_TEST(test_ghashtable_free_keys_and_values)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new_full(g_int_hash, g_int_equal, free_key_dummy, free_value_dummy);

    char *value_one   = "One";
    char *value_two   = "Two";
    char *value_three = "Three";

    g_hash_table_insert(htable, (void*) 1, value_one);
    g_hash_table_insert(htable, (void*) 2, value_two);
    g_hash_table_insert(htable, (void*) 3, value_three);

    keys_freed = 0;
    values_freed = 0;

    g_hash_table_remove(htable, (void*) 2);

    ck_assert_int_eq(keys_freed, 1);
    ck_assert_int_eq(values_freed, 1);
    ck_assert_ptr_eq(last_freed_key, (void*) 2);
    ck_assert_ptr_eq(last_freed_value, (void*) value_two);

    g_hash_table_destroy(htable);

    ck_assert_int_eq(keys_freed, 3);
    ck_assert_int_eq(values_freed, 3);

    if (last_freed_key == (void*) 1) {
        ck_assert_ptr_eq(last_freed_value, value_one);
    } else if (last_freed_key == (void*) 3) {
        ck_assert_ptr_eq(last_freed_value, value_three);
    } else {
        ck_abort_msg("Unexpected value for last_freed_value");
    }
}
END_TEST

Suite* ghashtable_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("GHashTable");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_ghashtable_new);
    tcase_add_test(tc_core, test_ghashtable_new_full);
    tcase_add_test(tc_core, test_ghashtable_find_free_slot);
    tcase_add_test(tc_core, test_ghashtable_insert_lookup);
    tcase_add_test(tc_core, test_ghashtable_foreach);
    tcase_add_test(tc_core, test_ghashtable_remove);
    tcase_add_test(tc_core, test_ghashtable_lookup_after_remove);
    tcase_add_test(tc_core, test_ghashtable_insert_after_remove);
    tcase_add_test(tc_core, test_ghashtable_double_insert);
    tcase_add_test(tc_core, test_ghashtable_resize);
    tcase_add_test(tc_core, test_ghashtable_free_keys_and_values);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc, char **argv)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = ghashtable_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
