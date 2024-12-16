#include "../json.h" 


int main()
{
    // Find a json object in an array

    JSON array = json_array_alloc();

    JSON findstr = json_string_alloc("Hello, World!");

    json_push(array, json_number_alloc(2));
    json_push(array, json_number_alloc(124));
    json_push(array, json_string_alloc("Hello"));
    json_push(array, json_copy(findstr));

    printf("%s string `%s` in array\n", json_in(array, findstr) ? "Found" : "Did not find", findstr->string->value);

    json_free(array);
    json_free(findstr);

    // Get a json key-value

    JSON object = json_object_alloc();

    json_add_key_value(object, "Superman", json_string_alloc("Clark Kent"));
    json_add_key_value(object, "Batman", json_string_alloc("Bruce Wayne"));
    json_add_key_value(object, "Flash", json_string_alloc("Jay Garrick"));

    const char* hero = "Batman";
    JSON* search = json_get(object, hero);
    if (search) {
        printf("%s's actual name is %s\n", hero, (*search)->string->value);
    } else {
        printf("No superhero called %s\n", hero);
    }

    json_free(object);


    return EXIT_SUCCESS;
}
