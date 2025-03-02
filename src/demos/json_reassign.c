#define JSON_IMPLEMENTATION
#include "../json.h" 


int main()
{
    // Replace a JSON object by another while freeing

    JSON object = json_object_alloc();
    json_add_key_value(object, "Hello", json_number_alloc(3));

    JSON array = json_array_alloc();
    json_push(array, json_null_alloc());

    // The number 3 gets freed and replaced by this array
    json_reassign(json_get(object, "Hello"), array);

    // Print and free
    Writer writer = {0};
    writer_stdout_init(&writer);
    
    json_write(&writer, object);
    json_free(object);

    return EXIT_SUCCESS;
}
