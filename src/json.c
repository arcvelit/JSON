#include "json.h" 
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // Demo setup
    Object d = json_object_sized_alloc(2);
    d->pairs[0] = json_key_value_alloc("SOEN363", json_object_type_decimal_alloc(88.3));
    d->pairs[1] = json_key_value_alloc("ENGR391", json_object_type_decimal_alloc(65.4));

    Array l = json_array_sized_alloc(2);
    l->objects[0] = json_object_type_object_alloc(d);
    l->objects[1] = json_object_type_string_alloc("Paul", 5);

    ObjectType louka_sub = json_object_type_array_alloc(l);

    Object o = json_object_sized_alloc(3);
    o->pairs[0] = json_key_value_alloc("Louka", louka_sub);
    o->pairs[1] = json_key_value_alloc("Razvan", json_object_type_decimal_alloc(5.4));
    o->pairs[2] = json_key_value_alloc("William", json_object_type_boolean_alloc(false));

    // Parent object
    ObjectType obj = json_object_type_object_alloc(o);
    
    // Print and free
    logger_stdout_init();
    json_object_type_log(obj);

    json_object_type_free(obj);

    return EXIT_SUCCESS;
}
