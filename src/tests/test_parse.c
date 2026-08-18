#include "test_framework.h"

static int test_parse_object_returns_json_object(void) {
    json_t object = json_parse_cstring("{\"status\":200,\"ok\":true,\"name\":\"alice\"}");
    TEST_ASSERT(object != NULL, "parse object returned NULL");
    TEST_ASSERT(json_isobj(object), "parsed value is not an object");
    json_free(object);
    return TEST_RESULT_PASS;
}

static int test_parse_object_keeps_status_key(void) {
    json_t object = json_parse_cstring("{\"status\":200,\"ok\":true,\"name\":\"alice\"}");
    TEST_ASSERT(object != NULL, "object should parse");

    json_t* status = json_get(object, "status");
    TEST_ASSERT(status != NULL, "status key missing");
    TEST_ASSERT(json_isnum(*status), "status is not numeric");
    TEST_ASSERT(json_isint(*status), "status is not an integer");
    TEST_ASSERT((*status)->number->value == 200.0, "status value mismatch");

    json_free(object);
    return TEST_RESULT_PASS;
}

static int test_parse_object_keeps_boolean_and_string_keys(void) {
    json_t object = json_parse_cstring("{\"status\":200,\"ok\":true,\"name\":\"alice\"}");
    TEST_ASSERT(object != NULL, "object should parse");

    json_t* ok = json_get(object, "ok");
    TEST_ASSERT(ok != NULL, "ok key missing");
    TEST_ASSERT((*ok)->type == JSON_BOOLEAN_TYPE, "ok is not boolean");
    TEST_ASSERT((*ok)->boolean->value == JSON_TRUE, "boolean value mismatch");

    json_t* name = json_get(object, "name");
    TEST_ASSERT(name != NULL, "name key missing");
    TEST_ASSERT(json_isstr(*name), "name is not a string");
    TEST_ASSERT(json_strcmp((*name)->string->value, "alice") == 0, "string value mismatch");

    json_free(object);
    return TEST_RESULT_PASS;
}

static int test_parse_array_returns_expected_shape(void) {
    json_t array = json_parse_cstring("[1, 2.5, true, {\"x\": \"y\"}]");
    TEST_ASSERT(array != NULL, "parse array returned NULL");
    TEST_ASSERT(json_isarr(array), "parsed value is not an array");
    TEST_ASSERT(array->array->size == 4, "array size mismatch");
    json_free(array);
    return TEST_RESULT_PASS;
}

static int test_parse_array_element_types(void) {
    json_t array = json_parse_cstring("[1, 2.5, true, {\"x\": \"y\"}]");
    TEST_ASSERT(array != NULL, "parse array returned NULL");

    TEST_ASSERT(array->array->objects[0]->type == JSON_NUMBER_TYPE, "first element is not a number");
    TEST_ASSERT(array->array->objects[0]->number->value == 1.0, "first number mismatch");

    TEST_ASSERT(array->array->objects[1]->type == JSON_NUMBER_TYPE, "second element is not a number");
    TEST_ASSERT(array->array->objects[1]->number->value == 2.5, "second number mismatch");

    TEST_ASSERT(array->array->objects[2]->type == JSON_BOOLEAN_TYPE, "third element is not boolean");
    TEST_ASSERT(array->array->objects[2]->boolean->value == JSON_TRUE, "third boolean mismatch");

    TEST_ASSERT(array->array->objects[3]->type == JSON_OBJECT_TYPE, "fourth element is not object");
    TEST_ASSERT(array->array->objects[3]->object->keys == 1, "nested object keys mismatch");

    json_free(array);
    return TEST_RESULT_PASS;
}

static int test_invalid_json_is_rejected(void) {
    json_t invalid = json_parse_cstring("{\"a\":}");
    TEST_ASSERT(invalid == NULL, "invalid JSON should be rejected");
    return TEST_RESULT_PASS;
}

static int test_empty_json_is_rejected(void) {
    json_t empty = json_parse_cstring("");
    TEST_ASSERT(empty == NULL, "empty JSON should be rejected");
    return TEST_RESULT_PASS;
}

static void test_parse_suite(test_stats_t* stats) {
    run_test_case("parse object returns json object", test_parse_object_returns_json_object, stats);
    run_test_case("parse object keeps status key", test_parse_object_keeps_status_key, stats);
    run_test_case("parse object keeps boolean and string keys", test_parse_object_keeps_boolean_and_string_keys, stats);
    run_test_case("parse array returns expected shape", test_parse_array_returns_expected_shape, stats);
    run_test_case("parse array element types", test_parse_array_element_types, stats);
    run_test_case("invalid json is rejected", test_invalid_json_is_rejected, stats);
    run_test_case("empty json is rejected", test_empty_json_is_rejected, stats);
}
