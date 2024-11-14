#include "json.h" 
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // Demo setup
    Object d = _json_OBJ_sized_alloc(2);
    d->pairs[0] = _json_KV_alloc("SOEN363", json_decimal_alloc(88.3));
    d->pairs[1] = _json_KV_alloc("ENGR391", json_decimal_alloc(65.4));

    /*
    JSON d = json_object_alloc();
    json_add_key_value(d, "SOEN363", json_decimal_alloc(88.3));
    json_add_key_value(d, "ENGR391", json_decimal_alloc(65.4));
    */

    Array l = _json_ARR_sized_alloc(2);
    l->objects[0] = json_object_alloc(d);
    l->objects[1] = json_string_alloc("Paul", 5);
    
    /*
    JSON l = json_array_alloc();
    json_push(l, d);
    json_push(l, json_decimal_alloc(32.3));
    */


    Object o = _json_OBJ_sized_alloc(3);
    o->pairs[0] = _json_KV_alloc("Louka", json_array_alloc(l));
    o->pairs[1] = _json_KV_alloc("Razvan", json_decimal_alloc(5.4));
    o->pairs[2] = _json_KV_alloc("William", json_boolean_alloc(false));

    /* 
    JSON o = json_object_alloc();
    json_add_key_value(o, "Louka", l);
    json_add_key_value(o, "Razvan", json_null_alloc());
    json_add_key_value(o, "William", json_boolean_alloc(true));
    */

    // Parent object
    JSON obj = json_object_alloc(o);
    
    // Print and free
    logger_stdout_init();
    json_log(obj);

    json_free(obj);

    return EXIT_SUCCESS;
}
