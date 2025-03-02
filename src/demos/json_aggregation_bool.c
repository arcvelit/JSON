#define JSON_IMPLEMENTATION
#include "../json.h" 

// All are true
bool all(JSON json_wrap, bool accumulator) {
    return accumulator && json_wrap->boolean->value;
}

// At least one is false
bool some_false(JSON json_wrap, bool accumulator) {
    return accumulator || !json_wrap->boolean->value;
}


int main()
{
    // Use aggregation functions on JSON lists
    
    JSON list = json_array_alloc();
    json_push(list, json_boolean_alloc(true));
    json_push(list, json_boolean_alloc(true));
    json_push(list, json_boolean_alloc(false));

    // See if all are true, one is false
    bool forall = json_reducebool(list, true, all);
    bool exists = json_reducebool(list, false, some_false);

    // Print and free
    printf("%s\n", JSON_BOOL_TO_STRING(forall));
    printf("%s\n", JSON_BOOL_TO_STRING(exists));
    
    json_free(list);

    return EXIT_SUCCESS;
}
