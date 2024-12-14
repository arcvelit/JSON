#include "../json.h" 


int main()
{
    // Modify the value of JSON primitives
     
    JSON list = json_array_alloc();

    JSON number1 = json_number_alloc(1);
    JSON number2 = json_number_alloc(69.420);
    JSON boolean = json_boolean_alloc(true);
    JSON string = json_string_alloc("Hello World!");

    json_number_reset(number1, 32);
    json_number_reset(number2, 15.45);
    json_boolean_reset(boolean, false);
    json_string_reset(string, "Merry Christmas!");

    json_push(list, number1);
    json_push(list, number2);
    json_push(list, boolean);
    json_push(list, string);

    // Print and free
    Writer writer = {0};
    writer_stdout_init(&writer);

    json_write(&writer, list);
    json_free(list);

    return EXIT_SUCCESS;
}
