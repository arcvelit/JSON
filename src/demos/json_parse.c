#define JSON_IMPLEMENTATION
#include "../json.h" 

int main()
{
    Writer writer = {0};
    writer_stdout_init(&writer);

    // Parse a JSON object from cstr

    const char* str = "{\n\t\"status\": 200\n}";
    // JSON object1 = json_parse_string(str, json_strlen(str));
    json_t object1 = json_parse_cstring(str);
    if (object1) {
        json_write(&writer, object1);
        json_free(object1);
    }

    // Parse a JSON object from file
    
    json_t object2 = json_parse_file("data.json");
    if (object2) {
        json_write(&writer, object2);
        json_free(object2);
    }

    return EXIT_SUCCESS;
}
