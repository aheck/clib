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

typedef int32_t(*GCompareFunc) (const void *a, const void *b);
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
    GList *first = g_list_last(list);

    GList *new_entry = g_list_alloc();
    if (new_entry == NULL) {
        return NULL;
    }

    new_entry->data = data;
    new_entry->prev = NULL;
    new_entry->next = first;

    first->prev = new_entry;

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

    uint32_t len = g_list_length(list);

    GList *elems = (GList*) malloc(sizeof(GList) * len);
    if (elems == NULL) {
        return NULL;
    }

    for (uint32_t i; i < len; list = list->next, i++) {
        elems[i].data = list->data;
        if (i > 0) {
            elems[i].prev = &elems[i - 1];
        } else {
            elems[i].prev = NULL;
        }

        if (i < (len - 1)) {
            elems[i].next = &elems[i + 1];
        } else {
            elems[i].next = NULL;
        }
    }

    return elems;
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
        if (func(list->data, data)) {
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
