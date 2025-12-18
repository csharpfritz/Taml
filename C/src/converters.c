/**
 * TAML Format Converters
 * Converts between TAML and JSON/XML/YAML formats
 */

#define _POSIX_C_SOURCE 200809L

#include "../include/taml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 4096

/* Internal helper functions */
static char* escape_json_string(const char *str);
static char* value_to_json(const taml_value_t *value, int indent);
static taml_value_t* json_parse_value(const char **json);
static void skip_whitespace(const char **str);

/* Escape JSON string */
static char* escape_json_string(const char *str) {
    if (!str) return strdup("null");
    
    size_t len = strlen(str);
    char *escaped = (char*)malloc(len * 2 + 3); /* Worst case: every char escaped + quotes */
    if (!escaped) return NULL;
    
    char *p = escaped;
    *p++ = '"';
    
    for (size_t i = 0; i < len; i++) {
        switch (str[i]) {
            case '"':  *p++ = '\\'; *p++ = '"'; break;
            case '\\': *p++ = '\\'; *p++ = '\\'; break;
            case '\b': *p++ = '\\'; *p++ = 'b'; break;
            case '\f': *p++ = '\\'; *p++ = 'f'; break;
            case '\n': *p++ = '\\'; *p++ = 'n'; break;
            case '\r': *p++ = '\\'; *p++ = 'r'; break;
            case '\t': *p++ = '\\'; *p++ = 't'; break;
            default:   *p++ = str[i]; break;
        }
    }
    
    *p++ = '"';
    *p = '\0';
    
    return escaped;
}

/* Convert TAML value to JSON string */
static char* value_to_json(const taml_value_t *value, int indent) {
    if (!value) return strdup("null");
    
    char *result = NULL;
    size_t size = BUFFER_SIZE;
    result = (char*)malloc(size);
    if (!result) return NULL;
    result[0] = '\0';
    
    char *indent_str = (char*)malloc(indent + 1);
    memset(indent_str, ' ', indent);
    indent_str[indent] = '\0';
    
    switch (value->type) {
        case TAML_TYPE_NULL:
            strcpy(result, "null");
            break;
            
        case TAML_TYPE_BOOL:
            strcpy(result, value->data.bool_val ? "true" : "false");
            break;
            
        case TAML_TYPE_INT:
            snprintf(result, size, "%lld", value->data.int_val);
            break;
            
        case TAML_TYPE_FLOAT:
            snprintf(result, size, "%g", value->data.float_val);
            break;
            
        case TAML_TYPE_STRING: {
            char *escaped = escape_json_string(value->data.string_val);
            if (escaped) {
                strcpy(result, escaped);
                free(escaped);
            }
            break;
        }
        
        case TAML_TYPE_OBJECT: {
            strcpy(result, "{\n");
            
            if (value->data.object_val) {
                taml_pair_t *pair = value->data.object_val->pairs;
                bool first = true;
                
                while (pair) {
                    if (!first) strcat(result, ",\n");
                    first = false;
                    
                    strcat(result, indent_str);
                    strcat(result, "  ");
                    
                    char *key = escape_json_string(pair->key);
                    strcat(result, key);
                    free(key);
                    
                    strcat(result, ": ");
                    
                    char *val = value_to_json(pair->value, indent + 2);
                    strcat(result, val);
                    free(val);
                    
                    pair = pair->next;
                }
            }
            
            strcat(result, "\n");
            strcat(result, indent_str);
            strcat(result, "}");
            break;
        }
        
        case TAML_TYPE_ARRAY: {
            strcpy(result, "[\n");
            
            if (value->data.array_val) {
                taml_array_item_t *item = value->data.array_val->items;
                bool first = true;
                
                while (item) {
                    if (!first) strcat(result, ",\n");
                    first = false;
                    
                    strcat(result, indent_str);
                    strcat(result, "  ");
                    
                    char *val = value_to_json(item->value, indent + 2);
                    strcat(result, val);
                    free(val);
                    
                    item = item->next;
                }
            }
            
            strcat(result, "\n");
            strcat(result, indent_str);
            strcat(result, "]");
            break;
        }
    }
    
    free(indent_str);
    return result;
}

/* Skip whitespace in JSON */
static void skip_whitespace(const char **str) {
    while (**str && isspace(**str)) {
        (*str)++;
    }
}

/* Simple JSON parser (basic implementation) */
static taml_value_t* json_parse_value(const char **json) {
    skip_whitespace(json);
    
    if (!**json) return NULL;
    
    /* Parse null */
    if (strncmp(*json, "null", 4) == 0) {
        *json += 4;
        return taml_create_null();
    }
    
    /* Parse boolean */
    if (strncmp(*json, "true", 4) == 0) {
        *json += 4;
        return taml_create_bool(true);
    }
    if (strncmp(*json, "false", 5) == 0) {
        *json += 5;
        return taml_create_bool(false);
    }
    
    /* Parse string */
    if (**json == '"') {
        (*json)++;
        const char *start = *json;
        while (**json && **json != '"') {
            if (**json == '\\') (*json)++;
            (*json)++;
        }
        size_t len = *json - start;
        char *str = strndup(start, len);
        if (**json == '"') (*json)++;
        
        taml_value_t *val = taml_create_string(str);
        free(str);
        return val;
    }
    
    /* Parse number */
    if (isdigit(**json) || **json == '-') {
        char *end;
        double d = strtod(*json, &end);
        *json = end;
        
        if (strchr(*json - (end - *json), '.')) {
            return taml_create_float(d);
        } else {
            return taml_create_int((long long)d);
        }
    }
    
    /* Parse object */
    if (**json == '{') {
        (*json)++;
        taml_value_t *obj = taml_create_object();
        
        skip_whitespace(json);
        if (**json == '}') {
            (*json)++;
            return obj;
        }
        
        while (**json && **json != '}') {
            skip_whitespace(json);
            
            /* Parse key */
            if (**json != '"') break;
            (*json)++;
            const char *key_start = *json;
            while (**json && **json != '"') {
                if (**json == '\\') (*json)++;
                (*json)++;
            }
            size_t key_len = *json - key_start;
            char *key = strndup(key_start, key_len);
            if (**json == '"') (*json)++;
            
            skip_whitespace(json);
            if (**json == ':') (*json)++;
            
            /* Parse value */
            taml_value_t *val = json_parse_value(json);
            if (val) {
                taml_object_set(obj, key, val);
            }
            free(key);
            
            skip_whitespace(json);
            if (**json == ',') (*json)++;
        }
        
        if (**json == '}') (*json)++;
        return obj;
    }
    
    /* Parse array */
    if (**json == '[') {
        (*json)++;
        taml_value_t *arr = taml_create_array();
        
        skip_whitespace(json);
        if (**json == ']') {
            (*json)++;
            return arr;
        }
        
        while (**json && **json != ']') {
            skip_whitespace(json);
            
            taml_value_t *val = json_parse_value(json);
            if (val) {
                taml_array_append(arr, val);
            }
            
            skip_whitespace(json);
            if (**json == ',') (*json)++;
        }
        
        if (**json == ']') (*json)++;
        return arr;
    }
    
    return NULL;
}

/* Convert JSON to TAML */
taml_document_t* taml_from_json(const char *json_text) {
    if (!json_text) return NULL;
    
    const char *p = json_text;
    taml_value_t *root = json_parse_value(&p);
    
    if (!root) return NULL;
    
    taml_document_t *doc = (taml_document_t*)malloc(sizeof(taml_document_t));
    if (!doc) {
        taml_free_value(root);
        return NULL;
    }
    
    doc->root = root;
    doc->error_message = NULL;
    doc->error_line = 0;
    
    return doc;
}

/* Convert TAML to JSON */
char* taml_to_json(const taml_document_t *doc) {
    if (!doc || !doc->root) return NULL;
    
    return value_to_json(doc->root, 0);
}

/* XML and YAML converters - basic stubs */
/* These would require full XML/YAML parser implementations */

taml_document_t* taml_from_xml(const char *xml_text) {
    /* Basic XML parser would go here */
    /* For now, return NULL to indicate not implemented */
    (void)xml_text;
    return NULL;
}

char* taml_to_xml(const taml_document_t *doc) {
    /* Basic XML serializer would go here */
    (void)doc;
    return NULL;
}

taml_document_t* taml_from_yaml(const char *yaml_text) {
    /* Basic YAML parser would go here */
    /* For now, return NULL to indicate not implemented */
    (void)yaml_text;
    return NULL;
}

char* taml_to_yaml(const taml_document_t *doc) {
    /* Basic YAML serializer would go here */
    (void)doc;
    return NULL;
}
