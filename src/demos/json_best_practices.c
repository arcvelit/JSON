#define JSON_IMPLEMENTATION
#include "../json.h" 


int main()
{
    /* 
    * I thought it was worth mentionning that using this header can go very very 
    * wrong if not used properly (no garbage collector, owernship transfer, etc.)
    * Here are some important things to take into consideration.
    * 
    * 1. Cyclic references
    * 
    * There are no checks for cyclic references, this may crash your program
    * while using the API. It will most definitely crash if you try printing 
    * the object. Avoid them by using the API as much as possible.
    * 
    * 
    * 2. Ownership transfer
    * 
    * Every time you call json_..._alloc(), there is a memory allocation made
    * that you must free later on with json_free(). Alloc methods transfer the 
    * ownership of the JSON object to you. 
    * 
    * Aggregation functions like json_reduce...() also make an allocation.
    * 
    * 
    * 3. Dangling pointers
    * 
    * Avoid keeping long lasting references to objects. This is because json_free() 
    * is a recursive function that also frees the children of an object. You will
    * lose ownership of objects if you nest them in other objects.
    * 
    * 
    * 4. Final notes
    * 
    * Generally, use the API builtins.
    * 
    * 
    */
    return EXIT_SUCCESS;
}
