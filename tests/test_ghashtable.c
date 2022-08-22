#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "ghashtable.h"

START_TEST(test_ghashtable_new)
{
    GHashTable *htable = NULL;
    htable = g_hash_table_new(NULL, NULL);

    ck_assert_ptr_nonnull(htable);

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
