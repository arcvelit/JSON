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

#define __JSON_MULTIOBJECT_INITIAL_CAP 2

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

typedef struct _json_key_value_pair _json_key_value_pair;
typedef struct _json_object_wrap    _json_object_wrap;
typedef struct _json_logger         _json_logger;

typedef _json_object    *_Object;
typedef _json_array     *_Array;
typedef _json_string    *_String;
typedef _json_integer   *_Integer;
typedef _json_decimal   *_Decimal;
typedef _json_boolean   *_Boolean;

typedef _json_logger    Logger;

typedef _json_key_value_pair *_KeyValue;

typedef char *c_str;
typedef _json_object_wrap   *JSON;

typedef enum {
    JSON_NULL,
    // Header types
    JSON_OBJECT, JSON_ARRAY,
    // Primitive types
    JSON_INTEGER, JSON_DECIMAL, JSON_STRING, JSON_BOOLEAN
} JSONType;

typedef enum {
    LOGGER_STDOUT,
    LOGGER_FILE
} _LoggerType;

struct _json_logger {
    _LoggerType type;
    FILE* stream;
};


/*  
    ================================
     Declarations    
    ================================
*/ 


// TODO: ADD ALL OF THEM

void logger_stdout_init(Logger* logger);
int logger_file_init(Logger* logger, const c_str filename);
void logger_file_close(Logger* logger);

void _logger_logf(Logger* logger, const c_str message, ...);
void _logf_indent(Logger* logger, size_t depth);

JSON*   _json_OBJ_multi_alloc(size_t size);
_KeyValue* _json_KV_multi_alloc(size_t size);
void json_free(JSON json_wrap);

void _indent_json_object_wrap_log(Logger* logger, size_t depth, JSON json_wrap);



/*  
    ================================
     Logging    
    ================================
*/ 

void logger_stdout_init(Logger* logger) {
    logger->type = LOGGER_STDOUT;
    logger->stream = stdout;
}

int logger_file_init(Logger* logger, const c_str filename) {
    logger->type = LOGGER_FILE;
    logger->stream = fopen(filename, "w");
    return logger->stream ? 1 : 0;
}

void logger_file_close(Logger* logger) {
    if (logger->type == LOGGER_FILE && logger->stream) {
        fclose(logger->stream);
        logger->stream = NULL;
    }
}

/*  
    ================================
     Structures    
    ================================
*/ 

struct _json_key_value_pair {
    c_str   key;
    JSON    value;
};

struct _json_object {       /* JSON_OBJECT  MULTIOBJECT */
    size_t      keys;
    _KeyValue*  pairs;
    size_t      _capacity;
};

struct _json_array {        /* JSON_ARRAY   MULTIOBJECT */
    size_t  size;
    JSON*   objects;
    size_t  _capacity;
};

struct _json_string {       /* JSON_STRING  PRIMITIVE   */
    c_str   value;
    size_t  size;
};

struct _json_integer {      /* JSON_INTEGER PRIMITIVE   */
    int64_t value;
};

struct _json_decimal {      /* JSON_DOUBLE  PRIMITIVE   */
    double  value;
};

struct _json_boolean {      /* JSON_BOOLEAN PRIMITIVE   */
    bool    value;
};

struct _json_object_wrap {
    JSONType type;
    union {
        _Array   array;
        _Object  object;
        _String  string;
        _Integer integer;
        _Decimal decimal;
        _Boolean boolean;
    };
};


/*  
    ================================
     Functions    
    ================================
*/ 

/* Memory allocation guards: truthy if condition is satisfied */

void* __ALLOC_FAILED_GUARD(void* ptr, size_t line) {
    if (!(ptr)) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, line);
        return NULL;
    }
    return ptr;
}

void* __ALLOC_FAILED_GUARD_FREE(void* ptr, void* cleanup, size_t line) {
    if (!(ptr)) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, line);
        free(cleanup);
        return NULL;
    }
    return ptr;
}

void* __ALLOC_FAILED_MULTIOBJECT_GUARD(void* ptr, size_t line) {
    if (!(ptr)) {
        fprintf(stderr, "Memory reallocation failed at %s:%d\n", __FILE__, line);
    }
    return ptr;
}

void* ___TYPE_GUARD(JSON ptr, JSONType type, size_t line) {
    if (!(ptr && ptr->type == type)) {
        fprintf(stderr, "Type guard failed at %s:%d\n", __FILE__, line);
        return NULL;
    }
    return ptr;
}


/* _String alloc and free */

_String _json_STR_alloc(c_str string, size_t size) {
    _String new_string = (_String)malloc(sizeof(_json_string));
    if(!__ALLOC_FAILED_GUARD(new_string, __LINE__)) return NULL;

    new_string->value = (c_str)malloc(size + 1);
    if(!__ALLOC_FAILED_GUARD_FREE(new_string->value, new_string, __LINE__)) return NULL;

    strncpy(new_string->value, string, size);
    new_string->value[size] = '\0';
    new_string->size = size;
    return new_string;
}

void _json_STR_free(_String json_string) {
    if (json_string) {
        __FREE_DEBUG_PRINT("string");
        free(json_string->value);
        free(json_string);
    }
}


/* _Integer alloc and free */

_Integer _json_INT_alloc(int64_t value) {
    _Integer new_integer = (_Integer)malloc(sizeof(_json_integer));
    if(!__ALLOC_FAILED_GUARD(new_integer, __LINE__)) return NULL;

    new_integer->value = value;
    return new_integer;
}

void _json_INT_free(_Integer json_integer) {
    if (json_integer) {
        __FREE_DEBUG_PRINT("integer");
        free(json_integer);
    }
}


/* _Boolean alloc and free */

_Boolean _json_BOOL_alloc(bool value) {
    _Boolean new_boolean = (_Boolean)malloc(sizeof(_json_boolean));
    if(!__ALLOC_FAILED_GUARD(new_boolean, __LINE__)) return NULL;

    new_boolean->value = value;
    return new_boolean;
}

void _json_BOOL_free(_Boolean json_boolean) {
    if (json_boolean) {
        __FREE_DEBUG_PRINT("boolean");
        free(json_boolean);
    }
}


/* _Decimal alloc and free */

_Decimal _json_DEC_alloc(double value) {
    _Decimal new_decimal = (_Decimal)malloc(sizeof(_json_decimal));
    if(!__ALLOC_FAILED_GUARD(new_decimal, __LINE__)) return NULL;

    new_decimal->value = value;
    return new_decimal;
}

void _json_DEC_free(_Decimal json_decimal) {
    if (json_decimal) {
        __FREE_DEBUG_PRINT("decimal");
        free(json_decimal);
    }
}


/* _KeyValue alloc and free */

_KeyValue _json_KV_alloc(c_str key, size_t key_len, JSON json_wrap) {
    _KeyValue new_key_value = (_KeyValue)malloc(sizeof(_json_key_value_pair));
    if(!__ALLOC_FAILED_GUARD(new_key_value, __LINE__)) return NULL;

    new_key_value->key = (c_str)malloc(key_len + 1);
    if(!__ALLOC_FAILED_GUARD_FREE(new_key_value->key, new_key_value, __LINE__)) return NULL;

    strncpy(new_key_value->key, key, key_len);
    new_key_value->value = json_wrap;
    return new_key_value;
}

void _json_KV_free(_KeyValue key_value) {
    if (key_value) {
        __FREE_DEBUG_PRINT("key value");
        if (key_value->key)   free(key_value->key);
        if (key_value->value) json_free(key_value->value);
        free(key_value);
    }
}


/* _Object alloc and free */

_Object _json_OBJ_alloc() {
    _Object new_object = (_Object)malloc(sizeof(_json_object));
    if(!__ALLOC_FAILED_GUARD(new_object, __LINE__)) return NULL;

    new_object->pairs = _json_KV_multi_alloc(__JSON_MULTIOBJECT_INITIAL_CAP);
    if(!__ALLOC_FAILED_GUARD_FREE(new_object->pairs, new_object, __LINE__)) return NULL;

    new_object->_capacity = __JSON_MULTIOBJECT_INITIAL_CAP;
    new_object->keys = 0;
    return new_object;
}

_KeyValue* _json_KV_multi_alloc(size_t size) {
    _KeyValue* new_multi_key_value = (_KeyValue*)malloc(sizeof(_KeyValue) * size);
    if(!__ALLOC_FAILED_GUARD(new_multi_key_value, __LINE__)) return NULL;

    return new_multi_key_value;
}

void _json_OBJ_free(_Object object) {
    if (object) {
        __FREE_DEBUG_PRINT("object");
        if (object->pairs) {
            for (size_t i = 0; i < object->keys; i++)
                _json_KV_free(object->pairs[i]);
            free(object->pairs);
        }
        free(object);
    }
}


/* _Array alloc and free */

_Array _json_ARR_alloc() {
    _Array new_array = (_Array)malloc(sizeof(_json_array));
    if(!__ALLOC_FAILED_GUARD(new_array, __LINE__)) return NULL;

    new_array->objects = _json_OBJ_multi_alloc(__JSON_MULTIOBJECT_INITIAL_CAP);
    if(!__ALLOC_FAILED_GUARD_FREE(new_array->objects, new_array, __LINE__)) return NULL;

    new_array->_capacity = __JSON_MULTIOBJECT_INITIAL_CAP;
    new_array->size = 0;
    return new_array;
}

JSON* _json_OBJ_multi_alloc(size_t size) {
    JSON* new_multi_object = (JSON*)malloc(sizeof(JSON) * size);
    if(!__ALLOC_FAILED_GUARD(new_multi_object, __LINE__)) return NULL;

    return new_multi_object;
}

void _json_ARR_free(_Array array) {
    if (array) {
        __FREE_DEBUG_PRINT("array");
        if (array->objects) {
            for (size_t i = 0; i < array->size; i++)
                json_free(array->objects[i]);
            free(array->objects);
        }
        free(array);
    }
}

/* JSON alloc and free */

JSON json_null_alloc() {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if(!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return  NULL;

    new_json_wrap->type = JSON_NULL;
    new_json_wrap->object = NULL;
    return new_json_wrap;
}

JSON json_integer_alloc(uint64_t integer) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if(!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_INTEGER;
    new_json_wrap->integer = _json_INT_alloc(integer);
    if(!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->integer, new_json_wrap, __LINE__)) return NULL;

    return new_json_wrap;
}

JSON json_decimal_alloc(double decimal) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if(!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_DECIMAL;
    new_json_wrap->decimal = _json_DEC_alloc(decimal);
    if(!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->decimal, new_json_wrap, __LINE__)) return NULL;

    return new_json_wrap;
}

JSON json_boolean_alloc(bool boolean) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if(!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_BOOLEAN;
    new_json_wrap->boolean = _json_BOOL_alloc(boolean);
    if(!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->boolean, new_json_wrap, __LINE__)) return NULL;

    return new_json_wrap;
}

JSON json_string_alloc(c_str string) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if(!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->string = _json_STR_alloc(string, strlen(string));
    if(!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->string, new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_STRING;
    return new_json_wrap;
}

JSON json_object_alloc() {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if(!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->object = _json_OBJ_alloc();
    if(!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->object, new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_OBJECT;
    return new_json_wrap;
}

JSON json_array_alloc() {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if(!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->array = _json_ARR_alloc();
    if(!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->array, new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_ARRAY;
    return new_json_wrap;
}

void json_free(JSON json_wrap) {
    if (json_wrap) {
        switch (json_wrap->type) {
            case JSON_INTEGER: 
                _json_INT_free(json_wrap->integer);
            break;

            case JSON_DECIMAL:
                _json_DEC_free(json_wrap->decimal);
            break;

            case JSON_STRING:
                _json_STR_free(json_wrap->string);
            break;

            case JSON_BOOLEAN:
                _json_BOOL_free(json_wrap->boolean);
            break;

            case JSON_OBJECT:
                _json_OBJ_free(json_wrap->object);
            break;

            case JSON_ARRAY:
                _json_ARR_free(json_wrap->array);
            break;

            case JSON_NULL:
                __FREE_DEBUG_PRINT("null");
            break;

            default:
                fprintf(stderr, "Memory free failed for json_wrap at %s:%d\n", __FILE__, __LINE__);
            break;
        }

        free(json_wrap);
    }
}


/*  
    ================================
     JSON API    
    ================================
*/ 

void json_add_key_value(JSON json_wrap, c_str key, JSON value) {
    if(!___TYPE_GUARD(json_wrap, JSON_OBJECT, __LINE__)) return;

    _Object ob = json_wrap->object;

    if (ob->keys == ob->_capacity) {
        size_t new_capacity = ob->_capacity * 2;
        _KeyValue* new_pairs = realloc(ob->pairs, new_capacity * sizeof(_KeyValue));
        if(!__ALLOC_FAILED_MULTIOBJECT_GUARD(new_pairs, __LINE__)) return;

        ob->_capacity = new_capacity;
        ob->pairs = new_pairs;
    }

    ob->pairs[ob->keys++] = _json_KV_alloc(key, strlen(key), value);    
}

void json_push(JSON json_wrap, JSON value) {
    if(!___TYPE_GUARD(json_wrap, JSON_ARRAY, __LINE__)) return;

    _Array ar = json_wrap->array;

    if (ar->size == ar->_capacity) {
        size_t new_capacity = ar->_capacity * 2;
        JSON* new_objects = realloc(ar->objects, new_capacity * sizeof(JSON));
        if(!__ALLOC_FAILED_MULTIOBJECT_GUARD(new_objects, __LINE__)) return;

        ar->_capacity = new_capacity;
        ar->objects = new_objects;
    }

    ar->objects[ar->size++] = value;
}

void json_foreach(JSON json_wrap, void (*func)(JSON)) {
    if(!___TYPE_GUARD(json_wrap, JSON_ARRAY, __LINE__)) return;
    
    va_list args;
    va_start(args, func);

    _Array ar = json_wrap->array;
    for(size_t i = 0; i < ar->size; i++)
        func(ar->objects[i]);

    va_end(args);
}


/*  
    ================================
     Logging cont'd   
    ================================
*/ 

void _logger_logf(Logger* logger, const c_str message, ...) {
    if (logger && logger->stream) {
        va_list args;
        va_start(args, message);
        vfprintf(logger->stream, message, args);
        va_end(args);
    }
}

void _logf_indent(Logger* logger, size_t depth) {
    while(depth-- > 0) _logger_logf(logger, __JSON_TABULATION);
}

void _logf_line(Logger* logger, const c_str message) {
    _logger_logf(logger, "%s\n", message);
}

/* Print objects */

void _json_INT_log(Logger* logger, _Integer integer) {
    _logger_logf(logger, __JSON_INTEGER_PRINT_FMT, integer->value);
}

void _json_DEC_log(Logger* logger, _Decimal decimal) {
    _logger_logf(logger, __JSON_DOUBLE_PRINT_FMT, decimal->value);
}

void _json_STR_log(Logger* logger, _String string) {
    _logger_logf(logger, __JSON_STRING_PRINT_FMT, string->value);
}

void _json_BOOL_log(Logger* logger, _Boolean boolean) {
    _logger_logf(logger, "%s", __JSON_BOOL_TO_STRING(boolean->value));
}

void _indent_json_ARR_log(Logger* logger, size_t depth, _Array array) {
    _logger_logf(logger, __JSON_ARRAY_OPEN);
    if (array->size == 0) {
        _logger_logf(logger, __JSON_ARRAY_CLOSE);
        return;
    }
    _logf_line(logger, "");

    for(size_t i = 0; i < array->size - 1; i++) {
        JSON ot = array->objects[i];

        _logf_indent(logger, depth+1);
        _indent_json_object_wrap_log(logger, depth+1, ot);
        if (array->size > 1)
            _logf_line(logger, __JSON_KEY_VALUE_SEPARATOR);
    }

    JSON ot = array->objects[array->size-1];
    _logf_indent(logger, depth+1);
    _indent_json_object_wrap_log(logger, depth+1, ot);

    _logf_line(logger, "");
    _logf_indent(logger, depth);
    _logger_logf(logger, __JSON_ARRAY_CLOSE);
}

void _indent_json_OBJ_log(Logger* logger, size_t depth, _Object object) {
    _logger_logf(logger, __JSON_OBJECT_OPEN);
    if (object->keys == 0) {
        _logger_logf(logger, __JSON_OBJECT_CLOSE);
        return;
    }
    _logf_line(logger, "");

    for(size_t i = 0; i < object->keys - 1; i++) {
        _KeyValue kv = object->pairs[i];

        _logf_indent(logger, depth+1);
        _logger_logf(logger, __JSON_KEY_PRINT_FMT, kv->key);
        _logger_logf(logger, __JSON_KEY_TO_VALUE);
        _indent_json_object_wrap_log(logger, depth+1, kv->value);
        if (object->keys > 1)
            _logf_line(logger, __JSON_KEY_VALUE_SEPARATOR);
    }

    _KeyValue kv = object->pairs[object->keys-1];
    _logf_indent(logger, depth+1);
    _logger_logf(logger, __JSON_KEY_PRINT_FMT, kv->key);
    _logger_logf(logger, __JSON_KEY_TO_VALUE);
    _indent_json_object_wrap_log(logger, depth+1, kv->value);

    _logf_line(logger, "");
    _logf_indent(logger, depth);
    _logger_logf(logger, __JSON_OBJECT_CLOSE);
}

void _indent_json_object_wrap_log(Logger* logger, size_t depth, JSON json_wrap) {

    switch(json_wrap->type) {

        case JSON_INTEGER:
            _json_INT_log(logger, json_wrap->integer);
        break;

        case JSON_DECIMAL:
            _json_DEC_log(logger, json_wrap->decimal);
        break;

        case JSON_BOOLEAN:
            _json_BOOL_log(logger, json_wrap->boolean);
        break;

        case JSON_STRING:
            _json_STR_log(logger, json_wrap->string);
        break;

        case JSON_NULL:
            _logger_logf(logger, __JSON_NULL_PRINT);
        break;

        case JSON_ARRAY:
            _indent_json_ARR_log(logger, depth, json_wrap->array);
        break;

        case JSON_OBJECT:
            _indent_json_OBJ_log(logger, depth, json_wrap->object);
        break;
    }
}

void json_log(Logger* logger, JSON json_wrap) {
    _indent_json_object_wrap_log(logger, 0, json_wrap);
    _logf_line(logger, "");
}




#endif // JSON_C