/*
 * GArray
 *
 * Copyright (c) 2023 Andreas Heck <aheck@gmx.de>
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

#ifndef _GARRAY_H
#define _GARRAY_H

// We would need to define _GNU_SOURCE for stdlib.h to declare qsort_r. Since we
// are header-only we better declare it ourselves to make sure we don't define
// _GNU_SOURCE when we shouldn't
#ifdef __linux__
void qsort_r(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *, void *), void *arg);
#endif

// Needed for qsort_s
#if (defined _WIN32 || defined _WIN64 || defined __WINDOWS__)
#include <search.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef int32_t(*GCompareFunc) (const void *a, const void *b);
typedef int32_t(*GCompareDataFunc) (const void *a, const void *b, void *user_data);
typedef void (*GDestroyNotify)(void *data);

typedef struct GArray {
    char *data;
    unsigned int len;
    unsigned int _allocated_elements;
    bool _zero_terminated;
    bool _clear;
    unsigned int _element_size;
    GDestroyNotify _clear_func;
} GArray;

GArray* g_array_new(bool zero_terminated, bool clear, unsigned int element_size);
void* g_array_steal(GArray *array, size_t *len);
GArray* g_array_sized_new(bool zero_terminated, bool clear, unsigned int element_size, unsigned int reserved_size);
GArray* g_array_copy(GArray *array);
unsigned int g_array_get_element_size(GArray *array);
#define g_array_append_val(a, v) g_array_append_vals(a, &v, 1);
GArray* g_array_append_vals (GArray *array, const void *data, unsigned int len);
#define g_array_prepend_val(a, v) g_array_prepend_vals(a, &v, 1);
GArray* g_array_prepend_vals(GArray *array, const void *data, unsigned int len);
#define g_array_insert_val(a, i, v) g_array_insert_vals(a, i, &v, 1);
GArray* g_array_insert_vals (GArray *array, unsigned int index, const void *data, unsigned int len);
GArray* g_array_remove_index(GArray *array, unsigned int index);
GArray* g_array_remove_index_fast(GArray *array, unsigned int index);
GArray* g_array_remove_range(GArray *array, unsigned int index, unsigned int length);
void g_array_sort(GArray *array, GCompareFunc compare_func);
void g_array_sort_with_data(GArray *array, GCompareDataFunc compare_func, void *user_data);
bool g_array_binary_search(GArray *array, const void *target, GCompareFunc compare_func, unsigned int *out_match_index);
#define g_array_index(a, t, i) (t) a->data[i * a->_element_size]
GArray* g_array_set_size(GArray *array, unsigned int length);
void g_array_set_clear_func(GArray *array, GDestroyNotify clear_func);

char* g_array_free(GArray *array, bool free_segment);

#ifdef _CLIB_IMPL
struct _garray_qsort_r_data
{
    void *user_data;
    int (*compare_func)(const void *a, const void *b, void *user_data);
};

int _garray_qsort_r_arg_swap(void *wrapper_arg, const void *a, const void *b)
{
    struct _garray_qsort_r_data *wrapper = (struct _garray_qsort_r_data*) wrapper_arg;
    return (wrapper->compare_func)(a, b, wrapper->user_data);
}

void _g_array_zero_terminate(GArray *array)
{
    memset(&array->data[array->len * array->_element_size], 0, array->_element_size);
}

void _g_array_resize_if_needed(GArray *array, unsigned int new_elements)
{
    unsigned int new_len = array->len + new_elements;
    unsigned int needed = new_len;

    if (array->_zero_terminated) {
        needed++;
    }

    if (needed <= array->_allocated_elements) {
        return;
    }

    array->data = realloc(array->data, needed * array->_element_size);
    if (array->data == NULL) {
        fprintf(stderr, "FATAL ERROR: _g_array_resize_if_needed: Out of memory");
        exit(1);
    }

    if (array->_clear) {
        memset(&array->data[array->_allocated_elements * array->_element_size], 0,
                (needed - array->_allocated_elements) * array->_element_size);
    }

    array->_allocated_elements = needed;
}

GArray* g_array_new(bool zero_terminated, bool clear, unsigned int element_size)
{
    return g_array_sized_new(zero_terminated, clear, element_size, 0);
}

void* g_array_steal(GArray *array, size_t *len)
{
    void *data;

    *len = array->len;

    data = array->data;
    array->data = NULL;
    array->len = 0;
    array->_allocated_elements = 0;

    if (array->_zero_terminated) {
        if (array->_clear) {
            array->data = calloc(array->_element_size, 1);
        } else {
            array->data = malloc(array->_element_size);
        }

        if (array->data == NULL) {
            fprintf(stderr, "FATAL ERROR: g_array_steal: Out of memory");
            exit(1);
        }

        array->_allocated_elements = 1;

        _g_array_zero_terminate(array);
    }

    return data;
}

GArray* g_array_sized_new(bool zero_terminated, bool clear, unsigned int element_size, unsigned int reserved_size)
{
    GArray *array;

    array = malloc(sizeof(GArray));
    if (array == NULL) {
        fprintf(stderr, "FATAL ERROR: g_array_sized_new: Out of memory");
        exit(1);
    }

    array->data = NULL;
    array->len = 0;
    array->_allocated_elements = reserved_size;
    array->_zero_terminated = zero_terminated;
    array->_clear = clear;
    array->_element_size = element_size;
    array->_clear_func = NULL;

    if (zero_terminated) {
        array->_allocated_elements++;
    }

    if (array->_allocated_elements == 0) {
        return array;
    }

    if (clear) {
        array->data = calloc(array->_allocated_elements * array->_element_size, 1);
    } else {
        array->data = malloc(array->_allocated_elements * array->_element_size);
    }

    if (array->data == NULL) {
        fprintf(stderr, "FATAL ERROR: g_array_sized_new: Out of memory");
        exit(1);
    }

    if (!clear && zero_terminated) {
        _g_array_zero_terminate(array);
    }

    return array;
}

GArray* g_array_copy(GArray *array)
{
    GArray *copy;

    if (array == NULL) {
        return NULL;
    }

    copy = malloc(sizeof(GArray));
    if (copy == NULL) {
        fprintf(stderr, "FATAL ERROR: g_array_copy: Out of memory");
        exit(1);
    }

    memcpy(copy, array, sizeof(GArray));

    copy->data = malloc(copy->_allocated_elements * copy->_element_size);
    if (copy->data == NULL) {
        fprintf(stderr, "FATAL ERROR: g_array_copy: Out of memory");
        exit(1);
    }

    memcpy(copy->data, array->data, copy->_allocated_elements);

    return copy;
}

unsigned int g_array_get_element_size(GArray *array)
{
    return array->_element_size;
}

GArray* g_array_append_vals(GArray *array, const void *data, unsigned int len)
{
    _g_array_resize_if_needed(array, len);

    memcpy(&array->data[array->len * array->_element_size], data, len * array->_element_size);
    array->len += len;
    if (array->_zero_terminated) {
        _g_array_zero_terminate(array);
    }

    return array;
}

GArray* g_array_prepend_vals(GArray *array, const void *data, unsigned int len)
{
    _g_array_resize_if_needed(array, len);

    memmove(&array->data[len * array->_element_size], &array->data[0], array->len * array->_element_size);
    memcpy(array->data, data, len * array->_element_size);
    array->len += len;
    if (array->_zero_terminated) {
        _g_array_zero_terminate(array);
    }

    return array;
}

GArray* g_array_insert_vals(GArray *array, unsigned int index, const void *data, unsigned int len)
{
    unsigned int needed = len;

    // we need to allocate extra bytes if the index to insert at is outside of
    // the array
    if (index > (array->len - 1)) {
        needed += index - array->len;
    }

    _g_array_resize_if_needed(array, needed);

    if (index < array->len) {
        memmove(&array->data[(index + len) * array->_element_size], &array->data[index * array->_element_size], (array->len - index) * array->_element_size);
    }
    memcpy(&array->data[index * array->_element_size], data, len * array->_element_size);
    array->len += needed;

    if (array->_zero_terminated) {
        _g_array_zero_terminate(array);
    }

    return array;
}

GArray* g_array_remove_index(GArray *array, unsigned int index)
{
    if (array->_clear_func) {
        array->_clear_func(&array->data[index]);
    }

    // do we need to move other elements back?
    if (array->len > 1 && index < (array->len - 1)) {
        memmove(&array->data[index * array->_element_size], &array->data[(index + 1) * array->_element_size], (array->len - index - 1) * array->_element_size);
    }

    array->len--;

    if (array->_zero_terminated) {
        _g_array_zero_terminate(array);
    }

    return array;
}

GArray* g_array_remove_index_fast(GArray *array, unsigned int index)
{
    if (array->_clear_func) {
        array->_clear_func(&array->data[index]);
    }

    if (array->len > 1 && index < (array->len - 1)) {
        memcpy(&array->data[index * array->_element_size], &array->data[(array->len - 1) * array->_element_size], array->_element_size);
    }

    array->len--;

    if (array->_zero_terminated) {
        _g_array_zero_terminate(array);
    }

    return array;
}

GArray* g_array_remove_range(GArray *array, unsigned int index, unsigned int length)
{
    if (array->_clear_func) {
        for (int i = index; i < (index + length - 1); i++) {
            array->_clear_func(&array->data[i]);
        }
    }

    // do we need to move other elements back?
    if (array->len > 1 && index < (array->len - length)) {
        memmove(&array->data[index * array->_element_size], &array->data[(index + length) * array->_element_size], (array->len - index - length) * array->_element_size);
    }

    array->len -= length;

    if (array->_zero_terminated) {
        _g_array_zero_terminate(array);
    }

    return array;
}

void g_array_sort(GArray *array, GCompareFunc compare_func)
{
    qsort(array->data, array->len, array->_element_size, compare_func);
}

void g_array_sort_with_data(GArray *array, GCompareDataFunc compare_func, void *user_data)
{
#if (defined __linux__)
    qsort_r(array->data, array->len, array->_element_size, compare_func, user_data);
#elif (defined _WIN32 || defined _WIN64 || defined __WINDOWS__)
    struct _garray_qsort_r_data tmp;
    tmp.user_data = user_data;
    tmp.compare_func = compare_func;
    qsort_s(array->data, array->len, array->_element_size, &_garray_qsort_r_arg_swap, &tmp);
#else
    // BSD / macOS
    struct _garray_qsort_r_data tmp;
    tmp.user_data = user_data;
    tmp.compare_func = compare_func;
    qsort_r(array->data, array->len, array->_element_size, &tmp, &_garray_qsort_r_arg_swap);
#endif
}

bool g_array_binary_search(GArray *array, const void *target, GCompareFunc compare_func, unsigned int *out_match_index)
{
    if (array == NULL) {
        return false;
    }

    if (target == NULL) {
        return false;
    }

    if (array->len <= 0) {
        return false;
    }

    int l = 0;
    int r = array->len - 1;
    unsigned int m = 0;

    while (l <= r) {
        m = (r - l) / 2 + l; // (l + r) / 2 but prevent overflow

        if (compare_func(&array->data[m * array->_element_size], target) < 0) {
            l = m + 1;
        } else if (compare_func(&array->data[m * array->_element_size], target) > 0) {
            r = m - 1;
        } else {
            // ensure we always return the left-most element
            while (m > 0) {
                if (compare_func(&array->data[m * array->_element_size], &array->data[(m - 1) * array->_element_size]) != 0) {
                    break;
                }
                m--;
            }

            if (out_match_index) {
                *out_match_index = m;
            }

            return true;
        }
    }

    return false;
}

GArray* g_array_set_size(GArray *array, unsigned int length)
{
    if (length <= array->len) {
        if (array->_clear_func && array->len > length) {
            // call clear func on all elements that are going to be removed
            int start_index = (array->len - length) - 1;
            for (int i = start_index; i < array->len; i++) {
                array->_clear_func(&array->data[i]);
            }
        }

        array->len = length;

        if (array->_zero_terminated) {
            _g_array_zero_terminate(array);
        }

        return array;
    }

    _g_array_resize_if_needed(array, length - array->len);
    array->len = length;
    if (array->_zero_terminated) {
        _g_array_zero_terminate(array);
    }

    return array;
}

void g_array_set_clear_func(GArray *array, GDestroyNotify clear_func)
{
    array->_clear_func = clear_func;
}

char* g_array_free(GArray *array, bool free_segment)
{
    char *data;

    if (array == NULL) {
        return NULL;
    }

    if (free_segment == false) {
        data = array->data;
        free(array);
        return data;
    }

    if (array->_clear_func != NULL) {
        for (int i = 0; i < array->len; i++) {
            array->_clear_func(&array->data[i]);
        }
    }

    free(array->data);
    free(array);

    return NULL;
}

#endif
#endif
