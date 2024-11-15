//#define __JSON_FREE_DEBUG

#include "../json.h" 
#include <stdio.h>
#include <stdlib.h>


int main()
{
    // Demo setup
    JSON list = json_array_alloc();

    JSON integer = json_integer_alloc(1);
    JSON decimal = json_decimal_alloc(69.420);
    JSON boolean = json_boolean_alloc(true);
    JSON string = json_string_alloc("Hello World!");

    json_integer_reset(integer, 32);
    json_decimal_reset(decimal, 15.45);
    json_boolean_reset(boolean, false);
    json_string_reset(string, "Merry Christmas!");

    json_push(list, integer);
    json_push(list, decimal);
    json_push(list, boolean);
    json_push(list, string);

    // Print and free
    Writer writer = {0};
    writer_stdout_init(&writer);

    json_write(&writer, list);
    json_free(list);

    return EXIT_SUCCESS;
}
