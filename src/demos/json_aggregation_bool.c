#define JSON_IMPLEMENTATION
#include "../json.h" 

// All are true
JSON_BOOL all(json_t json_wrap, JSON_BOOL accumulator) {
    return accumulator && json_wrap->boolean->value;
}

// At least one is false
JSON_BOOL some_false(json_t json_wrap, JSON_BOOL accumulator) {
    return accumulator || !json_wrap->boolean->value;
}

int main()
{
    // Use aggregation functions on JSON lists
    
    json_t list = json_array_alloc();
    json_push(list, json_boolean_alloc(JSON_TRUE));
    json_push(list, json_boolean_alloc(JSON_TRUE));
    json_push(list, json_boolean_alloc(JSON_FALSE));

    // See if all are true, one is false
    JSON_BOOL forall = json_reduce_bool(list, JSON_TRUE, all);
    JSON_BOOL exists = json_reduce_bool(list, JSON_FALSE, some_false);

    // Print and free
    printf("%s\n", JSON_BOOL_TO_STRING(forall));
    printf("%s\n", JSON_BOOL_TO_STRING(exists));
    
    json_free(list);

    return EXIT_SUCCESS;
}
