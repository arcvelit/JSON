# JSON
A basic stb-style JSON single header library for C.

## Motivations & Limitations
This personal project aims to build confidence in writing C programs and explore the language's idiomatic patterns.
This project does not aim to provide a 1-to-1 javascript experience: there is no garbage collector, and you must still be aware
of its data allocation and deallocation mechanics. However, it does provide the abstraction of the Javascript Object Notation 
by allowing primitives, objects with key-value pairs, and arrays of objects.  

## Behaviour

### Strings
All strings provided to the API must be null-terminated. The same is true for strings provided by the API. Note that the API only accepts `const char *` for string 
arguments, and it will make a copy of the string if you are trying to store it in a JSON object.

### Numbers
All numbers are stored as `double` for simplicity. 

### Objects
Everything is wrapped in the 'JSON' type, including primitives. The API is designed so that it handles most of the runtime type 
checking for you, except in allocations. 

### Null
Null objects are allocated JSON of type JSON_NULL, and their object field is NULL.

### Allocations
Read the `json_best_practices.c` demo.  

**TL;DR** Every JSON is dynamically allocated, and the API provides alloc functions for you. If an object is part of a hierarchy, it will be freed using the API free on its parent. For example, if you free an array, all members will be freed, too. This encourages 
"allocating and letting the parent deal with it" to abstract memory management as much as possible.

⚠️ **For this reason, long-lasting references to child objects are not recommended.**

### Parsing
You can now parse JSON from strings and files. Refer to the demos.

### Writing 
Writing supports two types of streams: stdout and file. Methods for initiating and closing streams are provided to you. Close 
the file writers when they are no longer needed. Plenty of demos show the functionalities of these writers.

### Copying
Making copies of JSON through the API will create and return deep copies.

### Debug macros
* The `__JSON_FREE_DEBUG` define instruction logs deallocations of JSON objects.
* The `__JSON_ARENA_DEBUG` define instruction logs arena region growth.
* The `__JSON_LEXER_DEBUG` define instruction logs the type of JSON token that is lexed.

## Getting started
Start by including the header. Since this is a stb-style library, you must define the `JSON_IMPLEMENTATION` directive.  

Let's allocate an array and print the JSON object to the console.

```c
#define JSON_IMPLEMENTATION
#include "json.h"

int main() {

  JSON array = json_array_alloc();
  json_push(array, json_number_alloc(3));
  json_push(array, json_number_alloc(0.7525));
  json_push(array, json_boolean_alloc(true));
  json_push(array, json_string_alloc("program"));

  Writer writer = {0};
  writer_stdout_init(&writer);
  
  json_write(&writer, array);
  json_free(array);

  return 0;
}
```
Follow the project demos in `/src/demos/` for other examples.

## Performance
Performance was not the utmost priority when designing this library. However, a small test was conducted using this 25Mb [large JSON file](https://github.com/json-iterator/test-data/blob/master/large-file.json). The file was parsed and dumped into a file with `-O3` GCC optimizations. Many small writes to files instead of buffering may impact performance.  

![image](https://github.com/user-attachments/assets/12ae170e-209c-4b02-b101-04b1bf2b4ab6)

## TODO
* Replace recursion in writing, tokenizing, parsing (performance)
* User hash map instead of array for key-value pairs (performance)
* More built-ins for multi-objects (optional)

## Environment
The source code should be **highly portable** and compatible with C99 and later standards.
