#include "test_framework.h"
#include <string.h>

static char* read_file_contents(const char* filename, size_t* read_count) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }

    char buffer[256] = {0};
    *read_count = fread(buffer, 1, sizeof(buffer) - 1, file);
    fclose(file);

    char* copy = malloc(*read_count + 1);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, buffer, *read_count);
    copy[*read_count] = '\0';
    return copy;
}

static int test_json_write_file_creates_expected_output(void) {
    const char* filename = "test_output.json";
    json_t object = json_object_alloc();
    TEST_ASSERT(json_add_key_value(object, "name", json_string_alloc("alice")), "failed to add name");
    TEST_ASSERT(json_add_key_value(object, "active", json_boolean_alloc(JSON_TRUE)), "failed to add active");
    TEST_ASSERT(json_add_key_value(object, "score", json_number_alloc(10.5)), "failed to add score");

    Writer writer = {0};
    TEST_ASSERT(writer_file_init(&writer, filename), "failed to open output file");

    json_write(&writer, object);
    writer_file_close(&writer);
    json_free(object);

    size_t read_count = 0;
    char* output = read_file_contents(filename, &read_count);
    remove(filename);

    TEST_ASSERT(output != NULL, "output file was not created");
    TEST_ASSERT(read_count > 0, "output file is empty");
    TEST_ASSERT(strstr(output, "\"name\"") != NULL, "name key missing from file output");
    TEST_ASSERT(strstr(output, "\"active\"") != NULL, "active key missing from file output");
    TEST_ASSERT(strstr(output, "\"score\"") != NULL, "score key missing from file output");

    free(output);
    return TEST_RESULT_PASS;
}

static void test_write_file_suite(test_stats_t* stats) {
    run_test_case("json write file creates expected output", test_json_write_file_creates_expected_output, stats);
}
