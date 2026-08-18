#define JSON_IMPLEMENTATION
#include <json.h>

#include "test_framework.h"
#include "test_parse.c"
#include "test_mutation.c"
#include "test_aggregation.c"
#include "test_write_file.c"

int main(void) {
    test_stats_t stats = {0};

    run_suite("parse", test_parse_suite, &stats);
    run_suite("mutation", test_mutation_suite, &stats);
    run_suite("aggregation", test_aggregation_suite, &stats);
    run_suite("write_file", test_write_file_suite, &stats);

    printf("\n=== GLOBAL SUMMARY ===\n");
    printf("passed: %d\n", stats.passed);
    printf("failed: %d\n", stats.failed);
    printf("total:  %d\n", stats.total);

    if (stats.failed == 0) {
        printf("GLOBAL RESULT: ALL TESTS PASSED\n");
        return EXIT_SUCCESS;
    }

    printf("GLOBAL RESULT: FAILURES DETECTED\n");
    return EXIT_FAILURE;
}
