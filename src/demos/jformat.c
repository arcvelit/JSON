#define JSON_IMPLEMENTATION
#include "../json.h" 

#include <string.h>

#define STDIN_ACC_CAP  16*1024
#define STDIN_BUFF_CAP    1024

/**
 * @brief Read from stdin, output formatted json to stdout
 */

int main(int argc, char** argv)
{
    // Shift args
    --argc;
    ++argv;
    if (argc > 0) {
        fprintf(stderr, "ERROR: too many arguments\n");
    }

    // Accumulate stdin
    char buffer[STDIN_BUFF_CAP];
    size_t size = 0;
    char acc[STDIN_ACC_CAP];
    
    while (fgets(buffer, sizeof(buffer), stdin)) {
        size_t buflen = strlen(buffer);
        strcpy(&acc[size], buffer);
        size += buflen;
        if (size >= STDIN_ACC_CAP) {
            fprintf(stderr, "ERROR: stdin buffer overflow (increase capacity)\n");
            return EXIT_FAILURE;
        }
    }
    if (size == 0) {
        fprintf(stderr, "ERROR: nothing passed to stdin\n");
        return EXIT_FAILURE;
    }

    // Initialize stdout writer
    Writer writer = {0};
    writer_stdout_init(&writer);
    
    // Parses stdin string
    json_t parsed = json_parse_string(acc, size);
    if (parsed) {
        json_write(&writer, parsed);
        json_free(parsed);
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
