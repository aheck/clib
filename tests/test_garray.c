#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#define _CLIB_IMPL 1
#include "garray.h"

int compare_int(const void *a, const void *b)
{
    return *((int*) a) - *((int*) b);
}

int compare_int_with_data(const void *a, const void *b, void *user_data)
{
    ck_assert_int_eq(*((int*) user_data), 42);
    return *((int*) a) - *((int*) b);
}

START_TEST(test_garray_new)
{
    GArray *array = NULL;
    array = g_array_new(false, false, sizeof(int));

    ck_assert_ptr_nonnull(array);
    ck_assert_ptr_null(array->data);
    ck_assert_int_eq(array->len, 0);
    ck_assert_int_eq(array->_allocated_elements, 0);
    ck_assert_int_eq(array->_zero_terminated, false);
    ck_assert_int_eq(array->_clear, false);
    ck_assert_int_eq(array->_element_size, 4);
    ck_assert_ptr_null(array->_clear_func);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_sized_new)
{
    GArray *array = NULL;
    array = g_array_sized_new(false, false, sizeof(int), 10);

    ck_assert_ptr_nonnull(array);
    ck_assert_ptr_nonnull(array->data);
    ck_assert_int_eq(array->len, 0);
    ck_assert_int_eq(array->_allocated_elements, 10);
    ck_assert_int_eq(array->_zero_terminated, false);
    ck_assert_int_eq(array->_clear, false);
    ck_assert_int_eq(array->_element_size, 4);
    ck_assert_ptr_null(array->_clear_func);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_steal)
{
    GArray *array = NULL;
    size_t len = 0;

    array = g_array_new(false, false, sizeof(int));

    int vals[] = {34, 82, 43, 12, 71};
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));

    ck_assert_int_eq(array->len, 5);
    ck_assert_int_eq(array->_allocated_elements, 5);

    void *array_data = g_array_steal(array, &len);

    ck_assert_int_eq(len, 5);
    ck_assert_ptr_nonnull(array_data);
    ck_assert_int_eq(array->len, 0);
    ck_assert_ptr_null(array->data);
    ck_assert_int_eq(array->_allocated_elements, 0);

    free(array_data);
    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_steal_zero_terminated)
{
    GArray *array = NULL;
    size_t len = 0;

    array = g_array_new(true, false, sizeof(int));

    int vals[] = {34, 82, 43, 12, 71};
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));

    ck_assert_int_eq(array->len, 5);
    ck_assert_int_eq(array->_allocated_elements, 6);

    void *array_data = g_array_steal(array, &len);

    ck_assert_int_eq(len, 5);
    ck_assert_ptr_nonnull(array_data);
    ck_assert_int_eq(array->len, 0);
    ck_assert_int_eq(array->_allocated_elements, 1);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    free(array_data);
    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_append_val)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int val = 5;

    array = g_array_new(false, false, sizeof(int));
    result = g_array_append_val(array, val);

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 1);

    ck_assert_int_eq(g_array_index(array, int, 0), 5);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_append_val_many)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    array = g_array_new(false, false, sizeof(int));
    for (int i = 0; i < (sizeof(vals) / sizeof(int)); i++) {
        result = g_array_append_val(array, vals[i]);
    }

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 10);

    ck_assert_int_eq(g_array_index(array, int, 0), 0);
    ck_assert_int_eq(g_array_index(array, int, 1), 1);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);
    ck_assert_int_eq(g_array_index(array, int, 3), 3);
    ck_assert_int_eq(g_array_index(array, int, 4), 4);
    ck_assert_int_eq(g_array_index(array, int, 5), 5);
    ck_assert_int_eq(g_array_index(array, int, 6), 6);
    ck_assert_int_eq(g_array_index(array, int, 7), 7);
    ck_assert_int_eq(g_array_index(array, int, 8), 8);
    ck_assert_int_eq(g_array_index(array, int, 9), 9);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_append_vals)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals[] = {4, 8, 2, 7, 1, 3, 6};

    array = g_array_new(false, false, sizeof(int));
    result = g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 7);

    ck_assert_int_eq(g_array_index(array, int, 0), 4);
    ck_assert_int_eq(g_array_index(array, int, 1), 8);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);
    ck_assert_int_eq(g_array_index(array, int, 3), 7);
    ck_assert_int_eq(g_array_index(array, int, 4), 1);
    ck_assert_int_eq(g_array_index(array, int, 5), 3);
    ck_assert_int_eq(g_array_index(array, int, 6), 6);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_prepend_val)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int val1 = 5;
    int val2 = 8;

    array = g_array_new(false, false, sizeof(int));
    result = g_array_append_val(array, val1);
    result = g_array_prepend_val(array, val2);

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 2);

    ck_assert_int_eq(g_array_index(array, int, 0), 8);
    ck_assert_int_eq(g_array_index(array, int, 1), 5);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_prepend_val_many)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    array = g_array_new(false, false, sizeof(int));
    for (int i = 0; i < (sizeof(vals) / sizeof(int)); i++) {
        result = g_array_prepend_val(array, vals[i]);
    }

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 10);

    ck_assert_int_eq(g_array_index(array, int, 0), 9);
    ck_assert_int_eq(g_array_index(array, int, 1), 8);
    ck_assert_int_eq(g_array_index(array, int, 2), 7);
    ck_assert_int_eq(g_array_index(array, int, 3), 6);
    ck_assert_int_eq(g_array_index(array, int, 4), 5);
    ck_assert_int_eq(g_array_index(array, int, 5), 4);
    ck_assert_int_eq(g_array_index(array, int, 6), 3);
    ck_assert_int_eq(g_array_index(array, int, 7), 2);
    ck_assert_int_eq(g_array_index(array, int, 8), 1);
    ck_assert_int_eq(g_array_index(array, int, 9), 0);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_prepend_vals)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals1[] = {4, 8, 2, 7, 1, 3, 6};
    int vals2[] = {12, 67, 89};

    array = g_array_new(false, false, sizeof(int));
    result = g_array_append_vals(array, vals1, sizeof(vals1) / sizeof(int));
    result = g_array_prepend_vals(array, vals2, sizeof(vals2) / sizeof(int));

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 10);

    ck_assert_int_eq(g_array_index(array, int, 0), 12);
    ck_assert_int_eq(g_array_index(array, int, 1), 67);
    ck_assert_int_eq(g_array_index(array, int, 2), 89);

    ck_assert_int_eq(g_array_index(array, int, 3), 4);
    ck_assert_int_eq(g_array_index(array, int, 4), 8);
    ck_assert_int_eq(g_array_index(array, int, 5), 2);
    ck_assert_int_eq(g_array_index(array, int, 6), 7);
    ck_assert_int_eq(g_array_index(array, int, 7), 1);
    ck_assert_int_eq(g_array_index(array, int, 8), 3);
    ck_assert_int_eq(g_array_index(array, int, 9), 6);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_insert_val)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int val1 = 5;
    int val2 = 8;
    int val3 = 13;

    array = g_array_new(false, false, sizeof(int));
    result = g_array_append_val(array, val1);
    result = g_array_append_val(array, val3);
    result = g_array_insert_val(array, 1, val2);

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 3);

    ck_assert_int_eq(g_array_index(array, int, 0), 5);
    ck_assert_int_eq(g_array_index(array, int, 1), 8);
    ck_assert_int_eq(g_array_index(array, int, 2), 13);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_insert_val_beginning)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int val1 = 5;
    int val2 = 8;
    int val3 = 13;

    array = g_array_new(false, false, sizeof(int));
    result = g_array_append_val(array, val1);
    result = g_array_append_val(array, val2);
    result = g_array_insert_val(array, 0, val3);

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 3);

    ck_assert_int_eq(g_array_index(array, int, 0), 13);
    ck_assert_int_eq(g_array_index(array, int, 1), 5);
    ck_assert_int_eq(g_array_index(array, int, 2), 8);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_insert_val_end)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int val1 = 5;
    int val2 = 8;
    int val3 = 13;

    array = g_array_new(false, false, sizeof(int));
    result = g_array_append_val(array, val1);
    result = g_array_append_val(array, val2);
    result = g_array_insert_val(array, 2, val3);

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 3);

    ck_assert_int_eq(g_array_index(array, int, 0), 5);
    ck_assert_int_eq(g_array_index(array, int, 1), 8);
    ck_assert_int_eq(g_array_index(array, int, 2), 13);

    g_array_free(array, true);
}
END_TEST


START_TEST(test_garray_insert_vals)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals1[] = {4, 8, 2, 7, 1, 3, 6};
    int vals2[] = {12, 67, 89};

    array = g_array_new(false, false, sizeof(int));
    result = g_array_append_vals(array, vals1, sizeof(vals1) / sizeof(int));
    result = g_array_insert_vals(array, 3, vals2, sizeof(vals2) / sizeof(int));

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 10);

    ck_assert_int_eq(g_array_index(array, int, 0), 4);
    ck_assert_int_eq(g_array_index(array, int, 1), 8);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);

    ck_assert_int_eq(g_array_index(array, int, 3), 12);
    ck_assert_int_eq(g_array_index(array, int, 4), 67);
    ck_assert_int_eq(g_array_index(array, int, 5), 89);

    ck_assert_int_eq(g_array_index(array, int, 6), 7);
    ck_assert_int_eq(g_array_index(array, int, 7), 1);
    ck_assert_int_eq(g_array_index(array, int, 8), 3);
    ck_assert_int_eq(g_array_index(array, int, 9), 6);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_insert_vals_non_existing_index)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals1[] = {4, 8, 2, 7, 1, 3, 6};
    int vals2[] = {12, 67, 89};

    array = g_array_new(false, false, sizeof(int));
    result = g_array_append_vals(array, vals1, sizeof(vals1) / sizeof(int));
    result = g_array_insert_vals(array, 10, vals2, sizeof(vals2) / sizeof(int));

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 13);

    ck_assert_int_eq(g_array_index(array, int, 0), 4);
    ck_assert_int_eq(g_array_index(array, int, 1), 8);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);
    ck_assert_int_eq(g_array_index(array, int, 3), 7);
    ck_assert_int_eq(g_array_index(array, int, 4), 1);
    ck_assert_int_eq(g_array_index(array, int, 5), 3);
    ck_assert_int_eq(g_array_index(array, int, 6), 6);

    ck_assert_int_eq(g_array_index(array, int, 10), 12);
    ck_assert_int_eq(g_array_index(array, int, 11), 67);
    ck_assert_int_eq(g_array_index(array, int, 12), 89);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_insert_vals_non_existing_index_clear)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals1[] = {4, 8, 2, 7, 1, 3, 6};
    int vals2[] = {12, 67, 89};

    array = g_array_new(false, true, sizeof(int));
    result = g_array_append_vals(array, vals1, sizeof(vals1) / sizeof(int));
    result = g_array_insert_vals(array, 10, vals2, sizeof(vals2) / sizeof(int));

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 13);

    ck_assert_int_eq(g_array_index(array, int, 0), 4);
    ck_assert_int_eq(g_array_index(array, int, 1), 8);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);
    ck_assert_int_eq(g_array_index(array, int, 3), 7);
    ck_assert_int_eq(g_array_index(array, int, 4), 1);
    ck_assert_int_eq(g_array_index(array, int, 5), 3);
    ck_assert_int_eq(g_array_index(array, int, 6), 6);

    ck_assert_int_eq(g_array_index(array, int, 7), 0);
    ck_assert_int_eq(g_array_index(array, int, 8), 0);
    ck_assert_int_eq(g_array_index(array, int, 9), 0);

    ck_assert_int_eq(g_array_index(array, int, 10), 12);
    ck_assert_int_eq(g_array_index(array, int, 11), 67);
    ck_assert_int_eq(g_array_index(array, int, 12), 89);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_remove_index)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals[] = {4, 8, 2, 7, 1, 3, 6};

    array = g_array_new(false, false, sizeof(int));
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));
    result = g_array_remove_index(array, 4);

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 6);

    ck_assert_int_eq(g_array_index(array, int, 0), 4);
    ck_assert_int_eq(g_array_index(array, int, 1), 8);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);
    ck_assert_int_eq(g_array_index(array, int, 3), 7);
    ck_assert_int_eq(g_array_index(array, int, 4), 3);
    ck_assert_int_eq(g_array_index(array, int, 5), 6);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_remove_index_beginning)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals[] = {4, 8, 2, 7, 1, 3, 6};

    array = g_array_new(false, false, sizeof(int));
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));
    result = g_array_remove_index(array, 0);

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 6);

    ck_assert_int_eq(g_array_index(array, int, 0), 8);
    ck_assert_int_eq(g_array_index(array, int, 1), 2);
    ck_assert_int_eq(g_array_index(array, int, 2), 7);
    ck_assert_int_eq(g_array_index(array, int, 3), 1);
    ck_assert_int_eq(g_array_index(array, int, 4), 3);
    ck_assert_int_eq(g_array_index(array, int, 5), 6);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_remove_index_end)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals[] = {4, 8, 2, 7, 1, 3, 6};

    array = g_array_new(false, false, sizeof(int));
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));
    result = g_array_remove_index(array, 6);

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 6);

    ck_assert_int_eq(g_array_index(array, int, 0), 4);
    ck_assert_int_eq(g_array_index(array, int, 1), 8);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);
    ck_assert_int_eq(g_array_index(array, int, 3), 7);
    ck_assert_int_eq(g_array_index(array, int, 4), 1);
    ck_assert_int_eq(g_array_index(array, int, 5), 3);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_remove_index_fast)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals[] = {4, 8, 2, 7, 1, 3, 6};

    array = g_array_new(false, false, sizeof(int));
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));
    result = g_array_remove_index_fast(array, 2);

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 6);

    ck_assert_int_eq(g_array_index(array, int, 0), 4);
    ck_assert_int_eq(g_array_index(array, int, 1), 8);
    ck_assert_int_eq(g_array_index(array, int, 2), 6);
    ck_assert_int_eq(g_array_index(array, int, 3), 7);
    ck_assert_int_eq(g_array_index(array, int, 4), 1);
    ck_assert_int_eq(g_array_index(array, int, 5), 3);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_remove_range)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals[] = {4, 8, 2, 7, 1, 3, 6};

    array = g_array_new(false, false, sizeof(int));
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));
    result = g_array_remove_range(array, 1, 3);

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 4);

    ck_assert_int_eq(g_array_index(array, int, 0), 4);
    ck_assert_int_eq(g_array_index(array, int, 1), 1);
    ck_assert_int_eq(g_array_index(array, int, 2), 3);
    ck_assert_int_eq(g_array_index(array, int, 3), 6);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_sort)
{
    GArray *array = NULL;
    int vals[] = {4, 8, 5, 2, 7, 9, 1, 3, 0, 6};

    array = g_array_new(false, false, sizeof(int));

    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));
    g_array_sort(array, compare_int);

    ck_assert_int_eq(array->len, 10);

    ck_assert_int_eq(g_array_index(array, int, 0), 0);
    ck_assert_int_eq(g_array_index(array, int, 1), 1);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);
    ck_assert_int_eq(g_array_index(array, int, 3), 3);
    ck_assert_int_eq(g_array_index(array, int, 4), 4);
    ck_assert_int_eq(g_array_index(array, int, 5), 5);
    ck_assert_int_eq(g_array_index(array, int, 6), 6);
    ck_assert_int_eq(g_array_index(array, int, 7), 7);
    ck_assert_int_eq(g_array_index(array, int, 8), 8);
    ck_assert_int_eq(g_array_index(array, int, 9), 9);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_sort_with_data)
{
    GArray *array = NULL;
    int vals[] = {4, 8, 5, 2, 7, 9, 1, 3, 0, 6};
    int user_data = 42;

    array = g_array_new(false, false, sizeof(int));

    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));
    g_array_sort_with_data(array, compare_int_with_data, &user_data);

    ck_assert_int_eq(array->len, 10);

    ck_assert_int_eq(g_array_index(array, int, 0), 0);
    ck_assert_int_eq(g_array_index(array, int, 1), 1);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);
    ck_assert_int_eq(g_array_index(array, int, 3), 3);
    ck_assert_int_eq(g_array_index(array, int, 4), 4);
    ck_assert_int_eq(g_array_index(array, int, 5), 5);
    ck_assert_int_eq(g_array_index(array, int, 6), 6);
    ck_assert_int_eq(g_array_index(array, int, 7), 7);
    ck_assert_int_eq(g_array_index(array, int, 8), 8);
    ck_assert_int_eq(g_array_index(array, int, 9), 9);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_binary_search_null)
{
    GArray *array = NULL;
    bool result;
    int target = 5;
    unsigned int match_index = 0;

    array = g_array_new(false, false, sizeof(int));

    result = g_array_binary_search(NULL, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    result = g_array_binary_search(NULL, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_binary_search_empty)
{
    GArray *array = NULL;
    bool result;

    array = g_array_new(false, false, sizeof(int));

    unsigned int match_index = 0;
    int target = 5;

    result = g_array_binary_search(array, &target, compare_int, &match_index);

    ck_assert_int_eq(result, false);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_binary_search_one)
{
    GArray *array = NULL;
    int target;
    bool result;

    array = g_array_new(false, false, sizeof(int));

    int val = 5;
    g_array_append_val(array, val);

    unsigned int match_index = 42;

    // smaller
    target = 4;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    // bigger
    target = 6;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    // match
    target = 5;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, true);
    ck_assert_int_eq(match_index, 0);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_binary_search_two)
{
    GArray *array = NULL;
    int target;
    bool result;
    int vals[] = {4, 8};

    array = g_array_new(false, false, sizeof(int));
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));

    unsigned int match_index = 42;

    // smaller
    target = 3;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    // in between
    target = 5;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    target = 6;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    target = 7;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    // bigger
    target = 9;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    // match 1
    target = 4;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, true);
    ck_assert_int_eq(match_index, 0);

    // match 2
    target = 8;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, true);
    ck_assert_int_eq(match_index, 1);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_binary_search_three)
{
    GArray *array = NULL;
    int target;
    bool result;
    int vals[] = {4, 6, 8};

    array = g_array_new(false, false, sizeof(int));
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));

    unsigned int match_index = 42;

    // smaller
    target = 3;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    // in between
    target = 5;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    target = 7;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    // bigger
    target = 9;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, false);

    // match 1
    target = 4;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, true);
    ck_assert_int_eq(match_index, 0);

    // match 2
    target = 6;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, true);
    ck_assert_int_eq(match_index, 1);

    // match 3
    target = 8;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, true);
    ck_assert_int_eq(match_index, 2);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_binary_search_same)
{
    GArray *array = NULL;
    int target;
    bool result;
    int vals[] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

    array = g_array_new(false, false, sizeof(int));
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));

    unsigned int match_index = 42;

    // searching for 10 should give us the left-most element (index 0)
    target = 10;
    result = g_array_binary_search(array, &target, compare_int, &match_index);
    ck_assert_int_eq(result, true);
    ck_assert_int_eq(match_index, 0);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_binary_search_extensive)
{
    GArray *array = NULL;
    bool result;

    array = g_array_new(false, false, sizeof(int));

    const int max = 100;
    unsigned int match_index = 42;

    for (int i = 0; i < max; i++) {
        g_array_append_val(array, i);

        int j;
        for (j = 0; j <= i; j++) {
            result = g_array_binary_search(array, &j, compare_int, &match_index);
            ck_assert_int_eq(result, true);
            ck_assert_int_eq(match_index, j);
        }

        // the next element that hasn't been appended yet should not be found
        j++;
        result = g_array_binary_search(array, &j, compare_int, &match_index);
        ck_assert_int_eq(result, false);
    }

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_set_size)
{
    GArray *array = NULL;
    GArray *result = NULL;

    int vals[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    array = g_array_new(false, false, sizeof(int));

    result = g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));

    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 10);

    result = g_array_set_size(array, 20);
    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 20);
    ck_assert_int_eq(array->_allocated_elements, 20);

    ck_assert_int_eq(g_array_index(array, int, 0), 0);
    ck_assert_int_eq(g_array_index(array, int, 1), 1);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);
    ck_assert_int_eq(g_array_index(array, int, 3), 3);
    ck_assert_int_eq(g_array_index(array, int, 4), 4);
    ck_assert_int_eq(g_array_index(array, int, 5), 5);
    ck_assert_int_eq(g_array_index(array, int, 6), 6);
    ck_assert_int_eq(g_array_index(array, int, 7), 7);
    ck_assert_int_eq(g_array_index(array, int, 8), 8);
    ck_assert_int_eq(g_array_index(array, int, 9), 9);

    result = g_array_set_size(array, 5);
    ck_assert_ptr_eq(array, result);
    ck_assert_int_eq(array->len, 5);
    ck_assert_int_eq(array->_allocated_elements, 20);

    ck_assert_int_eq(g_array_index(array, int, 0), 0);
    ck_assert_int_eq(g_array_index(array, int, 1), 1);
    ck_assert_int_eq(g_array_index(array, int, 2), 2);
    ck_assert_int_eq(g_array_index(array, int, 3), 3);
    ck_assert_int_eq(g_array_index(array, int, 4), 4);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_zero_termination)
{
    GArray *array = NULL;
    int val;

    //
    // directly after creation
    //
    array = g_array_new(true, false, sizeof(int));

    ck_assert_int_eq(array->len, 0);
    ck_assert_int_eq(array->_allocated_elements, 1);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    //
    // after appending one value
    //
    val = 5;
    g_array_append_val(array, val);

    ck_assert_int_eq(array->len, 1);
    ck_assert_int_eq(array->_allocated_elements, 2);
    ck_assert_int_eq(g_array_index(array, int, 0), 5);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    //
    // after prepending one value
    //
    val = 4;
    g_array_prepend_val(array, val);

    ck_assert_int_eq(array->len, 2);
    ck_assert_int_eq(array->_allocated_elements, 3);
    ck_assert_int_eq(g_array_index(array, int, 0), 4);
    ck_assert_int_eq(g_array_index(array, int, 1), 5);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    //
    // after appending multiple values
    //
    int vals1[] = {89, 23, 11, 9};
    g_array_append_vals(array, vals1, sizeof(vals1) / sizeof(int));

    ck_assert_int_eq(array->len, 6);
    ck_assert_int_eq(array->_allocated_elements, 7);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    //
    // after prepending multiple values
    //
    int vals2[] = {45, 28, 77, 31};
    g_array_prepend_vals(array, vals2, sizeof(vals2) / sizeof(int));

    ck_assert_int_eq(array->len, 10);
    ck_assert_int_eq(array->_allocated_elements, 11);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    //
    // after removing an element
    //
    g_array_remove_index(array, 5);

    ck_assert_int_eq(array->len, 9);
    ck_assert_int_eq(array->_allocated_elements, 11);

    ck_assert_int_eq(g_array_index(array, int, 0), 45);
    ck_assert_int_eq(g_array_index(array, int, 1), 28);
    ck_assert_int_eq(g_array_index(array, int, 2), 77);
    ck_assert_int_eq(g_array_index(array, int, 3), 31);
    ck_assert_int_eq(g_array_index(array, int, 4), 4);
    ck_assert_int_eq(g_array_index(array, int, 5), 89);
    ck_assert_int_eq(g_array_index(array, int, 6), 23);
    ck_assert_int_eq(g_array_index(array, int, 7), 11);
    ck_assert_int_eq(g_array_index(array, int, 8), 9);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    //
    // after removing an element with g_array_remove_index_fast
    //
    g_array_remove_index_fast(array, 5);

    ck_assert_int_eq(array->len, 8);
    ck_assert_int_eq(array->_allocated_elements, 11);

    ck_assert_int_eq(g_array_index(array, int, 0), 45);
    ck_assert_int_eq(g_array_index(array, int, 1), 28);
    ck_assert_int_eq(g_array_index(array, int, 2), 77);
    ck_assert_int_eq(g_array_index(array, int, 3), 31);
    ck_assert_int_eq(g_array_index(array, int, 4), 4);
    ck_assert_int_eq(g_array_index(array, int, 5), 9);
    ck_assert_int_eq(g_array_index(array, int, 6), 23);
    ck_assert_int_eq(g_array_index(array, int, 7), 11);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    //
    // after removing a range of elements
    //
    g_array_remove_range(array, 3, 3);

    ck_assert_int_eq(array->len, 5);
    ck_assert_int_eq(array->_allocated_elements, 11);

    ck_assert_int_eq(g_array_index(array, int, 0), 45);
    ck_assert_int_eq(g_array_index(array, int, 1), 28);
    ck_assert_int_eq(g_array_index(array, int, 2), 77);
    ck_assert_int_eq(g_array_index(array, int, 3), 23);
    ck_assert_int_eq(g_array_index(array, int, 4), 11);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    //
    // after appending by using insert
    //
    int insert_val = 76;
    g_array_insert_val(array, 5, insert_val);

    ck_assert_int_eq(array->len, 6);
    ck_assert_int_eq(array->_allocated_elements, 11);

    ck_assert_int_eq(g_array_index(array, int, 0), 45);
    ck_assert_int_eq(g_array_index(array, int, 1), 28);
    ck_assert_int_eq(g_array_index(array, int, 2), 77);
    ck_assert_int_eq(g_array_index(array, int, 3), 23);
    ck_assert_int_eq(g_array_index(array, int, 4), 11);
    ck_assert_int_eq(g_array_index(array, int, 5), 76);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    //
    // after changing the size of the array
    //
    g_array_set_size(array, 20);

    ck_assert_int_eq(array->len, 20);
    ck_assert_int_eq(array->_allocated_elements, 21);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    g_array_set_size(array, 0);

    ck_assert_int_eq(array->len, 0);
    ck_assert_int_eq(array->_allocated_elements, 21);

    // zero terminated?
    ck_assert_int_eq(g_array_index(array, int, array->len), 0);

    g_array_free(array, true);
}
END_TEST

START_TEST(test_garray_free)
{
    GArray *array = NULL;

    array = g_array_new(false, false, sizeof(int));

    int vals[] = {34, 82, 43, 12, 71};
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));

    char *result = g_array_free(array, true);
    ck_assert_ptr_null(result);
}
END_TEST

START_TEST(test_garray_free_without_segment)
{
    GArray *array = NULL;

    array = g_array_new(false, false, sizeof(int));

    int vals[] = {34, 82, 43, 12, 71};
    g_array_append_vals(array, vals, sizeof(vals) / sizeof(int));

    char *result = g_array_free(array, false);
    ck_assert_ptr_nonnull(result);

    free(result);
}
END_TEST

Suite* garray_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("GArray");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_garray_new);
    tcase_add_test(tc_core, test_garray_sized_new);

    tcase_add_test(tc_core, test_garray_steal);
    tcase_add_test(tc_core, test_garray_steal_zero_terminated);

    tcase_add_test(tc_core, test_garray_append_val);
    tcase_add_test(tc_core, test_garray_append_val_many);
    tcase_add_test(tc_core, test_garray_append_vals);

    tcase_add_test(tc_core, test_garray_prepend_val);
    tcase_add_test(tc_core, test_garray_prepend_val_many);
    tcase_add_test(tc_core, test_garray_prepend_vals);

    tcase_add_test(tc_core, test_garray_insert_val);
    tcase_add_test(tc_core, test_garray_insert_val_beginning);
    tcase_add_test(tc_core, test_garray_insert_val_end);
    tcase_add_test(tc_core, test_garray_insert_vals);
    tcase_add_test(tc_core, test_garray_insert_vals_non_existing_index);
    tcase_add_test(tc_core, test_garray_insert_vals_non_existing_index_clear);

    tcase_add_test(tc_core, test_garray_remove_index);
    tcase_add_test(tc_core, test_garray_remove_index_beginning);
    tcase_add_test(tc_core, test_garray_remove_index_end);

    tcase_add_test(tc_core, test_garray_remove_index_fast);

    tcase_add_test(tc_core, test_garray_remove_range);

    tcase_add_test(tc_core, test_garray_sort);
    tcase_add_test(tc_core, test_garray_sort_with_data);

    tcase_add_test(tc_core, test_garray_binary_search_null);
    tcase_add_test(tc_core, test_garray_binary_search_empty);
    tcase_add_test(tc_core, test_garray_binary_search_one);
    tcase_add_test(tc_core, test_garray_binary_search_two);
    tcase_add_test(tc_core, test_garray_binary_search_three);
    tcase_add_test(tc_core, test_garray_binary_search_same);
    tcase_add_test(tc_core, test_garray_binary_search_extensive);

    tcase_add_test(tc_core, test_garray_set_size);

    tcase_add_test(tc_core, test_garray_zero_termination);

    tcase_add_test(tc_core, test_garray_free);
    tcase_add_test(tc_core, test_garray_free_without_segment);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc, char **argv)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = garray_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
