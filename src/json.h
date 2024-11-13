#ifndef JSON_C
#define JSON_C

#include <inttypes.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*  
    ================================
     Preprocessor defines    
    ================================
*/ 

#define __JSON_OBJECT_OPEN "{"
#define __JSON_OBJECT_CLOSE "}"
#define __JSON_ARRAY_OPEN "["
#define __JSON_ARRAY_CLOSE "]"
#define __JSON_KEY_TO_VALUE ": "
#define __JSON_KEY_VALUE_SEPARATOR ","
#define __JSON_ARRAY_ELEMENT_SEPARATOR ","

#define __JSON_TABULATION "    "

#define __JSON_KEY_PRINT_FMT "\"%s\""
#define __JSON_DOUBLE_PRINT_FMT "%f"
#define __JSON_INTEGER_PRINT_FMT "%" PRId64
#define __JSON_STRING_PRINT_FMT "\"%s\""
#define __JSON_BOOLEAN_TRUE_PRINT_FMT "true" 
#define __JSON_BOOLEAN_FALSE_PRINT_FMT "false" 
#define __JSON_NULL_PRINT "null"

#define __JSON_BOOL_TO_STRING(b) ((b) ? __JSON_BOOLEAN_TRUE_PRINT_FMT : __JSON_BOOLEAN_FALSE_PRINT_FMT)


#ifdef __JSON_FREE_DEBUG
    #define __FREE_DEBUG_PRINT(MESSAGE) printf("\nDEBUG: Freeing %s\n", MESSAGE)
#else
    #define __FREE_DEBUG_PRINT(MESSAGE)
#endif


/*  
    ================================
     Type defintions    
    ================================
*/ 

typedef struct _json_array   _json_array;
typedef struct _json_object  _json_object;
typedef struct _json_string  _json_string;
typedef struct _json_integer _json_integer;
typedef struct _json_decimal _json_decimal;
typedef struct _json_boolean _json_boolean;

typedef _json_array*   Array;
typedef _json_object*  Object;
typedef _json_string*  String;
typedef _json_integer* Integer;
typedef _json_decimal* Decimal;
typedef _json_boolean* Boolean;

typedef struct _json_logger _json_logger;

typedef struct _json_object_type    _json_object_type;
typedef struct _json_key_value_pair _json_key_value_pair;

typedef _json_object_type*    ObjectType;
typedef _json_key_value_pair* KeyValue;

typedef char* c_str;

typedef enum {
    JSON_NULL,
    // Header types
    JSON_OBJECT, JSON_ARRAY,
    // Atomic types
    JSON_INTEGER, JSON_DECIMAL, JSON_STRING, JSON_BOOLEAN
} JsonType;

typedef enum {
    LOGGER_STDOUT,
    LOGGER_FILE
} LoggerType;

struct _json_logger {
    LoggerType type;
    FILE* stream;
};


/*  
    ================================
     Declarations    
    ================================
*/ 

ObjectType* json_object_type_multi_alloc(size_t size);
void json_object_type_free(ObjectType object_type);

void _ident_json_object_type_print(size_t depth, ObjectType object_type);


/*  
    ================================
     Logging    
    ================================
*/ 

_json_logger* __json_global_logger = NULL;

void logger_stdout_init() {
    if (!__json_global_logger)
        __json_global_logger = (_json_logger*)malloc(sizeof(_json_logger));
    __json_global_logger->type = LOGGER_STDOUT;
    __json_global_logger->stream = stdout;
}

int logger_file_init(const char *filename) {
    if (!__json_global_logger)
        __json_global_logger = (_json_logger*)malloc(sizeof(_json_logger));
    __json_global_logger->type = LOGGER_FILE;
    __json_global_logger->stream = fopen(filename, "w");
    return __json_global_logger->stream ? 1 : 0;
}

void logger_file_close() {
    if (__json_global_logger->type == LOGGER_FILE && __json_global_logger->stream) {
        fclose(__json_global_logger->stream);
        __json_global_logger->stream = NULL;
    }
}

void _logger_logf(const char *message, ...) {
    if (__json_global_logger && __json_global_logger->stream) {
        va_list args;
        va_start(args, message);
        vfprintf(__json_global_logger->stream, message, args);
        va_end(args);
    }
}


/*  
    ================================
     Structures    
    ================================
*/ 

struct _json_key_value_pair {
    c_str   key;
    ObjectType value;
};

struct _json_object {       /* JSON_OBJECT  */
    size_t      keys;
    KeyValue*   pairs;
};

struct _json_array {        /* JSON_ARRAY   */
    size_t  size;
    ObjectType* objects;
};

struct _json_string {       /* JSON_STRING  */
    c_str   value;
    size_t  size;
};

struct _json_integer {      /* JSON_INTEGER */
    int64_t value;
};

struct _json_decimal {      /* JSON_DOUBLE  */
    double  value;
};

struct _json_boolean {      /* JSON_BOOLEAN    */
    bool value;
};

struct _json_object_type {
    JsonType type;
    union {
        Array   array;
        Object  object;
        String  string;
        Integer integer;
        Decimal decimal;
        Boolean boolean;
    };
};


/*  
    ================================
     Functions    
    ================================
*/ 

/* Memory allocation guards */

#define __ALLOC_FAILED_GUARD(ptr, line) \
    if (!(ptr)) { \
        fprintf(stderr, "Memory allocation failed for %s at %s:%d\n", #ptr, __FILE__, line); \
        return NULL; \
    }

#define ___ALLOC_FAILED_GUARD_FREE(ptr, cleanup, line) \
    if (!(ptr)) { \
        fprintf(stderr, "Memory allocation failed for %s at %s:%d\n", #ptr, __FILE__, line); \
        free(cleanup); \
        return NULL; \
    }

/* String alloc and free */


String json_string_empty_alloc() {
    String new_string = (String)malloc(sizeof(_json_string));
    __ALLOC_FAILED_GUARD(new_string, __LINE__);

    new_string->size = 0;
    new_string->value = NULL;
    return new_string;
}

String json_string_alloc(c_str string, size_t size) {
    String new_string = (String)malloc(sizeof(_json_string));
    __ALLOC_FAILED_GUARD(new_string, __LINE__);

    new_string->value = (c_str)malloc(size + 1);
    ___ALLOC_FAILED_GUARD_FREE(new_string->value, new_string, __LINE__);

    strncpy(new_string->value, string, size);
    new_string->value[size] = '\0';
    new_string->size = size;
    return new_string;
}

void json_string_free(String json_string) {
    if (json_string) {
        __FREE_DEBUG_PRINT("string");
        free(json_string->value);
        free(json_string);
    }
}

/* Integer alloc and free */

Integer json_integer_alloc(int64_t value) {
    Integer new_integer = (Integer)malloc(sizeof(_json_integer));
    __ALLOC_FAILED_GUARD(new_integer, __LINE__);

    new_integer->value = value;
    return new_integer;
}

void json_integer_free(Integer json_integer) {
    if (json_integer) {
        __FREE_DEBUG_PRINT("integer");
        free(json_integer);
    }
}

/* Boolean alloc and free */

Boolean json_boolean_alloc(bool value) {
    Boolean new_boolean = (Boolean)malloc(sizeof(_json_boolean));
    __ALLOC_FAILED_GUARD(new_boolean, __LINE__);

    new_boolean->value = value;
    return new_boolean;
}

void json_boolean_free(Boolean json_boolean) {
    if (json_boolean) {
        __FREE_DEBUG_PRINT("boolean");
        free(json_boolean);
    }
}

/* Decimal alloc and free */

Decimal json_decimal_alloc(double value) {
    Decimal new_decimal = (Decimal)malloc(sizeof(_json_decimal));
    __ALLOC_FAILED_GUARD(new_decimal, __LINE__);

    new_decimal->value = value;
    return new_decimal;
}

void json_decimal_free(Decimal json_decimal) {
    if (json_decimal) {
        __FREE_DEBUG_PRINT("decimal");
        free(json_decimal);
    }
}

/* Generic decimal and integer free */

void json_numeric_free(void* json_numeric) {
    if (json_numeric) {
        free(json_numeric);
    }
}

/* KeyValue alloc and free */

KeyValue json_key_value_empty_alloc() {
    KeyValue new_key_value = (KeyValue)malloc(sizeof(_json_key_value_pair));
    __ALLOC_FAILED_GUARD(new_key_value, __LINE__);

    new_key_value->key = NULL;
    new_key_value->value = NULL;
    return new_key_value;
}

KeyValue json_key_value_alloc(c_str key, ObjectType object_type) {
    KeyValue new_key_value = (KeyValue)malloc(sizeof(_json_key_value_pair));
    __ALLOC_FAILED_GUARD(new_key_value, __LINE__);

    new_key_value->key = key;
    new_key_value->value = object_type;
    return new_key_value;
}

KeyValue* json_key_value_multi_alloc(size_t size) {
    KeyValue* new_multi_key_value = (KeyValue*)malloc(sizeof(_json_key_value_pair*) * size);
    __ALLOC_FAILED_GUARD(new_multi_key_value, __LINE__);

    return new_multi_key_value;
}

void json_key_value_free(KeyValue key_value) {
    if (key_value) {
        __FREE_DEBUG_PRINT("key value");
        if (key_value->key)   free(key_value->key);
        if (key_value->value) json_object_type_free(key_value->value);
        free(key_value);
    }
}

/* Object alloc and free */

Object json_object_empty_alloc() {
    Object new_object = (Object)malloc(sizeof(_json_object));
    __ALLOC_FAILED_GUARD(new_object, __LINE__);

    new_object->keys = 0;
    new_object->pairs = NULL;
    return new_object;
}

Object json_object_alloc(KeyValue* key_values, size_t keys) {
    Object new_object = (Object)malloc(sizeof(_json_object));
    __ALLOC_FAILED_GUARD(new_object, __LINE__);

    new_object->keys = keys;
    new_object->pairs = key_values;
    return new_object;
}

Object json_object_sized_alloc(size_t size) {
    Object new_object = (Object)malloc(sizeof(_json_object));
    __ALLOC_FAILED_GUARD(new_object, __LINE__);

    new_object->keys = size;
    new_object->pairs = json_key_value_multi_alloc(size);
    ___ALLOC_FAILED_GUARD_FREE(new_object->pairs, new_object, __LINE__);

    return new_object;
}

void json_object_free(Object object) {
    if (object) {
        __FREE_DEBUG_PRINT("object");
        if (object->pairs) {
            for (size_t i = 0; i < object->keys; i++)
                json_key_value_free(object->pairs[i]);
            free(object->pairs);
        }
        free(object);
    }
}

/* Array alloc and free */

Array json_array_empty_alloc() {
    Array new_array = (Array)malloc(sizeof(_json_array));
    __ALLOC_FAILED_GUARD(new_array, __LINE__);

    new_array->size = 0;
    new_array->objects = NULL;
    return new_array;
}

Array json_array_sized_alloc(size_t size) {
    Array new_array = (Array)malloc(sizeof(_json_array));
    __ALLOC_FAILED_GUARD(new_array, __LINE__);

    new_array->size = size;
    new_array->objects = json_object_type_multi_alloc(size);
    ___ALLOC_FAILED_GUARD_FREE(new_array->objects, new_array, __LINE__);

    return new_array;
}

Array json_array_alloc(ObjectType* objects, size_t size) {
    Array new_array = (Array)malloc(sizeof(_json_array));
    __ALLOC_FAILED_GUARD(new_array, __LINE__);

    new_array->size = size;
    new_array->objects = objects;
    return new_array;
}

void json_array_free(Array array) {
    if (array) {
        __FREE_DEBUG_PRINT("array");
        if (array->objects) {
            for (size_t i = 0; i < array->size; i++)
                json_object_type_free(array->objects[i]);
            free(array->objects);
        }
        free(array);
    }
}

/* ObjectType alloc and free */

ObjectType json_object_type_null_alloc() {
    ObjectType new_object_type = (ObjectType)malloc(sizeof(_json_object_type));
    __ALLOC_FAILED_GUARD(new_object_type, __LINE__);

    new_object_type->type = JSON_NULL;
    new_object_type->object = NULL;
    return new_object_type;
}


ObjectType json_object_type_integer_alloc(uint64_t integer) {
    ObjectType new_object_type = (ObjectType)malloc(sizeof(_json_object_type));
    __ALLOC_FAILED_GUARD(new_object_type, __LINE__);

    new_object_type->type = JSON_INTEGER;
    new_object_type->integer = json_integer_alloc(integer);
    return new_object_type;
}

ObjectType json_object_type_decimal_alloc(double decimal) {
    ObjectType new_object_type = (ObjectType)malloc(sizeof(_json_object_type));
    __ALLOC_FAILED_GUARD(new_object_type, __LINE__);

    new_object_type->type = JSON_DECIMAL;
    new_object_type->decimal = json_decimal_alloc(decimal);
    return new_object_type;
}

ObjectType json_object_type_boolean_alloc(bool boolean) {
    ObjectType new_object_type = (ObjectType)malloc(sizeof(_json_object_type));
    __ALLOC_FAILED_GUARD(new_object_type, __LINE__);

    new_object_type->type = JSON_BOOLEAN;
    new_object_type->boolean = json_boolean_alloc(boolean);
    return new_object_type;
}

ObjectType json_object_type_string_alloc(c_str string, size_t size) {
    ObjectType new_object_type = (ObjectType)malloc(sizeof(_json_object_type));
    __ALLOC_FAILED_GUARD(new_object_type, __LINE__);

    new_object_type->type = JSON_STRING;
    new_object_type->string = json_string_alloc(string, size);
    return new_object_type;
}

ObjectType json_object_type_object_alloc(Object object) {
    ObjectType new_object_type = (ObjectType)malloc(sizeof(_json_object_type));
    __ALLOC_FAILED_GUARD(new_object_type, __LINE__);

    new_object_type->type = JSON_OBJECT;
    new_object_type->object = object;
    return new_object_type;
}

ObjectType json_object_type_empty_object_alloc() {
    ObjectType new_object_type = (ObjectType)malloc(sizeof(_json_object_type));
    __ALLOC_FAILED_GUARD(new_object_type, __LINE__);

    new_object_type->type = JSON_OBJECT;
    new_object_type->object = json_object_empty_alloc();
    ___ALLOC_FAILED_GUARD_FREE(new_object_type->object, new_object_type, __LINE__);

    return new_object_type;
}
ObjectType json_object_type_array_alloc(Array array) {
    ObjectType new_object_type = (ObjectType)malloc(sizeof(_json_object_type));
    __ALLOC_FAILED_GUARD(new_object_type, __LINE__);

    new_object_type->type = JSON_ARRAY;
    new_object_type->array = array;
    return new_object_type;
}

ObjectType* json_object_type_multi_alloc(size_t size) {
    ObjectType* new_multi_object = (ObjectType*)malloc(sizeof(_json_object_type*) * size);
    __ALLOC_FAILED_GUARD(new_multi_object, __LINE__);

    return new_multi_object;
}

void json_object_type_free(ObjectType object_type) {
    if (object_type) {
        switch (object_type->type) {
            case JSON_INTEGER: 
                json_integer_free(object_type->integer);
            break;

            case JSON_DECIMAL:
                json_decimal_free(object_type->decimal);
            break;

            case JSON_STRING:
                json_string_free(object_type->string);
            break;

            case JSON_BOOLEAN:
                json_boolean_free(object_type->boolean);
            break;

            case JSON_OBJECT:
                json_object_free(object_type->object);
            break;

            case JSON_ARRAY:
                json_array_free(object_type->array);
            break;

            case JSON_NULL:
            // object->(type) object value is null
            break;

            default:
                fprintf(stderr, "Memory free failed for object_type at %s:%d\n", __FILE__, __LINE__);
            break;
        }

        free(object_type);
    }
}



/* Print utils */

void _logf_indent(size_t depth) {
    while(depth-- > 0) _logger_logf(__JSON_TABULATION);
}

void _logf_line(const char* message) {
    _logger_logf("%s\n", message);
}

/* Print objects */

void json_integer_print(Integer integer) {
    _logger_logf(__JSON_INTEGER_PRINT_FMT, integer->value);
}

void json_decimal_print(Decimal decimal) {
    _logger_logf(__JSON_DOUBLE_PRINT_FMT, decimal->value);
}

void json_string_print(String string) {
    _logger_logf(__JSON_STRING_PRINT_FMT, string->value);
}

void json_boolean_print(Boolean boolean) {
    _logger_logf("%s", __JSON_BOOL_TO_STRING(boolean->value));
}

void _ident_json_array_print(size_t depth, Array array) {
    _logger_logf(__JSON_ARRAY_OPEN);
    if (array->size == 0) {
        _logger_logf(__JSON_ARRAY_CLOSE);
        return;
    }
    _logf_line("");

    for(size_t i = 0; i < array->size - 1; i++) {
        ObjectType ot = array->objects[i];

        _logf_indent(depth+1);
        _ident_json_object_type_print(depth+1, ot);
        if (array->size > 1)
            _logf_line(__JSON_KEY_VALUE_SEPARATOR);
    }

    ObjectType ot = array->objects[array->size-1];
    _logf_indent(depth+1);
    _ident_json_object_type_print(depth+1, ot);

    _logf_line("");
    _logf_indent(depth);
    _logger_logf(__JSON_ARRAY_CLOSE);
}

void json_array_print(Array array) {
    _ident_json_array_print(0, array);
    _logf_line("");
}

void _indent_json_object_print(size_t depth, Object object) {
    _logger_logf(__JSON_OBJECT_OPEN);
    if (object->keys == 0) {
        _logger_logf(__JSON_OBJECT_CLOSE);
        return;
    }
    _logf_line("");

    for(size_t i = 0; i < object->keys - 1; i++) {
        KeyValue kv = object->pairs[i];

        _logf_indent(depth+1);
        _logger_logf(__JSON_KEY_PRINT_FMT, kv->key);
        _logger_logf(__JSON_KEY_TO_VALUE);
        _ident_json_object_type_print(depth+1, kv->value);
        if (object->keys > 1)
            _logf_line(__JSON_KEY_VALUE_SEPARATOR);
    }

    KeyValue kv = object->pairs[object->keys-1];
    _logf_indent(depth+1);
    _logger_logf(__JSON_KEY_PRINT_FMT, kv->key);
    _logger_logf(__JSON_KEY_TO_VALUE);
    _ident_json_object_type_print(depth, kv->value);

    _logf_line("");
    _logf_indent(depth);
    _logger_logf(__JSON_OBJECT_CLOSE);
}

void json_object_print(Object object) {
    _indent_json_object_print(0, object);
    _logf_line("");
}


void _ident_json_object_type_print(size_t depth, ObjectType object_type) {

    switch(object_type->type) {

        case JSON_INTEGER:
            json_integer_print(object_type->integer);
        break;

        case JSON_DECIMAL:
            json_decimal_print(object_type->decimal);
        break;

        case JSON_BOOLEAN:
            json_boolean_print(object_type->boolean);
        break;

        case JSON_STRING:
            json_string_print(object_type->string);
        break;

        case JSON_NULL:
            _logger_logf(__JSON_NULL_PRINT);
        break;

        case JSON_ARRAY:
            _ident_json_array_print(depth, object_type->array);
        break;

        case JSON_OBJECT:
            _indent_json_object_print(depth, object_type->object);
        break;
    }
}

void json_object_type_print(ObjectType object_type) {
    _ident_json_object_type_print(0, object_type);
    _logf_line("");
}




#endif // JSON_C