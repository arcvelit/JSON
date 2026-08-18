#include "test_framework.h"

static int test_object_and_array_mutation(void) {
    json_t object = json_object_alloc();
    json_t value = json_number_alloc(42);

    TEST_ASSERT(json_add_key_value(object, "answer", value), "failed to add key to object");

    json_t* answer = json_get(object, "answer");
    TEST_ASSERT(answer != NULL, "answer key missing");
    TEST_ASSERT(json_isnum(*answer), "answer is not numeric");
    TEST_ASSERT((*answer)->number->value == 42.0, "answer value mismatch before reset");

    json_number_reset(*answer, 7.0);
    TEST_ASSERT((*answer)->number->value == 7.0, "number reset did not update value");

    json_t array = json_array_alloc();
    TEST_ASSERT(json_push(array, json_string_alloc("hello")), "failed to push string to array");
    TEST_ASSERT(json_push(array, json_copy(*answer)), "failed to push copied number to array");
    TEST_ASSERT(array->array->size == 2, "array size mismatch after push");

    json_free(object);
    json_free(array);
    return TEST_RESULT_PASS;
}

static void test_mutation_suite(test_stats_t* stats) {
    run_test_case("object and array mutation", test_object_and_array_mutation, stats);
}
