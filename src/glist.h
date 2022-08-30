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

GList* g_list_first(GList *list);
GList* g_list_last(GList *list);
GList* g_list_alloc();

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

    return new_entry;
}

GList* g_list_insert(GList *list, void *data, int32_t position)
{
    return NULL;
}

GList* g_list_insert_before(GList *list, GList *sibling, void *data)
{
    return NULL;
}

GList* g_list_remove(GList *list, const void *data)
{
    return NULL;
}

GList* g_list_remove_link(GList *list, GList *llink)
{
    return NULL;
}

GList* g_list_delete_link(GList *list, GList *link_)
{
    return NULL;
}

GList* g_list_remove_all(GList *list, const void *data)
{
    return NULL;
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

void g_list_free_1(GList *list)
{

}

uint32_t g_list_length(GList *list)
{
    uint32_t len;

    if (list == NULL) {
        return 0;
    }

    list = g_list_first(list);

    for (len = 1; list->next != NULL; list = list->next) {
        len++;
    }

    return len;
}

GList* g_list_copy(GList *list)
{
    return NULL;
}

GList* g_list_reverse(GList *list)
{
    return NULL;
}

GList* g_list_concat(GList *list1, GList *list2)
{
    return NULL;
}

void g_list_foreach(GList *list, GFunc func, void *user_data)
{

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
    return NULL;
}

void* g_list_nth_data(GList *list, uint32_t n)
{
    return NULL;
}

GList* g_list_nth_prev(GList *list, uint32_t n)
{
    return NULL;
}

GList* g_list_find(GList *list, const void *data)
{
    return NULL;
}

GList* g_list_find_custom(GList *list, const void *data, GCompareFunc func)
{
    return NULL;
}

int32_t g_list_position(GList *list, GList *llink)
{
    return 0;
}

int32_t g_list_index(GList *list, const void *data)
{
    return 0;
}
