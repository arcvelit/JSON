#define JSON_IMPLEMENTATION
#include "../json.h" 

int main()
{
    // Create a deep copy of a JSON

    JSON object1 = json_object_alloc();
    json_add_key_value(object1, "A", json_null_alloc());
    json_add_key_value(object1, "B", json_number_alloc(1));

    // Object2 -> A: null B: 1 C: null
    JSON object2 = json_copy(object1);
    json_add_key_value(object2, "C", json_null_alloc());
    
    // Object1 -> A: null B: 69
    json_number_reset(object1->object->pairs[1]->value, 69);

    // Print and free
    Writer writer = {0};
    writer_stdout_init(&writer);

    printf("List:\n");
    json_write(&writer, object1);
    printf("Copy:\n");
    json_write(&writer, object2);

    json_free(object1);
    json_free(object2);

    return EXIT_SUCCESS;
}
