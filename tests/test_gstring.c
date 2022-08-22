#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "gstring.h"

START_TEST(test_gstring_new_null)
{
    GString *string = NULL;
    string = g_string_new(NULL);

    ck_assert_ptr_nonnull(string);
    ck_assert_ptr_nonnull(string->str);
    ck_assert_int_eq(string->len, 0);
    ck_assert_int_eq(string->allocated_len, GSTRING_MIN_BUF_SIZE);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_new_empty)
{
    GString *string = NULL;
    string = g_string_new("");

    ck_assert_ptr_nonnull(string);
    ck_assert_ptr_nonnull(string->str);

    ck_assert_int_eq(string->len, 0);
    ck_assert_int_eq(string->allocated_len, GSTRING_MIN_BUF_SIZE);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_new_small_string)
{
    GString *string = NULL;
    string = g_string_new("Hello World");

    ck_assert_ptr_nonnull(string);
    ck_assert_ptr_nonnull(string->str);
    ck_assert_str_eq("Hello World", string->str);

    ck_assert_int_eq(string->len, 11);
    ck_assert_int_eq(string->allocated_len, GSTRING_MIN_BUF_SIZE);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_new_big_string)
{
    GString *string = NULL;

    // alphabet * 2 = 52 characters
    string = g_string_new("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    ck_assert_ptr_nonnull(string);
    ck_assert_ptr_nonnull(string->str);
    ck_assert_str_eq("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", string->str);

    ck_assert_int_eq(string->len, 52);
    ck_assert_int_eq(string->allocated_len, 53);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_new_len)
{
    GString *string = NULL;
    const char *data = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0abcdefghijklmnopqrstuvwxyz";

    string = g_string_new_len(data, 52 + 1); // 2 * alphabet + internal null byte
    ck_assert_ptr_nonnull(string);
    ck_assert_ptr_nonnull(string->str);
    ck_assert_int_eq(string->len, 52 + 1);
    ck_assert_int_eq(string->allocated_len, 53 + 1);
    ck_assert_int_eq(string->str[string->len], '\0'); // terminating null byte

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_sized_new)
{
    GString *string = NULL;

    string = g_string_sized_new(4096);
    ck_assert_ptr_nonnull(string);
    ck_assert_ptr_nonnull(string->str);
    ck_assert_int_eq(string->len, 0);
    ck_assert_int_eq(string->allocated_len, 4096);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_free)
{
    GString *string = NULL;
    char *result = NULL;

    string = g_string_new("Hello World");
    result = g_string_free(string, true);
    ck_assert_ptr_null(result);

    string = g_string_new("Hello World");
    result = g_string_free(string, false);
    ck_assert_ptr_nonnull(result);
    ck_assert_str_eq(result, "Hello World");
}
END_TEST

START_TEST(test_gstring_assign_empty)
{
    GString *string = NULL;

    string = g_string_new("Hello World");

    g_string_assign(string, "");

    ck_assert_int_eq(string->len, 0);
    ck_assert_int_eq(string->allocated_len, GSTRING_MIN_BUF_SIZE);
    ck_assert_str_eq("", string->str);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_assign)
{
    GString *string = NULL;

    string = g_string_new("");

    g_string_assign(string, "Hello World");

    ck_assert_int_eq(string->len, 11);
    ck_assert_int_eq(string->allocated_len, GSTRING_MIN_BUF_SIZE);
    ck_assert_str_eq(string->str, "Hello World");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_append)
{
    GString *string = NULL;

    string = g_string_new("Hello World");

    g_string_append(string, "!!!");
    ck_assert_int_eq(string->len, 14);
    ck_assert_int_eq(string->allocated_len, 32);
    ck_assert_str_eq("Hello World!!!", string->str);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_append_realloc)
{
    GString *string = NULL;

    string = g_string_new("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    ck_assert_int_eq(string->len, 26);
    ck_assert_int_eq(string->allocated_len, 32);
    ck_assert_str_eq("ABCDEFGHIJKLMNOPQRSTUVWXYZ", string->str);

    g_string_append(string, "abcdefghijklmnopqrstuvwxyz");

    ck_assert_int_eq(string->len, 52);
    // required would be 53 bytes but GString reallocs double the requested space
    ck_assert_int_eq(string->allocated_len, 53 * 2);

    ck_assert_str_eq("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", string->str);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_append_c)
{
    GString *string = NULL;

    string = g_string_new("Hello World");

    g_string_append_c(string, '!');
    ck_assert_int_eq(string->len, 12);
    ck_assert_int_eq(string->allocated_len, 32);
    ck_assert_str_eq("Hello World!", string->str);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_append_c_realloc)
{
    GString *string = NULL;
    size_t len;

    string = g_string_new("Hello World");
    len = string->len;

    for (int i = 0; i < GSTRING_MIN_BUF_SIZE - len; i++) {
        g_string_append_c(string, '!');
    }

    ck_assert_int_eq(string->len, 32);
    ck_assert_int_eq(string->allocated_len, 66);
    ck_assert_str_eq("Hello World!!!!!!!!!!!!!!!!!!!!!", string->str);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_prepend)
{
    GString *string = NULL;

    string = g_string_new("Hello World");

    g_string_prepend(string, "*=-");
    ck_assert_int_eq(string->len, 14);
    ck_assert_str_eq(string->str, "*=-Hello World");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_prepend_realloc)
{
    GString *string = NULL;

    string = g_string_new("Hello World");
    ck_assert_int_eq(string->allocated_len, GSTRING_MIN_BUF_SIZE);

    g_string_prepend(string, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    ck_assert_int_eq(string->len, 63);
    ck_assert_str_eq(string->str, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzHello World");
    ck_assert_int_eq(string->allocated_len, 64 * 2);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_prepend_c)
{
    GString *string = NULL;
    size_t len;

    string = g_string_new("Hello World");
    len = string->len;

    for (int i = 0; i < GSTRING_MIN_BUF_SIZE - len; i++) {
        g_string_prepend_c(string, '=');
    }

    ck_assert_int_eq(string->len, 32);
    ck_assert_int_eq(string->allocated_len, 66);
    ck_assert_str_eq(string->str, "=====================Hello World");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_prepend_c_realloc)
{
    GString *string = NULL;

    string = g_string_new("Hello World");


    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_insert)
{
    GString *string = NULL;

    string = g_string_new("Hello World");

    g_string_insert(string, 5, " you cruel");
    ck_assert_int_eq(string->len, 21);
    ck_assert_str_eq(string->str, "Hello you cruel World");

    g_string_insert(string, 21, "!!!");
    ck_assert_int_eq(string->len, 24);
    ck_assert_str_eq(string->str, "Hello you cruel World!!!");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_insert_realloc)
{
    GString *string = NULL;

    string = g_string_new("Hello World");

    g_string_insert(string, 5, " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    ck_assert_int_eq(string->len, 64);
    ck_assert_int_eq(string->allocated_len, 130);
    ck_assert_str_eq(string->str, "Hello ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz World");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_insert_c)
{
    GString *string = NULL;

    string = g_string_new("Hello World");

    g_string_insert_c(string, 5, '*');
    ck_assert_int_eq(string->len, 12);
    ck_assert_str_eq(string->str, "Hello* World");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_insert_c_realloc)
{
    GString *string = NULL;

    string = g_string_new("Hello World");
    size_t len = string->len;

    for (int i = 0; i < GSTRING_MIN_BUF_SIZE - len; i++) {
        g_string_insert_c(string, 5, '*');
    }

    ck_assert_int_eq(string->len, 32);
    ck_assert_int_eq(string->allocated_len, 66);
    ck_assert_str_eq(string->str, "Hello********************* World");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_erase)
{
    GString *string = NULL;

    string = g_string_new("Hello you cruel World");
    // remove " you cruel "
    g_string_erase(string, 5, 10);

    ck_assert_int_eq(string->len, 11);
    ck_assert_str_eq("Hello World", string->str);

    // remove " World"
    g_string_erase(string, 5, 10);

    ck_assert_int_eq(string->len, 5);
    ck_assert_str_eq("Hello", string->str);

    // truncate via erase
    g_string_erase(string, 3, -1);
    ck_assert_int_eq(string->len, 3);
    ck_assert_str_eq("Hel", string->str);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_truncate)
{
    GString *string = NULL;

    string = g_string_new("Hello World");

    g_string_truncate(string, 5);
    ck_assert_int_eq(string->len, 5);
    ck_assert_str_eq("Hello", string->str);

    g_string_truncate(string, 0);
    ck_assert_int_eq(string->len, 0);
    ck_assert_str_eq("", string->str);

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_printf_int)
{
    GString *string = NULL;
    string = g_string_new("");

    g_string_printf(string, "%d", -1234);
    ck_assert_int_eq(string->len, 5);
    ck_assert_str_eq(string->str, "-1234");

    g_string_printf(string, "%d", 1234);
    ck_assert_int_eq(string->len, 4);
    ck_assert_str_eq(string->str, "1234");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_printf_hex)
{
    GString *string = NULL;
    string = g_string_new("");

    g_string_printf(string, "0x%x", 255);
    ck_assert_int_eq(string->len, 4);
    ck_assert_str_eq(string->str, "0xff");

    g_string_printf(string, "0x%X", 255);
    ck_assert_int_eq(string->len, 4);
    ck_assert_str_eq(string->str, "0xFF");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_printf_string)
{
    GString *string = NULL;
    string = g_string_new("");

    g_string_printf(string, "%s", "Hello World");
    ck_assert_int_eq(string->len, 11);
    ck_assert_str_eq(string->str, "Hello World");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_printf_float)
{
    GString *string = NULL;
    string = g_string_new("");

    g_string_printf(string, "%f", 17.38);
    ck_assert_int_eq(string->len, 9);
    ck_assert_str_eq(string->str, "17.380000");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_printf)
{
    GString *string = NULL;
    string = g_string_new("");

    g_string_printf(string, "String: '%s'; Number: %d; Hex: 0x%x", "STR1", 42, 0xa1);
    ck_assert_int_eq(string->len, 37);
    ck_assert_str_eq(string->str, "String: 'STR1'; Number: 42; Hex: 0xa1");

    g_string_free(string, true);
}
END_TEST

START_TEST(test_gstring_equal)
{
    GString *string1 = NULL;
    GString *string2 = NULL;

    string1 = g_string_new("Hello World");
    string2 = g_string_new("");

    ck_assert(!g_string_equal(string1, string2));

    g_string_assign(string2, "Hello World");
    ck_assert(g_string_equal(string1, string2));

    g_string_assign(string2, "Hello Worlt");
    ck_assert(!g_string_equal(string1, string2));

    g_string_free(string1, true);
    g_string_free(string2, true);
}
END_TEST

Suite* gstring_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("GString");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_gstring_new_null);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_new_empty);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_new_small_string);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_new_big_string);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_new_len);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_sized_new);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_free);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_assign_empty);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_assign);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_append);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_append_realloc);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_append_c);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_append_c_realloc);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_prepend);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_prepend_realloc);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_prepend_c);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_prepend_c_realloc);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_insert);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_insert_realloc);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_insert_c);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_insert_c_realloc);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_erase);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_truncate);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_printf_int);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_printf_hex);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_printf_string);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_printf_float);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_printf);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_core, test_gstring_equal);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc, char **argv)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = gstring_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
