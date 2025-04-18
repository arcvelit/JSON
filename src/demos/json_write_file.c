#define JSON_IMPLEMENTATION
#include "../json.h" 

int main()
{
    // Write JSON objects to a file
    
    json_t courses = json_object_alloc();
    json_add_key_value(courses, "SOEN363", json_number_alloc(88.3));
    json_add_key_value(courses, "ENGR391", json_number_alloc(65.4));

    json_t list_nulls = json_array_alloc();
    json_push(list_nulls, json_null_alloc());
    json_push(list_nulls, json_null_alloc());
    json_push(list_nulls, json_null_alloc());

    json_t list = json_array_alloc();
    json_push(list, courses);
    json_push(list, json_number_alloc(32.3));

    json_t object = json_array_alloc();
    json_push(object, json_number_alloc(12));
    json_push(object, json_string_alloc("Kingslayer"));

    json_t obj = json_object_alloc();
    json_add_key_value(obj, "Louka", list);
    json_add_key_value(obj, "Razvan", object);
    json_add_key_value(obj, "William", json_boolean_alloc(JSON_TRUE));
    json_add_key_value(obj, "NULLS", list_nulls);

    // Print and free
    Writer writer = {0};
    if (!writer_file_init(&writer, "file.json")) 
        return EXIT_FAILURE;

    json_write(&writer, obj);
    json_free(obj);

    writer_file_close(&writer);

    return EXIT_SUCCESS;
}
