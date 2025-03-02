#define JSON_IMPLEMENTATION
#include "../json.h" 

// Product reduce
double multiplication(JSON json_wrap, double accumulator) {
    return accumulator * json_wrap->number->value;
}

// Sum reduce
double addition(JSON json_wrap, double accumulator) {
    return accumulator + json_wrap->number->value;
}


int main()
{
    // Use aggregation functions on JSON lists
    
    JSON list = json_array_alloc();
    json_push(list, json_number_alloc(1));
    json_push(list, json_number_alloc(2));
    json_push(list, json_number_alloc(3));

    // Get the product and sum of the array elements
    double product = json_reducenum(list, 1, multiplication);
    double sum = json_reducenum(list, 0, addition);

    // Print and free
    printf("%g\n", product);
    printf("%g\n", sum);
    
    json_free(list);

    return EXIT_SUCCESS;
}
