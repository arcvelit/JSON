//#define __JSON_FREE_DEBUG

#include "../json.h" 
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // Demo setup
    JSON courses = json_object_alloc();
    json_add_key_value(courses, "SOEN363", json_decimal_alloc(88.3));
    json_add_key_value(courses, "ENGR391", json_decimal_alloc(65.4));

    JSON list_nulls = json_array_alloc();
    json_push(list_nulls, json_null_alloc());
    json_push(list_nulls, json_null_alloc());
    json_push(list_nulls, json_null_alloc());

    JSON list = json_array_alloc();
    json_push(list, courses);
    json_push(list, json_decimal_alloc(32.3));

    JSON object = json_array_alloc();
    json_push(object, json_integer_alloc(12));
    json_push(object, json_string_alloc("Kingslayer"));

    JSON obj = json_object_alloc();
    json_add_key_value(obj, "Louka", list);
    json_add_key_value(obj, "Razvan", object);
    json_add_key_value(obj, "William", json_boolean_alloc(true));
    json_add_key_value(obj, "NULLS", list_nulls);

    // Print and free
    Writer writer = {0};
    if (!writer_file_init(&writer, "data.json")) 
        return EXIT_FAILURE;

    json_write(&writer, obj);
    json_free(obj);

    return EXIT_SUCCESS;
}
