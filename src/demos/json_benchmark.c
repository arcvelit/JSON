#define JSON_IMPLEMENTATION
#include "../json.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{   
    const char* i_file = "input.json";
    const char* o_file = "ouput.json";

    clock_t start_parse = clock();

    JSON obj = json_parse_file(i_file);
    
    clock_t end_parse = clock();
    
    if (obj) {
        Writer writer = {0}; 
        writer_file_init(&writer, o_file);

        clock_t start_write = clock();
        
        json_write(&writer, obj);
        
        clock_t end_write = clock();
        
        json_free(obj);

        printf("Parsing time: %lf seconds\n", (double)(end_parse - start_parse) / CLOCKS_PER_SEC);
        printf("Writing time: %lf seconds\n", (double)(end_write - start_write) / CLOCKS_PER_SEC);
    }

    return EXIT_SUCCESS;
}
