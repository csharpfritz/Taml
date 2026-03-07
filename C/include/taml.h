/**
 * TAML - Tab Annotated Markup Language
 * C implementation of TAML parser, serializer, and validator
 * 
 * Copyright (c) 2024
 * MIT License
 */

#ifndef TAML_H
#define TAML_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version Information */
#define TAML_VERSION_MAJOR 0
#define TAML_VERSION_MINOR 1
#define TAML_VERSION_PATCH 0

/* Error Codes */
typedef enum {
    TAML_OK = 0,
    TAML_ERROR_NULL_INPUT = -1,
    TAML_ERROR_MEMORY = -2,
    TAML_ERROR_INVALID_INDENTATION = -3,
    TAML_ERROR_INVALID_TAB_IN_KEY = -4,
    TAML_ERROR_INVALID_TAB_IN_VALUE = -5,
    TAML_ERROR_INCONSISTENT_INDENT = -6,
    TAML_ERROR_ORPHANED_LINE = -7,
    TAML_ERROR_PARENT_WITH_VALUE = -8,
    TAML_ERROR_EMPTY_KEY = -9,
    TAML_ERROR_MIXED_INDENT = -10,
    TAML_ERROR_PARSE_FAILED = -11
} taml_error_t;

/* Value Types */
typedef enum {
    TAML_TYPE_NULL,
    TAML_TYPE_BOOL,
    TAML_TYPE_INT,
    TAML_TYPE_FLOAT,
    TAML_TYPE_STRING,
    TAML_TYPE_OBJECT,
    TAML_TYPE_ARRAY
} taml_type_t;

/* Forward declarations */
typedef struct taml_node taml_node_t;
typedef struct taml_object taml_object_t;
typedef struct taml_array taml_array_t;

/* TAML Value */
typedef struct taml_value {
    taml_type_t type;
    union {
        bool bool_val;
        long long int_val;
        double float_val;
        char *string_val;
        taml_object_t *object_val;
        taml_array_t *array_val;
    } data;
} taml_value_t;

/* TAML Object (key-value pairs) */
typedef struct taml_pair {
    char *key;
    taml_value_t *value;
    struct taml_pair *next;
} taml_pair_t;

struct taml_object {
    taml_pair_t *pairs;
    size_t count;
};

/* TAML Array */
typedef struct taml_array_item {
    taml_value_t *value;
    struct taml_array_item *next;
} taml_array_item_t;

struct taml_array {
    taml_array_item_t *items;
    size_t count;
};

/* TAML Document */
typedef struct taml_document {
    taml_value_t *root;
    char *error_message;
    int error_line;
} taml_document_t;

/* Parser Options */
typedef struct taml_parser_options {
    bool strict;              /* Strict parsing mode */
    bool type_conversion;     /* Convert strings to native types */
} taml_parser_options_t;

/* Serializer Options */
typedef struct taml_serializer_options {
    int indent_level;         /* Starting indentation level */
    bool type_conversion;     /* Convert native types to strings */
} taml_serializer_options_t;

/* Core API - Parser */

/**
 * Parse TAML text into a document structure
 * 
 * @param text TAML formatted text
 * @param options Parser options (can be NULL for defaults)
 * @return Parsed document or NULL on error
 */
taml_document_t* taml_parse(const char *text, const taml_parser_options_t *options);

/**
 * Parse TAML text from a file
 * 
 * @param filename Path to TAML file
 * @param options Parser options (can be NULL for defaults)
 * @return Parsed document or NULL on error
 */
taml_document_t* taml_parse_file(const char *filename, const taml_parser_options_t *options);

/* Core API - Serializer */

/**
 * Serialize a TAML document to string
 * 
 * @param doc TAML document
 * @param options Serializer options (can be NULL for defaults)
 * @return TAML formatted string (caller must free) or NULL on error
 */
char* taml_stringify(const taml_document_t *doc, const taml_serializer_options_t *options);

/**
 * Serialize a TAML value to string
 * 
 * @param value TAML value
 * @param options Serializer options (can be NULL for defaults)
 * @return TAML formatted string (caller must free) or NULL on error
 */
char* taml_stringify_value(const taml_value_t *value, const taml_serializer_options_t *options);

/**
 * Write TAML document to a file
 * 
 * @param doc TAML document
 * @param filename Output file path
 * @param options Serializer options (can be NULL for defaults)
 * @return TAML_OK on success, error code on failure
 */
taml_error_t taml_write_file(const taml_document_t *doc, const char *filename, 
                              const taml_serializer_options_t *options);

/* Core API - Validator */

/**
 * Validate TAML text without parsing
 * 
 * @param text TAML formatted text
 * @param error_msg Output buffer for error message (can be NULL)
 * @param error_msg_size Size of error message buffer
 * @param error_line Output variable for error line number (can be NULL)
 * @return TAML_OK if valid, error code otherwise
 */
taml_error_t taml_validate(const char *text, char *error_msg, 
                           size_t error_msg_size, int *error_line);

/**
 * Validate TAML file without parsing
 * 
 * @param filename Path to TAML file
 * @param error_msg Output buffer for error message (can be NULL)
 * @param error_msg_size Size of error message buffer
 * @param error_line Output variable for error line number (can be NULL)
 * @return TAML_OK if valid, error code otherwise
 */
taml_error_t taml_validate_file(const char *filename, char *error_msg,
                                 size_t error_msg_size, int *error_line);

/* Format Conversion API */

/**
 * Convert JSON to TAML
 * 
 * @param json_text JSON formatted text
 * @return TAML document or NULL on error
 */
taml_document_t* taml_from_json(const char *json_text);

/**
 * Convert TAML to JSON
 * 
 * @param doc TAML document
 * @return JSON formatted string (caller must free) or NULL on error
 */
char* taml_to_json(const taml_document_t *doc);

/**
 * Convert XML to TAML
 * 
 * @param xml_text XML formatted text
 * @return TAML document or NULL on error
 */
taml_document_t* taml_from_xml(const char *xml_text);

/**
 * Convert TAML to XML
 * 
 * @param doc TAML document
 * @return XML formatted string (caller must free) or NULL on error
 */
char* taml_to_xml(const taml_document_t *doc);

/**
 * Convert YAML to TAML
 * 
 * @param yaml_text YAML formatted text
 * @return TAML document or NULL on error
 */
taml_document_t* taml_from_yaml(const char *yaml_text);

/**
 * Convert TAML to YAML
 * 
 * @param doc TAML document
 * @return YAML formatted string (caller must free) or NULL on error
 */
char* taml_to_yaml(const taml_document_t *doc);

/* Memory Management */

/**
 * Free a TAML document and all associated memory
 * 
 * @param doc Document to free
 */
void taml_free_document(taml_document_t *doc);

/**
 * Free a TAML value and all associated memory
 * 
 * @param value Value to free
 */
void taml_free_value(taml_value_t *value);

/**
 * Free a string returned by TAML functions
 * 
 * @param str String to free
 */
void taml_free_string(char *str);

/* Value Creation and Access */

/**
 * Create a new TAML object
 * 
 * @return New TAML object or NULL on error
 */
taml_value_t* taml_create_object(void);

/**
 * Create a new TAML array
 * 
 * @return New TAML array or NULL on error
 */
taml_value_t* taml_create_array(void);

/**
 * Create a string value
 * 
 * @param str String value
 * @return New TAML string value or NULL on error
 */
taml_value_t* taml_create_string(const char *str);

/**
 * Create a number value
 * 
 * @param num Number value
 * @return New TAML number value
 */
taml_value_t* taml_create_int(long long num);

/**
 * Create a float value
 * 
 * @param num Float value
 * @return New TAML float value
 */
taml_value_t* taml_create_float(double num);

/**
 * Create a boolean value
 * 
 * @param val Boolean value
 * @return New TAML boolean value
 */
taml_value_t* taml_create_bool(bool val);

/**
 * Create a null value
 * 
 * @return New TAML null value
 */
taml_value_t* taml_create_null(void);

/**
 * Get a value from an object by key
 * 
 * @param obj Object value
 * @param key Key to look up
 * @return Value or NULL if not found
 */
taml_value_t* taml_object_get(const taml_value_t *obj, const char *key);

/**
 * Set a value in an object
 * 
 * @param obj Object value
 * @param key Key to set
 * @param value Value to set
 * @return TAML_OK on success, error code on failure
 */
taml_error_t taml_object_set(taml_value_t *obj, const char *key, taml_value_t *value);

/**
 * Get array size
 * 
 * @param arr Array value
 * @return Array size or 0 if not an array
 */
size_t taml_array_size(const taml_value_t *arr);

/**
 * Get value from array by index
 * 
 * @param arr Array value
 * @param index Index
 * @return Value or NULL if out of bounds
 */
taml_value_t* taml_array_get(const taml_value_t *arr, size_t index);

/**
 * Append value to array
 * 
 * @param arr Array value
 * @param value Value to append
 * @return TAML_OK on success, error code on failure
 */
taml_error_t taml_array_append(taml_value_t *arr, taml_value_t *value);

/* Utility Functions */

/**
 * Get error message for error code
 * 
 * @param error Error code
 * @return Error message string
 */
const char* taml_error_string(taml_error_t error);

/**
 * Get library version string
 * 
 * @return Version string
 */
const char* taml_version(void);

#ifdef __cplusplus
}
#endif

#endif /* TAML_H */
