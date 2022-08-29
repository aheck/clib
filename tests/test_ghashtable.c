#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "ghashtable.h"

// Fake hash function for testing
//
// The static hash value makes the location of the data in the hash table
// predictable
uint32_t fake_int_hash_0(void *v)
{
    return 0;
}

void free_key_dummy(void *data)
{
}

void free_value_dummy(void *data)
{
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
    tcase_add_test(tc_core, test_ghashtable_remove);
    tcase_add_test(tc_core, test_ghashtable_lookup_after_remove);
    tcase_add_test(tc_core, test_ghashtable_insert_after_remove);
    tcase_add_test(tc_core, test_ghashtable_double_insert);

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
