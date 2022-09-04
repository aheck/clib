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

    GList *last = list->next->next;

    ck_assert_str_eq(last->data, "Element 3");
    ck_assert_str_eq(last->prev->data, "Element 2");
    ck_assert_str_eq(last->prev->prev->data, "Element 1");

    g_list_free(list);
}
END_TEST

START_TEST(test_glist_prepend)
{
    GList *list = NULL;
    list = g_list_prepend(list, "Element 1");

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 1);

    list = g_list_prepend(list, "Element 2");
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 2);

    list = g_list_prepend(list, "Element 3");
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    ck_assert_str_eq(list->data, "Element 3");
    ck_assert_str_eq(list->next->data, "Element 2");
    ck_assert_str_eq(list->next->next->data, "Element 1");

    GList *last = list->next->next;

    ck_assert_str_eq(last->data, "Element 1");
    ck_assert_str_eq(last->prev->data, "Element 2");
    ck_assert_str_eq(last->prev->prev->data, "Element 3");

    g_list_free(list);
}
END_TEST

START_TEST(test_glist_insert)
{
    GList *list = NULL;
    list = g_list_append(list, "Element 1");
    list = g_list_append(list, "Element 2");
    list = g_list_append(list, "Element 3");

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    g_list_insert(list, "New Element", 1);

    ck_assert_str_eq(list->data, "Element 1");
    ck_assert_str_eq(list->next->data, "New Element");
    ck_assert_str_eq(list->next->next->data, "Element 2");
    ck_assert_str_eq(list->next->next->next->data, "Element 3");

    list = g_list_reverse(list);

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 4);

    ck_assert_str_eq(list->data, "Element 3");
    ck_assert_str_eq(list->next->data, "Element 2");
    ck_assert_str_eq(list->next->next->data, "New Element");
    ck_assert_str_eq(list->next->next->next->data, "Element 1");

    g_list_free(list);
}
END_TEST

START_TEST(test_glist_insert_before)
{
    GList *list = NULL;
    list = g_list_append(list, "Element 1");
    list = g_list_append(list, "Element 2");
    list = g_list_append(list, "Element 3");

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    g_list_insert_before(list, list->next, "New Element");

    ck_assert_str_eq(list->data, "Element 1");
    ck_assert_str_eq(list->next->data, "New Element");
    ck_assert_str_eq(list->next->next->data, "Element 2");
    ck_assert_str_eq(list->next->next->next->data, "Element 3");

    list = g_list_reverse(list);

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 4);

    ck_assert_str_eq(list->data, "Element 3");
    ck_assert_str_eq(list->next->data, "Element 2");
    ck_assert_str_eq(list->next->next->data, "New Element");
    ck_assert_str_eq(list->next->next->next->data, "Element 1");

    g_list_free(list);
}
END_TEST

START_TEST(test_glist_remove)
{
    GList *list = NULL;
    list = g_list_append(list, "Element 1");
    list = g_list_append(list, "Element 2");
    list = g_list_append(list, "Element 3");

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    list = g_list_remove(list, "Element 2");
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 2);

    ck_assert_str_eq(list->data, "Element 1");
    ck_assert_str_eq(list->next->data, "Element 3");

    list = g_list_remove(list, "Element 3");
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 1);

    ck_assert_str_eq(list->data, "Element 1");

    list = g_list_remove(list, "Element 1");
    ck_assert_ptr_null(list);

    g_list_free(list);
}
END_TEST

START_TEST(test_glist_remove_link)
{
    GList *list = NULL;
    list = g_list_append(list, "Element 1");
    list = g_list_append(list, "Element 2");
    list = g_list_append(list, "Element 3");

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    list = g_list_remove_link(list, list->next);
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 2);

    ck_assert_str_eq(list->data, "Element 1");
    ck_assert_str_eq(list->next->data, "Element 3");

    list = g_list_remove_link(list, list->next);
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 1);

    ck_assert_str_eq(list->data, "Element 1");

    list = g_list_remove_link(list, list);
    ck_assert_ptr_null(list);

    g_list_free(list);
}
END_TEST

START_TEST(test_glist_delete_link)
{
    GList *list = NULL;
    list = g_list_append(list, "Element 1");
    list = g_list_append(list, "Element 2");
    list = g_list_append(list, "Element 3");

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    list = g_list_delete_link(list, list->next);
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 2);

    ck_assert_str_eq(list->data, "Element 1");
    ck_assert_str_eq(list->next->data, "Element 3");

    list = g_list_delete_link(list, list->next);
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 1);

    ck_assert_str_eq(list->data, "Element 1");

    list = g_list_delete_link(list, list);
    ck_assert_ptr_null(list);

    g_list_free(list);
}
END_TEST

START_TEST(test_glist_remove_all)
{
    GList *list = NULL;
    list = g_list_append(list, "Element 1");
    list = g_list_append(list, "Delete Me");
    list = g_list_append(list, "Element 2");
    list = g_list_append(list, "Element 3");
    list = g_list_append(list, "Delete Me");

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 5);

    list = g_list_remove_all(list, "Delete Me");
    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    ck_assert_str_eq(list->data, "Element 1");
    ck_assert_str_eq(list->next->data, "Element 2");
    ck_assert_str_eq(list->next->next->data, "Element 3");

    g_list_free(list);
}
END_TEST

START_TEST(test_glist_copy)
{
    GList *list = NULL;
    list = g_list_append(list, "Element 1");
    list = g_list_append(list, "Element 2");
    list = g_list_append(list, "Element 3");

    ck_assert_ptr_nonnull(list);
    ck_assert_int_eq(g_list_length(list), 3);

    GList *new_list = g_list_copy(list);
    ck_assert_ptr_nonnull(new_list);
    ck_assert_int_eq(g_list_length(new_list), 3);

    ck_assert_str_eq(new_list->data, "Element 1");
    ck_assert_str_eq(new_list->next->data, "Element 2");
    ck_assert_str_eq(new_list->next->next->data, "Element 3");

    ck_assert_ptr_ne(new_list, list);
    ck_assert_ptr_ne(new_list->next, list->next);
    ck_assert_ptr_ne(new_list->next->next, list->next->next);

    g_list_free(list);
    g_list_free(new_list);
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

START_TEST(test_glist_concat)
{
    GList *list1 = NULL;
    list1 = g_list_append(list1, "Element 1");
    list1 = g_list_append(list1, "Element 2");
    list1 = g_list_append(list1, "Element 3");

    ck_assert_ptr_nonnull(list1);
    ck_assert_int_eq(g_list_length(list1), 3);

    GList *list2 = NULL;
    list2 = g_list_append(list2, "List 2 Element 1");
    list2 = g_list_append(list2, "List 2 Element 2");

    ck_assert_ptr_nonnull(list2);
    ck_assert_int_eq(g_list_length(list2), 2);

    list1 = g_list_concat(list1, list2);

    ck_assert_str_eq(list1->data, "Element 1");
    ck_assert_str_eq(list1->next->data, "Element 2");
    ck_assert_str_eq(list1->next->next->data, "Element 3");
    ck_assert_str_eq(list1->next->next->next->data, "List 2 Element 1");
    ck_assert_str_eq(list1->next->next->next->next->data, "List 2 Element 2");

    g_list_free(list1);
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
    tcase_add_test(tc_core, test_glist_prepend);
    tcase_add_test(tc_core, test_glist_insert);
    tcase_add_test(tc_core, test_glist_insert_before);
    tcase_add_test(tc_core, test_glist_remove);
    tcase_add_test(tc_core, test_glist_remove_link);
    tcase_add_test(tc_core, test_glist_delete_link);
    tcase_add_test(tc_core, test_glist_remove_all);
    tcase_add_test(tc_core, test_glist_copy);
    tcase_add_test(tc_core, test_glist_concat);
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
