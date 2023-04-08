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

#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define GSTRING_MIN_BUF_SIZE 32

#define TRUE  true
#define FALSE false

typedef struct GString {
    char *str;
    size_t len;
    size_t allocated_len;
} GString;

GString* g_string_new(const char *init);
GString* g_string_new_len(const char *init, size_t len);
GString* g_string_sized_new(ssize_t dfl_size);
GString* g_string_assign(GString *string, const char *rval);
GString* g_string_append(GString *string, const char *val);
GString* g_string_append_c(GString *string, char c);
GString* g_string_append_len(GString *string, const char *val, ssize_t len);
GString* g_string_prepend(GString *string, const char *val);
GString* g_string_prepend_c(GString *string, char c);
GString* g_string_prepend_len(GString *string, const char *val, ssize_t len);
GString* g_string_insert(GString *string, ssize_t pos, const char *val);
GString* g_string_insert_c(GString *string, ssize_t pos, char c);
GString* g_string_insert_len(GString *string, ssize_t pos, const char *val, ssize_t len);
GString* g_string_overwrite(GString *string, size_t pos, const char *val);
GString* g_string_overwrite_len(GString *string, size_t pos, const char *val, ssize_t len);
unsigned int g_string_replace(GString *string, const char *find, const char *replace, unsigned int limit);
GString* g_string_erase(GString *string, ssize_t pos, ssize_t len);
GString* g_string_truncate(GString *string, size_t len);
void g_string_vprintf(GString *string, const char *format, va_list args);
void g_string_append_vprintf(GString *string, const char *format, va_list args);
void g_string_printf(GString *string, const char *format, ...);
void g_string_append_printf(GString *string, const char *format, ...);
bool g_string_equal(GString *v, GString *v2);
char* g_string_free(GString *string, bool free_segment);


#ifdef _CLIB_IMPL

#define _GSTRING_LTOA_BUF_SIZE (sizeof(long) * 8 + 1)

/*
**  Converts a long integer to a string.
**
**  Copyright 1988-90 by Robert B. Stout dba MicroFirm
**
**  Released to public domain, 1991
**
**  Parameters: 1 - number to be converted
**              2 - buffer in which to build the converted string
**              3 - number base to use for conversion
**
**  Returns:  A character pointer to the converted string if
**            successful, a NULL pointer if the number base specified
**            is out of range.
*/
char* _gstring_ltoa(long N, char *str, int base)
{
      register int i = 2;
      long uarg;
      char *tail, *head = str, buf[_GSTRING_LTOA_BUF_SIZE];

      if (36 < base || 2 > base)
            base = 10;                    /* can only use 0-9, A-Z        */
      tail = &buf[_GSTRING_LTOA_BUF_SIZE - 1];           /* last character position      */
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
        fprintf(stderr, "FATAL ERROR: g_string_new: Out of memory");
        exit(1);
    }

    string->str = malloc(buf_size);
    if (string->str == NULL) {
        fprintf(stderr, "FATAL ERROR: g_string_new: Out of memory");
        exit(1);
    }

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
        fprintf(stderr, "FATAL ERROR: g_string_new_len: Out of memory");
        exit(1);
    }

    size_t buf_size = len + 1;

    string->str = malloc(buf_size);
    if (string->str == NULL) {
        fprintf(stderr, "FATAL ERROR: g_string_new_len: Out of memory");
        exit(1);
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
        fprintf(stderr, "FATAL ERROR: g_string_sized_new: Out of memory");
        exit(1);
    }

    string->str = malloc(dfl_size);
    if (string->str == NULL) {
        fprintf(stderr, "FATAL ERROR: g_string_sized_new: Out of memory");
        exit(1);
    }

    string->len = 0;
    string->allocated_len = dfl_size;

    return string;
}

void _g_string_resize(GString *string, size_t requested_size)
{
    char *new_buf;
    size_t buf_size;

    if (requested_size <= string->allocated_len) {
        return;
    }

    buf_size = requested_size * 2;

    new_buf = realloc(string->str, buf_size);
    if (new_buf == NULL) {
        fprintf(stderr, "FATAL ERROR: g_string_new: Out of memory");
        exit(1);
    }

    string->str = new_buf;
    string->allocated_len = buf_size;
}

GString* g_string_assign(GString *string, const char *rval)
{
    size_t new_len;

    if (rval == NULL) {
        return string;
    }

    new_len = strlen(rval);

    if (new_len + 1 > string->allocated_len) {
        _g_string_resize(string, new_len + 1);
    }

    memcpy(string->str, rval, new_len + 1);
    string->len = new_len;

    return string;
}

GString* g_string_append(GString *string, const char *val)
{
    if (val == NULL) {
        return NULL;
    }

    return g_string_append_len(string, val, strlen(val));
}

GString* g_string_append_c(GString *string, char c)
{
    return g_string_append_len(string, &c, 1);
}

GString* g_string_append_len(GString *string, const char *val, ssize_t len)
{
    if (len <= 0) {
        return string;
    }

    if (val == NULL) {
        return string;
    }

    size_t new_len = len + string->len;

    if (new_len + 1 > string->allocated_len) {
        _g_string_resize(string, new_len + 1);
    }

    memcpy(&string->str[string->len], val, len);
    string->len = new_len;
    string->str[new_len] = '\0';

    return string;
}

GString* g_string_prepend(GString *string, const char *val)
{
    if (val == NULL) {
        return string;
    }

    return g_string_prepend_len(string, val, strlen(val));
}

GString* g_string_prepend_c(GString *string, char c)
{
    return g_string_prepend_len(string, &c, 1);
}

GString* g_string_prepend_len(GString *string, const char *val, ssize_t len)
{
    if (val == NULL) {
        return string;
    }

    size_t new_len = len + string->len;

    if (new_len + 1 > string->allocated_len) {
        _g_string_resize(string, new_len + 1);
    }

    // move current string to end
    memmove(&string->str[len], string->str, string->len + 1);

    // copy string that shall be prepended
    memcpy(string->str, val, len);

    string->len = new_len;

    return string;
}

GString* g_string_insert(GString *string, ssize_t pos, const char *val)
{
    if (val == NULL) {
        return string;
    }

    return g_string_insert_len(string, pos, val, strlen(val));
}

GString* g_string_insert_c(GString *string, ssize_t pos, char c)
{
    return g_string_insert_len(string, pos, &c, 1);
}

GString* g_string_insert_len(GString *string, ssize_t pos, const char *val, ssize_t len)
{
    if (pos < 0) {
        return string;
    }

    // pos beyond last char of string
    if (pos > (ssize_t) string->len) {
        return string;
    }

    if (len == 0) {
        return string;
    }

    ssize_t new_len = string->len + len;

    // enlarge buffer?
    if (new_len + 1 > (ssize_t) string->allocated_len) {
        _g_string_resize(string, new_len + 1);
    }

    memmove(&string->str[pos + len], &string->str[pos], string->len - pos + 1);
    memcpy(&string->str[pos], val, len);
    string->len += len;

    return string;
}

GString* g_string_overwrite(GString *string, size_t pos, const char *val)
{
    if (val == NULL) {
        return string;
    }

    return g_string_overwrite_len(string, pos, val, strlen(val));
}

GString* g_string_overwrite_len(GString *string, size_t pos, const char *val, ssize_t len)
{
    if (pos < 0) {
        return string;
    }

    size_t overwrite_len = pos + len;

    // enlarge buffer?
    if (overwrite_len >= string->allocated_len) {
        _g_string_resize(string, overwrite_len + 1);
    }

    memcpy(&string->str[pos], val, len);

    if (overwrite_len > string->len) {
        string->len += len - pos;
        string->str[string->len] = '\0';
    }

    return string;
}

unsigned int g_string_replace(GString *string, const char *find, const char *replace, unsigned int limit)
{
    unsigned int replacements = 0;

    if (find == NULL || replace == NULL) {
        return 0;
    }

    if (replace[0] == '\0') {
        return 0;
    }

    size_t find_len = strlen(find);
    size_t replace_len = strlen(replace);

    // if the find string is empty we insert the replace string at the beginning
    // and end of the string as well as after each character
    if (find[0] == '\0') {
        for (ssize_t i = 0; i < string->len; i++) {
            g_string_insert(string, i, replace);
            i += replace_len;
            replacements++;

            if (limit != 0) {
                if (replacements == limit) {
                    break;
                }
            }
        }

        if (limit == 0 || limit < replacements) {
            g_string_append(string, "_");
            replacements++;
        }

        return replacements;
    }

    // calculate how many chars of the find and the replace string overlap
    size_t overlap = find_len;
    if (replace_len > find_len) {
        overlap = replace_len - find_len;
    } else if (find_len > replace_len) {
        overlap = find_len - replace_len;
    }

    char *pos = string->str;
    while ((pos = strstr(pos, find))) {
        // overwrite overlapping part of the string
        memcpy(pos, replace, overlap);

        if (replace_len > find_len) {
            // insert the non-overlapping part of the replace string
            g_string_insert(string, (pos + overlap) - string->str, &replace[overlap]);
        } else if (find_len > replace_len) {
            // remove the non-overlapping part of the find string
            g_string_erase(string, (pos + overlap) - string->str, find_len - replace_len);
        }

        pos += replace_len;
        replacements++;

        // enforce limit
        if (limit != 0) {
            if (replacements == limit) {
                return replacements;
            }
        }
    }

    return replacements;
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
    if (pos > (ssize_t) (string->len - 1)) {
        return string;
    }

    // do we erase to the end of the string?
    if (len < 0 || (pos + len) >= (ssize_t) string->len) {
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

void g_string_vprintf(GString *string, const char *format, va_list args)
{
    g_string_truncate(string, 0);

    g_string_append_vprintf(string, format, args);
}

void g_string_append_vprintf(GString *string, const char *format, va_list args)
{
    const char *c;
    bool format_mode = false;
    bool precision_mode = false;
    int precision = -1;
    int int_arg;
    double double_arg;
    char buf[20];
    char format_buf[20];
    char *cur;

    if (format == NULL) {
        return;
    }

    for (c = format; *c; c++) {
        if (format_mode) {
            if (*c == '%') {
                g_string_append_c(string, '%');
            } else if (*c == 'c') {
                g_string_append_c(string, va_arg(args, int));
            } else if (*c == 's') {
                g_string_append(string, va_arg(args, char*));
            } else if (*c == 'd') {
                int_arg = va_arg(args, int);
                _gstring_ltoa(int_arg, (char*) &buf, 10);
                g_string_append(string, buf);
            } else if (*c == 'f') {
                double_arg = va_arg(args, double);
                if (precision_mode) {
                    snprintf(format_buf, sizeof(format_buf), "%%.%df", precision);
                    snprintf(buf, sizeof(buf), format_buf, double_arg);
                } else {
                    snprintf(buf, sizeof(buf), "%f", double_arg);
                }
                g_string_append(string, buf);
            } else if (*c == 'x' || *c == 'X') {
                int_arg = va_arg(args, int);
                _gstring_ltoa(int_arg, (char*) &buf, 16);
                if (*c == 'x') {
                    for (cur = (char*) &buf; *cur; cur++) {
                        *cur = tolower(*cur);
                    }
                }
                g_string_append(string, buf);
            } else if (*c == '.') {
                precision_mode = true;
                continue;
            } else if (precision_mode && isdigit(*c)) {
                precision = *c - '0';
                continue;
            }

            format_mode = false;
            precision_mode = false;
        } else if (*c != '%') {
            g_string_append_c(string, *c);
        } else {
            format_mode = true;
        }
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
#endif
