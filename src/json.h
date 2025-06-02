/*
 * Author: Louka Fortin-Sirianni
 * Date: 03/04/2025
 * File: json.h
 * Description: This file is a STB-style header library for JSON. Define the
 *              macro JSON_IMPLEMENTATION to include implementation. It is a 
 *              simple utility for parsing, building, and writing Javascript
 *              Object Notation. Don't hold back!
 * Version: 2.0
 * License: MIT
 */

#ifndef __JSON_C
#define __JSON_C

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
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
#define __JSON_BOOLEAN_FALSE_PRINT_FMT    "false" 
#define __JSON_NULL_PRINT_FMT             "null"

#define JSON_HAS_NO_FRACTIONAL_PART(number) ((number) == (int)(number))

#define JSON_BOOL_TO_STRING(b) ((b) ? __JSON_BOOLEAN_TRUE_PRINT_FMT : __JSON_BOOLEAN_FALSE_PRINT_FMT)

#ifdef JSON_ALLOC_DEBUG
#define __ALLOC_DEBUG_PRINT(MESSAGE) fprintf(stderr, "DEBUG: Allocated %s\n", (MESSAGE))
#else
#define __ALLOC_DEBUG_PRINT(MESSAGE)
#endif // JSON_ALLOC_DEBUG

#ifdef JSON_FREE_DEBUG
#define __FREE_DEBUG_PRINT(MESSAGE) fprintf(stderr, "DEBUG: Freed %s\n", (MESSAGE))
#else
#define __FREE_DEBUG_PRINT(MESSAGE)
#endif // JSON_FREE_DEBUG

#ifdef JSON_LEXER_DEBUG
#define __LEXER_DEBUG_PRINT(TYPE) fprintf(stderr, "DEBUG: Lexed %s\n", _get_json_token_name((TYPE)));
#else 
#define __LEXER_DEBUG_PRINT(TYPE)
#endif // JSON_LEXER_DEBUG

#ifdef JSON_ARENA_DEBUG
#define __ARENA_DEBUG_PRINT() fprintf(stderr, "DEBUG: Arena grew (size: %zu)\n", _json_global_arena.regions);
#else 
#define __ARENA_DEBUG_PRINT()
#endif // JSON_ARENA_DEBUG

#define JSON_IS_WHITESPACE(c)  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\f' || (c) == '\v')
#define JSON_IS_NUMBER_CHAR(c) ((c) == '+' || (c) ==  '-' || (c) ==  'E' || (c) ==  'e' || (c) ==  '.' || ((c) >=  '0' && (c) <=  '9'))
#define JSON_IS_ALPHANUM(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z') || ((c) >= '0' && (c) <= '9'))

#ifndef JSON_MULTIOBJECT_INITIAL_CAP
#define JSON_MULTIOBJECT_INITIAL_CAP 4
#endif // JSON_MULTIOBJECT_INITIAL_CAP

#define JSON_MEM_ASSERT(PTR)                           \
    do {                                               \
        if (!(PTR)) {                                  \
            fprintf(stderr, "FATAL: Out of memory\n"); \
            exit(EXIT_FAILURE);                        \
        }                                              \
    } while (0)


/*  
    ================================
     C Lib    
    ================================
*/ 

typedef unsigned char JSON_BOOL;

#define JSON_FALSE 0
#define JSON_TRUE  1

/**
 * @brief  Replaces <string.h> strlen
 * @param  _cstr pointer to cstring
 * @return size of cstring
 * @note   assumes null termination
 */
size_t json_cstr_len(const char* _cstr);
/**
 * @brief  Replaces <string.h> strdup
 * @param  _cstr pointer to cstring
 * @return copied string pointer
 * @note   allocates memory with malloc
 * @note   assumes null termination
 */
char* json_cstr_dup(const char* _cstr);
/**
 * @brief  Replaces <string.h> memcpy
 * @param  _dst pointer to buffer written to
 * @param  _src pointer to buffer written from
 * @param  _size size of buffer
 * @return pointer to destination
 */
void* json_memcpy(void* _dst, const void* _src, size_t _size);
/**
 * @brief  Replaces <string.h> memset
 * @param  _dst pointer to buffer written to
 * @param  _val value to set buffer cells
 * @param  _size size of buffer
 * @return pointer to destination
 */
void* json_memset(void* _dst, const unsigned char _val, size_t _size);
/**
 * @brief  Replaces <string.h> strncmp
 * @param  _p1 pointer to first string
 * @param  _p2 pointer to second string
 * @param  _max maximum comparison length
 * @return 0 for match; 1 otherwise
 * @note   assumes null termination
 */
int json_strncmp(const char* _p1, const char* _p2, size_t _max);
/**
 * @brief  Replaces <string.h> strcmp
 * @param  _p1 pointer to first string
 * @param  _p2 pointer to second string
 * @return 0 for match; 1 otherwise
 * @note   assumes null termination
 */
int json_strcmp(const char* _p1, const char* _p2);

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
typedef struct _json_T_wrap    _json_T_wrap;
typedef struct _json_writer         _json_writer;
typedef struct _json_reader         _json_reader;

typedef _json_number    *_Number;
typedef _json_boolean   *_Boolean;
typedef _json_string    *_String;
typedef _json_object    *_Object;
typedef _json_array     *_Array;

typedef _json_key_value_pair *_KeyValue;

/**
 * @brief Standard JSON library type
 */
typedef _json_T_wrap    *json_t;

/**
 * @brief Standard JSON writer type
 */
typedef _json_writer    Writer;

/*  
    ================================
     Structures    
    ================================
*/ 

/**
 * @brief Types of JSON
 */
typedef enum {
    JSON_NUMBER_TYPE, 
    JSON_STRING_TYPE, 
    JSON_BOOLEAN_TYPE,
    JSON_OBJECT_TYPE, 
    JSON_ARRAY_TYPE,
    JSON_NULL_TYPE,
} _json_type;

struct _json_key_value_pair {
    char* key;
    json_t value;
};

struct _json_object {
    size_t      keys;
    _KeyValue*  pairs;
    size_t      _capacity;
};

struct _json_array {
    size_t  size;
    json_t*   objects;
    size_t  _capacity;
};

struct _json_string {
    char*   value;
};

struct _json_number {
    double  value;
};

struct _json_boolean {
    JSON_BOOL    value;
};

struct _json_T_wrap {
    _json_type type;
    union {
        _Array   array;
        _Object  object;
        _String  string;
        _Number  number;
        _Boolean boolean;
    };
};

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
} _json_ast_type;

typedef struct _json_ast _json_ast;

struct _json_ast {
    _json_ast_type type;
    union {
        _string_view as_str;
        double as_num;
        JSON_BOOL as_bool;
    };
    size_t row;
    size_t col;
    _json_ast* next;
    _json_ast* child;
};

/*  
    ================================
     Arena    
    ================================
*/

#ifndef JSON_ARENA_REGION_CAP
#define JSON_ARENA_REGION_CAP 256
#endif // JSON_ARENA_REGION_CAP

typedef struct  _json_ast_arena_reg _json_ast_arena_reg;

struct _json_ast_arena_reg {
    _json_ast buf[JSON_ARENA_REGION_CAP];
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
    (json_strncmp(&(LEXER)->source[(LEXER)->cursor], (BUF), (BUFLEN)) == 0) && \
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
 * ;; LL1 Context-free grammar
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

typedef struct {
    _json_ast* lookahead;
    _json_lexer* lexer;
} _json_parser;

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

/**
 * @brief Inits a writer to use stdout
 * @param writer writer reference
 */
void writer_stdout_init(Writer* writer);
/**
 * @brief  Inits a writer to use a file
 * @param  filename filename cstr
 * @return 1 for success; 0 otherwise
 */
int  writer_file_init(Writer* writer, const char* filename);
/**
 * @brief Closes file stream of writer
 * @param writer writer reference
 */
void writer_file_close(Writer* writer);
/**
 * @brief Variadic format print to writer
 * @param writer writer reference
 * @param message message or format
 * @param ... vargs
 * @note  Similar to fprintf
 */
void writer_writef(Writer* writer, const char* message, ...);

/* Type guard */

/**
 * @brief Type guard for JSON wrapper
 * @param wrap JSON wrapper
 * @param type type of wrapper
 * @return 1 for assert failed; 0 otherwise
 */
int JSON_TYPE_GUARD(json_t wrap, _json_type type);

/* Allocations */

/**
 * @brief  Allocates a null 
 * @return allocated wrapper
 */
json_t json_null_alloc();
/**
 * @brief  Allocates a number 
 * @param  number value
 * @return allocated wrapper
 */
json_t json_number_alloc(double number);
/**
 * @brief  Allocates a boolean
 * @param  boolean value 
 * @return allocated wrapper
 */
json_t json_boolean_alloc(JSON_BOOL boolean);
/**
 * @brief  Allocates a string
 * @param  string value
 * @return allocated wrapper
 * @note   String is copied for internal use
 */
json_t json_string_alloc(const char* string);
/**
 * @brief  Allocates a string from string view
 * @param  sv string view
 * @return allocated wrapper
 * @note   String is copied for internal use
 */
json_t json_string_from_sv_alloc(const _string_view sv);
/**
 * @brief  Allocates an object 
 * @return allocated wrapper
 */
json_t json_object_alloc();
/**
 * @brief  Allocates an array 
 * @return allocated wrapper
 */
json_t json_array_alloc();

/**
 * @brief Frees an allocated wrapper
 * @param wrap wrapper
 * @note  Children and members are recursively freed
 */
void json_free(json_t wrap);

/**
 * @brief Dereferences the pointer and set the wrapper to null
 * @param wrap wrapper
 * @note  Introduced to avoid bad frees or null dereferences
 */
void json_ptr_free(json_t* wrap_ptr);

/* Utilities */

/**
 * @brief Adds a key value to an object
 * @param wrap object wrapper
 * @param sv key string view
 * @param value json wrapper
 * @return JSON_TRUE for success; JSON_FALSE otherwise
 */
JSON_BOOL json_add_key_sv_value(json_t wrap, const _string_view sv, json_t value);
/**
 * @brief Adds a key value to an object
 * @param wrap object wrapper
 * @param key key cstr
 * @param value json wrapper
 * @return JSON_TRUE for success; JSON_FALSE otherwise
 */
JSON_BOOL json_add_key_value(json_t wrap, const char* key, json_t value);
/**
 * @brief Appends an array member
 * @param wrap array wrapper
 * @param value json wrapper
 * @return JSON_TRUE for success; JSON_FALSE otherwise
 */
JSON_BOOL json_push(json_t wrap, json_t value);

/**
 * @brief Applies an action on an array
 * @param wrap array wrapper
 * @param action action pointer
 */
void json_foreach(json_t wrap, void (*action)(json_t));
/**
 * @brief  Accumulates a number action on an array
 * @param  wrap array wrapper
 * @param  action action pointer
 * @return accumulated number
 */
double json_reduce_num(json_t wrap, double accumulator, double (*action)(json_t, double));
/**
 * @brief  Accumulates a boolean action on an array
 * @param  wrap array wrapper
 * @param  action action pointer
 * @return accumulated boolean
 */
JSON_BOOL json_reduce_bool(json_t wrap, JSON_BOOL accumulator, JSON_BOOL (*action)(json_t, JSON_BOOL));

/* Writing*/

/**
 * @brief Writes a JSON wrapper to stream
 * @param writer writer reference
 * @param wrap json wrapper
 */
void json_write(Writer* writer, json_t wrap);

/* Reset */

/**
 * @brief Modifies the value of a numeric wrapper
 * @param wrap number wrap
 * @param value new value
 */
void json_number_reset(json_t wrap, double value);
/**
 * @brief Modifies the value of a boolean wrapper
 * @param wrap JSON_BOOL wrap
 * @param value new value
 */
void json_boolean_reset(json_t wrap, JSON_BOOL value);
/**
 * @brief Modifies the value of a string wrapper
 * @param wrap number wrap
 * @param value new string
 * @note  Frees the old string
 */
void json_string_reset(json_t wrap, const char* value);

/* Copying */

/**
 * @brief  Copies a wrapper
 * @param  wrap json wrapper
 * @return Deep copy of wrapper
 */
json_t json_copy(json_t wrap);

/* Data */

/**
 * @brief  Gets the value of a key
 * @param  wrap object wrapper 
 * @param  key null terminated string
 * @return pointer to value wrapper or NULL
 */
json_t* json_get(json_t wrap, const char* key);
/**
 * @brief  Reveals if wrapper has an equal in the array
 * @param  array_wrap json array wrapper
 * @param  wrap comparing wrapper
 * @return JSON_TRUE if found; JSON_FALSE otherwise
 * @note   Calls json_eq on members
 */
JSON_BOOL  json_in(json_t array_wrap, json_t wrap);
/**
 * @brief Reassigns the value of a wrapper
 * @param wrap_ptr wrapper reference
 * @param new_wrap new value
 * @note  The old valued is freed with json_free()
 */
void  json_reassign(json_t* wrap_ptr, json_t new_wrap);

/**
 * @brief  Null type equality
 * @param  wrap json wrapper
 * @return JSON_TRUE for matching type; JSON_FALSE otherwise
 */
JSON_BOOL json_isnull(json_t wrap);
/**
 * @brief  Numeric type equality
 * @param  wrap json wrapper
 * @return JSON_TRUE for matching type; JSON_FALSE otherwise
 */
JSON_BOOL json_isnum(json_t wrap);
/**
 * @brief  Numeric type equality without fractional part
 * @param  wrap json wrapper
 * @return JSON_TRUE for matching type; JSON_FALSE otherwise
 */
JSON_BOOL json_isint(json_t wrap);
/**
 * @brief  Numeric type equality with fractional part
 * @param  wrap json wrapper
 * @return JSON_TRUE for matching type; JSON_FALSE otherwise
 */
JSON_BOOL json_isdec(json_t wrap);
/**
 * @brief  String type equality
 * @param  wrap json wrapper
 * @return JSON_TRUE for matching type; JSON_FALSE otherwise
 */
JSON_BOOL json_isstr(json_t wrap);
/**
 * @brief  Object type equality
 * @param  wrap json wrapper
 * @return JSON_TRUE for matching type; JSON_FALSE otherwise
 */
JSON_BOOL json_isobj(json_t wrap);
/**
 * @brief  Array type equality
 * @param  wrap json wrapper
 * @return JSON_TRUE for matching type; JSON_FALSE otherwise
 */
JSON_BOOL json_isarr(json_t wrap);

/**
 * @brief  Reference equality
 * @param  wrap json wrapper
 * @param  other json wrapper
 * @return JSON_TRUE for matching reference; JSON_FALSE otherwise
 * @note   Equivalent to == operator in javascript without type coercion
 */
JSON_BOOL json_is(json_t wrap, json_t other);
/**
 * @brief  Type and value equality
 * @param  wrap json wrapper
 * @param  other json wrapper
 * @return JSON_TRUE for matching type and value; JSON_FALSE otherwise
 * @note   Equivalent to === operator in javascript
 */
JSON_BOOL json_eq(json_t wrap, json_t other);

/* Parsing */

/**
 * @brief  Parses json from a sized string
 * @param  buf sized string buffer
 * @param  len length of buffer
 * @return json wrapper
 */
json_t json_parse_string(const char* buf, size_t len);
/**
 * @brief  Parses json from a null terminated string
 * @param  _cstr null terminated string
 * @return json wrapper
 */
json_t json_parse_cstring(const char* _cstr);
/**
 * @brief  Parses json from a file
 * @param  filename cstring
 * @return json wrapper
 */
json_t json_parse_file(const char* filename);

#endif // __JSON_C

#ifdef JSON_IMPLEMENTATION

const char* _get_json_token_name(int value) {
    // unsafe as heck but debug only
    const char** name = _json_token_names;
    while (value > 1) {
        value >>= 1;
        name++;
    }
    return *name;
}

/*  
    ================================
     C Lib   
    ================================
*/ 

size_t json_cstr_len(const char* _cstr) {
    size_t size = 0;
    while (*_cstr++) {
        ++size;
    }
    return size;
}

char* json_cstr_dup(const char* _cstr) {
    size_t buf_size = json_cstr_len(_cstr) + 1; // null termination
    char* new_cstr = malloc(buf_size);
    if (new_cstr) {
        json_memcpy(new_cstr, _cstr, buf_size);
    }
    return new_cstr;
}

void* json_memcpy(void* _dst, const void* _src, size_t _size) {
    unsigned char* dst = _dst;
    const unsigned char* src = _src;
    while (_size--) {
        *dst++ = *src++;
    }
    return _dst;
}

void* json_memset(void* _dst, const unsigned char _val, size_t _size) {
    unsigned char* dst = _dst;
    while (_size--) {
        *dst++ = _val;
    }
    return _dst;
}

int json_strncmp(const char* _p1, const char* _p2, size_t _max) {
    while (_max--) {
        if (*_p1 != *_p2) {
            return 1;
        }
        if (*_p1 == '\0') {
            return 0;
        }
        ++_p1;
        ++_p2;
    }
    return 0;
}

int json_strcmp(const char* _p1, const char* _p2) {
    while (1) {
        if (*_p1 != *_p2) {
            return 1;
        }
        if (*_p1 == '\0') {
            return 0;
        }
        ++_p1;
        ++_p2;
    }
    return 0;
}

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
json_t* _json_internal_object_multi_alloc(size_t size);

/* Guards: truthy if assert failed */

int JSON_TYPE_GUARD(json_t wrap, _json_type type) {
    if (wrap && wrap->type == type) return 0;
    
    fprintf(stderr, "WARNING: Type assert raised\n");
    return 1;
}

/* _String alloc and free */

_String _json_internal_string_from_sv_alloc(const _string_view sv) {
    _String new_string = malloc(sizeof(_json_string));
    JSON_MEM_ASSERT(new_string);
    
    char* sv_dup = malloc((sv.size + 1) * sizeof(char));
    JSON_MEM_ASSERT(sv_dup);

    json_memcpy(sv_dup, sv.data, sv.size);
    sv_dup[sv.size] = '\0';

    new_string->value = sv_dup;
    __ALLOC_DEBUG_PRINT("string");
    return new_string;
}

_String _json_internal_string_alloc(const char* string) {
    _String new_string = malloc(sizeof(_json_string));
    JSON_MEM_ASSERT(new_string);
    
    new_string->value = json_cstr_dup(string);
    JSON_MEM_ASSERT(new_string->value);

    __ALLOC_DEBUG_PRINT("string");
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

_Boolean _json_internal_boolean_alloc(JSON_BOOL value) {
    _Boolean new_boolean = malloc(sizeof(_json_boolean));
    JSON_MEM_ASSERT(new_boolean);

    new_boolean->value = value;
    __ALLOC_DEBUG_PRINT("boolean");
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
    _Number new_number = malloc(sizeof(_json_number));
    JSON_MEM_ASSERT(new_number);

    new_number->value = value;
    __ALLOC_DEBUG_PRINT("number");
    return new_number;
}

void _json_internal_number_free(_Number json_number) {
    if (json_number) {
        __FREE_DEBUG_PRINT("number");
        free(json_number);
    }
}

/* _KeyValue alloc and free */

_KeyValue _json_internal_kv_sv_alloc(const _string_view sv, json_t wrap) {
    _KeyValue new_key_value = malloc(sizeof(_json_key_value_pair));
    JSON_MEM_ASSERT(new_key_value);

    char* sv_dup = malloc((sv.size + 1) * sizeof(char));
    JSON_MEM_ASSERT(sv_dup);

    json_memcpy(sv_dup, sv.data, sv.size);
    sv_dup[sv.size] = '\0';

    new_key_value->key = sv_dup;

    new_key_value->value = wrap;
    __ALLOC_DEBUG_PRINT("key value");
    return new_key_value;
}

_KeyValue _json_internal_kv_alloc(const char* key, json_t wrap) {
    _KeyValue new_key_value = malloc(sizeof(_json_key_value_pair));
    JSON_MEM_ASSERT(new_key_value);

    new_key_value->key = json_cstr_dup(key);
    JSON_MEM_ASSERT(new_key_value->key);

    new_key_value->value = wrap;
    __ALLOC_DEBUG_PRINT("key value");
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
    _Object new_object = malloc(sizeof(_json_object));
    JSON_MEM_ASSERT(new_object);

    new_object->pairs = _json_internal_kv_multi_alloc(JSON_MULTIOBJECT_INITIAL_CAP);
    JSON_MEM_ASSERT(new_object->pairs);

    new_object->_capacity = JSON_MULTIOBJECT_INITIAL_CAP;
    new_object->keys = 0;
    __ALLOC_DEBUG_PRINT("object");
    return new_object;
}

_KeyValue* _json_internal_kv_multi_alloc(size_t size) {
    _KeyValue* new_multi_key_value = malloc(sizeof(_KeyValue) * size);
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
    _Array new_array = malloc(sizeof(_json_array));
    JSON_MEM_ASSERT(new_array);

    new_array->objects = _json_internal_object_multi_alloc(JSON_MULTIOBJECT_INITIAL_CAP);
    JSON_MEM_ASSERT(new_array->objects);

    new_array->_capacity = JSON_MULTIOBJECT_INITIAL_CAP;
    new_array->size = 0;
    __ALLOC_DEBUG_PRINT("array");
    return new_array;
}

json_t* _json_internal_object_multi_alloc(size_t size) {
    json_t* new_multi_object = malloc(sizeof(json_t) * size);
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

json_t json_null_alloc() {
    json_t new_wrap = malloc(sizeof(_json_T_wrap));
    JSON_MEM_ASSERT(new_wrap);

    new_wrap->type = JSON_NULL_TYPE;
    new_wrap->object = NULL;

    __ALLOC_DEBUG_PRINT("null");
    return new_wrap;
}

json_t json_number_alloc(double number) {
    json_t new_wrap = malloc(sizeof(_json_T_wrap));
    JSON_MEM_ASSERT(new_wrap);

    new_wrap->type = JSON_NUMBER_TYPE;
    new_wrap->number = _json_internal_number_alloc(number);
    JSON_MEM_ASSERT(new_wrap->number);

    return new_wrap;
}

json_t json_boolean_alloc(JSON_BOOL boolean) {
    json_t new_wrap = malloc(sizeof(_json_T_wrap));
    JSON_MEM_ASSERT(new_wrap);

    new_wrap->type = JSON_BOOLEAN_TYPE;
    new_wrap->boolean = _json_internal_boolean_alloc(boolean);
    JSON_MEM_ASSERT(new_wrap->boolean);

    return new_wrap;
}

json_t json_string_alloc(const char* string) {
    json_t new_wrap = malloc(sizeof(_json_T_wrap));
    JSON_MEM_ASSERT(new_wrap);

    new_wrap->string = _json_internal_string_alloc(string);
    JSON_MEM_ASSERT(new_wrap->string);

    new_wrap->type = JSON_STRING_TYPE;
    return new_wrap;
}

json_t json_string_from_sv_alloc(const _string_view sv) {
    json_t new_wrap = malloc(sizeof(_json_T_wrap));
    JSON_MEM_ASSERT(new_wrap);

    new_wrap->string = _json_internal_string_from_sv_alloc(sv);
    JSON_MEM_ASSERT(new_wrap->string);

    new_wrap->type = JSON_STRING_TYPE;
    return new_wrap;
}

json_t json_object_alloc() {
    json_t new_wrap = malloc(sizeof(_json_T_wrap));
    JSON_MEM_ASSERT(new_wrap);

    new_wrap->object = _json_internal_object_alloc();
    JSON_MEM_ASSERT(new_wrap->object);

    new_wrap->type = JSON_OBJECT_TYPE;
    return new_wrap;
}

json_t json_array_alloc() {
    json_t new_wrap = malloc(sizeof(_json_T_wrap));
    JSON_MEM_ASSERT(new_wrap);

    new_wrap->array = _json_internal_array_alloc();
    JSON_MEM_ASSERT(new_wrap->array);

    new_wrap->type = JSON_ARRAY_TYPE;
    return new_wrap;
}

void json_free(json_t wrap) {
    if (wrap) {
        switch (wrap->type) {
            case JSON_NUMBER_TYPE:  _json_internal_number_free(wrap->number); break;
            case JSON_BOOLEAN_TYPE: _json_internal_boolean_free(wrap->boolean); break;
            case JSON_STRING_TYPE:  _json_internal_string_free(wrap->string); break;
            case JSON_OBJECT_TYPE:  _json_internal_object_free(wrap->object); break;
            case JSON_ARRAY_TYPE:   _json_internal_array_free(wrap->array); break;
            case JSON_NULL_TYPE:    __FREE_DEBUG_PRINT("null"); break;
            default: {
                fprintf(stderr, "WARNING: Free error\n");
                return;
            }
        }
        free(wrap);
    }
}

void json_ptr_free(json_t* wrap_ptr) {
    if (wrap_ptr) {
        json_free(*wrap_ptr);
        *wrap_ptr = NULL;
    }
}

_Object _json_internal_object_copy(_Object object) {

    _Object new_object = _json_internal_object_alloc();

    new_object->keys = object->keys;
    new_object->_capacity = new_object->_capacity;
    new_object->pairs = _json_internal_kv_multi_alloc(new_object->_capacity);

    for (size_t i = 0; i < new_object->keys; i++) {
        const char* old_key = object->pairs[i]->key;
        json_t new_value = json_copy(object->pairs[i]->value);
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

json_t json_copy(json_t wrap) {
    if (!wrap) return NULL;

    switch (wrap->type) {
        case JSON_NUMBER_TYPE:  return json_number_alloc(wrap->number->value);
        case JSON_BOOLEAN_TYPE: return json_boolean_alloc(wrap->boolean->value);
        case JSON_STRING_TYPE:  return json_string_alloc(wrap->string->value);
        case JSON_NULL_TYPE:    return json_null_alloc();
        case JSON_OBJECT_TYPE: {
            json_t json_ret = json_object_alloc();
            json_ret->object = _json_internal_object_copy(wrap->object);
            return json_ret;
        }
        case JSON_ARRAY_TYPE: {
            json_t json_ret = json_array_alloc();
            json_ret->array = _json_internal_array_copy(wrap->array);
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

JSON_BOOL json_add_key_sv_value(json_t wrap, const _string_view sv, json_t value) {
    if (JSON_TYPE_GUARD(wrap, JSON_OBJECT_TYPE)) return JSON_FALSE;

    _Object ob = wrap->object;

    for (size_t i = 0; i < ob->keys; i++) {
        if (json_strncmp(ob->pairs[i]->key, sv.data, sv.size) == 0) {
            return JSON_FALSE;
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
    return JSON_TRUE;    
}

JSON_BOOL json_add_key_value(json_t wrap, const char* key, json_t value) {
    if (JSON_TYPE_GUARD(wrap, JSON_OBJECT_TYPE)) return JSON_FALSE;

    _Object ob = wrap->object;

    for (size_t i = 0; i < ob->keys; i++) {
        if (json_strcmp(ob->pairs[i]->key, key) == 0) {
            return JSON_FALSE;
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
    return JSON_TRUE;    
}

json_t* json_get(json_t wrap, const char* key) {
    if (JSON_TYPE_GUARD(wrap, JSON_OBJECT_TYPE)) return NULL;

    for (size_t i = 0; i < wrap->object->keys; i++)
        if (json_strcmp(key, wrap->object->pairs[i]->key) == 0)
            return &wrap->object->pairs[i]->value;

    return NULL;
}

JSON_BOOL json_in(json_t json_array, json_t json_wrap) {
    if (JSON_TYPE_GUARD(json_array, JSON_ARRAY_TYPE)) return 0;

    for (size_t i = 0; i < json_array->array->size; i++) {
        if (json_eq(json_array->array->objects[i], json_wrap))
            return JSON_TRUE;
    }

    return JSON_FALSE;
}

void json_reassign(json_t* wrap_ptr, json_t new_wrap) {
    if (wrap_ptr && new_wrap) {
        json_free(*wrap_ptr);
        *wrap_ptr = new_wrap;
    }
}

JSON_BOOL json_push(json_t array_wrap, json_t value) {
    if (JSON_TYPE_GUARD(array_wrap, JSON_ARRAY_TYPE)) return JSON_FALSE;

    _Array ar = array_wrap->array;

    if (ar->size == ar->_capacity) {
        size_t new_capacity = ar->_capacity * 2;
        json_t* new_objects = realloc(ar->objects, new_capacity * sizeof(json_t));
        JSON_MEM_ASSERT(new_objects);

        ar->_capacity = new_capacity;
        ar->objects = new_objects;
    }

    ar->objects[ar->size++] = value;
    return JSON_TRUE;
}

void json_foreach(json_t array_wrap, void (*action)(json_t)) {
    if (JSON_TYPE_GUARD(array_wrap, JSON_ARRAY_TYPE)) return;
    
    _Array ar = array_wrap->array;
    for (size_t i = 0; i < ar->size; i++)
        action(ar->objects[i]);
}

double json_reduce_num(json_t array_wrap, double accumulator, double (*action)(json_t, double)) {
    if (JSON_TYPE_GUARD(array_wrap, JSON_ARRAY_TYPE)) return 0.0;
    
    _Array ar = array_wrap->array;
    for (size_t i = 0; i < ar->size; i++) {
        if (JSON_TYPE_GUARD(ar->objects[i], JSON_NUMBER_TYPE)) continue;
        accumulator = action(ar->objects[i], accumulator);
    }

    return accumulator;
}

JSON_BOOL json_reduce_bool(json_t array_wrap, JSON_BOOL accumulator, JSON_BOOL (*action)(json_t, JSON_BOOL)) {
    if (JSON_TYPE_GUARD(array_wrap, JSON_ARRAY_TYPE)) return JSON_FALSE;
    
    _Array ar = array_wrap->array;
    for (size_t i = 0; i < ar->size; i++) {
        if (JSON_TYPE_GUARD(ar->objects[i], JSON_BOOLEAN_TYPE)) continue;
        accumulator = action(ar->objects[i], accumulator);
    }

    return accumulator;
}

/*  
    ================================
     Predicates   
    ================================
*/ 

JSON_BOOL json_isnull(json_t wrap) {
    return wrap && 
    wrap->object && 
    wrap->type == JSON_NULL_TYPE;
}

JSON_BOOL json_isnum(json_t wrap) {
    return wrap && 
    wrap->number &&
    wrap->type == JSON_NUMBER_TYPE;
}

JSON_BOOL json_isint(json_t wrap) {
    return json_isnum(wrap) &&
    JSON_HAS_NO_FRACTIONAL_PART(wrap->number->value);
}

JSON_BOOL json_isdec(json_t wrap) {
    return json_isnum(wrap) && 
    !JSON_HAS_NO_FRACTIONAL_PART(wrap->number->value);
}

JSON_BOOL json_isstr(json_t wrap) {
    return wrap &&
    wrap->string && 
    wrap->type == JSON_STRING_TYPE;
}

JSON_BOOL json_isobj(json_t wrap) {
    return wrap &&
    wrap->object && 
    wrap->type == JSON_OBJECT_TYPE;
}

JSON_BOOL json_isarr(json_t wrap) {
    return wrap && 
    wrap->array && 
    wrap->type == JSON_ARRAY_TYPE;
}

JSON_BOOL json_is(json_t wrap, json_t other) {
    return wrap && wrap == other;
}

JSON_BOOL json_eq(json_t wrap, json_t other) {

    if (wrap && other && wrap->type == other->type) {
        switch (wrap->type) {
            case JSON_NUMBER_TYPE:  return wrap->number->value == other->number->value;
            case JSON_BOOLEAN_TYPE: return wrap->boolean->value == other->boolean->value;
            case JSON_STRING_TYPE : return json_strcmp(wrap->string->value, other->string->value) == 0;
            default: return JSON_FALSE;
        }
    }
    return JSON_FALSE;
}

/*  
    ================================
     Utilities   
    ================================
*/ 

void _json_internal_number_reset(_Number number, double value) {
    number->value = value;
}

void _json_internal_boolean_reset(_Boolean boolean, JSON_BOOL value) {
    boolean->value = value;
}

void json_number_reset(json_t json_num, double value) {
    if (JSON_TYPE_GUARD(json_num, JSON_NUMBER_TYPE)) return;

    _json_internal_number_reset(json_num->number, value);
}

void json_boolean_reset(json_t json_bool, JSON_BOOL value) {
    if (JSON_TYPE_GUARD(json_bool, JSON_BOOLEAN_TYPE)) return;

    _json_internal_boolean_reset(json_bool->boolean, value);
}

void json_string_reset(json_t json_str, const char* value) {
    if (JSON_TYPE_GUARD(json_str, JSON_STRING_TYPE)) return;

    _json_internal_string_free(json_str->string);
    json_str->string = _json_internal_string_alloc(value);
}

/*  
    ================================
     Writing cont'd   
    ================================
*/ 

void _indent_json_object_wrap_write(Writer* writer, size_t depth, json_t wrap);

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
        json_t ot = array->objects[i];

        _writef_indent(writer, depth+1);
        _indent_json_object_wrap_write(writer, depth+1, ot);
        if (array->size > 1)
            _writef_line(writer, __JSON_KEY_VALUE_SEPARATOR);
    }

    json_t ot = array->objects[array->size-1];
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

void _indent_json_object_wrap_write(Writer* writer, size_t depth, json_t wrap) {

    switch(wrap->type) {

        case JSON_NUMBER_TYPE:
            _json_internal_number_write(writer, wrap->number);
        break;

        case JSON_BOOLEAN_TYPE:
            _json_internal_boolean_write(writer, wrap->boolean);
        break;

        case JSON_STRING_TYPE:
            _json_internal_string_write(writer, wrap->string);
        break;

        case JSON_NULL_TYPE:
            writer_writef(writer, __JSON_NULL_PRINT_FMT);
        break;

        case JSON_ARRAY_TYPE:
            _indent_json_internal_array_write(writer, depth, wrap->array);
        break;

        case JSON_OBJECT_TYPE:
            _indent_json_internal_object_write(writer, depth, wrap->object);
        break;
    }
}

void json_write(Writer* writer, json_t wrap) {
    _indent_json_object_wrap_write(writer, 0, wrap);
    _writef_line(writer, "");
}

/*  
    ================================
     Lexing utils  
    ================================
*/

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

_json_ast* _json_global_arena_alloc() {
    if (_json_global_arena.end->size == JSON_ARENA_REGION_CAP) {
        _json_global_arena_grow();
    }
    _json_ast* slot = &_json_global_arena.end->buf[_json_global_arena.end->size];
    json_memset(slot, 0, sizeof(_json_ast));
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

void _json_update_lex_state(_json_lexer* lexer, _json_ast* token, size_t lookat, size_t begin) {
    lexer->cursor = lookat;
    token->col = 1 + begin - lexer->line_begin;
    token->row = lexer->line_count;
}

JSON_BOOL _lex_punct(_json_lexer* lexer, _json_ast* token) {
    switch(lexer->source[lexer->cursor]) {
        case '[': token->type = JSON_TOKEN_LBRACKET; break;
        case ']': token->type = JSON_TOKEN_RBRACKET; break;
        case '{': token->type = JSON_TOKEN_LBRACE; break;
        case '}': token->type = JSON_TOKEN_RBRACE; break;
        case ':': token->type = JSON_TOKEN_COLON; break;
        case ',': token->type = JSON_TOKEN_COMMA; break;
        default: return JSON_FALSE;
    }
    _json_update_lex_state(lexer, token, lexer->cursor + 1, lexer->cursor);
    return JSON_TRUE;
}

JSON_BOOL _lex_bool_lit(_json_lexer* lexer, _json_ast* token) {
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
        return JSON_TRUE;
    }
    return JSON_FALSE;
}

JSON_BOOL _lex_null_lit(_json_lexer* lexer, _json_ast* token) {
    size_t lookat = lexer->cursor;
    const char nullit[] = "null";
    const int nullen = sizeof(nullit) - 1;
    if (JSON_LEX_STRNCMPBUF(lexer, nullit, nullen)) {
        lookat += nullen;
        _json_update_lex_state(lexer, token, lookat, lexer->cursor);
        token->type = JSON_TOKEN_NULL;
        return JSON_TRUE;
    }
    return JSON_FALSE;
}

JSON_BOOL _lex_num_lit(_json_lexer* lexer, _json_ast* token) {
    size_t lookat = lexer->cursor;

    if (!JSON_IS_NUMBER_CHAR(lexer->source[lookat])) {
        return JSON_FALSE;
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
    json_memcpy(numlit, &lexer->source[begin], length);
    numlit[length] = '\0';

    // Parse double from buffer
    char *end;
    errno = 0;
    double as_num = strtod(numlit, &end);
    
    _json_update_lex_state(lexer, token, lookat, begin);    
    if (errno != 0 || numlit == end) {
        JSON_LOG_PARSE_ERROR("incorrect numeric format", *token);
        return JSON_FALSE;
    }
    
    token->type = JSON_TOKEN_NUMBER;
    token->as_num = as_num;
    return JSON_TRUE;
}

JSON_BOOL _lex_str_lit(_json_lexer* lexer, _json_ast* token) {
    size_t lookat = lexer->cursor;

    if (lexer->source[lookat] != '\"') {
        return JSON_FALSE;
    }

    size_t begin = lookat++;
    JSON_BOOL escaped = JSON_FALSE;
    while (lookat < lexer->tape_size) {
        if (lexer->source[lookat] == '\n') {
            break;
        } else if (escaped) {
            escaped = JSON_FALSE;
        } else if (lexer->source[lookat] == '\\') {
            escaped = JSON_TRUE;
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
        return JSON_FALSE;
    }

    token->type = JSON_TOKEN_STRING;
    token->as_str.data = &lexer->source[begin+1];
    token->as_str.size = end - begin - 1;
    return JSON_TRUE;
}

_json_ast* _lex_next_token(_json_lexer* lexer) {
    
    _json_ast* token = _json_global_arena_alloc();

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
        _json_update_lex_state(lexer, token, lexer->cursor, lexer->cursor);
        token->type = JSON_TOKEN_EOF;
        __LEXER_DEBUG_PRINT(token->type);
        return token;    
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

    __LEXER_DEBUG_PRINT(token->type);
    return token;
}

/*  
    ================================
     Parsing    
    ================================
*/

const _json_ast_type JSON_FIRST_SET_JSON       = JSON_TOKEN_STRING | JSON_TOKEN_NUMBER | JSON_TOKEN_BOOLEAN | JSON_TOKEN_NULL | JSON_TOKEN_LBRACE | JSON_TOKEN_LBRACKET;

const _json_ast_type JSON_FOLLOW_SET_ARRAYBODY  = JSON_TOKEN_RBRACKET;
const _json_ast_type JSON_FOLLOW_SET_ARRAYTAIL  = JSON_TOKEN_RBRACKET;
const _json_ast_type JSON_FOLLOW_SET_OBJECTBODY = JSON_TOKEN_RBRACE;
const _json_ast_type JSON_FOLLOW_SET_OBJECTTAIL = JSON_TOKEN_RBRACE;

JSON_BOOL _json_parse_json(_json_parser* parser, _json_ast** parent);
JSON_BOOL _json_parse_array_body(_json_parser* parser, _json_ast** parent);
JSON_BOOL _json_parse_array_tail(_json_parser* parser, _json_ast** sibling);
JSON_BOOL _json_parse_object_body(_json_parser* parser, _json_ast** parent);
JSON_BOOL _json_parse_object_tail(_json_parser* parser, _json_ast** sibling);

void _json_parser_init(_json_parser* parser, _json_lexer* lexer) {
    parser->lexer = lexer;
    parser->lookahead = _lex_next_token(lexer);
}

JSON_BOOL _json_parse_match(_json_parser* parser, _json_ast_type type) {
    if (parser->lookahead->type == type) {
        if (parser->lookahead->type != JSON_TOKEN_EOF)
            parser->lookahead = _lex_next_token(parser->lexer);
        return JSON_TRUE;
    }
    JSON_LOG_PARSE_ERROR_MATCH(type, *parser->lookahead);
    return JSON_FALSE;
}

JSON_BOOL _json_parse_match_set(_json_parser* parser, _json_ast** parent, _json_ast_type type) {
    if (parser->lookahead->type == type) {
        *parent = parser->lookahead;
        parser->lookahead = _lex_next_token(parser->lexer);
        return JSON_TRUE;
    }
    JSON_LOG_PARSE_ERROR_MATCH(type, *parser->lookahead);
    return JSON_FALSE;
}

JSON_BOOL _json_parse_json(_json_parser* parser, _json_ast** parent) {
    _json_ast* constructed = NULL;
    _json_ast* child = NULL;
    
    const _json_ast* const lookahead = parser->lookahead;
    if (lookahead->type & JSON_TOKEN_NUMBER) {
        if (_json_parse_match_set(parser, &constructed, JSON_TOKEN_NUMBER)) {
            *parent = constructed;
            return JSON_TRUE;
        }
    } else if (lookahead->type & JSON_TOKEN_NULL) {
        if (_json_parse_match_set(parser, &constructed, JSON_TOKEN_NULL)) {
            *parent = constructed;
            return JSON_TRUE;
        }
    } else if (lookahead->type & JSON_TOKEN_BOOLEAN) {
        if (_json_parse_match_set(parser, &constructed, JSON_TOKEN_BOOLEAN)) {
            *parent = constructed;
            return JSON_TRUE;
        }
    } else if (lookahead->type & JSON_TOKEN_STRING) {
        if (_json_parse_match_set(parser, &constructed, JSON_TOKEN_STRING)) {
            *parent = constructed;
            return JSON_TRUE;
        }
    } else if (lookahead->type & JSON_TOKEN_LBRACKET) {
        if (
            _json_parse_match_set(parser, &constructed, JSON_TOKEN_LBRACKET) && 
            _json_parse_array_body(parser, &child) &&
            _json_parse_match(parser, JSON_TOKEN_RBRACKET)
        ) {
            constructed->child = child;
            *parent = constructed;
            return JSON_TRUE;
        }
    } else if (lookahead->type & JSON_TOKEN_LBRACE) {
        if (
            _json_parse_match_set(parser, &constructed, JSON_TOKEN_LBRACE) && 
            _json_parse_object_body(parser, &child) && 
            _json_parse_match(parser, JSON_TOKEN_RBRACE)
        ) {
            constructed->child = child;
            *parent = constructed;
            return JSON_TRUE;
        }
    }
    JSON_LOG_PARSE_ERROR("expected a json", *parser->lookahead);
    return JSON_FALSE;
}

JSON_BOOL _json_parse_array_body(_json_parser* parser, _json_ast** parent) {
    _json_ast* sibling = NULL;    
    const _json_ast* const  lookahead = parser->lookahead;
    if (lookahead->type & JSON_FIRST_SET_JSON) {
        if (
            _json_parse_json(parser, parent) && 
            _json_parse_array_tail(parser, &sibling)
        ) { // JSON ARRAYTAIL
            (*parent)->next = sibling;
            return JSON_TRUE;
        }
    } else if (lookahead->type & JSON_FOLLOW_SET_ARRAYBODY) { // epsilon
        return JSON_TRUE;
    }
    JSON_LOG_PARSE_ERROR("bad array format", *parser->lookahead);
    return JSON_FALSE;
}

JSON_BOOL _json_parse_array_tail(_json_parser* parser, _json_ast** sibling) {
    const _json_ast* const lookahead = parser->lookahead;
    if (lookahead->type & JSON_TOKEN_COMMA) {
        if (
            _json_parse_match(parser, JSON_TOKEN_COMMA) && 
            _json_parse_array_body(parser, sibling)
        ) { // comma ARRAYBODY
            return JSON_TRUE;
        }
    } else if (lookahead->type & JSON_FOLLOW_SET_ARRAYTAIL) { // epsilon
        return JSON_TRUE;
    }
    JSON_LOG_PARSE_ERROR("unterminated array", *parser->lookahead);
    return JSON_FALSE;
}

JSON_BOOL _json_parse_object_body(_json_parser* parser, _json_ast** parent) {
    _json_ast* sibling1 = NULL;
    _json_ast* sibling2 = NULL;
    _json_ast* lookahead = parser->lookahead;
    if (lookahead->type & JSON_TOKEN_STRING) {
        if (
            _json_parse_match_set(parser, parent, JSON_TOKEN_STRING) &&
            _json_parse_match(parser, JSON_TOKEN_COLON) &&
            _json_parse_json(parser, &sibling1) &&
            _json_parse_object_tail(parser, &sibling2)
        ) { // strlit colon JSON OBJECTTAIL
            sibling1->next = sibling2;
            (*parent)->next = sibling1;
            return JSON_TRUE;
        }
    } else if (lookahead->type & JSON_FOLLOW_SET_OBJECTBODY) { // epsilon
        return JSON_TRUE;
    }
    JSON_LOG_PARSE_ERROR("bad object format", *parser->lookahead);
    return JSON_FALSE;
}

JSON_BOOL _json_parse_object_tail(_json_parser* parser, _json_ast** parent) {
    _json_ast* lookahead = parser->lookahead;
    if (lookahead->type & JSON_TOKEN_COMMA) {
        if (
            _json_parse_match(parser, JSON_TOKEN_COMMA) && 
            _json_parse_object_body(parser, parent)
        ) { // comma OBJECTBODY
            return JSON_TRUE;
        }
    } else if (lookahead->type & JSON_FOLLOW_SET_OBJECTTAIL) { // epsilon
        return JSON_TRUE;
    }
    JSON_LOG_PARSE_ERROR("unterminated object", *parser->lookahead);
    return JSON_FALSE;
}

json_t _json_build_from_ast(const _json_ast* const base) {
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
            json_t array = json_array_alloc();
            const _json_ast* ptr = base->child;
            while (ptr) {
                json_push(array, _json_build_from_ast(ptr));
                ptr = ptr->next;
            }
            return array;
        }
        case JSON_TOKEN_LBRACE: {
            json_t object = json_object_alloc();
            const _json_ast* ptr = base->child;
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

json_t _json_internal_parse(const char* buf, size_t len) {
    _json_global_arena_ensure_init();

    json_t wrap = NULL;

    _json_lexer lexer = {0};
    _json_lexer_init(&lexer, buf, len);
    _json_parser parser = {0};
    _json_parser_init(&parser, &lexer);

    _json_ast* base = parser.lookahead;
    if (
        _json_parse_json(&parser, &base) && 
        _json_parse_match(&parser, JSON_TOKEN_EOF)
    ) {
        wrap = _json_build_from_ast(base);
    } else {
        fprintf(stderr, "ERROR: %s\n", JSON_GET_PARSE_ERROR());
    }
    
    _json_global_arena_reset();
    return wrap;
}

json_t json_parse_cstring(const char* _cstr) {
    return json_parse_string(_cstr, json_cstr_len(_cstr));
}

json_t json_parse_string(const char* buf, size_t len) {    
    return _json_internal_parse(buf, len);
}

json_t json_parse_file(const char* filename) {

    size_t len;
    const char* file_buf = _read_file_to_cstr(filename, &len);
    if (!file_buf) return NULL;

    json_t ret = json_parse_string(file_buf, len);
    free((char*)file_buf);

    return ret;
}

#endif // JSON_IMPLEMENTATION
