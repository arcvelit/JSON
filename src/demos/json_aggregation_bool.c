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
    JSON forall = json_reducebool(list, true, all);
    JSON exists = json_reducebool(list, false, some_false);

    // Print and free
    Writer writer = {0};
    writer_stdout_init(&writer);

    json_write(&writer, forall);
    json_write(&writer, exists);
    json_free(list);

    return EXIT_SUCCESS;
}
