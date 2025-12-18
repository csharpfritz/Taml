/**
 * TAML Serializer Implementation
 */

#define _POSIX_C_SOURCE 200809L

#include "../include/taml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_BUFFER_SIZE 4096
#define BUFFER_GROW_SIZE 2048

typedef struct {
    char *buffer;
    size_t size;
    size_t capacity;
} string_builder_t;

/* Internal helper functions */
static string_builder_t* sb_create(void);
static void sb_free(string_builder_t *sb);
static bool sb_append(string_builder_t *sb, const char *str);
static bool sb_append_char(string_builder_t *sb, char c);
static bool sb_append_indent(string_builder_t *sb, int level);
static char* sb_to_string(string_builder_t *sb);
static bool serialize_value(string_builder_t *sb, const taml_value_t *value, 
                           int indent_level, const taml_serializer_options_t *options);

/* String builder implementation */
static string_builder_t* sb_create(void) {
    string_builder_t *sb = (string_builder_t*)malloc(sizeof(string_builder_t));
    if (!sb) return NULL;
    
    sb->capacity = INITIAL_BUFFER_SIZE;
    sb->size = 0;
    sb->buffer = (char*)malloc(sb->capacity);
    if (!sb->buffer) {
        free(sb);
        return NULL;
    }
    sb->buffer[0] = '\0';
    
    return sb;
}

static void sb_free(string_builder_t *sb) {
    if (sb) {
        if (sb->buffer) free(sb->buffer);
        free(sb);
    }
}

static bool sb_append(string_builder_t *sb, const char *str) {
    if (!sb || !str) return false;
    
    size_t len = strlen(str);
    if (sb->size + len + 1 > sb->capacity) {
        size_t new_capacity = sb->capacity + BUFFER_GROW_SIZE;
        while (sb->size + len + 1 > new_capacity) {
            new_capacity += BUFFER_GROW_SIZE;
        }
        
        char *new_buffer = (char*)realloc(sb->buffer, new_capacity);
        if (!new_buffer) return false;
        
        sb->buffer = new_buffer;
        sb->capacity = new_capacity;
    }
    
    strcpy(sb->buffer + sb->size, str);
    sb->size += len;
    
    return true;
}

static bool sb_append_char(string_builder_t *sb, char c) {
    if (!sb) return false;
    
    if (sb->size + 2 > sb->capacity) {
        size_t new_capacity = sb->capacity + BUFFER_GROW_SIZE;
        char *new_buffer = (char*)realloc(sb->buffer, new_capacity);
        if (!new_buffer) return false;
        
        sb->buffer = new_buffer;
        sb->capacity = new_capacity;
    }
    
    sb->buffer[sb->size++] = c;
    sb->buffer[sb->size] = '\0';
    
    return true;
}

static bool sb_append_indent(string_builder_t *sb, int level) {
    for (int i = 0; i < level; i++) {
        if (!sb_append_char(sb, '\t')) return false;
    }
    return true;
}

static char* sb_to_string(string_builder_t *sb) {
    if (!sb || !sb->buffer) return NULL;
    return strdup(sb->buffer);
}

/* Serialize a value */
static bool serialize_value(string_builder_t *sb, const taml_value_t *value, 
                           int indent_level, const taml_serializer_options_t *options) {
    if (!sb || !value) return false;
    
    switch (value->type) {
        case TAML_TYPE_NULL:
            return sb_append(sb, "~");
            
        case TAML_TYPE_BOOL:
            return sb_append(sb, value->data.bool_val ? "true" : "false");
            
        case TAML_TYPE_INT: {
            char buf[32];
            snprintf(buf, sizeof(buf), "%lld", value->data.int_val);
            return sb_append(sb, buf);
        }
        
        case TAML_TYPE_FLOAT: {
            char buf[32];
            snprintf(buf, sizeof(buf), "%g", value->data.float_val);
            return sb_append(sb, buf);
        }
        
        case TAML_TYPE_STRING:
            if (!value->data.string_val) {
                return sb_append(sb, "~");
            } else if (strlen(value->data.string_val) == 0) {
                return sb_append(sb, "\"\"");
            } else {
                return sb_append(sb, value->data.string_val);
            }
            
        case TAML_TYPE_OBJECT: {
            if (!value->data.object_val) return false;
            
            taml_pair_t *pair = value->data.object_val->pairs;
            while (pair) {
                sb_append_indent(sb, indent_level);
                sb_append(sb, pair->key);
                
                if (pair->value) {
                    if (pair->value->type == TAML_TYPE_OBJECT || 
                        pair->value->type == TAML_TYPE_ARRAY) {
                        sb_append_char(sb, '\n');
                        serialize_value(sb, pair->value, indent_level + 1, options);
                    } else {
                        sb_append_char(sb, '\t');
                        serialize_value(sb, pair->value, indent_level, options);
                        sb_append_char(sb, '\n');
                    }
                } else {
                    sb_append_char(sb, '\n');
                }
                
                pair = pair->next;
            }
            return true;
        }
        
        case TAML_TYPE_ARRAY: {
            if (!value->data.array_val) return false;
            
            taml_array_item_t *item = value->data.array_val->items;
            while (item) {
                sb_append_indent(sb, indent_level);
                
                if (item->value) {
                    if (item->value->type == TAML_TYPE_OBJECT || 
                        item->value->type == TAML_TYPE_ARRAY) {
                        serialize_value(sb, item->value, indent_level + 1, options);
                    } else {
                        serialize_value(sb, item->value, indent_level, options);
                        sb_append_char(sb, '\n');
                    }
                }
                
                item = item->next;
            }
            return true;
        }
        
        default:
            return false;
    }
}

/* Main stringify function */
char* taml_stringify(const taml_document_t *doc, const taml_serializer_options_t *options) {
    if (!doc || !doc->root) return NULL;
    
    int indent_level = options ? options->indent_level : 0;
    
    string_builder_t *sb = sb_create();
    if (!sb) return NULL;
    
    if (!serialize_value(sb, doc->root, indent_level, options)) {
        sb_free(sb);
        return NULL;
    }
    
    char *result = sb_to_string(sb);
    sb_free(sb);
    
    return result;
}

/* Stringify a value */
char* taml_stringify_value(const taml_value_t *value, const taml_serializer_options_t *options) {
    if (!value) return NULL;
    
    int indent_level = options ? options->indent_level : 0;
    
    string_builder_t *sb = sb_create();
    if (!sb) return NULL;
    
    if (!serialize_value(sb, value, indent_level, options)) {
        sb_free(sb);
        return NULL;
    }
    
    char *result = sb_to_string(sb);
    sb_free(sb);
    
    return result;
}

/* Write to file */
taml_error_t taml_write_file(const taml_document_t *doc, const char *filename, 
                              const taml_serializer_options_t *options) {
    if (!doc || !filename) return TAML_ERROR_NULL_INPUT;
    
    char *str = taml_stringify(doc, options);
    if (!str) return TAML_ERROR_PARSE_FAILED;
    
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        free(str);
        return TAML_ERROR_PARSE_FAILED;
    }
    
    fprintf(fp, "%s", str);
    fclose(fp);
    free(str);
    
    return TAML_OK;
}
