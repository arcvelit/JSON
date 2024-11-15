//#define __JSON_FREE_DEBUG

#include "../json.h" 
#include <stdio.h>
#include <stdlib.h>

// All are true
bool boolean_all(JSON json_wrap, bool accumulator) {
    return accumulator && json_wrap->boolean->value;
}

// At least one is false
bool boolean_some_false(JSON json_wrap, bool accumulator) {
    return accumulator || !json_wrap->boolean->value;
}


int main()
{
    // Demo setup
    JSON list = json_array_alloc();
    json_push(list, json_boolean_alloc(true));
    json_push(list, json_boolean_alloc(true));
    json_push(list, json_boolean_alloc(false));

    // See if all are true, one is false
    JSON all = json_reducebool(list, true, boolean_all);
    JSON some_false = json_reducebool(list, false, boolean_some_false);

    // Print and free
    Writer writer = {0};
    writer_stdout_init(&writer);

    json_write(&writer, all);
    json_write(&writer, some_false);
    json_free(list);

    return EXIT_SUCCESS;
}
