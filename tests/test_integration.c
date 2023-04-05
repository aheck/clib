#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#define _CLIB_IMPL 1
#include "garray.h"
#include "ghashtable.h"
#include "glist.h"
#include "gstring.h"

START_TEST(test_integration)
{
    // Dummy test
    //
    // The purpose of this test binary is to ensure that it is possible to
    // compile a binary containing all headers of CLib at the same time.
}
END_TEST

Suite* all_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("All");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_integration);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc, char **argv)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = all_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
