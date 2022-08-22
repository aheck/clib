/*
 * GString
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

#ifndef _GSTRING_H
#define _GSTRING_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <ctype.h>

#define GSTRING_MIN_BUF_SIZE 32
#define GSTRING_DEBUG 1

#define TRUE  true
#define FALSE false

#define BUFSIZE (sizeof(long) * 8 + 1)

char *ltoa(long N, char *str, int base)
{
      register int i = 2;
      long uarg;
      char *tail, *head = str, buf[BUFSIZE];

      if (36 < base || 2 > base)
            base = 10;                    /* can only use 0-9, A-Z        */
      tail = &buf[BUFSIZE - 1];           /* last character position      */
      *tail-- = '\0';

      if (10 == base && N < 0L)
      {
            *head++ = '-';
            uarg    = -N;
      }
      else  uarg = N;

      if (uarg)
      {
            for (i = 1; uarg; ++i)
            {
                  register ldiv_t r;

                  r       = ldiv(uarg, base);
                  *tail-- = (char)(r.rem + ((9L < r.rem) ?
                                  ('A' - 10L) : '0'));
                  uarg    = r.quot;
            }
      }
      else  *tail-- = '0';

      memcpy(head, ++tail, i);
      return str;
}

typedef struct GString {
    char *str;
    size_t len;
    size_t allocated_len;
} GString;

GString* g_string_new(const char *init)
{
    size_t init_len = 0;
    size_t buf_size = GSTRING_MIN_BUF_SIZE;
    GString *string;

    if (init != NULL) {
        init_len = strlen(init);
    }

    if (init_len + 1 > buf_size) {
        buf_size = init_len + 1;
    }

    string = malloc(sizeof(GString));
    if (string == NULL) {
        return NULL;
    }

    string->str = malloc(buf_size);
    if (string->str == NULL) {
        free(string);
        return NULL;
    }

#ifdef GSTRING_DEBUG
        memset(string->str, '+', buf_size);
        string->str[buf_size - 1] = '\0';
#endif

    // fill GString attributes
    if (init_len > 0) {
        memcpy(string->str, init, init_len + 1);
    } else {
        string->str[0] = '\0';
    }

    string->len = init_len;
    string->allocated_len = buf_size;

    return string;
}

GString* g_string_new_len(const char *init, size_t len)
{
    GString *string;

    string = malloc(sizeof(GString));
    if (string == NULL) {
        return NULL;
    }

    size_t buf_size = len + 1;

    string->str = malloc(buf_size);
    if (string->str == NULL) {
        free(string);
        return NULL;
    }

    memcpy(string->str, init, len);

    string->str[buf_size - 1] = '\0';
    string->len = len;
    string->allocated_len = buf_size;

    return string;
}

GString* g_string_sized_new(ssize_t dfl_size)
{
    GString *string;

    string = malloc(sizeof(GString));
    if (string == NULL) {
        return NULL;
    }

    string->str = malloc(dfl_size);
    if (string->str == NULL) {
        free(string);
        return NULL;
    }

    string->len = 0;
    string->allocated_len = dfl_size;

    return string;
}

bool _g_string_resize(GString *string, size_t requested_size)
{
    char *new_buf;
    size_t buf_size;

    if (requested_size <= string->allocated_len) {
        return false;
    }

    buf_size = requested_size * 2;

    new_buf = realloc(string->str, buf_size);

    if (new_buf == NULL) {
        return false;
    }

    string->str = new_buf;
    string->allocated_len = buf_size;

#ifdef GSTRING_DEBUG
    // FIXME
    memset(&string->str[string->len + 1], '+', buf_size - string->len - 1);
    string->str[buf_size - 1] = '\0';
#endif

    return true;
}

GString* g_string_assign(GString *string, const char *rval)
{
    size_t new_len;

    if (rval == NULL) {
        return string;
    }

    new_len = strlen(rval);

    if (new_len + 1 > string->allocated_len) {
        if (!_g_string_resize(string, new_len + 1)) {
            return string;
        }
    }

    memcpy(string->str, rval, new_len + 1);
    string->len = new_len;

    return string;
}

GString* g_string_append(GString *string, const char *val)
{
    size_t val_len;
    size_t new_len;

    if (val == NULL) {
        return string;
    }

    val_len = strlen(val);
    new_len = val_len + string->len;

    if (new_len + 1 > string->allocated_len) {
        if (!_g_string_resize(string, new_len + 1)) {
            return string;
        }
    }

    memcpy(&string->str[string->len], val, val_len + 1);
    string->len = new_len;

    return string;
}

GString* g_string_append_c(GString *string, const char c)
{
    size_t new_len;

    new_len = string->len + 1;

    if (new_len + 1 > string->allocated_len) {
        if (!_g_string_resize(string, new_len + 1)) {
            return string;
        }
    }

    string->str[string->len] = c;
    string->len = new_len;
    string->str[string->len] = '\0';

    return string;
}

GString* g_string_prepend(GString *string, const char *val)
{
    size_t val_len;
    size_t new_len;

    if (val == NULL) {
        return string;
    }

    val_len = strlen(val);
    new_len = val_len + string->len;

    if (new_len + 1 > string->allocated_len) {
        if (!_g_string_resize(string, new_len + 1)) {
            return string;
        }
    }

    // move current string to end
    memmove(&string->str[val_len], string->str, string->len + 1);

    // copy string that shall be prepended
    memcpy(string->str, val, val_len);

    string->len = new_len;

    return string;
}

GString* g_string_prepend_c(GString *string, const char c)
{
    size_t new_len;

    new_len = string->len + 1;

    if (new_len + 1 > string->allocated_len) {
        if (!_g_string_resize(string, new_len + 1)) {
            return string;
        }
    }

    // move current string to end
    memmove(&string->str[1], string->str, string->len + 1);

    // copy char that shall be prepended
    string->str[0] = c;

    string->len = new_len;

    return string;
}

GString* g_string_insert(GString *string, ssize_t pos, const char *val)
{
    size_t val_len;

    if (pos < 0) {
        return string;
    }

    // pos beyond last char of string
    if (pos > string->len) {
        return string;
    }

    val_len = strlen(val);

    if (val_len == 0) {
        return string;
    }

    ssize_t new_len = string->len + val_len;

    // enlarge buffer?
    if (new_len + 1 > string->allocated_len) {
        _g_string_resize(string, new_len + 1);
    }

    memmove(&string->str[pos + val_len], &string->str[pos], string->len - pos + 1);
    memcpy(&string->str[pos], val, val_len);
    string->len += val_len;

    return string;
}

GString* g_string_insert_c(GString *string, ssize_t pos, char c)
{
    if (pos < 0) {
        return string;
    }

    // pos beyond last char of string
    if (pos > string->len) {
        return string;
    }

    ssize_t new_len = string->len + 1;

    // enlarge buffer?
    if (new_len + 1 > string->allocated_len) {
        _g_string_resize(string, new_len + 1);
    }

    memmove(&string->str[pos + 1], &string->str[pos], string->len - pos + 1);
    string->str[pos] = c;
    string->len++;

    return string;
}

GString* g_string_erase(GString *string, ssize_t pos, ssize_t len)
{
    if (pos < 0) {
        return string;
    }

    if (len == 0) {
        return string;
    }

    // pos beyond last char of string
    if (pos > string->len - 1) {
        return string;
    }

    // do we erase to the end of the string?
    if (len < 0 || (pos + len) >= string->len) {
        string->str[pos] = '\0';
        string->len = pos;
        return string;
    }

    memmove(&string->str[pos], &string->str[pos + len], string->len - pos - len + 1);
    string->len = string->len - len;

    return string;
}

GString* g_string_truncate(GString *string, size_t len)
{
    if (len >= string->len) {
        return string;
    }

    string->str[len] = '\0';
    string->len = len;

    return string;
}

void g_string_append_vprintf(GString *string, const char *format, va_list args);

void g_string_vprintf(GString *string, const char *format, va_list args)
{
    g_string_truncate(string, 0);

    g_string_append_vprintf(string, format, args);
}

void g_string_append_vprintf(GString *string, const char *format, va_list args)
{
    const char *c;
    char last_c = 0;
    int int_arg;
    double double_arg;
    char buf[20];
    char *cur;

    if (format == NULL) {
        return;
    }

    for (c = format; *c; c++) {
        if (last_c == '%') {
            if (*c == '%') {
                g_string_append_c(string, '%');
            } else if (*c == 's') {
                g_string_append(string, va_arg(args, char*));
            } else if (*c == 'd') {
                int_arg = va_arg(args, int);
                ltoa(int_arg, (char*) &buf, 10);
                g_string_append(string, buf);
            } else if (*c == 'f') {
                double_arg = va_arg(args, double);
                snprintf(buf, sizeof(buf), "%f", double_arg);
                g_string_append(string, buf);
            } else if (*c == 'x' || *c == 'X') {
                int_arg = va_arg(args, int);
                ltoa(int_arg, (char*) &buf, 16);
                if (*c == 'x') {
                    for (cur = (char*) &buf; *cur; cur++) {
                        *cur = tolower(*cur);
                    }
                }
                g_string_append(string, buf);
            }
        } else if (*c != '%') {
            g_string_append_c(string, *c);
        }

        last_c = *c;
    }
}

void g_string_printf(GString *string, const char *format, ...)
{
    va_list args;

    g_string_truncate(string, 0);

    va_start(args, format);
    g_string_vprintf(string, format, args);
    va_end(args);
}

void g_string_append_printf(GString *string, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    g_string_append_vprintf(string, format, args);
    va_end(args);
}

bool g_string_equal(GString *v, GString *v2)
{
    if (v->len != v2->len) {
        return false;
    }

    return memcmp(v->str, v2->str, v->len) == 0;
}

char* g_string_free(GString *string, bool free_segment)
{
    char *segment;

    if (free_segment) {
        free(string->str);
        free(string);
        return NULL;
    }

    segment = string->str;

    free(string);

    return segment;
}

#endif
