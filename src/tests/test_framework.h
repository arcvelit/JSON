#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int passed;
    int failed;
    int total;
} test_stats_t;

typedef void (*test_suite_fn)(test_stats_t*);

#define TEST_RESULT_PASS 1
#define TEST_RESULT_FAIL 0

#define TEST_ASSERT(condition, message)                                         \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "FAIL: %s (%s:%d)\n", message, __FILE__, __LINE__); \
            return TEST_RESULT_FAIL;                                            \
        }                                                                       \
    } while (0)

static void run_test_case(const char* name, int (*test_func)(void), test_stats_t* stats) {
    printf("\n-- %s --\n", name);
    stats->total++;

    if (test_func()) {
        stats->passed++;
        printf("PASS: %s\n", name);
    } else {
        stats->failed++;
        printf("FAIL: %s\n", name);
    }
}

static void run_suite(const char* name, test_suite_fn suite_func, test_stats_t* stats) {
    const int before_passed = stats->passed;
    const int before_failed = stats->failed;
    const int before_total = stats->total;

    printf("\n=== %s ===\n", name);
    suite_func(stats);

    printf("suite: %s | passed: %d | failed: %d | total: %d\n",
           name,
           stats->passed - before_passed,
           stats->failed - before_failed,
           stats->total - before_total);
}

#endif
