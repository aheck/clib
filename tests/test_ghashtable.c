#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "ghashtable.h"

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

START_TEST(test_ghashtable_insert_lookup)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(g_int_hash, g_int_equal);

    ck_assert_int_eq(htable->num_used, 0);

    g_hash_table_insert(htable, 42, (void*) "Answer to everything");
    ck_assert_int_eq(htable->num_used, 1);
    char *result = g_hash_table_lookup(htable, 42);
    ck_assert_ptr_nonnull(result);
    ck_assert_str_eq(result, "Answer to everything");

    result = NULL;
    g_hash_table_insert(htable, 56, (void*) "Fifty Six");
    g_hash_table_insert(htable, 67, (void*) "Sixty Seven");
    g_hash_table_insert(htable, 23, (void*) "Twenty Three");

    ck_assert_int_eq(htable->num_used, 4);

    result = g_hash_table_lookup(htable, 42);
    ck_assert_str_eq(result, "Answer to everything");

    result = g_hash_table_lookup(htable, 23);
    ck_assert_str_eq(result, "Twenty Three");

    result = g_hash_table_lookup(htable, 67);
    ck_assert_str_eq(result, "Sixty Seven");

    result = g_hash_table_lookup(htable, 56);
    ck_assert_str_eq(result, "Fifty Six");

    g_hash_table_destroy(htable);
}
END_TEST

START_TEST(test_ghashtable_remove)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(g_int_hash, g_int_equal);

    ck_assert_int_eq(htable->num_used, 0);

    g_hash_table_insert(htable, 56, (void*) "Fifty Six");
    g_hash_table_insert(htable, 67, (void*) "Sixty Seven");
    g_hash_table_insert(htable, 23, (void*) "Twenty Three");

    ck_assert_int_eq(htable->num_used, 3);

    g_hash_table_remove(htable, 56);
    ck_assert_int_eq(htable->num_used, 2);

    char *result = g_hash_table_lookup(htable, 56);
    ck_assert_ptr_null(result);

    result = g_hash_table_lookup(htable, 67);
    ck_assert_str_eq(result, "Sixty Seven");

    result = g_hash_table_lookup(htable, 23);
    ck_assert_str_eq(result, "Twenty Three");

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
    tcase_add_test(tc_core, test_ghashtable_insert_lookup);
    tcase_add_test(tc_core, test_ghashtable_remove);

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
