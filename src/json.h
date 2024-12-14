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


/*  
    ================================
     Preprocessor defines    
    ================================
*/ 

#define __JSON_MULTIOBJECT_INITIAL_CAP 4

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
#define __JSON_NULL_PRINT                 "null"

#define __JSON_NO_FRACTIONAL_PART(number) ((number) == (int)(number))


#define __JSON_BOOL_TO_STRING(b) ((b) ? __JSON_BOOLEAN_TRUE_PRINT_FMT : __JSON_BOOLEAN_FALSE_PRINT_FMT)

#ifdef __JSON_FREE_DEBUG
    #define __FREE_DEBUG_PRINT(MESSAGE) printf("\nDEBUG: Freeing %s\n", MESSAGE)
#else
    #define __FREE_DEBUG_PRINT(MESSAGE)
#endif

#define __JSON_IS_WHITESPACE(c)  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\f' || (c) == '\v')
#define __JSON_IS_NUMBER_CHAR(c) ((c) == '+' || (c) ==  '-' || (c) ==  'E' || (c) ==  'e' || (c) ==  '.' || ((c) >=  '0' && (c) <=  '9'))

#define __JSON_BUFFER_INITIAL_CAP   64
#define __JSON_STR_INITIAL_CAP       8


/*  
    ================================
     Type defintions    
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

typedef char *c_str;


typedef enum {
    // Primitive types
    JSON_NUMBER, JSON_STRING, JSON_BOOLEAN,
    // Header types
    JSON_OBJECT, JSON_ARRAY,
    // Null
    JSON_NULL,
} JSONType;

//   Keep both aligned
// ---------------------
const char* _json_token_names[] = {
    "TOKEN_NUMBER",
    "TOKEN_STRING",
    "TOKEN_BOOLEAN",
    "TOKEN_NULL",
    "TOKEN_LBRACE",
    "TOKEN_RBRACE",
    "TOKEN_LBRACKET",
    "TOKEN_RBRACKET",
    "TOKEN_COMMA",
    "TOKEN_COLON"
};

typedef enum {
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_BOOLEAN,
    TOKEN_NULL,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_COLON
} _json_token_type;
// ---------------------

typedef struct {
    _json_token_type type;
    union {
        const char* strlit;
        double number;
        bool boolean;
    };
} _json_token;

typedef enum {
    WRITER_STDOUT,
    WRITER_FILE,
} _WriterType;

struct _json_writer {
    _WriterType type;
    FILE* stream;
};


// String Builder ======================================

#define RAM_ASSERT(expr) assert((expr) && "Buy more RAM lol")

typedef struct {
    char* items;
    size_t size;
    size_t _cap;
} _string_builder;

// Allocates initial buffer
void  sb_init(_string_builder* sb);

// Append a sized char buffer
void  sb_append_buffer(_string_builder* sb, const char* buf, size_t size);

// Append a null-terminated char array
void  sb_append_cstr(_string_builder* sb, const c_str str);

// Append single char
void  sb_append_char(_string_builder* sb, const char c);

// Transfers ownership of string
char* sb_tostring_alloc(_string_builder* sb);

// Transfers ownership of string
// Discards the string builder for you
char* sb_collapse_alloc(_string_builder* sb);

// Discards the string builder
void  sb_free(_string_builder* sb);

void sb_init(_string_builder* sb) {

    sb->items = malloc(__JSON_STR_INITIAL_CAP);
    RAM_ASSERT(sb->items);

    sb->_cap = __JSON_STR_INITIAL_CAP;
    sb->size = 0;
}

void sb_append_buffer(_string_builder* sb, const char* buf, size_t size) {
    
    size_t new_size = sb->size + size;

    if (new_size > sb->_cap) {

        size_t new_capacity = sb->_cap * 2;
        while (new_capacity < new_size) new_capacity *= 2;

        char* new_items = realloc(sb->items, new_capacity);
        RAM_ASSERT(new_items);

        sb->_cap = new_capacity;
        sb->items = new_items;
    }

    memcpy(sb->items + sb->size, buf, size);
    sb->size = new_size;
}

void sb_append_cstr(_string_builder* sb, const c_str str) {

    size_t size = strlen(str);
    sb_append_buffer(sb, str, size);
}

void sb_append_char(_string_builder* sb, const char c) {

    if (sb->size == sb->_cap) {

        size_t new_capacity = sb->_cap * 2;
        char* new_items = realloc(sb->items, new_capacity);
        RAM_ASSERT(new_items);

        sb->_cap = new_capacity;
        sb->items = new_items;
    }

    sb->items[sb->size++] = c;
}

char* sb_tostring_alloc(_string_builder* sb) {

    char* copy = malloc(sb->size + 1);
    RAM_ASSERT(copy);

    memcpy(copy, sb->items, sb->size);
    copy[sb->size] = '\0';

    return copy;
}

char* sb_collapse_alloc(_string_builder* sb) {

    char* str = realloc(sb->items, sb->size + 1);
    RAM_ASSERT(str);
    str[sb->size] = '\0';

    sb->items = NULL;
    return str;
}

void sb_free(_string_builder* sb) {
    if (sb && sb->items) {
        free(sb->items);
        sb->items = NULL;
    }
}

// =====================================================


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
void writer_writef(Writer* writer, const c_str message, ...);
void writer_swrite(Writer* writer, const c_str fmt, const c_str message);

/* Internal writing */
void _writef_indent(Writer* writer, size_t depth);
void _writef_line(Writer* writer, const c_str message);

/* Internal reading */
char* _read_file_content(const char* filename);

/* Internal guards */
int __ALLOC_FAILED_GUARD(void* ptr, size_t line);
int __ALLOC_FAILED_GUARD_FREE(void* ptr, void* cleanup, size_t line);
int __ALLOC_FAILED_GUARD_MULTIOBJECT(void* ptr, size_t line);
int __TYPE_GUARD(JSON ptr, JSONType type, size_t line);

/* Internal struct allocators */
_Number  _json_internal_number_alloc(double value);
_Boolean _json_internal_boolean_alloc(bool value);
_String  _json_internal_string_alloc(const c_str string, size_t size);
_Object  _json_internal_object_alloc();
_Array   _json_internal_array_alloc();

_KeyValue _json_internal_kv_alloc(const c_str key, size_t key_len, JSON json_wrap);
_KeyValue* _json_internal_kv_multi_alloc(size_t size);
JSON* _json_internal_object_multi_alloc(size_t size);

/* Internal struct deallocators */
void _json_internal_number_free(_Number json_number);
void _json_internal_boolean_free(_Boolean json_boolean);
void _json_internal_string_free(_String json_string);
void _json_internal_object_free(_Object object);
void _json_internal_array_free(_Array array);
void _json_internal_kv_free(_KeyValue key_value);

/* API Allocators */
JSON json_null_alloc();
JSON json_number_alloc(double number);
JSON json_boolean_alloc(bool boolean);
JSON json_string_alloc(const c_str string);
JSON json_object_alloc();
JSON json_array_alloc();

/* API Deallocators */
void json_free(JSON json_wrap);

/* API Utilities */
bool json_add_key_value(JSON json_wrap, const c_str key, JSON value);
void json_push(JSON json_wrap, JSON value);
void json_foreach(JSON json_wrap, void (*func)(JSON));
JSON json_reducenum(JSON json_wrap, double accumulator, double (*func)(JSON, double));
JSON json_reducebool(JSON json_wrap, bool accumulator, bool (*func)(JSON, bool));

void _json_internal_number_write(Writer* writer, _Number number);
void _json_internal_boolean_write(Writer* writer, _Boolean boolean);
void _json_internal_string_write(Writer* writer, _String string);

void _indent_json_internal_object_write(Writer* writer, size_t depth, _Object object);
void _indent_json_internal_array_write(Writer* writer, size_t depth, _Array array);
void _indent_json_object_wrap_write(Writer* writer, size_t depth, JSON json_wrap);

/* API Writing*/
void json_write(Writer* writer, JSON json_wrap);

/* Internal reassignment */
void _json_internal_number_reset(_Number number, double value);
void _json_internal_boolean_reset(_Boolean boolean, bool value);

/* API Reassignment */
void json_number_reset(JSON json_wrap, double value);
void json_boolean_reset(JSON json_wrap, bool value);
void json_string_reset(JSON json_wrap, const c_str value);

/* Internal copying */
_Array _json_internal_array_copy(_Array array);
_Object _json_internal_object_copy(_Object object);

/* API Copying */
JSON json_copy(JSON json_wrap);

/* API Data */
JSON* json_get(JSON json_wrap, const c_str key);
bool  json_in(JSON json_array, JSON json_wrap);
void  json_reassign(JSON* json_wrap_ptr, JSON new_wrap);

/* API Predicates */
bool json_isnull(JSON json_wrap);
bool json_isnum(JSON json_wrap);
bool json_isint(JSON json_wrap);
bool json_isdec(JSON json_wrap);
bool json_isstr(JSON json_wrap);
bool json_isobj(JSON json_wrap);
bool json_isarr(JSON json_wrap);

bool json_is(JSON json_wrap, JSON other_wrap);
bool json_eq(JSON json_wrap, JSON other_wrap);

/* Internal lexer */
bool _json_lex_number(char** filestr_ptr, _json_token* token);
bool _json_lex_string(char** filestr_ptr, _json_token* token);
bool _json_lex_boolean(char** filestr_ptr, _json_token* token);
bool _json_lex_null(char** filestr_ptr, _json_token* token);
bool _json_lex_structural(char** filestr_ptr, _json_token* token);
_json_token* _json_lex(char* filestr, size_t* len);

/* Internal lexing utils */
char* _safe_escape_string_copy(const c_str message);
bool  _get_escaped_char(char c, char* ec);
char  _get_escape_code(char c); 

/* Internal parser utils */
void _json_free_tokens(_json_token* tokens, size_t len);
JSON _json_parse_tokens(_json_token* tokens, size_t len);

/* API Parsers */
JSON json_parse_string(char* _cstr);
JSON json_parse_file(const char* filename);


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

void writer_writef(Writer* writer, const c_str message, ...) {
    if (writer && writer->stream) {
        va_list args;
        va_start(args, message);
        vfprintf(writer->stream, message, args);
        va_end(args);
    }
}

// Safe write for escape codes
void writer_swrite(Writer* writer, const c_str fmt, const c_str message) {

    char* new_message = _safe_escape_string_copy(message);
    writer_writef(writer, fmt, new_message);
    free(new_message);
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

struct _json_number {      /* JSON_NUMBER   PRIMITIVE   */
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
        _Number  number;
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
        fprintf(stderr, "Memory allocation error at %s:%d\n", __FILE__, line);
        return 0;
    }
    return 1;
}

int __ALLOC_FAILED_GUARD_FREE(void* ptr, void* cleanup, size_t line) {
    if (!ptr) {
        fprintf(stderr, "Memory allocation error at %s:%d\n", __FILE__, line);
        free(cleanup);
        return 0;
    }
    return 1;
}

int __ALLOC_FAILED_GUARD_MULTIOBJECT(void* ptr, size_t line) {
    if (!ptr) {
        fprintf(stderr, "Memory reallocation error at %s:%d\n", __FILE__, line);
        return 0;
    }
    return 1;
}

int __TYPE_GUARD(JSON ptr, JSONType type, size_t line) {
    if (!(ptr && ptr->type == type)) {
        fprintf(stderr, "Type guard error at %s:%d\n", __FILE__, line);
        return 0;
    }
    return 1;
}


/* _String alloc and free */

_String _json_internal_string_alloc(const c_str string, size_t size) {
    _String new_string = (_String)malloc(sizeof(_json_string));
    if (!__ALLOC_FAILED_GUARD(new_string, __LINE__)) return NULL;

    new_string->value = (const c_str)malloc(size + 1);
    if (!__ALLOC_FAILED_GUARD_FREE(new_string->value, new_string, __LINE__)) return NULL;

    strncpy(new_string->value, string, size);
    new_string->value[size] = '\0';
    new_string->size = size;
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
    if (!__ALLOC_FAILED_GUARD(new_boolean, __LINE__)) return NULL;

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
    if (!__ALLOC_FAILED_GUARD(new_number, __LINE__)) return NULL;

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

_KeyValue _json_internal_kv_alloc(const c_str key, size_t key_len, JSON json_wrap) {
    _KeyValue new_key_value = (_KeyValue)malloc(sizeof(_json_key_value_pair));
    if (!__ALLOC_FAILED_GUARD(new_key_value, __LINE__)) return NULL;

    new_key_value->key = (c_str)malloc(key_len + 1);
    if (!__ALLOC_FAILED_GUARD_FREE(new_key_value->key, new_key_value, __LINE__)) return NULL;

    strncpy(new_key_value->key, key, key_len);
    new_key_value->key[key_len] = '\0';
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
    if (!__ALLOC_FAILED_GUARD(new_object, __LINE__)) return NULL;

    new_object->pairs = _json_internal_kv_multi_alloc(__JSON_MULTIOBJECT_INITIAL_CAP);
    if (!__ALLOC_FAILED_GUARD_FREE(new_object->pairs, new_object, __LINE__)) return NULL;

    new_object->_capacity = __JSON_MULTIOBJECT_INITIAL_CAP;
    new_object->keys = 0;
    return new_object;
}

_KeyValue* _json_internal_kv_multi_alloc(size_t size) {
    _KeyValue* new_multi_key_value = (_KeyValue*)malloc(sizeof(_KeyValue) * size);
    if (!__ALLOC_FAILED_GUARD(new_multi_key_value, __LINE__)) return NULL;

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
    if (!__ALLOC_FAILED_GUARD(new_array, __LINE__)) return NULL;

    new_array->objects = _json_internal_object_multi_alloc(__JSON_MULTIOBJECT_INITIAL_CAP);
    if (!__ALLOC_FAILED_GUARD_FREE(new_array->objects, new_array, __LINE__)) return NULL;

    new_array->_capacity = __JSON_MULTIOBJECT_INITIAL_CAP;
    new_array->size = 0;
    return new_array;
}

JSON* _json_internal_object_multi_alloc(size_t size) {
    JSON* new_multi_object = (JSON*)malloc(sizeof(JSON) * size);
    if (!__ALLOC_FAILED_GUARD(new_multi_object, __LINE__)) return NULL;

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
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return  NULL;

    new_json_wrap->type = JSON_NULL;
    new_json_wrap->object = NULL;
    return new_json_wrap;
}

JSON json_number_alloc(double number) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_NUMBER;
    new_json_wrap->number = _json_internal_number_alloc(number);
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->number, new_json_wrap, __LINE__)) return NULL;

    return new_json_wrap;
}

JSON json_boolean_alloc(bool boolean) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_BOOLEAN;
    new_json_wrap->boolean = _json_internal_boolean_alloc(boolean);
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->boolean, new_json_wrap, __LINE__)) return NULL;

    return new_json_wrap;
}

JSON json_string_alloc(const c_str string) {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->string = _json_internal_string_alloc(string, strlen(string));
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->string, new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_STRING;
    return new_json_wrap;
}

JSON json_object_alloc() {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->object = _json_internal_object_alloc();
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->object, new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_OBJECT;
    return new_json_wrap;
}

JSON json_array_alloc() {
    JSON new_json_wrap = (JSON)malloc(sizeof(_json_object_wrap));
    if (!__ALLOC_FAILED_GUARD(new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->array = _json_internal_array_alloc();
    if (!__ALLOC_FAILED_GUARD_FREE(new_json_wrap->array, new_json_wrap, __LINE__)) return NULL;

    new_json_wrap->type = JSON_ARRAY;
    return new_json_wrap;
}

void json_free(JSON json_wrap) {

    if (json_wrap) {
        switch (json_wrap->type) {
            case JSON_NUMBER:
                _json_internal_number_free(json_wrap->number);
            break;

            case JSON_BOOLEAN:
                _json_internal_boolean_free(json_wrap->boolean);
            break;

            case JSON_STRING:
                _json_internal_string_free(json_wrap->string);
            break;

            case JSON_OBJECT:
                _json_internal_object_free(json_wrap->object);
            break;

            case JSON_ARRAY:
                _json_internal_array_free(json_wrap->array);
            break;

            case JSON_NULL:
                __FREE_DEBUG_PRINT("null");
            break;

            default:
                fprintf(stderr, "Memory free error for json_wrap at %s:%d\n", __FILE__, __LINE__);
                return;
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
        const c_str old_key = object->pairs[i]->key;
        JSON new_value = json_copy(object->pairs[i]->value);
        new_object->pairs[i] = _json_internal_kv_alloc(old_key, strlen(old_key), new_value);
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

        case JSON_NUMBER:
            return json_number_alloc(json_wrap->number->value);

        case JSON_BOOLEAN:
            return json_boolean_alloc(json_wrap->boolean->value);

        case JSON_STRING:
            return json_string_alloc(json_wrap->string->value);

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

        case JSON_NULL:
            return json_null_alloc();

        default:
            fprintf(stderr, "Copy error for json_wrap at %s:%d\n", __FILE__, __LINE__);
            return NULL;
    }

}


/*  
    ================================
     Dynamic multiobjects    
    ================================
*/ 

bool json_add_key_value(JSON json_obj, const c_str key, JSON value) {
    if (!__TYPE_GUARD(json_obj, JSON_OBJECT, __LINE__)) return false;

    _Object ob = json_obj->object;

    for (size_t i = 0; i < ob->keys; i++) {
        if (strcmp(ob->pairs[i]->key, key) == 0) {
            return false;
        }
    }

    if (ob->keys == ob->_capacity) {
        size_t new_capacity = ob->_capacity * 2;
        _KeyValue* new_pairs = realloc(ob->pairs, new_capacity * sizeof(_KeyValue));
        if (!__ALLOC_FAILED_GUARD_MULTIOBJECT(new_pairs, __LINE__)) return false;

        ob->_capacity = new_capacity;
        ob->pairs = new_pairs;
    }

    ob->pairs[ob->keys++] = _json_internal_kv_alloc(key, strlen(key), value);
    return true;    
}

JSON* json_get(JSON json_obj, const c_str key) {
    if (!__TYPE_GUARD(json_obj, JSON_OBJECT, __LINE__)) return NULL;

    for (size_t i = 0; i < json_obj->object->keys; i++)
        if (strcmp(key, json_obj->object->pairs[i]->key) == 0)
            return &json_obj->object->pairs[i]->value;

    return NULL;
}

bool json_in(JSON json_array, JSON json_wrap) {
    if (!__TYPE_GUARD(json_array, JSON_ARRAY, __LINE__)) return NULL;

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

void json_push(JSON json_array, JSON value) {
    if (!__TYPE_GUARD(json_array, JSON_ARRAY, __LINE__)) return;

    _Array ar = json_array->array;

    if (ar->size == ar->_capacity) {
        size_t new_capacity = ar->_capacity * 2;
        JSON* new_objects = realloc(ar->objects, new_capacity * sizeof(JSON));
        if (!__ALLOC_FAILED_GUARD_MULTIOBJECT(new_objects, __LINE__)) return;

        ar->_capacity = new_capacity;
        ar->objects = new_objects;
    }

    ar->objects[ar->size++] = value;
}

void json_foreach(JSON json_array, void (*func)(JSON)) {
    if (!__TYPE_GUARD(json_array, JSON_ARRAY, __LINE__)) return;
    
    _Array ar = json_array->array;
    for (size_t i = 0; i < ar->size; i++)
        func(ar->objects[i]);
}

JSON json_reducenum(JSON json_array, double accumulator, double (*func)(JSON, double)) {
    if (!__TYPE_GUARD(json_array, JSON_ARRAY, __LINE__)) return NULL;
    
    _Array ar = json_array->array;
    for (size_t i = 0; i < ar->size; i++) {
        if (!__TYPE_GUARD(ar->objects[i], JSON_NUMBER, __LINE__)) return NULL;
        accumulator = func(ar->objects[i], accumulator);
    }

    return json_number_alloc(accumulator);
}

JSON json_reducebool(JSON json_array, bool accumulator, bool (*func)(JSON, bool)) {
    if (!__TYPE_GUARD(json_array, JSON_ARRAY, __LINE__)) return NULL;
    
    _Array ar = json_array->array;
    for (size_t i = 0; i < ar->size; i++) {
        if (!__TYPE_GUARD(ar->objects[i], JSON_BOOLEAN, __LINE__)) return NULL;
        accumulator = func(ar->objects[i], accumulator);
    }

    return json_boolean_alloc(accumulator);
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
    __JSON_NO_FRACTIONAL_PART(json_wrap->number->value);
}

bool json_isdec(JSON json_wrap) {
    return json_isnum(json_wrap) && 
    !__JSON_NO_FRACTIONAL_PART(json_wrap->number->value);
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

        switch (json_wrap->type)
        {
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
    if (!__TYPE_GUARD(json_num, JSON_NUMBER, __LINE__)) return;

    _json_internal_number_reset(json_num->number, value);
}

void json_boolean_reset(JSON json_bool, bool value) {
    if (!__TYPE_GUARD(json_bool, JSON_BOOLEAN, __LINE__)) return;

    _json_internal_boolean_reset(json_bool->boolean, value);
}

void json_string_reset(JSON json_str, const c_str value) {
    if (!__TYPE_GUARD(json_str, JSON_STRING, __LINE__)) return;

    _json_internal_string_free(json_str->string);
    json_str->string = _json_internal_string_alloc(value, strlen(value));
}


/*  
    ================================
     Writing cont'd   
    ================================
*/ 

void _writef_indent(Writer* writer, size_t depth) {
    while(depth-- > 0) writer_writef(writer, __JSON_TABULATION);
}

void _writef_line(Writer* writer, const c_str message) {
    writer_writef(writer, "%s\n", message);
}

void _json_internal_number_write(Writer* writer, _Number number) {
    writer_writef(writer, __JSON_NUMBER_PRINT_FMT, number->value);
}

void _json_internal_string_write(Writer* writer, _String string) {
    writer_swrite(writer, __JSON_STRING_PRINT_FMT, string->value);
}

void _json_internal_boolean_write(Writer* writer, _Boolean boolean) {
    writer_writef(writer, "%s", __JSON_BOOL_TO_STRING(boolean->value));
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

        writer_swrite(writer, __JSON_KEY_PRINT_FMT, kv->key);
        writer_writef(writer, __JSON_KEY_TO_VALUE);
        _indent_json_object_wrap_write(writer, depth+1, kv->value);
        if (object->keys > 1)
            _writef_line(writer, __JSON_KEY_VALUE_SEPARATOR);
    }

    _KeyValue kv = object->pairs[object->keys-1];
    _writef_indent(writer, depth+1);
    writer_swrite(writer, __JSON_KEY_PRINT_FMT, kv->key);
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
            writer_writef(writer, __JSON_NULL_PRINT);
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
     Parsing   
    ================================
*/ 

bool _get_escaped_char(char c, char* ec) {
    switch (c) {
        case '\a': *ec = 'a';  break;  // Bell (alert)
        case '\b': *ec = 'b';  break;  // Backspace
        case '\f': *ec = 'f';  break;  // Formfeed
        case '\n': *ec = 'n';  break;  // Newline
        case '\r': *ec = 'r';  break;  // Carriage return
        case '\t': *ec = 't';  break;  // Horizontal tab
        case '\v': *ec = 'v';  break;  // Vertical tab
        case '\\': *ec = '\\'; break;  // Backslash
        case '\'': *ec = '\''; break;  // Single quote
        case '\"': *ec = '\"'; break;  // Double quote
        case '\?': *ec = '?';  break;  // Question mark
        case '\0': *ec = '0';  break;  // Null character
        default: {
            return false;
        }
    }
    return true;
}

char _get_escape_code(char c) {
    switch (c) {
        case 'a': return '\a';  // Bell (alert)
        case 'b': return '\b';  // Backspace
        case 'f': return '\f';  // Formfeed
        case 'n': return '\n';  // Newline
        case 'r': return '\r';  // Carriage return
        case 't': return '\t';  // Horizontal tab
        case 'v': return '\v';  // Vertical tab
        case '\\': return '\\'; // Backslash
        case '\'': return '\''; // Single quote
        case '\"': return '\"'; // Double quote
        case '?': return '\?';  // Question mark
        case '0': return '\0';  // Null character
        default: {
            fprintf(stderr, "Encountered unknown escape sequence");
            exit(EXIT_FAILURE);
        }
    }
}

// Reveals escape characters with backslash
char* _safe_escape_string_copy(const c_str message) {

    char esc[2] = {'\\', '\0'};
    _string_builder sb = {0};
    sb_init(&sb);

    char* it = message;
    while(*it != '\0') {
        if (_get_escaped_char(*it, &esc[1])) {
            sb_append_buffer(&sb, esc, 2);
        } else {
            sb_append_char(&sb, *it);
        }
        it++;
    }

    return sb_collapse_alloc(&sb);
}

void _json_free_tokens(_json_token* tokens, size_t len) {

    for (size_t i = 0; i < len; i++) {
        if (tokens[i].type == TOKEN_STRING) {
            free((void*)tokens[i].strlit);
        }
    }

    free(tokens);
}

char* _read_file_content(const char* filename) {

    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "File open error at %s:%d\n", __FILE__, __LINE__);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    rewind(file);

    char *file_cstr = malloc(length + 1);
    if (!file_cstr) {
        fclose(file);
        fprintf(stderr, "file_cstr allocation error at %s:%d\n", __FILE__, __LINE__);
        return NULL;
    }

    size_t bytesRead = fread(file_cstr, sizeof(char), length, file);
    if (bytesRead != length) {
        free(file_cstr);
        fclose(file);
        fprintf(stderr, "File read error (%d/%d bytes) at %s:%d\n", bytesRead, length, __FILE__, __LINE__);
        return NULL;
    }
    
    file_cstr[length] = '\0';

    fclose(file);
    return file_cstr;
}

bool _json_lex_number(char** filestr_ptr, _json_token* token) {

    char* peek = *filestr_ptr;
    while (__JSON_IS_NUMBER_CHAR(*peek)) {
        peek++;
    }

    size_t len = peek - *filestr_ptr;

    if (len > 0) {

        char* numstr = malloc(len + 1);
        if (!__ALLOC_FAILED_GUARD(numstr, __LINE__)) return false;

        memcpy(numstr, *filestr_ptr, len);
        numstr[len] = '\0';

        char* end;
        double number = strtod(numstr, &end);

        char final = *end;
        free(numstr);

        if (final != '\0') return false;
                
        token->type = TOKEN_NUMBER;
        token->number = number;

        *filestr_ptr = peek;
        return true;
    } 

    return false;
}

bool _json_lex_string(char** filestr_ptr, _json_token* token) {

    if (**filestr_ptr != '"') return false;

    char* peek = *filestr_ptr + 1;
    int escaped = 0;

    // Get number of escaped
    while (*peek && *peek != '"')
    {
        if (*peek == '\\') {
            escaped++;
            peek++;
        }
        peek++;
    }
    
    if (*peek != '"') return false;

    // Allocate string
    size_t len = (peek - (*filestr_ptr + 1)) - escaped;
    peek = *filestr_ptr + 1;

    char* str = malloc(len + 1);
    if (!__ALLOC_FAILED_GUARD(str, __LINE__)) return false;
    str[len] = '\0';

    size_t it = 0;
    while (it < len) {
        if (*peek == '\\') {
            peek++;
            str[it++] = _get_escape_code(*(peek++));
        }
        else {
            str[it++] = *(peek++);
        }
    }

    // Skip the final quote
    *filestr_ptr = peek + 1;
    token->type = TOKEN_STRING;
    token->strlit = str;

    return true;
}

bool _json_lex_boolean(char** filestr_ptr, _json_token* token) {

    if (strncasecmp(*filestr_ptr, "true", 4) == 0) {
        token->type = TOKEN_BOOLEAN;
        token->boolean = true;
        *filestr_ptr += 4;
        return true;
    } 
    else if (strncasecmp(*filestr_ptr, "false", 5) == 0) {
        token->type = TOKEN_BOOLEAN;
        token->boolean = false;
        *filestr_ptr += 5;
        return true;
    }

    return false;
}

bool _json_lex_null(char** filestr_ptr, _json_token* token) {

    if (strncasecmp(*filestr_ptr, "null", 4) == 0) {
        token->type = TOKEN_NULL;
        *filestr_ptr += 4;
        return true;
    }

    return false;
}

bool _json_lex_structural(char** filestr_ptr, _json_token* token) {

    switch (**filestr_ptr) {
        case '{': {
            token->type = TOKEN_LBRACE;
            *filestr_ptr += 1;
            break;
        }
        case '}': {
            token->type = TOKEN_RBRACE;
            break;
        }
        case '[': {
            token->type = TOKEN_LBRACKET;
            break;
        }
        case ']': {
            token->type = TOKEN_RBRACKET;
            break;
        }
        case ',': {
            token->type = TOKEN_COMMA;
            break;
        }
        case ':': {
            token->type = TOKEN_COLON;
            break;
        }
        default: return false;
    }

    *filestr_ptr += 1;
    return true;
}

_json_token* _json_lex(char* filestr, size_t* len) {

    // Debugging
    char* line_start = filestr;
    size_t line_count = 1;

    // Tokens
    size_t tokens_cap = __JSON_BUFFER_INITIAL_CAP;
    size_t tokens_size = 0;
    _json_token* tokens = malloc(tokens_cap * sizeof(_json_token));

    while (*filestr) {

        _json_token token = {0};

        // Jump

        while (__JSON_IS_WHITESPACE(*filestr)) {
            if (*filestr == '\n') {
                line_count++;
                line_start = filestr;
            }
            filestr++;
        }

        // Lexical analysis

        // Note: This if-else skeleton is very odd but I really like 
        // the control flow that looks like GOTO while being a somewhat safer

        if (_json_lex_number(&filestr, &token)) {

        } else if (_json_lex_string(&filestr, &token)) {

        } else if (_json_lex_boolean(&filestr, &token)) {

        } else if (_json_lex_null(&filestr, &token)) {

        } else if (_json_lex_structural(&filestr, &token)) {

        } else {
            printf("Lexer error following %d:%d\n", line_count, (int)(filestr - line_start));
            _json_free_tokens(tokens, tokens_size);
            return NULL;
        }

        // Push new token
        if (tokens_size == tokens_cap) {

            size_t new_capacity = tokens_cap * 2;
            _json_token* new_tokens = realloc(tokens, new_capacity * sizeof(_json_token));
            if (!__ALLOC_FAILED_GUARD_MULTIOBJECT(new_tokens, __LINE__)) {
                _json_free_tokens(tokens, tokens_size);
                return NULL;
            }

            tokens_cap = new_capacity;
            tokens = new_tokens;
        }

        tokens[tokens_size++] = token;
    }

    *len = tokens_size;
    return tokens;
}

JSON _json_parse_tokens(_json_token* tokens, size_t len) {
    return NULL;
}

JSON json_parse_string(char* _cstr) {

    size_t len;
    _json_token* tokens = _json_lex(_cstr, &len);
    if (!tokens) return NULL;

    JSON ret = _json_parse_tokens(tokens, len);
    _json_free_tokens(tokens, len);

    return ret;
}

JSON json_parse_file(const char* filename) {

    char* file_cstr = _read_file_content(filename);
    if (!file_cstr) return NULL;

    JSON ret = json_parse_string(file_cstr);
    free(file_cstr);

    return ret;
}


#endif // JSON_C