//#define __JSON_FREE_DEBUG

#include "../json.h" 
#include <stdio.h>
#include <stdlib.h>

// Product reduce
int64_t integer_multiplication(JSON json_wrap, int64_t accumulator) {
    return accumulator * json_wrap->integer->value;
}

// Sum reduce
int64_t integer_addition(JSON json_wrap, int64_t accumulator) {
    return accumulator + json_wrap->integer->value;
}


int main()
{
    // Demo setup
    JSON list = json_array_alloc();
    json_push(list, json_integer_alloc(1));
    json_push(list, json_integer_alloc(2));
    json_push(list, json_integer_alloc(3));

    // Get the product and sum of the array elements
    JSON product = json_reduceint(list, 1, integer_multiplication);
    JSON sum = json_reduceint(list, 0, integer_addition);

    // Print and free
    Writer writer = {0};
    writer_stdout_init(&writer);

    json_write(&writer, product);
    json_write(&writer, sum);
    json_free(list);

    return EXIT_SUCCESS;
}
