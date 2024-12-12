//#define __JSON_FREE_DEBUG

#include "json.h" 


int main()
{
    // Try it yourself here

    // ...

    char* file_cstr = _read_file_content("data.json");
    if (file_cstr)
    {
        printf("%s", file_cstr);
    }

    return EXIT_SUCCESS;
}
