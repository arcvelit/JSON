# JSON
This is a very simple JSON single header library to play with JSON objects in C.
## Motivations & Limitations
This personal project aims to build confidence in writing C programs and explore the language's idiomatic patterns.
This project does not aim to provide a 1-to-1 javascript experience: there is no garbage collector, and you must still be aware
of its data allocation and deallocation mechanics. However, it does provide the abstraction of the Javascript Object Notation 
by allowing primitives, objects with key-value pairs, and arrays of objects.  

Currently, this library allows you to build JSON objects in memory and write them to the stream of your choice. The goal is 
to allow for reading and parsing JSON files to have them in live memory.
## Behaviour
### Strings
All strings provided to the API must be null-terminated. Same for strings provided by the API. Note that the API only accepts `const char *` for string 
arguments, and it will make a copy of the string if you are trying to store it in a JSON object.

### Objects
Everything is wrapped in the 'JSON' type, including primitives. The API is designed so that it handles most of the runtime type 
checking for you, except in allocations. 

### Allocations
Every JSON is dynamically allocated and the API provides alloc functions for you. If an object is part of a hierarchy, it will be freed
by using the API free on its parent. For example, if you free an array, all members of the array will be freed too. This highly encourages 
"allocating and letting the parent deal with it" to abstract memory management as much as possible.

⚠️ **For this reason, it is not recommended to keep long-lasting references to child objects.**

### Nulls
Null objects are allocated JSON of type JSON_NULL and their object field is NULL.

### Writing 
Writing supports two types of streams: stdout and file. Functions to initiate and close them are provided to you. Make sure to close 
the file writers when they are no longer needed. Plenty of demos show the functionalities of these writers.

### Copying
Making copies of JSON through the API will create and return deep copies.

### Debug macros
The `__JSON_FREE_DEBUG` define instruction prints on the console which type of JSON is being deallocated.

## Getting started
Start by including the header. You can allocate a new array and print the formatted JSON object to the console.

```c
#include "json.h"

int main() {

  JSON array = json_array_alloc();
  json_push(array, json_integer_alloc(3));
  json_push(array, json_decimal_alloc(0.7525));
  json_push(array, json_boolean_alloc(true));
  json_push(array, json_string_alloc("program"));

  Writer writer = {0};
  writer_stdout_init(&writer);

  json_write(&writer, array);

  return 0;
}
```
Follow the project demos in `/src/demos/` for other examples.

## TODO
* Parse JSON from file and string streams (L)
* Replace integer & decimal by Number type (L)
* Make it more convenient to work with multi-objects (M)

## Environment
The source code should be **highly portable** and compatible with C99 and later standards. This limitation permits the convenience
of using fixed-size integer types from `<stdint.h>` and boolean macros from `<stdbool.h>`.
