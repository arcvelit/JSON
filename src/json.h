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
typedef struct _json_writer         _json_writer;
typedef struct _json_reader         _json_reader;

typedef _json_integer   *_Integer;
typedef _json_decimal   *_Decimal;
typedef _json_boolean   *_Boolean;
typedef _json_string    *_String;
typedef _json_object    *_Object;
typedef _json_array     *_Array;

typedef _json_key_value_pair *_KeyValue;

typedef _json_object_wrap    *JSON;

typedef _json_writer    Writer;
typedef _json_reader    _Reader;

typedef char *c_str;


typedef enum {
    // Primitive types
    JSON_INTEGER, JSON_DECIMAL, JSON_STRING, JSON_BOOLEAN,
    // Header types
    JSON_OBJECT, JSON_ARRAY,
    // Null
    JSON_NULL,
} JSONType;

typedef enum {
    WRITER_STDOUT,
    WRITER_FILE
} _WriterType;

typedef enum {
    READER_FILE,
    READER_C_STR
} _ReaderType;

struct _json_writer {
    _WriterType type;
    FILE* stream;
};

struct _json_reader {
    _ReaderType type;
    FILE* stream;
};


/*  
    ================================
     Declarations    
    ================================
*/ 

// [\w\*]* [\w]*\(([\w\*]* [\w\*]*,?)*\) {

/* API Writing */
void writer_stdout_init(Writer* writer);
int  writer_file_init(Writer* writer, const c_str filename);
void writer_file_close(Writer* writer);

/* Internal type safety */
int __ALLOC_FAILED_GUARD(void* ptr, size_t line);
int __ALLOC_FAILED_GUARD_FREE(void* ptr, void* cleanup, size_t line);
int __ALLOC_FAILED_GUARD_MULTIOBJECT(void* ptr, size_t line);
int __TYPE_GUARD(JSON ptr, JSONType type, size_t line);

/* Internal struct allocators */
_Integer _json_INT_alloc(int64_t value);
_Decimal _json_DEC_alloc(double value);
_Boolean _json_BOOL_alloc(bool value);
_String  _json_STR_alloc(const c_str string, size_t size);
_Object  _json_OBJ_alloc();
_Array   _json_ARR_alloc();

_KeyValue   _json_KV_alloc(const c_str key, size_t key_len, JSON json_wrap);
_KeyValue*  _json_KV_multi_alloc(size_t size);
JSON*       _json_OBJ_multi_alloc(size_t size);

/* Internal struct deallocators */
void _json_INT_free(_Integer json_integer);
void _json_DEC_free(_Decimal json_decimal);
void _json_BOOL_free(_Boolean json_boolean);
void _json_STR_free(_String json_string);
void _json_OBJ_free(_Object object);
void _json_ARR_free(_Array array);
void _json_KV_free(_KeyValue key_value);

/* API Allocators */
JSON json_null_alloc();
JSON json_integer_alloc(int64_t integer);
JSON json_decimal_alloc(double decimal);
JSON json_boolean_alloc(bool boolean);
JSON json_string_alloc(const c_str string);
JSON json_object_alloc();
JSON json_array_alloc();

/* API Deallocators */
void json_free(JSON json_wrap);

/* API Utilities */
void json_add_key_value(JSON json_wrap, const c_str key, JSON value);
void json_push(JSON json_wrap, JSON value);
void json_foreach(JSON json_wrap, void (*func)(JSON));
JSON json_reduceint(JSON json_wrap, int64_t accumulator, int64_t (*func)(JSON, int64_t));
JSON json_reducedec(JSON json_wrap, double accumulator, double (*func)(JSON, double));
JSON json_reducebool(JSON json_wrap, bool accumulator, bool (*func)(JSON, bool));

/* Internal writing */
void _writer_writef(Writer* writer, const c_str message, ...);
void _writef_indent(Writer* writer, size_t depth);
void _writef_line(Writer* writer, const c_str message);

void _json_INT_write(Writer* writer, _Integer integer);
void _json_DEC_write(Writer* writer, _Decimal decimal);
void _json_BOOL_write(Writer* writer, _Boolean boolean);
void _json_STR_write(Writer* writer, _String string);

void _indent_json_OBJ_write(Writer* writer, size_t depth, _Object object);
void _indent_json_ARR_write(Writer* writer, size_t depth, _Array array);
void _indent_json_object_wrap_write(Writer* writer, size_t depth, JSON json_wrap);

/* API Writing*/
void json_write(Writer* writer, JSON json_wrap);

/* Internal reassignment */
void _json_INT_reset(_Integer integer, int64_t value);
void _json_DEC_reset(_Decimal decimal, double value);
void _json_BOOL_reset(_Boolean boolean, bool value);

/* API Reassignment */
void json_integer_reset(JSON json_wrap, int64_t value);
void json_decimal_reset(JSON json_wrap, double value);
void json_boolean_reset(JSON json_wrap, bool value);
void json_string_reset(JSON json_wrap, const c_str value);


/*  
    ================================
     Writing    
    ================================
*/ 

void writer_stdout_init(Writer* writer) {
    writer->type = WRITER_STDOUT;
    writer->stream = stdout;
}

int writer_file_init(Writer* writer, const c_str filename) {
    writer->type = WRITER_FILE;
    writer->stream = fopen(filename, "w");
    return writer->stream ? 1 : 0;
}

void writer_file_close(Writer* writer) {
    if (writer->type == WRITER_FILE && writer->stream) {
        fclose(writer->stream);
        writer->stream = NULL;
    }
}

/*  
    ================================
     Structures    
    ================================
*/ 

struct _json_key_value_pair {
    c_str key;
    JSON value;
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

int __ALLOC_FAILED_GUARD(void* ptr, size_t line) {
    if (!(ptr)) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, line);
        return 0;
    }
    return 1;
}

int __ALLOC_FAILED_GUARD_FREE(void* ptr, void* cleanup, size_t line) {
    if (!ptr) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, line);
        free(cleanup);
        return 0;
    }
    return 1;
}

int __ALLOC_FAILED_GUARD_MULTIOBJECT(void* ptr, size_t line) {
    if (!ptr) {
        fprintf(stderr, "Memory reallocation failed at %s:%d\n", __FILE__, line);
        return 0;
    }
    return 1;
}

int __TYPE_GUARD(JSON ptr, JSONType type, size_t line) {
    if (!(ptr && ptr->type == type)) {
        fprintf(stderr, "Type guard failed at %s:%d\n", __FILE__, line);
        return 0;
    }
    return 1;
}


/* _String alloc and free */

_String _json_STR_alloc(const c_str string, size_t size) {
    _String new_string = (_String)malloc(sizeof(_json_string));
    if (!__ALLOC_FAILED_GUARD(new_string, __LINE__)) return NULL;

    new_string->value = (const c_str)malloc(size + 1);
    if (!__ALLOC_FAILED_GUARD_FREE(new_string->value, new_string, __LINE__)) return NULL;

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
    if (!__ALLOC_FAILED_GUARD(new_integer, __LINE__)) return NULL;

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
    if (!__ALLOC_FAILED_GUARD(new_boolean, __LINE__)) return NULL;

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
    if (!__ALLOC_FAILED_GUARD(new_decimal, __LINE__)) return NULL;

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

_KeyValue _json_KV_alloc(const c_str key, size_t key_len, JSON json_wrap) {
    _KeyValue new_key_value = (_KeyValue)malloc(sizeof(_json_key_value_pair));
    if (!__ALLOC_FAILED_GUARD(new_key_value, __LINE__)) return NULL;

    new_key_value->key = (c_str)malloc(key_len + 1);
    if (!__ALLOC_FAILED_GUARD_FREE(new_key_value->key, new_key_value, __LINE__)) return NULL;

    strncpy(new_key_value->key, key, key_len);
    new_key_value->key[key_len] = '\0';
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
    if (!__ALLOC_FAILED_GUARD(new_object, __LINE__)) return NULL;

    new_object->pairs = _json_KV_multi_alloc(__JSON_MULTIOBJECT_INITIAL_CAP);
    if (!__ALLOC_FAILED_GUARD_FREE(new_object->pairs, new_object, __LINE__)) return NULL;

    new_object->_capacity = __JSON_MULTIOBJECT_INITIAL_CAP;
    new_object->keys = 0;
    return new_object;
}

_KeyValue* _json_KV_multi_alloc(size_t size) {
    _KeyValue* new_multi_key_value = (_KeyValue*)malloc(sizeof(_KeyValue) * size);
    if (!__ALLOC_FAILED_GUARD(new_multi_key_value, __LINE__)) return NULL;

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
    if (!__ALLOC_FAILED_GUARD(new_array, __LINE__)) return NULL;

    new_array->objects = _json_OBJ_multi_alloc(__JSON_MULTIOBJECT_INITIAL_CAP);
    if (!__ALLOC_FAILED_GUARD_FREE(new_array->objects, new_array, __LINE__)) return NULL;

    new_array->_capacity = __JSON_MULTIOBJECT_INITIAL_CAP;
    new_array->size = 0;
    return new_array;
}

JSON* _json_OBJ_multi_alloc(size_t size) {
    JSON* new_multi_object = (JSON*)malloc(sizeof(JSON) * size);
    if (!__ALLOC_FAILED_GUARD(new_multi_object, __LINE__)) return NULL;

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
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return  NULL;

    new_json_wrap->type = JSON_NULL;
    new_json_wrap->object = NULL;
    return new_json_wrap;
}

JSON json_integer_alloc(int64_t integer) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_INTEGER;
    new_json_wrap->integer = _json_INT_alloc(integer);
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->integer, new_json_wrap, __LINE__)) return NULL;

    return new_json_wrap;
}

JSON json_decimal_alloc(double decimal) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_DECIMAL;
    new_json_wrap->decimal = _json_DEC_alloc(decimal);
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->decimal, new_json_wrap, __LINE__)) return NULL;

    return new_json_wrap;
}

JSON json_boolean_alloc(bool boolean) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_BOOLEAN;
    new_json_wrap->boolean = _json_BOOL_alloc(boolean);
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->boolean, new_json_wrap, __LINE__)) return NULL;

    return new_json_wrap;
}

JSON json_string_alloc(const c_str string) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->string = _json_STR_alloc(string, strlen(string));
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->string, new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_STRING;
    return new_json_wrap;
}

JSON json_object_alloc() {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->object = _json_OBJ_alloc();
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->object, new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_OBJECT;
    return new_json_wrap;
}

JSON json_array_alloc() {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->array = _json_ARR_alloc();
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->array, new_json_wrap, __LINE__)) return NULL;

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

            case JSON_BOOLEAN:
                _json_BOOL_free(json_wrap->boolean);
            break;

            case JSON_STRING:
                _json_STR_free(json_wrap->string);
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
     Dynamic multiobjects    
    ================================
*/ 

void json_add_key_value(JSON json_wrap, const c_str key, JSON value) {
    if (!__TYPE_GUARD(json_wrap, JSON_OBJECT, __LINE__)) return;

    _Object ob = json_wrap->object;

    if (ob->keys == ob->_capacity) {
        size_t new_capacity = ob->_capacity * 2;
        _KeyValue* new_pairs = realloc(ob->pairs, new_capacity * sizeof(_KeyValue));
        if (!__ALLOC_FAILED_GUARD_MULTIOBJECT(new_pairs, __LINE__)) return;

        ob->_capacity = new_capacity;
        ob->pairs = new_pairs;
    }

    ob->pairs[ob->keys++] = _json_KV_alloc(key, strlen(key), value);    
}

void json_push(JSON json_wrap, JSON value) {
    if (!__TYPE_GUARD(json_wrap, JSON_ARRAY, __LINE__)) return;

    _Array ar = json_wrap->array;

    if (ar->size == ar->_capacity) {
        size_t new_capacity = ar->_capacity * 2;
        JSON* new_objects = realloc(ar->objects, new_capacity * sizeof(JSON));
        if (!__ALLOC_FAILED_GUARD_MULTIOBJECT(new_objects, __LINE__)) return;

        ar->_capacity = new_capacity;
        ar->objects = new_objects;
    }

    ar->objects[ar->size++] = value;
}

void json_foreach(JSON json_wrap, void (*func)(JSON)) {
    if (!__TYPE_GUARD(json_wrap, JSON_ARRAY, __LINE__)) return;
    
    _Array ar = json_wrap->array;
    for (size_t i = 0; i < ar->size; i++)
        func(ar->objects[i]);
}

JSON json_reduceint(JSON json_wrap, int64_t accumulator, int64_t (*func)(JSON, int64_t)) {
    if (!__TYPE_GUARD(json_wrap, JSON_ARRAY, __LINE__)) exit(EXIT_FAILURE);
    
    _Array ar = json_wrap->array;
    for (size_t i = 0; i < ar->size; i++) {
        if (!__TYPE_GUARD(ar->objects[i], JSON_INTEGER, __LINE__)) exit(EXIT_FAILURE);
        accumulator = func(ar->objects[i], accumulator);
    }

    return json_integer_alloc(accumulator);
}

JSON json_reducedec(JSON json_wrap, double accumulator, double (*func)(JSON, double)) {
    if (!__TYPE_GUARD(json_wrap, JSON_ARRAY, __LINE__)) exit(EXIT_FAILURE);
    
    _Array ar = json_wrap->array;
    for (size_t i = 0; i < ar->size; i++) {
        if (!__TYPE_GUARD(ar->objects[i], JSON_DECIMAL, __LINE__)) exit(EXIT_FAILURE);
        accumulator = func(ar->objects[i], accumulator);
    }

    return json_decimal_alloc(accumulator);
}

JSON json_reducebool(JSON json_wrap, bool accumulator, bool (*func)(JSON, bool)) {
    if (!__TYPE_GUARD(json_wrap, JSON_ARRAY, __LINE__)) exit(EXIT_FAILURE);
    
    _Array ar = json_wrap->array;
    for (size_t i = 0; i < ar->size; i++) {
        if (!__TYPE_GUARD(ar->objects[i], JSON_BOOLEAN, __LINE__)) exit(EXIT_FAILURE);
        accumulator = func(ar->objects[i], accumulator);
    }

    return json_boolean_alloc(accumulator);
}


/*  
    ================================
     Utilities   
    ================================
*/ 

void _json_INT_reset(_Integer integer, int64_t value) {
    integer->value = value;
}

void _json_DEC_reset(_Decimal decimal, double value) {
    decimal->value = value;
}

void _json_BOOL_reset(_Boolean boolean, bool value) {
    boolean->value = value;
}

void json_integer_reset(JSON json_wrap, int64_t value) {
    if (!__TYPE_GUARD(json_wrap, JSON_INTEGER, __LINE__)) exit(EXIT_FAILURE);

    _json_INT_reset(json_wrap->integer, value);
}

void json_decimal_reset(JSON json_wrap, double value) {
    if (!__TYPE_GUARD(json_wrap, JSON_DECIMAL, __LINE__)) exit(EXIT_FAILURE);

    _json_DEC_reset(json_wrap->decimal, value);
}

void json_boolean_reset(JSON json_wrap, bool value) {
    if (!__TYPE_GUARD(json_wrap, JSON_BOOLEAN, __LINE__)) exit(EXIT_FAILURE);

    _json_BOOL_reset(json_wrap->boolean, value);
}

void json_string_reset(JSON json_wrap, const c_str value) {
    if (!__TYPE_GUARD(json_wrap, JSON_STRING, __LINE__)) exit(EXIT_FAILURE);

    _json_STR_free(json_wrap->string);
    json_wrap->string = _json_STR_alloc(value, strlen(value));
}


/*  
    ================================
     Writing cont'd   
    ================================
*/ 

void _writer_writef(Writer* writer, const c_str message, ...) {
    if (writer && writer->stream) {
        va_list args;
        va_start(args, message);
        vfprintf(writer->stream, message, args);
        va_end(args);
    }
}

void _writef_indent(Writer* writer, size_t depth) {
    while(depth-- > 0) _writer_writef(writer, __JSON_TABULATION);
}

void _writef_line(Writer* writer, const c_str message) {
    _writer_writef(writer, "%s\n", message);
}

/* Print objects */

void _json_INT_write(Writer* writer, _Integer integer) {
    _writer_writef(writer, __JSON_INTEGER_PRINT_FMT, integer->value);
}

void _json_DEC_write(Writer* writer, _Decimal decimal) {
    _writer_writef(writer, __JSON_DOUBLE_PRINT_FMT, decimal->value);
}

void _json_STR_write(Writer* writer, _String string) {
    _writer_writef(writer, __JSON_STRING_PRINT_FMT, string->value);
}

void _json_BOOL_write(Writer* writer, _Boolean boolean) {
    _writer_writef(writer, "%s", __JSON_BOOL_TO_STRING(boolean->value));
}

void _indent_json_ARR_write(Writer* writer, size_t depth, _Array array) {
    _writer_writef(writer, __JSON_ARRAY_OPEN);
    if (array->size == 0) {
        _writer_writef(writer, __JSON_ARRAY_CLOSE);
        return;
    }
    _writef_line(writer, "");

    for (size_t i = 0; i < array->size - 1; i++) {
        JSON ot = array->objects[i];

        _writef_indent(writer, depth+1);
        _indent_json_object_wrap_write(writer, depth+1, ot);
        if (array->size > 1)
            _writef_line(writer, __JSON_KEY_VALUE_SEPARATOR);
    }

    JSON ot = array->objects[array->size-1];
    _writef_indent(writer, depth+1);
    _indent_json_object_wrap_write(writer, depth+1, ot);

    _writef_line(writer, "");
    _writef_indent(writer, depth);
    _writer_writef(writer, __JSON_ARRAY_CLOSE);
}

void _indent_json_OBJ_write(Writer* writer, size_t depth, _Object object) {
    _writer_writef(writer, __JSON_OBJECT_OPEN);
    if (object->keys == 0) {
        _writer_writef(writer, __JSON_OBJECT_CLOSE);
        return;
    }
    _writef_line(writer, "");

    for (size_t i = 0; i < object->keys - 1; i++) {
        _KeyValue kv = object->pairs[i];

        _writef_indent(writer, depth+1);
        _writer_writef(writer, __JSON_KEY_PRINT_FMT, kv->key);
        _writer_writef(writer, __JSON_KEY_TO_VALUE);
        _indent_json_object_wrap_write(writer, depth+1, kv->value);
        if (object->keys > 1)
            _writef_line(writer, __JSON_KEY_VALUE_SEPARATOR);
    }

    _KeyValue kv = object->pairs[object->keys-1];
    _writef_indent(writer, depth+1);
    _writer_writef(writer, __JSON_KEY_PRINT_FMT, kv->key);
    _writer_writef(writer, __JSON_KEY_TO_VALUE);
    _indent_json_object_wrap_write(writer, depth+1, kv->value);

    _writef_line(writer, "");
    _writef_indent(writer, depth);
    _writer_writef(writer, __JSON_OBJECT_CLOSE);
}

void _indent_json_object_wrap_write(Writer* writer, size_t depth, JSON json_wrap) {

    switch(json_wrap->type) {

        case JSON_INTEGER:
            _json_INT_write(writer, json_wrap->integer);
        break;

        case JSON_DECIMAL:
            _json_DEC_write(writer, json_wrap->decimal);
        break;

        case JSON_BOOLEAN:
            _json_BOOL_write(writer, json_wrap->boolean);
        break;

        case JSON_STRING:
            _json_STR_write(writer, json_wrap->string);
        break;

        case JSON_NULL:
            _writer_writef(writer, __JSON_NULL_PRINT);
        break;

        case JSON_ARRAY:
            _indent_json_ARR_write(writer, depth, json_wrap->array);
        break;

        case JSON_OBJECT:
            _indent_json_OBJ_write(writer, depth, json_wrap->object);
        break;
    }
}

void json_write(Writer* writer, JSON json_wrap) {
    _indent_json_object_wrap_write(writer, 0, json_wrap);
    _writef_line(writer, "");
}


/*  
    ================================
     Reading cont'd   
    ================================
*/ 






#endif // JSON_C