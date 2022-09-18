#include <stdio.h>
#include <time.h>

#define _CLIB_IMPL 1
#include "ghashtable.h"

#define CALC_SECONDS(start, end) (double) (end - start) / CLOCKS_PER_SEC

void measure_insert(uint32_t num_inserts)
{
    GHashTable *htable = g_hash_table_new(g_int_hash, g_int_equal);

    int start_time = clock();

    for (uint32_t i = 0; i < num_inserts; i++) {
        g_hash_table_insert(htable, (void*) (uint64_t) i, (void*) "Hello World");
    }

    int end_time = clock();
    g_hash_table_destroy(htable);

    printf("Inserting %d elements: %fs\n", num_inserts, CALC_SECONDS(start_time, end_time));
}

void perf_test_insert()
{
    printf("= perf_test_insert =\n\n");

    measure_insert(10);
    measure_insert(100);
    measure_insert(1000);
}

int main(int argc, char **argv)
{
    perf_test_insert();

    return 0;
}
