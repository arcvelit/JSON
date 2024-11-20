#define __JSON_FREE_DEBUG

#include "json.h" 


int main()
{
    // Demo setup
    JSON object = json_object_alloc();
    JSON integer = json_integer_alloc(3);
    json_add_key_value(object, "Hello", integer);

    JSON array = json_array_alloc();
    json_push(array, json_null_alloc());

    // The integer 3 gets freed and replaced by this array
    json_reassign(json_get(object, "Hello"), array);


    const c_str uh = integer->integer ? "no" : "yes";
    printf("IS IT NULL? ... %s\n", uh);

    // Print and free
    Writer writer = {0};
    writer_stdout_init(&writer);
    
    json_write(&writer, object);
    json_free(object);

    return EXIT_SUCCESS;
}
