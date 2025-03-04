# JSON
A basic stb-style JSON single header library for C.

## Motivations & Limitations
This personal project aims to build confidence in writing C programs and explore the language's idiomatic patterns.
This project does not aim to provide a 1-to-1 javascript experience: there is no garbage collector, and you must still be aware
of its data allocation and deallocation mechanics. However, it does provide the abstraction of the Javascript Object Notation 
by allowing primitives, objects with key-value pairs, and arrays of objects.  

## Behaviour

Read the [json_best_practices.c](src/demos/json_best_practices.c) demo.

| **Category**     | **Description**  |
|------------------|------------------|
| **Wrappers**     | Everything is wrapped in the `JSON` type, including primitives. The API handles most of the runtime type checking. |
| **Strings**      | All strings provided to the API must be null-terminated. The same is true for the strings provided by the API. The API only accepts `const char *` for string arguments and makes a copy if stored in a JSON wrapper. |
| **Numbers**      | All numbers are stored as `double` for simplicity. |
| **Null**         | Null objects are allocated as `JSON` of type `JSON_NULL`, and their object field is `NULL`. |

### Parsing
You can now parse JSON from strings and files. Refer to the [src/demos/json_parse.c](src/demos/json_parse.c) demo.

### User definable macros
Capacities
* `JSON_MULTIOBJECT_INITIAL_CAP` initial capacity of dynamic arrays and key-values (init: 4)
* `JSON_ARENA_REGION_CAP` number of tokens per region for the parsing arena (init: 256)  

Debug
* `JSON_FREE_DEBUG` logging deallocations of JSON wraps.
* `JSON_ALLOC_DEBUG` logging allocations of JSON wraps.
* `JSON_ARENA_DEBUG` logging arena region growth.
* `JSON_LEXER_DEBUG` logging the type of JSON token that is lexed.

## Getting started
First, include the header - since this is a stb-style library, you must define the `JSON_IMPLEMENTATION` directive for implementation. 

Let's allocate an array and print the JSON wraps to the console.

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
Follow the project demos in [src/demos/](src/demos/) for other examples.

## Performance
Performance was not the utmost priority when designing this library. However, a small test was conducted using this 25Mb [large JSON file](https://github.com/json-iterator/test-data/blob/master/large-file.json). The file was parsed and dumped into a file with `-O3` GCC optimizations. Many small writes to files instead of buffering may impact performance.   

![image](https://github.com/user-attachments/assets/13a9e99b-4d23-4ae3-b8fe-86e127a34958)

[src/json_benchmark.c](src/demos/json_benchmark.c)

## Nerd talk

The library uses a lightweight LL1 top-down predictive parser. You can find the grammar in the declarations part of the header.

## TODO
* Buffer small writes (performance)
* Use hash map instead of array for key-value pairs (performance)
* More built-ins for multi-objects (optional)

## Environment
The source code should be **highly portable** and compatible with C99 and later standards.
