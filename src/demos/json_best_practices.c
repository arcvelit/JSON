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
    * If an object is part of a hierarchy, it will be freed when its parent is freed.
    * For example, if you free an array, all members will be freed, too. This encourages 
    * "allocating and letting the parent deal with it" to abstract memory management as 
    * much as possible. 
    * 
    * For this reason, long-lasting references to child objects are not recommended.
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
