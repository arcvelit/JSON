//#define __JSON_FREE_DEBUG

#include "json.h" 
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

    JSON array = json_array_alloc();
    json_push(array, json_integer_alloc(12));
    json_push(array, json_string_alloc("Smartass"));

    JSON obj = json_object_alloc();
    json_add_key_value(obj, "Louka", list);
    json_add_key_value(obj, "Razvan", array);
    json_add_key_value(obj, "William", json_boolean_alloc(true));
    json_add_key_value(obj, "NULLS", list_nulls);

    // Print and free
    Logger logger = {0};
    logger_stdout_init(&logger);
    json_free(obj);

    return EXIT_SUCCESS;
}
