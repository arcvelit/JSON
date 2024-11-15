//#define __JSON_FREE_DEBUG

#include "json.h" 
#include <stdio.h>
#include <stdlib.h>


int main()
{
    // Demo setup
    JSON list = json_array_alloc();
    json_push(list, json_boolean_alloc(true));
    json_push(list, json_boolean_alloc(true));
    json_push(list, json_boolean_alloc(false));

    // Print and free
    Writer writer = {0};
    writer_stdout_init(&writer);

    json_write(&writer, list);
    json_free(list);

    return EXIT_SUCCESS;
}
