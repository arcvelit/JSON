#define JSON_IMPLEMENTATION
#include "../json.h" 


int main()
{
    Writer writer = {0};
    writer_stdout_init(&writer);

    // Parse a JSON object from cstr

    JSON object1 = json_parse_string("{\n\t\"status\": 200\n}");
    //json_write(&writer, object1);
    json_free(object1);

    // Parse a JSON object from file
    
    JSON object2 = json_parse_file("data.json");
    //json_write(&writer, object2);
    json_free(object2);

    return EXIT_SUCCESS;
}
