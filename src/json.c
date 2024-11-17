//#define __JSON_FREE_DEBUG

#include "json.h" 
#include <stdio.h>
#include <stdlib.h>


int main()
{
    // TODO: COPIES
    JSON object = json_object_alloc();
    json_add_key_value(object, "A", json_null_alloc());
    json_add_key_value(object, "B", json_integer_alloc(1));


    // Print and free
    Writer writer = {0};
    writer_stdout_init(&writer);
    
    json_write(&writer, json_get(object, "A"));
    json_free(object);

    return EXIT_SUCCESS;
}
