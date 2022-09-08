/*
 * GList
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

#include <math.h>

typedef int32_t(*GCompareFunc) (const void *a, const void *b);
typedef int32_t(*GCompareDataFunc) (const void *a, const void *b, void *user_data);
typedef void(*GFunc) (void *data, void *user_data);

typedef struct GList {
    void *data;
    struct GList *next;
    struct GList *prev;
} GList;

GList* g_list_insert_before(GList *list, GList *sibling, void *data);
GList* g_list_first(GList *list);
GList* g_list_last(GList *list);
GList* g_list_alloc();
GList* g_list_nth(GList *list, uint32_t n);

GList* g_list_append(GList *list, void *data)
{
    GList *last = g_list_last(list);

    GList *new_entry = g_list_alloc();
    if (new_entry == NULL) {
        return NULL;
    }

    new_entry->data = data;
    new_entry->prev = last;
    new_entry->next = NULL;

    if (last == NULL) {
        return new_entry;
    }

    last->next = new_entry;

    return list;
}

GList* g_list_prepend(GList *list, void *data)
{
    GList *first = g_list_first(list);

    GList *new_entry = g_list_alloc();
    if (new_entry == NULL) {
        return NULL;
    }

    new_entry->data = data;
    new_entry->prev = NULL;
    new_entry->next = first;

    if (first != NULL) {
        first->prev = new_entry;
    }

    return new_entry;
}

GList* g_list_insert(GList *list, void *data, int32_t position)
{
    if (position < 0) {
        return g_list_append(list, data);
    }

    GList *sibling = g_list_nth(list, position);
    if (sibling) {
        return g_list_insert_before(list, sibling, data);
    }

    return list;
}

GList* g_list_insert_before(GList *list, GList *sibling, void *data)
{
    if (sibling == NULL) {
        return g_list_append(list, data);
    }

    GList *new_elem = g_list_alloc();
    if (new_elem == NULL) {
        return NULL;
    }

    new_elem->data = data;

    if (sibling->prev) {
        sibling->prev->next = new_elem;
    }

    new_elem->prev = sibling->prev;
    new_elem->next = sibling;

    sibling->prev = new_elem;

    return list;
}

GList* g_list_insert_sorted(GList *list, void *data, GCompareFunc func)
{
    if (list == NULL) {
        return g_list_append(list, data);
    }

    GList *cur = list;
    while (cur) {
        int32_t comp = func(cur->data, data);

        if (comp >= 0) {
            GList * new_elem = g_list_alloc();
            if (new_elem == NULL) {
                return list;
            }

            new_elem->data = data;
            new_elem->next = cur;
            new_elem->prev = cur->prev;
            if (cur->prev) {
                cur->prev->next = new_elem;
            } else {
                list = new_elem;
            }
            cur->prev = new_elem;

            return list;
        }

        if (cur->next == NULL) {
            GList * new_elem = g_list_alloc();
            if (new_elem == NULL) {
                return list;
            }

            new_elem->data = data;
            new_elem->next = NULL;
            new_elem->prev = cur;
            cur->next = new_elem;

            return list;
        }

        cur = cur->next;
    }

    return list;
}

GList* g_list_remove(GList *list, const void *data)
{
    GList *cur = list;

    while (cur != NULL) {
        if (cur->data == data) {
            if (cur->prev) {
                cur->prev->next = cur->next;
            }

            if (cur->next) {
                cur->next->prev = cur->prev;
            }

            if (cur == list) {
                if (cur->prev) {
                    list = cur->prev;
                } else {
                    list = cur->next;
                }
            }

            free(cur);
            return list;
        }

        cur = cur->next;
    }

    return list;
}

GList* g_list_remove_link(GList *list, GList *llink)
{
    if (llink == NULL) {
        return list;
    }

    if (llink->prev) {
        llink->prev->next = llink->next;
    }

    if (llink->next) {
        llink->next->prev = llink->prev;
    }

    if (list == llink) {
        if (llink->prev) {
            list = llink->prev;
        } else {
            list = llink->next;
        }
    }

    llink->prev = llink->next = NULL;

    return list;
}

GList* g_list_delete_link(GList *list, GList *link_)
{
    list = g_list_remove_link(list, link_);
    free(link_);
    return list;
}

GList* g_list_remove_all(GList *list, const void *data)
{
    GList *next;
    GList *cur = list;

    while (cur != NULL) {
        if (cur->data == data) {
            if (cur->prev) {
                cur->prev->next = cur->next;
            }

            if (cur->next) {
                cur->next->prev = cur->prev;
            }

            if (cur == list) {
                if (cur->prev) {
                    list = cur->prev;
                } else {
                    list = cur->next;
                }
            }

            next = cur->next;
            free(cur);
            cur = next;
            continue;
        }

        cur = cur->next;
    }

    return list;
}

void g_list_free(GList *list)
{
    list = g_list_first(list);

    if (list == NULL) {
        return;
    }

    GList *next;

    while (list != NULL) {
        next = list->next;
        free(list);
        list = next;
    }
}

GList* g_list_alloc()
{
    return (GList*) malloc(sizeof(GList));
}

#define g_list_free1 g_list_free_1

void g_list_free_1(GList *list)
{
    free(list);
}

uint32_t g_list_length(GList *list)
{
    uint32_t len;

    if (list == NULL) {
        return 0;
    }

    for (len = 1; list->next != NULL; list = list->next) {
        len++;
    }

    return len;
}

GList* g_list_copy(GList *list)
{
    if (list == NULL) {
        return NULL;
    }

    GList *new_list = NULL;
    GList *new_prev = NULL;

    while (list) {
        GList *new_elem = g_list_alloc();
        if (new_elem == NULL) {
            if (new_list) {
                g_list_free(new_list);
            }

            return NULL;
        }

        if (new_list == NULL) {
            new_list = new_elem;
        }

        new_elem->data = list->data;
        new_elem->prev = new_prev;
        new_elem->next = NULL;
        if (new_prev) {
            new_prev->next = new_elem;
        }

        new_prev = new_elem;
        list = list->next;
    }

    return new_list;
}

GList* g_list_reverse(GList *list)
{
    if (list == NULL) {
        return NULL;
    }

    GList *cur = list;
    GList *old_next;

    while (cur) {
        old_next = cur->next;

        cur->next = cur->prev;
        cur->prev = old_next;

        list = cur;
        cur = old_next;
    }

    return list;
}

GList* _g_list_merge_sorted(GList *list1, GList *list2, GCompareFunc compare_func)
{
    GList *new_list;

    if (list1 == list2) {
        return list1;
    }

    if (list1 == NULL) {
        return list2;
    }

    if (list2 == NULL) {
        return list1;
    }

    GList *new_cur = NULL;

    // add first element to new list
    if (compare_func(list1->data, list2->data) < 0) {
        new_list = list1;
        list1 = list1->next;
    } else {
        new_list = list2;
        list2 = list2->next;
    }

    new_cur = new_list;
    int32_t comp;

    while (list1 || list2) {
        if (list1 == NULL) {
            comp = 1;
        } else if (list2 == NULL) {
            comp = -1;
        } else {
            comp = compare_func(list1->data, list2->data);
        }

        if (comp < 0) {
            new_cur->next = list1;
            list1->prev = new_cur;
            new_cur = list1;
            list1 = list1->next;
            new_cur->next = NULL;
        } else {
            new_cur->next = list2;
            list2->prev = new_cur;
            new_cur = list2;
            list2 = list2->next;
            new_cur->next = NULL;
        }
    }

    return new_list;
}

GList* g_list_sort(GList *list, GCompareFunc compare_func)
{
    // Bottom-Up Mergesort

    if (list == NULL) {
        return NULL;
    }

    if (compare_func == NULL) {
        return list;
    }

    uint32_t total_len = g_list_length(list);

    if (total_len == 1) {
        return list;
    }

    uint32_t max_lists = total_len;
    max_lists = ceil(max_lists / 2.0);

    if (max_lists % 2 != 0) {
        max_lists += 1;
    }

    uint32_t buf_size = sizeof(GList*) * max_lists;
    GList **sorted_lists = (GList**) malloc(buf_size);

    if (sorted_lists == NULL) {
        return list;
    }

    // Set last element to NULL to "normalize" lists with an uneven number of
    // elements
    sorted_lists[max_lists - 1] = NULL;

    // Break list into single element lists and merge them pair-wise.
    // The pointers to the merged lists are stored in sorted_lists.
    GList *cur = list;
    uint32_t i;
    for (i = 0; cur; i++) {
        GList *list1 = cur;
        GList *list2 = cur->next;

        if (list2) {
            cur = list2->next;
        } else {
            cur = NULL;
        }

        list1->prev = NULL;
        list1->next = NULL;
        if (list2) {
            list2->prev = NULL;
            list2->next = NULL;
        }

        sorted_lists[i] = _g_list_merge_sorted(list1, list2, compare_func);
    }

    uint32_t num_sublists = max_lists;
    while (num_sublists > 1) {
        uint32_t j = 0;
        for (i = 0; i < num_sublists; i += 2, j++) {
            GList *list1 = sorted_lists[i];
            GList *list2 = sorted_lists[i + 1];
            sorted_lists[j] = _g_list_merge_sorted(list1, list2, compare_func);
        }
        num_sublists = j;

        if (num_sublists > 1 && num_sublists % 2 != 0) {
            num_sublists += 1;
            sorted_lists[num_sublists - 1] = NULL;
        }
    }

    GList *result = sorted_lists[0];
    free(sorted_lists);

    return result;
}

GList* g_list_insert_sorted_with_data(GList *list, void *data, GCompareDataFunc func, void *user_data)
{
    return NULL;
}

GList* g_list_sort_with_data(GList *list, GCompareDataFunc compare_func, void *user_data)
{
    return NULL;
}

GList* g_list_concat(GList *list1, GList *list2)
{
    if (list1 == NULL || list2 == NULL) {
        return list1;
    }

    GList *last1 = g_list_last(list1);

    last1->next = list2;
    list2->prev = last1;

    return list1;
}

void g_list_foreach(GList *list, GFunc func, void *user_data)
{
    while (list) {
        func(list->data, user_data);
        list = list->next;
    }
}

GList* g_list_first(GList *list)
{
    if (list == NULL) {
        return NULL;
    }

    for(; list->prev != NULL; list = list->prev) {
    }

    return list;
}

GList* g_list_last(GList *list)
{
    if (list == NULL) {
        return NULL;
    }

    for(; list->next != NULL; list = list->next) {
    }

    return list;
}

#define g_list_previous(list) list->prev

#define g_list_next(list) list->next

GList* g_list_nth(GList *list, uint32_t n)
{
    uint32_t i;

    for (i = 0; list != NULL; list = list->next, i++) {
        if (i == n) {
            return list;
        }
    }

    return NULL;
}

void* g_list_nth_data(GList *list, uint32_t n)
{
    uint32_t i;

    for (i = 0; list != NULL; list = list->next, i++) {
        if (i == n) {
            return list->data;
        }
    }

    return NULL;
}

GList* g_list_nth_prev(GList *list, uint32_t n)
{
    uint32_t i;

    for (i = 0; list != NULL; list = list->prev, i++) {
        if (i == n) {
            return list;
        }
    }

    return NULL;
}

GList* g_list_find(GList *list, const void *data)
{
    while (list) {
        if (list->data == data) {
            return list;
        }

        list = list->next;
    }

    return NULL;
}

GList* g_list_find_custom(GList *list, const void *data, GCompareFunc func)
{
    while (list) {
        if (func(list->data, data) == 0) {
            return list;
        }

        list = list->next;
    }

    return NULL;
}

int32_t g_list_position(GList *list, GList *llink)
{
    uint32_t i;

    for (i = 0; list != NULL; list = list->next, i++) {
        if (list == llink) {
            return i;
        }
    }

    return -1;
}

int32_t g_list_index(GList *list, const void *data)
{
    uint32_t i;

    for (i = 0; list != NULL; list = list->next, i++) {
        if (list->data == data) {
            return i;
        }
    }

    return -1;
}
