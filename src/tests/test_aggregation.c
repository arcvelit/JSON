#include "test_framework.h"

static JSON_BOOL all_true(json_t json_wrap, JSON_BOOL accumulator) {
    return accumulator && json_wrap->boolean->value;
}

static JSON_BOOL some_false(json_t json_wrap, JSON_BOOL accumulator) {
    return accumulator || !json_wrap->boolean->value;
}

static double multiply(json_t json_wrap, double accumulator) {
    return accumulator * json_wrap->number->value;
}

static double add(json_t json_wrap, double accumulator) {
    return accumulator + json_wrap->number->value;
}

static int test_bool_aggregation_returns_expected_results(void) {
    json_t list = json_array_alloc();
    TEST_ASSERT(json_push(list, json_boolean_alloc(JSON_TRUE)), "failed to add true value");
    TEST_ASSERT(json_push(list, json_boolean_alloc(JSON_TRUE)), "failed to add second true value");
    TEST_ASSERT(json_push(list, json_boolean_alloc(JSON_FALSE)), "failed to add false value");

    JSON_BOOL all_values_true = json_reduce_bool(list, JSON_TRUE, all_true);
    JSON_BOOL any_false_value = json_reduce_bool(list, JSON_FALSE, some_false);

    TEST_ASSERT(all_values_true == JSON_FALSE, "all_true reduction should be false with a false element");
    TEST_ASSERT(any_false_value == JSON_TRUE, "some_false reduction should detect a false element");

    json_free(list);
    return TEST_RESULT_PASS;
}

static int test_number_aggregation_returns_expected_results(void) {
    json_t list = json_array_alloc();
    TEST_ASSERT(json_push(list, json_number_alloc(1.0)), "failed to add first number");
    TEST_ASSERT(json_push(list, json_number_alloc(2.0)), "failed to add second number");
    TEST_ASSERT(json_push(list, json_number_alloc(3.0)), "failed to add third number");

    double product = json_reduce_num(list, 1.0, multiply);
    double sum = json_reduce_num(list, 0.0, add);

    TEST_ASSERT(product == 6.0, "product reduction mismatch");
    TEST_ASSERT(sum == 6.0, "sum reduction mismatch");

    json_free(list);
    return TEST_RESULT_PASS;
}

static void test_aggregation_suite(test_stats_t* stats) {
    run_test_case("bool aggregation returns expected results", test_bool_aggregation_returns_expected_results, stats);
    run_test_case("number aggregation returns expected results", test_number_aggregation_returns_expected_results, stats);
}
