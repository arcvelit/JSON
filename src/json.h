#ifndef JSON_C
#define JSON_C

#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/*  
    ================================
     Preprocessor defines    
    ================================
*/ 

#define __JSON_OBJECT_OPEN               "{"
#define __JSON_OBJECT_CLOSE              "}"
#define __JSON_ARRAY_OPEN                "["
#define __JSON_ARRAY_CLOSE               "]"
#define __JSON_KEY_TO_VALUE             ": "
#define __JSON_KEY_VALUE_SEPARATOR       ","
#define __JSON_ARRAY_ELEMENT_SEPARATOR   ","

#define __JSON_TABULATION "    "

#define __JSON_KEY_PRINT_FMT            "\"%s\""
#define __JSON_NUMBER_PRINT_FMT             "%g"
#define __JSON_STRING_PRINT_FMT         "\"%s\""
#define __JSON_BOOLEAN_TRUE_PRINT_FMT     "true" 
#define __JSON_BOOLEAN_FALSE_PRINT_FMT   "false" 
#define __JSON_NULL_PRINT_FMT             "null"

#define JSON_HAS_NO_FRACTIONAL_PART(number) ((number) == (int)(number))

#define JSON_BOOL_TO_STRING(b) ((b) ? __JSON_BOOLEAN_TRUE_PRINT_FMT : __JSON_BOOLEAN_FALSE_PRINT_FMT)

#ifdef __JSON_FREE_DEBUG
    #define __FREE_DEBUG_PRINT(MESSAGE) printf("\nDEBUG: Freeing %s\n", MESSAGE)
#else
    #define __FREE_DEBUG_PRINT(MESSAGE)
#endif

#ifdef __JSON_LEXER_DEBUG
    #define __LEXER_DEBUG_PRINT(TYPE) printf("DEBUG: Lexed %s\n", _get_json_token_name(TYPE));
#else 
    #define __LEXER_DEBUG_PRINT(TYPE)
#endif

#ifdef __JSON_ARENA_DEBUG
    #define __ARENA_DEBUG_PRINT() printf("DEBUG: Arena grew (size: %zu)\n", _json_global_arena.regions);
#else 
    #define __ARENA_DEBUG_PRINT()
#endif

#define JSON_IS_WHITESPACE(c)  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\f' || (c) == '\v')
#define JSON_IS_NUMBER_CHAR(c) ((c) == '+' || (c) ==  '-' || (c) ==  'E' || (c) ==  'e' || (c) ==  '.' || ((c) >=  '0' && (c) <=  '9'))
#define JSON_IS_ALPHANUM(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z') || ((c) >= '0' && (c) <= '9'))

#define JSON_BUFFER_INITIAL_CAP         64
#define JSON_STR_INITIAL_CAP             8
#define JSON_MULTIOBJECT_INITIAL_CAP     4

#define JSON_MEM_ASSERT(PTR)                           \
    do {                                               \
        if (!(PTR)) {                                  \
            fprintf(stderr, "FATAL: Out of memory\n"); \
            exit(EXIT_FAILURE);                        \
        }                                              \
    } while (0)

/*  
    ================================
     String view    
    ================================
*/ 

typedef struct {
    const char* data;
    size_t size;
} _string_view;

/*  
    ================================
     Json types    
    ================================
*/ 

typedef struct _json_array   _json_array;
typedef struct _json_object  _json_object;
typedef struct _json_string  _json_string;
typedef struct _json_number  _json_number;
typedef struct _json_boolean _json_boolean;

typedef struct _json_key_value_pair _json_key_value_pair;
typedef struct _json_object_wrap    _json_object_wrap;
typedef struct _json_writer         _json_writer;
typedef struct _json_reader         _json_reader;

typedef _json_number    *_Number;
typedef _json_boolean   *_Boolean;
typedef _json_string    *_String;
typedef _json_object    *_Object;
typedef _json_array     *_Array;

typedef _json_key_value_pair *_KeyValue;

typedef _json_object_wrap    *JSON;

typedef _json_writer    Writer;

/*  
    ================================
     Structures    
    ================================
*/ 

typedef enum {
    JSON_NUMBER, 
    JSON_STRING, 
    JSON_BOOLEAN,
    JSON_OBJECT, 
    JSON_ARRAY,
    JSON_NULL,
} JsonType;

struct _json_key_value_pair {
    char* key;
    JSON value;
};

struct _json_object {
    size_t      keys;
    _KeyValue*  pairs;
    size_t      _capacity;
};

struct _json_array {
    size_t  size;
    JSON*   objects;
    size_t  _capacity;
};

struct _json_string {
    char*   value;
};

struct _json_number {
    double  value;
};

struct _json_boolean {
    bool    value;
};

struct _json_object_wrap {
    JsonType type;
    union {
        _Array   array;
        _Object  object;
        _String  string;
        _Number  number;
        _Boolean boolean;
    };
};

//   Keep both aligned
// ---------------------
const char* _json_token_names[] = {
    "number",
    "string",
    "boolean",
    "null",
    "'{'",
    "'}'",
    "'['",
    "']'",
    "','",
    "':'",
    "error",
    "eof"
};

typedef enum {
    JSON_TOKEN_NUMBER    =    1,
    JSON_TOKEN_STRING    =    2,
    JSON_TOKEN_BOOLEAN   =    4,
    JSON_TOKEN_NULL      =    8,
    JSON_TOKEN_LBRACE    =   16,
    JSON_TOKEN_RBRACE    =   32,
    JSON_TOKEN_LBRACKET  =   64,
    JSON_TOKEN_RBRACKET  =  128,
    JSON_TOKEN_COMMA     =  256,
    JSON_TOKEN_COLON     =  512,
    JSON_TOKEN_ERROR     = 1024,
    JSON_TOKEN_EOF       = 2048,
} _json_token_type;
// ---------------------

typedef struct _json_ast_token _json_ast_token;

struct _json_ast_token {
    _json_token_type type;
    union {
        _string_view as_str;
        double as_num;
        bool as_bool;
    };
    size_t row;
    size_t col;
    _json_ast_token* next;
    _json_ast_token* child;
};

/*  
    ================================
     Arena    
    ================================
*/

#define JSON_ARENA_REGION_CAP 256

typedef struct  _json_ast_arena_reg _json_ast_arena_reg;

struct _json_ast_arena_reg {
    _json_ast_token buf[JSON_ARENA_REGION_CAP];
    size_t size;
    _json_ast_arena_reg* next;
};

typedef struct {
    _json_ast_arena_reg* first;
    _json_ast_arena_reg* end;
    size_t regions;
} _json_ast_arena;

/*  
    ================================
     Lexing    
    ================================
*/

// Black magic to compare case insensitive buffers
#define JSON_LEX_STRNCMPBUF(LEXER,  BUF, BUFLEN) \
    (((LEXER)->cursor + (BUFLEN) <= (LEXER)->tape_size) && \
    (strncasecmp(&(LEXER)->source[(LEXER)->cursor], (BUF), (BUFLEN)) == 0) && \
    (!JSON_IS_ALPHANUM((LEXER)->source[(lexer)->cursor+(BUFLEN)])))

typedef struct {
    const char* source;
    size_t tape_size;
    size_t cursor;
    size_t line_count;
    size_t line_begin;
} _json_lexer;


/*  
    ================================
     Parsing    
    ================================
*/

/** 
 * -- Simplified context free grammar --
 * 
 * JSON -> nulllit | boollit | numlit | strlit
 * JSON -> lsqbr ARRAYBODY rsqbr | lbrace OBJECTBODY rbrace
 *  
 * ARRAYBODY -> JSON ARRAYTAIL | EPSILON
 * ARRAYTAIL -> comma ARRAYBODY | EPSILON
 * 
 * OBJECTBODY -> strlit colon JSON OBJECTTAIL | EPSILON
 * OBJECTTAIL -> comma OBJECTBODY | EPSILON
 * 
 */

const _json_token_type JSON_FIRST_SET_JSON       = JSON_TOKEN_STRING | JSON_TOKEN_NUMBER | JSON_TOKEN_BOOLEAN | JSON_TOKEN_NULL | JSON_TOKEN_LBRACE | JSON_TOKEN_LBRACKET;

const _json_token_type JSON_FOLLOW_SET_ARRAYBODY  = JSON_TOKEN_RBRACKET;
const _json_token_type JSON_FOLLOW_SET_ARRAYTAIL  = JSON_TOKEN_RBRACKET;
const _json_token_type JSON_FOLLOW_SET_OBJECTBODY = JSON_TOKEN_RBRACE;
const _json_token_type JSON_FOLLOW_SET_OBJECTTAIL = JSON_TOKEN_RBRACE;

char json_parse_error[128] = {0};

#define JSON_GET_PARSE_ERROR() (json_parse_error[0] ? json_parse_error : "unexpected error")

#define JSON_LOG_PARSE_ERROR(MESSAGE, TOKEN)           \
    do {                                               \
        if (!json_parse_error[0]) {                    \
            snprintf(json_parse_error, sizeof(json_parse_error), "%s at %zu:%zu", (MESSAGE), (size_t)(TOKEN).row, (size_t)(TOKEN).col); \
        }                                              \
    } while (0)

#define JSON_LOG_PARSE_ERROR_MATCH(TYPE, TOKEN)        \
    do {                                               \
        if (!json_parse_error[0]) {                    \
            snprintf(json_parse_error, sizeof(json_parse_error), "expected %s but got %s at %zu:%zu", _get_json_token_name(TYPE), _get_json_token_name((TOKEN).type), (size_t)(TOKEN).row, (size_t)(TOKEN).col); \
        }                                              \
    } while (0)


/*  
    ================================
     Writer    
    ================================
*/ 

typedef enum {
    WRITER_STDOUT,
    WRITER_FILE,
} _WriterType;

struct _json_writer {
    _WriterType type;
    FILE* stream;
};


// ====================================================

/* Writing */
void writer_stdout_init(Writer* writer);
int  writer_file_init(Writer* writer, const char* filename);
void writer_file_close(Writer* writer);
void writer_writef(Writer* writer, const char* message, ...);

/* Type guard */
int JSON_TYPE_GUARD(JSON ptr, JsonType type);

/* Allocators */
JSON json_null_alloc();
JSON json_number_alloc(double number);
JSON json_boolean_alloc(bool boolean);
JSON json_string_alloc(const char* string);
JSON json_string_from_sv_alloc(const _string_view sv);
JSON json_object_alloc();
JSON json_array_alloc();

/* Deallocators */
void json_free(JSON json_wrap);

/* Utilities */
bool json_add_key_sv_value(JSON json_obj, const _string_view sv, JSON value);
bool json_add_key_value(JSON json_wrap, const char* key, JSON value);
bool json_push(JSON json_wrap, JSON value);
void json_foreach(JSON json_wrap, void (*func)(JSON));
double json_reducenum(JSON json_wrap, double accumulator, double (*func)(JSON, double));
bool json_reducebool(JSON json_wrap, bool accumulator, bool (*func)(JSON, bool));

/* Writing*/
void json_write(Writer* writer, JSON json_wrap);

/* Reassignment */
void json_number_reset(JSON json_wrap, double value);
void json_boolean_reset(JSON json_wrap, bool value);
void json_string_reset(JSON json_wrap, const char* value);

/* Copying */
JSON json_copy(JSON json_wrap);

/* Data */
JSON* json_get(JSON json_wrap, const char* key);
bool  json_in(JSON json_array, JSON json_wrap);
void  json_reassign(JSON* json_wrap_ptr, JSON new_wrap);

/* Predicates */
bool json_isnull(JSON json_wrap);
bool json_isnum(JSON json_wrap);
bool json_isint(JSON json_wrap);
bool json_isdec(JSON json_wrap);
bool json_isstr(JSON json_wrap);
bool json_isobj(JSON json_wrap);
bool json_isarr(JSON json_wrap);

bool json_is(JSON json_wrap, JSON other_wrap);
bool json_eq(JSON json_wrap, JSON other_wrap);

/* Parsing */
JSON json_parse_string(const char* _cstr, size_t len);
JSON json_parse_file(const char* filename);

#endif // JSON_C

#ifdef JSON_IMPLEMENTATION

/*  
    ================================
     Writing    
    ================================
*/ 

void writer_stdout_init(Writer* writer) {
    writer->type = WRITER_STDOUT;
    writer->stream = stdout;
}

int writer_file_init(Writer* writer, const char* filename) {
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

void writer_writef(Writer* writer, const char* message, ...) {
    if (writer && writer->stream) {
        va_list args;
        va_start(args, message);
        vfprintf(writer->stream, message, args);
        va_end(args);
    }
}

/*  
    ================================
     Functions    
    ================================
*/ 

_KeyValue* _json_internal_kv_multi_alloc(size_t size);
JSON* _json_internal_object_multi_alloc(size_t size);

/* Guards: truthy if assert failed */

int JSON_TYPE_GUARD(JSON ptr, JsonType type) {
    if (ptr && ptr->type == type) return 0;
    
    fprintf(stderr, "WARNING: Type assert raised\n");
    return 1;
}

/* _String alloc and free */

_String _json_internal_string_from_sv_alloc(const _string_view sv) {
    _String new_string = (_String)malloc(sizeof(_json_string));
    JSON_MEM_ASSERT(new_string);
    
    char* sv_dup = malloc((sv.size + 1) * sizeof(char));
    JSON_MEM_ASSERT(sv_dup);

    memcpy(sv_dup, sv.data, sv.size);
    sv_dup[sv.size] = '\0';

    new_string->value = sv_dup;
    return new_string;
}

_String _json_internal_string_alloc(const char* string) {
    _String new_string = (_String)malloc(sizeof(_json_string));
    JSON_MEM_ASSERT(new_string);
    
    new_string->value = strdup(string);
    JSON_MEM_ASSERT(new_string->value);

    return new_string;
}

void _json_internal_string_free(_String json_string) {
    if (json_string) {
        __FREE_DEBUG_PRINT("string");
        if (json_string->value) free(json_string->value);
        free(json_string);
    }
}

/* _Boolean alloc and free */

_Boolean _json_internal_boolean_alloc(bool value) {
    _Boolean new_boolean = (_Boolean)malloc(sizeof(_json_boolean));
    JSON_MEM_ASSERT(new_boolean);

    new_boolean->value = value;
    return new_boolean;
}

void _json_internal_boolean_free(_Boolean json_boolean) {
    if (json_boolean) {
        __FREE_DEBUG_PRINT("boolean");
        free(json_boolean);
    }
}


/* _Number alloc and free */

_Number _json_internal_number_alloc(double value) {
    _Number new_number = (_Number)malloc(sizeof(_json_number));
    JSON_MEM_ASSERT(new_number);

    new_number->value = value;
    return new_number;
}

void _json_internal_number_free(_Number json_number) {
    if (json_number) {
        __FREE_DEBUG_PRINT("number");
        free(json_number);
    }
}

/* _KeyValue alloc and free */

_KeyValue _json_internal_kv_sv_alloc(const _string_view sv, JSON json_wrap) {
    _KeyValue new_key_value = (_KeyValue)malloc(sizeof(_json_key_value_pair));
    JSON_MEM_ASSERT(new_key_value);

    char* sv_dup = malloc((sv.size + 1) * sizeof(char));
    JSON_MEM_ASSERT(sv_dup);

    memcpy(sv_dup, sv.data, sv.size);
    sv_dup[sv.size] = '\0';

    new_key_value->key = sv_dup;

    new_key_value->value = json_wrap;
    return new_key_value;
}

_KeyValue _json_internal_kv_alloc(const char* key, JSON json_wrap) {
    _KeyValue new_key_value = (_KeyValue)malloc(sizeof(_json_key_value_pair));
    JSON_MEM_ASSERT(new_key_value);

    new_key_value->key = strdup(key);
    JSON_MEM_ASSERT(new_key_value->key);

    new_key_value->value = json_wrap;
    return new_key_value;
}

void _json_internal_kv_free(_KeyValue key_value) {
    if (key_value) {
        __FREE_DEBUG_PRINT("key value");
        if (key_value->key)   free(key_value->key);
        if (key_value->value) json_free(key_value->value);
        free(key_value);
    }
}

/* _Object alloc and free */

_Object _json_internal_object_alloc() {
    _Object new_object = (_Object)malloc(sizeof(_json_object));
    JSON_MEM_ASSERT(new_object);

    new_object->pairs = _json_internal_kv_multi_alloc(JSON_MULTIOBJECT_INITIAL_CAP);
    JSON_MEM_ASSERT(new_object->pairs);

    new_object->_capacity = JSON_MULTIOBJECT_INITIAL_CAP;
    new_object->keys = 0;
    return new_object;
}

_KeyValue* _json_internal_kv_multi_alloc(size_t size) {
    _KeyValue* new_multi_key_value = (_KeyValue*)malloc(sizeof(_KeyValue) * size);
    JSON_MEM_ASSERT(new_multi_key_value);

    return new_multi_key_value;
}

void _json_internal_object_free(_Object object) {
    if (object) {
        __FREE_DEBUG_PRINT("object");
        if (object->pairs) {
            for (size_t i = 0; i < object->keys; i++)
                _json_internal_kv_free(object->pairs[i]);
            free(object->pairs);
        }
        free(object);
    }
}

/* _Array alloc and free */

_Array _json_internal_array_alloc() {
    _Array new_array = (_Array)malloc(sizeof(_json_array));
    JSON_MEM_ASSERT(new_array);

    new_array->objects = _json_internal_object_multi_alloc(JSON_MULTIOBJECT_INITIAL_CAP);
    JSON_MEM_ASSERT(new_array->objects);

    new_array->_capacity = JSON_MULTIOBJECT_INITIAL_CAP;
    new_array->size = 0;
    return new_array;
}

JSON* _json_internal_object_multi_alloc(size_t size) {
    JSON* new_multi_object = (JSON*)malloc(sizeof(JSON) * size);
    JSON_MEM_ASSERT(new_multi_object);

    return new_multi_object;
}

void _json_internal_array_free(_Array array) {
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
    JSON_MEM_ASSERT(new_json_wrap);

    new_json_wrap->type = JSON_NULL;
    new_json_wrap->object = NULL;
    return new_json_wrap;
}

JSON json_number_alloc(double number) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    JSON_MEM_ASSERT(new_json_wrap);

    new_json_wrap->type = JSON_NUMBER;
    new_json_wrap->number = _json_internal_number_alloc(number);
    JSON_MEM_ASSERT(new_json_wrap->number);

    return new_json_wrap;
}

JSON json_boolean_alloc(bool boolean) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    JSON_MEM_ASSERT(new_json_wrap);

    new_json_wrap->type = JSON_BOOLEAN;
    new_json_wrap->boolean = _json_internal_boolean_alloc(boolean);
    JSON_MEM_ASSERT(new_json_wrap->boolean);

    return new_json_wrap;
}

JSON json_string_alloc(const char* string) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    JSON_MEM_ASSERT(new_json_wrap);

    new_json_wrap->string = _json_internal_string_alloc(string);
    JSON_MEM_ASSERT(new_json_wrap->string);

    new_json_wrap->type = JSON_STRING;
    return new_json_wrap;
}

JSON json_string_from_sv_alloc(const _string_view sv) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    JSON_MEM_ASSERT(new_json_wrap);

    new_json_wrap->string = _json_internal_string_from_sv_alloc(sv);
    JSON_MEM_ASSERT(new_json_wrap->string);

    new_json_wrap->type = JSON_STRING;
    return new_json_wrap;
}

JSON json_object_alloc() {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    JSON_MEM_ASSERT(new_json_wrap);

    new_json_wrap->object = _json_internal_object_alloc();
    JSON_MEM_ASSERT(new_json_wrap->object);

    new_json_wrap->type = JSON_OBJECT;
    return new_json_wrap;
}

JSON json_array_alloc() {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    JSON_MEM_ASSERT(new_json_wrap);

    new_json_wrap->array = _json_internal_array_alloc();
    JSON_MEM_ASSERT(new_json_wrap->array);

    new_json_wrap->type = JSON_ARRAY;
    return new_json_wrap;
}

void json_free(JSON json_wrap) {
    if (json_wrap) {
        switch (json_wrap->type) {
            case JSON_NUMBER:  _json_internal_number_free(json_wrap->number); break;
            case JSON_BOOLEAN: _json_internal_boolean_free(json_wrap->boolean); break;
            case JSON_STRING:  _json_internal_string_free(json_wrap->string); break;
            case JSON_OBJECT:  _json_internal_object_free(json_wrap->object); break;
            case JSON_ARRAY:   _json_internal_array_free(json_wrap->array); break;
            case JSON_NULL:    __FREE_DEBUG_PRINT("null"); break;
            default: {
                fprintf(stderr, "WARNING: Free error\n");
                return;
            }
        }
        free(json_wrap);
    }
}

_Object _json_internal_object_copy(_Object object) {

    _Object new_object = _json_internal_object_alloc();

    new_object->keys = object->keys;
    new_object->_capacity = new_object->_capacity;
    new_object->pairs = _json_internal_kv_multi_alloc(new_object->_capacity);

    for (size_t i = 0; i < new_object->keys; i++) {
        const char* old_key = object->pairs[i]->key;
        JSON new_value = json_copy(object->pairs[i]->value);
        new_object->pairs[i] = _json_internal_kv_alloc(old_key, new_value);
    }

    return new_object;
}

_Array _json_internal_array_copy(_Array array) {

    _Array new_array = _json_internal_array_alloc();

    new_array->size = array->size;
    new_array->_capacity = array->_capacity;
    new_array->objects = _json_internal_object_multi_alloc(new_array->_capacity);

    for (size_t i = 0; i < new_array->size; i++) {
        new_array->objects[i] = json_copy(array->objects[i]);
    }

    return new_array;
}

JSON json_copy(JSON json_wrap) {
    if (!json_wrap) return NULL;

    switch (json_wrap->type) {
        case JSON_NUMBER:  return json_number_alloc(json_wrap->number->value);
        case JSON_BOOLEAN: return json_boolean_alloc(json_wrap->boolean->value);
        case JSON_STRING:  return json_string_alloc(json_wrap->string->value);
        case JSON_NULL:    return json_null_alloc();
        case JSON_OBJECT: {
            JSON json_ret = json_object_alloc();
            json_ret->object = _json_internal_object_copy(json_wrap->object);
            return json_ret;
        }
        case JSON_ARRAY: {
            JSON json_ret = json_array_alloc();
            json_ret->array = _json_internal_array_copy(json_wrap->array);
            return json_ret;
        }
        default: {
            fprintf(stderr, "WARNING: Copy error\n");
            return NULL;
        }
    }
}

/*  
    ================================
     Dynamic multiobjects    
    ================================
*/ 

bool json_add_key_sv_value(JSON json_obj, const _string_view sv, JSON value) {
    if (JSON_TYPE_GUARD(json_obj, JSON_OBJECT)) return false;

    _Object ob = json_obj->object;

    for (size_t i = 0; i < ob->keys; i++) {
        if (strncmp(ob->pairs[i]->key, sv.data, sv.size) == 0) {
            return false;
        }
    }

    if (ob->keys == ob->_capacity) {
        size_t new_capacity = ob->_capacity * 2;
        _KeyValue* new_pairs = realloc(ob->pairs, new_capacity * sizeof(_KeyValue));
        JSON_MEM_ASSERT(new_pairs);

        ob->_capacity = new_capacity;
        ob->pairs = new_pairs;
    }

    ob->pairs[ob->keys++] = _json_internal_kv_sv_alloc(sv, value);
    return true;    
}

bool json_add_key_value(JSON json_obj, const char* key, JSON value) {
    if (JSON_TYPE_GUARD(json_obj, JSON_OBJECT)) return false;

    _Object ob = json_obj->object;

    for (size_t i = 0; i < ob->keys; i++) {
        if (strcmp(ob->pairs[i]->key, key) == 0) {
            return false;
        }
    }

    if (ob->keys == ob->_capacity) {
        size_t new_capacity = ob->_capacity * 2;
        _KeyValue* new_pairs = realloc(ob->pairs, new_capacity * sizeof(_KeyValue));
        JSON_MEM_ASSERT(new_pairs);

        ob->_capacity = new_capacity;
        ob->pairs = new_pairs;
    }

    ob->pairs[ob->keys++] = _json_internal_kv_alloc(key, value);
    return true;    
}

JSON* json_get(JSON json_obj, const char* key) {
    if (JSON_TYPE_GUARD(json_obj, JSON_OBJECT)) return NULL;

    for (size_t i = 0; i < json_obj->object->keys; i++)
        if (strcmp(key, json_obj->object->pairs[i]->key) == 0)
            return &json_obj->object->pairs[i]->value;

    return NULL;
}

bool json_in(JSON json_array, JSON json_wrap) {
    if (JSON_TYPE_GUARD(json_array, JSON_ARRAY)) return NULL;

    for (size_t i = 0; i < json_array->array->size; i++) {
        if (json_eq(json_array->array->objects[i], json_wrap))
            return true;
    }

    return false;
}

void json_reassign(JSON* json_wrap_ptr, JSON new_wrap) {
    if (json_wrap_ptr && new_wrap) {
        json_free(*json_wrap_ptr);
        *json_wrap_ptr = new_wrap;
    }
}

bool json_push(JSON json_array, JSON value) {
    if (JSON_TYPE_GUARD(json_array, JSON_ARRAY)) return false;

    _Array ar = json_array->array;

    if (ar->size == ar->_capacity) {
        size_t new_capacity = ar->_capacity * 2;
        JSON* new_objects = realloc(ar->objects, new_capacity * sizeof(JSON));
        JSON_MEM_ASSERT(new_objects);

        ar->_capacity = new_capacity;
        ar->objects = new_objects;
    }

    ar->objects[ar->size++] = value;
    return true;
}

void json_foreach(JSON json_array, void (*func)(JSON)) {
    if (JSON_TYPE_GUARD(json_array, JSON_ARRAY)) return;
    
    _Array ar = json_array->array;
    for (size_t i = 0; i < ar->size; i++)
        func(ar->objects[i]);
}

double json_reducenum(JSON json_array, double accumulator, double (*func)(JSON, double)) {
    if (JSON_TYPE_GUARD(json_array, JSON_ARRAY)) exit(EXIT_FAILURE);
    
    _Array ar = json_array->array;
    for (size_t i = 0; i < ar->size; i++) {
        if (JSON_TYPE_GUARD(ar->objects[i], JSON_NUMBER)) exit(EXIT_FAILURE);
        accumulator = func(ar->objects[i], accumulator);
    }

    return accumulator;
}

bool json_reducebool(JSON json_array, bool accumulator, bool (*func)(JSON, bool)) {
    if (JSON_TYPE_GUARD(json_array, JSON_ARRAY)) exit(EXIT_FAILURE);
    
    _Array ar = json_array->array;
    for (size_t i = 0; i < ar->size; i++) {
        if (JSON_TYPE_GUARD(ar->objects[i], JSON_BOOLEAN)) exit(EXIT_FAILURE);
        accumulator = func(ar->objects[i], accumulator);
    }

    return accumulator;
}

/*  
    ================================
     Predicates   
    ================================
*/ 

bool json_isnull(JSON json_wrap) {
    return json_wrap && 
    json_wrap->object && 
    json_wrap->type == JSON_NULL;
}

bool json_isnum(JSON json_wrap) {
    return json_wrap && 
    json_wrap->number &&
    json_wrap->type == JSON_NUMBER;
}

bool json_isint(JSON json_wrap) {
    return json_isnum(json_wrap) &&
    JSON_HAS_NO_FRACTIONAL_PART(json_wrap->number->value);
}

bool json_isdec(JSON json_wrap) {
    return json_isnum(json_wrap) && 
    !JSON_HAS_NO_FRACTIONAL_PART(json_wrap->number->value);
}

bool json_isstr(JSON json_wrap) {
    return json_wrap &&
    json_wrap->string && 
    json_wrap->type == JSON_STRING;
}

bool json_isobj(JSON json_wrap) {
    return json_wrap &&
    json_wrap->object && 
    json_wrap->type == JSON_OBJECT;
}

bool json_isarr(JSON json_wrap) {
    return json_wrap && 
    json_wrap->array && 
    json_wrap->type == JSON_ARRAY;
}

bool json_is(JSON json_wrap, JSON other_wrap) {
    return json_wrap && 
    json_wrap == other_wrap && 
    json_wrap->type == other_wrap->type; 
}

bool json_eq(JSON json_wrap, JSON other_wrap) {

    if (json_wrap && other_wrap && json_wrap->type == other_wrap->type) {
        switch (json_wrap->type) {
            case JSON_NUMBER: return json_wrap->number->value == other_wrap->number->value;
            case JSON_BOOLEAN: return json_wrap->boolean->value == other_wrap->boolean->value;
            case JSON_STRING : return strcmp(json_wrap->string->value, other_wrap->string->value) == 0;
            default: return false;
        }
    }
    return false;
}

/*  
    ================================
     Utilities   
    ================================
*/ 

void _json_internal_number_reset(_Number number, double value) {
    number->value = value;
}

void _json_internal_boolean_reset(_Boolean boolean, bool value) {
    boolean->value = value;
}

void json_number_reset(JSON json_num, double value) {
    if (JSON_TYPE_GUARD(json_num, JSON_NUMBER)) return;

    _json_internal_number_reset(json_num->number, value);
}

void json_boolean_reset(JSON json_bool, bool value) {
    if (JSON_TYPE_GUARD(json_bool, JSON_BOOLEAN)) return;

    _json_internal_boolean_reset(json_bool->boolean, value);
}

void json_string_reset(JSON json_str, const char* value) {
    if (JSON_TYPE_GUARD(json_str, JSON_STRING)) return;

    _json_internal_string_free(json_str->string);
    json_str->string = _json_internal_string_alloc(value);
}

/*  
    ================================
     Writing cont'd   
    ================================
*/ 

void _indent_json_object_wrap_write(Writer* writer, size_t depth, JSON json_wrap);

void _writef_indent(Writer* writer, size_t depth) {
    while (depth-- > 0) writer_writef(writer, __JSON_TABULATION);
}

void _writef_line(Writer* writer, const char* message) {
    writer_writef(writer, "%s\n", message);
}

void _json_internal_number_write(Writer* writer, _Number number) {
    writer_writef(writer, __JSON_NUMBER_PRINT_FMT, number->value);
}

void _json_internal_string_write(Writer* writer, _String string) {
    writer_writef(writer, __JSON_STRING_PRINT_FMT, string->value);
}

void _json_internal_boolean_write(Writer* writer, _Boolean boolean) {
    writer_writef(writer, "%s", JSON_BOOL_TO_STRING(boolean->value));
}

void _indent_json_internal_array_write(Writer* writer, size_t depth, _Array array) {
    writer_writef(writer, __JSON_ARRAY_OPEN);
    if (array->size == 0) {
        writer_writef(writer, __JSON_ARRAY_CLOSE);
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
    writer_writef(writer, __JSON_ARRAY_CLOSE);
}

void _indent_json_internal_object_write(Writer* writer, size_t depth, _Object object) {
    writer_writef(writer, __JSON_OBJECT_OPEN);
    if (object->keys == 0) {
        writer_writef(writer, __JSON_OBJECT_CLOSE);
        return;
    }
    _writef_line(writer, "");

    for (size_t i = 0; i < object->keys - 1; i++) {
        _KeyValue kv = object->pairs[i];

        _writef_indent(writer, depth+1);

        writer_writef(writer, __JSON_KEY_PRINT_FMT, kv->key);
        writer_writef(writer, __JSON_KEY_TO_VALUE);
        _indent_json_object_wrap_write(writer, depth+1, kv->value);
        if (object->keys > 1)
            _writef_line(writer, __JSON_KEY_VALUE_SEPARATOR);
    }

    _KeyValue kv = object->pairs[object->keys-1];
    _writef_indent(writer, depth+1);
    writer_writef(writer, __JSON_KEY_PRINT_FMT, kv->key);
    writer_writef(writer, __JSON_KEY_TO_VALUE);
    _indent_json_object_wrap_write(writer, depth+1, kv->value);

    _writef_line(writer, "");
    _writef_indent(writer, depth);
    writer_writef(writer, __JSON_OBJECT_CLOSE);
}

void _indent_json_object_wrap_write(Writer* writer, size_t depth, JSON json_wrap) {

    switch(json_wrap->type) {

        case JSON_NUMBER:
            _json_internal_number_write(writer, json_wrap->number);
        break;

        case JSON_BOOLEAN:
            _json_internal_boolean_write(writer, json_wrap->boolean);
        break;

        case JSON_STRING:
            _json_internal_string_write(writer, json_wrap->string);
        break;

        case JSON_NULL:
            writer_writef(writer, __JSON_NULL_PRINT_FMT);
        break;

        case JSON_ARRAY:
            _indent_json_internal_array_write(writer, depth, json_wrap->array);
        break;

        case JSON_OBJECT:
            _indent_json_internal_object_write(writer, depth, json_wrap->object);
        break;
    }
}

void json_write(Writer* writer, JSON json_wrap) {
    _indent_json_object_wrap_write(writer, 0, json_wrap);
    _writef_line(writer, "");
}

/*  
    ================================
     Lexing utils  
    ================================
*/

const char* _get_json_token_name(int value) {
    // unsafe as heck but debug only
    const char** name = _json_token_names;
    while (value > 1) {
        value >>= 1;
        name++;
    }
    return *name;
}

char* _read_file_to_cstr(const char* filename, size_t* len) {

    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "ERROR: Unable to read `%s`\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    rewind(file);

    char *file_buf = malloc(length);
    JSON_MEM_ASSERT(file_buf);

    size_t bytesRead = fread(file_buf, sizeof(char), length, file);
    if (bytesRead != length) {
        fprintf(stderr, "ERROR: File read mismatch `%s` (%zu/%zu bytes)\n", filename, bytesRead, length);
        free(file_buf);
        fclose(file);
        return NULL;
    }

    *len = length;
    fclose(file);
    return file_buf;
}

/*  
    ================================
     Arena    
    ================================
*/

_json_ast_arena _json_global_arena = {0};

void _json_global_arena_ensure_init() {
    if (!_json_global_arena.first) {
        _json_ast_arena_reg* region = malloc(sizeof(_json_ast_arena_reg));
        JSON_MEM_ASSERT(region);
        
        region->size = 0;
        region->next = NULL;
        _json_global_arena.first = region;
        _json_global_arena.end = _json_global_arena.first;
        _json_global_arena.regions = 1;
    }    
}

void _json_global_arena_grow() {
    if (_json_global_arena.end->next) {
        _json_global_arena.end = _json_global_arena.end->next;
        _json_global_arena.end->size = 0;
        return;
    }
    _json_ast_arena_reg* new_reg = malloc(sizeof(_json_ast_arena_reg));
    JSON_MEM_ASSERT(new_reg);

    new_reg->size = 0;
    new_reg->next = NULL;
    _json_global_arena.end->next = new_reg;
    _json_global_arena.end = _json_global_arena.end->next;
    _json_global_arena.regions++;
    __ARENA_DEBUG_PRINT();
}

void _json_global_arena_reset() {
    _json_global_arena.end = _json_global_arena.first;
    _json_global_arena.end->size = 0;
}

_json_ast_token* _json_global_arena_alloc() {
    if (_json_global_arena.end->size == JSON_ARENA_REGION_CAP) {
        _json_global_arena_grow();
    }
    _json_ast_token* slot = &_json_global_arena.end->buf[_json_global_arena.end->size];
    memset(slot, 0, sizeof(_json_ast_token));
    ++_json_global_arena.end->size;
    return slot;
}

/*  
    ================================
     Lexing    
    ================================
*/

void _json_lexer_init(_json_lexer* lexer, const char* buf, size_t len) {
    lexer->line_count = 1;
    lexer->tape_size = len;
    lexer->source = buf;
}
void _json_update_lex_state(_json_lexer* lexer, _json_ast_token* token, size_t lookat, size_t begin) {
    lexer->cursor = lookat;
    token->col = 1 + begin - lexer->line_begin;
    token->row = lexer->line_count;
}

bool _lex_punct(_json_lexer* lexer, _json_ast_token* token) {
    switch(lexer->source[lexer->cursor]) {
        case '[': token->type = JSON_TOKEN_LBRACKET; break;
        case ']': token->type = JSON_TOKEN_RBRACKET; break;
        case '{': token->type = JSON_TOKEN_LBRACE; break;
        case '}': token->type = JSON_TOKEN_RBRACE; break;
        case ':': token->type = JSON_TOKEN_COLON; break;
        case ',': token->type = JSON_TOKEN_COMMA; break;
        default: goto NOTFOUND;
    }

    _json_update_lex_state(lexer, token, lexer->cursor + 1, lexer->cursor);
    return true;

NOTFOUND:
    return false;
}

bool _lex_bool_lit(_json_lexer* lexer, _json_ast_token* token) {
    size_t lookat = lexer->cursor;
    const char truelit[] = "true";
    const int truelen = sizeof(truelit) - 1;
    const char falselit[] = "false";
    const int falselen = sizeof(falselit) - 1;
    if (JSON_LEX_STRNCMPBUF(lexer, truelit, truelen) || JSON_LEX_STRNCMPBUF(lexer, falselit, falselen)) {
        token->as_bool = lexer->source[lexer->cursor] == 't' || lexer->source[lexer->cursor] == 'T';
        lookat += (token->as_bool) ? truelen : falselen;
        _json_update_lex_state(lexer, token, lookat, lexer->cursor);
        token->type = JSON_TOKEN_BOOLEAN;
        return true;
    }
    return false;
}

bool _lex_null_lit(_json_lexer* lexer, _json_ast_token* token) {
    size_t lookat = lexer->cursor;
    const char nullit[] = "null";
    const int nullen = sizeof(nullit) - 1;
    if (JSON_LEX_STRNCMPBUF(lexer, nullit, nullen)) {
        lookat += nullen;
        _json_update_lex_state(lexer, token, lookat, lexer->cursor);
        token->type = JSON_TOKEN_NULL;
        return true;
    }
    return false;
}

bool _lex_num_lit(_json_lexer* lexer, _json_ast_token* token) {
    size_t lookat = lexer->cursor;

    if (!JSON_IS_NUMBER_CHAR(lexer->source[lookat])) {
        return false;
    }

    size_t begin = lexer->cursor;
    do {
        ++lookat;
    }
    while (JSON_IS_NUMBER_CHAR(lexer->source[lookat]));

    // Place in buffer to avoid copying string for \0
    char numlit[64];
    const size_t dsize = begin - lookat;
    const size_t length = (dsize < sizeof(numlit)) ? dsize : sizeof(numlit);
    memcpy(numlit, &lexer->source[begin], length);
    numlit[length] = '\0';

    // Parse double from buffer
    char *end;
    errno = 0;
    double as_num = strtod(numlit, &end);
    
    _json_update_lex_state(lexer, token, lookat, begin);    
    if (errno != 0 || numlit == end) {
        JSON_LOG_PARSE_ERROR("incorrect numeric format", *token);
        return false;
    }
    
    token->type = JSON_TOKEN_NUMBER;
    token->as_num = as_num;
    return true;
}

bool _lex_str_lit(_json_lexer* lexer, _json_ast_token* token) {
    size_t lookat = lexer->cursor;

    if (lexer->source[lookat] != '\"') {
        return false;
    }

    size_t begin = lookat++;
    bool escaped = false;
    while (lookat < lexer->tape_size) {
        if (lexer->source[lookat] == '\n') {
            break;
        } else if (escaped) {
            escaped = false;
        } else if (lexer->source[lookat] == '\\') {
            escaped = true;
        } else if (lexer->source[lookat] == '\"') {
            break;
        }
        ++lookat;
    }
    
    size_t end = lookat++;

    _json_update_lex_state(lexer, token, lookat, begin);
    if (lexer->source[end] == '\n' || end >= lexer->tape_size) {
        token->type = JSON_TOKEN_ERROR;
        JSON_LOG_PARSE_ERROR("unterminated string", *token);
        return false;
    }

    token->type = JSON_TOKEN_STRING;
    token->as_str.data = &lexer->source[begin+1];
    token->as_str.size = end - begin - 1;
    return true;
}

_json_ast_token* _lex_next_token(_json_lexer* lexer) {
    
    _json_ast_token* token = _json_global_arena_alloc();

    // Trim
    while (lexer->cursor < lexer->tape_size) {
        if (JSON_IS_WHITESPACE(lexer->source[lexer->cursor])) {
            if (lexer->source[lexer->cursor] == '\n') {
                lexer->line_count++;
                lexer->line_begin = lexer->cursor + 1;
            }
            lexer->cursor++;
            continue;
        }
        break;
    }

    // EOF
    if (lexer->cursor >= lexer->tape_size) {
        token->type = JSON_TOKEN_EOF;
        goto RETURN;
    }

    // Lexer looking at a valid character
    if (_lex_str_lit(lexer, token)) {
    } else if (_lex_num_lit(lexer, token)) {
    } else if (_lex_null_lit(lexer, token)) {
    } else if (_lex_bool_lit(lexer, token)) {
    } else if (_lex_punct(lexer, token)) {
    } else {
        _json_update_lex_state(lexer, token, lexer->cursor, lexer->cursor);
        JSON_LOG_PARSE_ERROR("unexpected token", *token);
        token->type = JSON_TOKEN_ERROR;
    }

RETURN:
    __LEXER_DEBUG_PRINT(token->type);
    return token;
}

/*  
    ================================
     Parsing    
    ================================
*/

bool _json_parse_json(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** parent);
bool _json_parse_array_body(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** parent);
bool _json_parse_array_tail(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** sibling);
bool _json_parse_object_body(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** parent);
bool _json_parse_object_tail(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** sibling);

bool _json_parse_match(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_token_type type) {
    if ((*lookahead_ptr)->type == type) {
        if ((*lookahead_ptr)->type != JSON_TOKEN_EOF)
            *lookahead_ptr = _lex_next_token(lexer);
        return true;
    }
    JSON_LOG_PARSE_ERROR_MATCH(type, **lookahead_ptr);
    return false;
}

bool _json_parse_match_set(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** parent, _json_token_type type) {
    if ((*lookahead_ptr)->type == type) {
        *parent = *lookahead_ptr;
        *lookahead_ptr = _lex_next_token(lexer);
        return true;
    }
    JSON_LOG_PARSE_ERROR_MATCH(type, **lookahead_ptr);
    return false;
}

bool _json_parse_json(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** parent) {
    _json_ast_token* constructed = NULL;
    _json_ast_token* child = NULL;
    
    const _json_ast_token* const lookahead = *lookahead_ptr;
    if (lookahead->type & JSON_TOKEN_NUMBER) {
        if (_json_parse_match_set(lexer, lookahead_ptr, &constructed, JSON_TOKEN_NUMBER)) {
            *parent = constructed;
            return true;
        }
    } else if (lookahead->type & JSON_TOKEN_NULL) {
        if (_json_parse_match_set(lexer, lookahead_ptr, &constructed, JSON_TOKEN_NULL)) {
            *parent = constructed;
            return true;
        }
    } else if (lookahead->type & JSON_TOKEN_BOOLEAN) {
        if (_json_parse_match_set(lexer, lookahead_ptr, &constructed, JSON_TOKEN_BOOLEAN)) {
            *parent = constructed;
            return true;
        }
    } else if (lookahead->type & JSON_TOKEN_STRING) {
        if (_json_parse_match_set(lexer, lookahead_ptr, &constructed, JSON_TOKEN_STRING)) {
            *parent = constructed;
            return true;
        }
    } else if (lookahead->type & JSON_TOKEN_LBRACKET) {
        if (
            _json_parse_match_set(lexer, lookahead_ptr, &constructed, JSON_TOKEN_LBRACKET) && 
            _json_parse_array_body(lexer, lookahead_ptr, &child) &&
            _json_parse_match(lexer, lookahead_ptr, JSON_TOKEN_RBRACKET)
        ) {
            constructed->child = child;
            *parent = constructed;
            return true;
        }
    } else if (lookahead->type & JSON_TOKEN_LBRACE) {
        if (
            _json_parse_match_set(lexer, lookahead_ptr, &constructed, JSON_TOKEN_LBRACE) && 
            _json_parse_object_body(lexer, lookahead_ptr, &child) && 
            _json_parse_match(lexer, lookahead_ptr, JSON_TOKEN_RBRACE)
        ) {
            constructed->child = child;
            *parent = constructed;
            return true;
        }
    }
    JSON_LOG_PARSE_ERROR("expected a json", **lookahead_ptr);
    return false;
}

bool _json_parse_array_body(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** parent) {
    _json_ast_token* sibling = NULL;    
    const _json_ast_token* const  lookahead = *lookahead_ptr;
    if (lookahead->type & JSON_FIRST_SET_JSON) {
        if (
            _json_parse_json(lexer, lookahead_ptr, parent) && 
            _json_parse_array_tail(lexer, lookahead_ptr, &sibling)
        ) { // JSON ARRAYTAIL
            (*parent)->next = sibling;
            return true;
        }
    } else if (lookahead->type & JSON_FOLLOW_SET_ARRAYBODY) { // epsilon
        return true;
    }
    JSON_LOG_PARSE_ERROR("bad array format", **lookahead_ptr);
    return false;
}

bool _json_parse_array_tail(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** sibling) {
    const _json_ast_token* const lookahead = *lookahead_ptr;
    if (lookahead->type & JSON_TOKEN_COMMA) {
        if (
            _json_parse_match(lexer, lookahead_ptr, JSON_TOKEN_COMMA) && 
            _json_parse_array_body(lexer, lookahead_ptr, sibling)
        ) { // comma ARRAYBODY
            return true;
        }
    } else if (lookahead->type & JSON_FOLLOW_SET_ARRAYTAIL) { // epsilon
        return true;
    }
    JSON_LOG_PARSE_ERROR("unterminated array", **lookahead_ptr);
    return false;
}

bool _json_parse_object_body(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** parent) {
    _json_ast_token* sibling1 = NULL;
    _json_ast_token* sibling2 = NULL;
    _json_ast_token* lookahead = *lookahead_ptr;
    if (lookahead->type & JSON_TOKEN_STRING) {
        if (
            _json_parse_match_set(lexer, lookahead_ptr, parent, JSON_TOKEN_STRING) &&
            _json_parse_match(lexer, lookahead_ptr, JSON_TOKEN_COLON) &&
            _json_parse_json(lexer, lookahead_ptr, &sibling1) &&
            _json_parse_object_tail(lexer, lookahead_ptr, &sibling2)
        ) { // strlit colon JSON OBJECTTAIL
            sibling1->next = sibling2;
            (*parent)->next = sibling1;
            return true;
        }
    } else if (lookahead->type & JSON_FOLLOW_SET_OBJECTBODY) { // epsilon
        return true;
    }
    JSON_LOG_PARSE_ERROR("bad object format", **lookahead_ptr);
    return false;
}

bool _json_parse_object_tail(_json_lexer* lexer, _json_ast_token** lookahead_ptr, _json_ast_token** parent) {
    _json_ast_token* lookahead = *lookahead_ptr;
    if (lookahead->type & JSON_TOKEN_COMMA) {
        if (
            _json_parse_match(lexer, lookahead_ptr, JSON_TOKEN_COMMA) && 
            _json_parse_object_body(lexer, lookahead_ptr, parent)
        ) { // comma OBJECTBODY
            return true;
        }
    } else if (lookahead->type & JSON_FOLLOW_SET_OBJECTTAIL) { // epsilon
        return true;
    }
    JSON_LOG_PARSE_ERROR("unterminated object", **lookahead_ptr);
    return false;
}

JSON _json_build_from_ast(const _json_ast_token* const base) {
    switch (base->type) {
        case JSON_TOKEN_NUMBER: {
            return json_number_alloc(base->as_num);
        }
        case JSON_TOKEN_NULL: {
            return json_null_alloc();
        }
        case JSON_TOKEN_BOOLEAN: {
            return json_boolean_alloc(base->as_bool);
        }
        case JSON_TOKEN_STRING: {
            return json_string_from_sv_alloc(base->as_str);
        }
        case JSON_TOKEN_LBRACKET: {
            JSON array = json_array_alloc();
            const _json_ast_token* ptr = base->child;
            while (ptr) {
                json_push(array, _json_build_from_ast(ptr));
                ptr = ptr->next;
            }
            return array;
        }
        case JSON_TOKEN_LBRACE: {
            JSON object = json_object_alloc();
            const _json_ast_token* ptr = base->child;
            while (ptr) {
                json_add_key_sv_value(object, ptr->as_str, _json_build_from_ast(ptr->next));
                ptr = ptr->next->next;
            }
            return object;
        }
        default: { // Unreachable
            fprintf(stderr, "FATAL: invalid AST after parsing (library error)\n");
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}

JSON json_parse_string(const char* buf, size_t len) {
    _json_global_arena_ensure_init();

    JSON json_wrap = NULL;
    
    _json_lexer lexer = {0};
    _json_lexer_init(&lexer, buf, len);

    _json_ast_token* lookahead = _lex_next_token(&lexer);
    _json_ast_token* base = lookahead;
    if (
        _json_parse_json(&lexer, &lookahead, &base) && 
        _json_parse_match(&lexer, &lookahead, JSON_TOKEN_EOF)
    ) {
        json_wrap = _json_build_from_ast(base);
    } else {
        printf("ERROR: %s\n", JSON_GET_PARSE_ERROR());
    }
    
    _json_global_arena_reset();
    return json_wrap;
}

JSON json_parse_file(const char* filename) {

    size_t len;
    const char* file_buf = _read_file_to_cstr(filename, &len);
    if (!file_buf) return NULL;

    JSON ret = json_parse_string(file_buf, len);
    free((char*)file_buf);

    return ret;
}

#endif // JSON_IMPLEMENTATION