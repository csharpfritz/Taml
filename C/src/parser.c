/**
 * TAML Parser Implementation
 */

#define _POSIX_C_SOURCE 200809L

#include "../include/taml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAB '\t'
#define NULL_VALUE "~"
#define EMPTY_STRING "\"\""

/* Internal parser state */
typedef struct {
    const char *text;
    size_t pos;
    int line;
    int col;
    bool strict;
    bool type_conversion;
    char error_msg[256];
    int error_line;
} parser_state_t;

/* Internal helper functions */
static bool is_numeric(const char *str);
static bool is_boolean(const char *str);
static taml_value_t* convert_value(const char *str, bool type_conversion);
static int count_leading_tabs(const char *line);
static bool has_tab_in_content(const char *str);
static char* trim_trailing_whitespace(char *str);
static taml_value_t* parse_lines(char **lines, int start, int end, int parent_indent, 
                                 parser_state_t *state);

/* Check if string is numeric */
static bool is_numeric(const char *str) {
    if (!str || !*str) return false;
    
    bool has_dot = false;
    bool has_digit = false;
    int i = 0;
    
    /* Allow leading sign */
    if (str[i] == '+' || str[i] == '-') i++;
    
    while (str[i]) {
        if (isdigit(str[i])) {
            has_digit = true;
        } else if (str[i] == '.') {
            if (has_dot) return false; /* Multiple dots */
            has_dot = true;
        } else {
            return false;
        }
        i++;
    }
    
    return has_digit;
}

/* Check if string is boolean */
static bool is_boolean(const char *str) {
    return strcmp(str, "true") == 0 || strcmp(str, "false") == 0;
}

/* Count leading tabs in a line */
static int count_leading_tabs(const char *line) {
    int count = 0;
    while (line[count] == TAB) {
        count++;
    }
    return count;
}

/* Check if string contains tabs */
static bool has_tab_in_content(const char *str) {
    return strchr(str, TAB) != NULL;
}

/* Trim trailing whitespace */
static char* trim_trailing_whitespace(char *str) {
    if (!str) return str;
    
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    
    return str;
}

/* Convert string value to appropriate type */
static taml_value_t* convert_value(const char *str, bool type_conversion) {
    taml_value_t *value = (taml_value_t*)malloc(sizeof(taml_value_t));
    if (!value) return NULL;
    
    /* Check for null */
    if (strcmp(str, NULL_VALUE) == 0) {
        value->type = TAML_TYPE_NULL;
        return value;
    }
    
    /* Check for empty string */
    if (strcmp(str, EMPTY_STRING) == 0) {
        value->type = TAML_TYPE_STRING;
        value->data.string_val = strdup("");
        return value;
    }
    
    if (type_conversion) {
        /* Check for boolean */
        if (is_boolean(str)) {
            value->type = TAML_TYPE_BOOL;
            value->data.bool_val = (strcmp(str, "true") == 0);
            return value;
        }
        
        /* Check for number */
        if (is_numeric(str)) {
            if (strchr(str, '.')) {
                value->type = TAML_TYPE_FLOAT;
                value->data.float_val = atof(str);
            } else {
                value->type = TAML_TYPE_INT;
                value->data.int_val = atoll(str);
            }
            return value;
        }
    }
    
    /* Default to string */
    value->type = TAML_TYPE_STRING;
    value->data.string_val = strdup(str);
    return value;
}

/* Parse lines into TAML structure */
static taml_value_t* parse_lines(char **lines, int start, int end, int parent_indent, 
                                 parser_state_t *state) {
    if (start >= end) return NULL;
    
    taml_value_t *result = NULL;
    bool is_array = true;
    int i = start;
    
    /* Determine if this is an array or object */
    while (i < end) {
        char *line = lines[i];
        if (!line || !*line) {
            i++;
            continue;
        }
        
        int indent = count_leading_tabs(line);
        if (indent <= parent_indent) break;
        if (indent != parent_indent + 1) {
            if (state->strict) {
                snprintf(state->error_msg, sizeof(state->error_msg),
                        "Invalid indentation level at line %d", i + 1);
                state->error_line = i + 1;
            }
            i++;
            continue;
        }
        
        char *content = line + indent;
        /* If line has a tab separator, it's a key-value pair (object) */
        if (strchr(content, TAB)) {
            is_array = false;
            break;
        }
        
        /* Check if this line has children - if so, it's a parent key (object) */
        int j = i + 1;
        while (j < end) {
            if (!lines[j] || !*lines[j]) {
                j++;
                continue;
            }
            int child_indent = count_leading_tabs(lines[j]);
            if (child_indent <= indent) break;
            if (child_indent == indent + 1) {
                /* Has children, so it's an object with parent keys */
                is_array = false;
                break;
            }
            j++;
        }
        
        if (!is_array) break;
        i++;
    }
    
    /* Create appropriate structure */
    if (is_array) {
        result = taml_create_array();
        if (!result) return NULL;
        
        i = start;
        while (i < end) {
            char *line = lines[i];
            if (!line || !*line) {
                i++;
                continue;
            }
            
            int indent = count_leading_tabs(line);
            if (indent <= parent_indent) break;
            if (indent != parent_indent + 1) {
                i++;
                continue;
            }
            
            char *content = line + indent;
            trim_trailing_whitespace(content);
            
            /* Check if this item has children */
            int j = i + 1;
            bool has_children = false;
            while (j < end) {
                if (!lines[j] || !*lines[j]) {
                    j++;
                    continue;
                }
                int child_indent = count_leading_tabs(lines[j]);
                if (child_indent <= indent) break;
                if (child_indent == indent + 1) {
                    has_children = true;
                    break;
                }
                j++;
            }
            
            if (has_children) {
                /* Find end of children */
                int child_end = j;
                while (child_end < end) {
                    if (!lines[child_end] || !*lines[child_end]) {
                        child_end++;
                        continue;
                    }
                    int child_indent = count_leading_tabs(lines[child_end]);
                    if (child_indent <= indent) break;
                    child_end++;
                }
                
                taml_value_t *child = parse_lines(lines, j, child_end, indent, state);
                if (child) {
                    taml_array_append(result, child);
                }
                i = child_end;
            } else {
                taml_value_t *value = convert_value(content, state->type_conversion);
                if (value) {
                    taml_array_append(result, value);
                }
                i++;
            }
        }
    } else {
        result = taml_create_object();
        if (!result) return NULL;
        
        i = start;
        while (i < end) {
            char *line = lines[i];
            if (!line || !*line) {
                i++;
                continue;
            }
            
            int indent = count_leading_tabs(line);
            if (indent <= parent_indent) break;
            if (indent != parent_indent + 1) {
                i++;
                continue;
            }
            
            char *content = line + indent;
            char *tab_pos = strchr(content, TAB);
            
            if (tab_pos) {
                /* Key-value pair */
                *tab_pos = '\0';
                char *key = content;
                char *value_str = tab_pos + 1;
                
                /* Skip additional tabs */
                while (*value_str == TAB) value_str++;
                
                trim_trailing_whitespace(key);
                trim_trailing_whitespace(value_str);
                
                /* Validate key */
                if (state->strict && has_tab_in_content(key)) {
                    snprintf(state->error_msg, sizeof(state->error_msg),
                            "Key contains invalid tab character at line %d", i + 1);
                    state->error_line = i + 1;
                    i++;
                    continue;
                }
                
                taml_value_t *value = convert_value(value_str, state->type_conversion);
                if (value) {
                    taml_object_set(result, key, value);
                }
                i++;
            } else {
                /* Parent key with children */
                char *key = content;
                trim_trailing_whitespace(key);
                
                /* Find children */
                int j = i + 1;
                int child_end = j;
                while (child_end < end) {
                    if (!lines[child_end] || !*lines[child_end]) {
                        child_end++;
                        continue;
                    }
                    int child_indent = count_leading_tabs(lines[child_end]);
                    if (child_indent <= indent) break;
                    child_end++;
                }
                
                taml_value_t *child = parse_lines(lines, j, child_end, indent, state);
                if (child) {
                    taml_object_set(result, key, child);
                }
                i = child_end;
            }
        }
    }
    
    return result;
}

/* Main parse function */
taml_document_t* taml_parse(const char *text, const taml_parser_options_t *options) {
    if (!text) return NULL;
    
    /* Set up options */
    parser_state_t state = {
        .text = text,
        .pos = 0,
        .line = 1,
        .col = 1,
        .strict = options ? options->strict : false,
        .type_conversion = options ? options->type_conversion : true,
        .error_line = 0
    };
    state.error_msg[0] = '\0';
    
    /* Split text into lines */
    char *text_copy = strdup(text);
    if (!text_copy) return NULL;
    
    /* Count lines */
    int line_count = 1;
    for (const char *p = text_copy; *p; p++) {
        if (*p == '\n') line_count++;
    }
    
    /* Allocate line array */
    char **lines = (char**)calloc(line_count + 1, sizeof(char*));
    if (!lines) {
        free(text_copy);
        return NULL;
    }
    
    /* Split into lines */
    int idx = 0;
    char *line = strtok(text_copy, "\n");
    while (line) {
        /* Skip empty lines and comments */
        char *trimmed = line;
        while (*trimmed == TAB) trimmed++;
        
        if (*trimmed && *trimmed != '#') {
            lines[idx++] = line;
        }
        line = strtok(NULL, "\n");
    }
    
    /* Parse the structure */
    taml_value_t *root = parse_lines(lines, 0, idx, -1, &state);
    
    /* Create document */
    taml_document_t *doc = (taml_document_t*)malloc(sizeof(taml_document_t));
    if (!doc) {
        free(lines);
        free(text_copy);
        if (root) taml_free_value(root);
        return NULL;
    }
    
    doc->root = root;
    doc->error_line = state.error_line;
    if (state.error_msg[0]) {
        doc->error_message = strdup(state.error_msg);
    } else {
        doc->error_message = NULL;
    }
    
    free(lines);
    free(text_copy);
    
    return doc;
}

/* Parse from file */
taml_document_t* taml_parse_file(const char *filename, const taml_parser_options_t *options) {
    if (!filename) return NULL;
    
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;
    
    /* Get file size */
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    /* Read file */
    char *buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }
    
    size_t read = fread(buffer, 1, size, fp);
    buffer[read] = '\0';
    fclose(fp);
    
    /* Parse */
    taml_document_t *doc = taml_parse(buffer, options);
    free(buffer);
    
    return doc;
}
