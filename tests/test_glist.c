#include <stdio.h>
#include <stdlib.h>

#include <check.h>

#include "glist.h"


START_TEST(test_glist_append)
{
    GList *list = NULL;
    list = g_list_append(list, "Element 1");

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 1);

    list = g_list_append(list, "Element 2");
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 2);

    list = g_list_append(list, "Element 3");
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    ck_assert_str_eq(list->data, "Element 1");
    ck_assert_str_eq(list->next->data, "Element 2");
    ck_assert_str_eq(list->next->next->data, "Element 3");

    g_list_free(list);
}
END_TEST

START_TEST(test_glist_reverse)
{
    GList *list = NULL;
    list = g_list_append(list, "Element 1");
    list = g_list_append(list, "Element 2");
    list = g_list_append(list, "Element 3");

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    ck_assert_str_eq(list->data, "Element 1");
    ck_assert_str_eq(list->next->data, "Element 2");
    ck_assert_str_eq(list->next->next->data, "Element 3");

    list = g_list_reverse(list);

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    ck_assert_str_eq(list->data, "Element 3");
    ck_assert_str_eq(list->next->data, "Element 2");
    ck_assert_str_eq(list->next->next->data, "Element 1");

    g_list_free(list);
}
END_TEST

Suite* glist_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("GList");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_glist_append);
    tcase_add_test(tc_core, test_glist_reverse);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc, char **argv)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = glist_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
