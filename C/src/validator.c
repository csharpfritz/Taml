/**
 * TAML Validator Implementation
 */

#define _POSIX_C_SOURCE 200809L

#include "../include/taml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAB '\t'

/* Internal validation functions */
static taml_error_t validate_line(const char *line, int line_num, int *indent_level, 
                                   int prev_indent, bool had_value, 
                                   char *error_msg, size_t error_msg_size);
static bool has_leading_spaces(const char *line);
static bool has_mixed_indent(const char *line);
static int count_leading_tabs(const char *line);
static bool has_tab_in_content(const char *str);

/* Check for leading spaces */
static bool has_leading_spaces(const char *line) {
    if (!line || !*line) return false;
    return *line == ' ';
}

/* Check for mixed spaces and tabs in indentation */
static bool has_mixed_indent(const char *line) {
    if (!line) return false;
    
    bool found_space = false;
    bool found_tab = false;
    
    for (const char *p = line; *p; p++) {
        if (*p == ' ') found_space = true;
        else if (*p == '\t') found_tab = true;
        else break;
    }
    
    return found_space && found_tab;
}

/* Count leading tabs */
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

/* Validate a single line */
static taml_error_t validate_line(const char *line, int line_num, int *indent_level, 
                                   int prev_indent, bool had_value, 
                                   char *error_msg, size_t error_msg_size) {
    if (!line) return TAML_OK;
    
    /* Skip empty lines */
    if (!*line) return TAML_OK;
    
    /* Skip comments */
    const char *trimmed = line;
    while (*trimmed == TAB) trimmed++;
    if (*trimmed == '#') return TAML_OK;
    
    /* Check for leading spaces */
    if (has_leading_spaces(line)) {
        if (error_msg) {
            snprintf(error_msg, error_msg_size, 
                    "Line %d: Indentation must use tabs, not spaces", line_num);
        }
        return TAML_ERROR_INVALID_INDENTATION;
    }
    
    /* Check for mixed indentation */
    if (has_mixed_indent(line)) {
        if (error_msg) {
            snprintf(error_msg, error_msg_size,
                    "Line %d: Mixed spaces and tabs in indentation", line_num);
        }
        return TAML_ERROR_MIXED_INDENT;
    }
    
    /* Count indentation */
    int indent = count_leading_tabs(line);
    *indent_level = indent;
    
    /* Check indentation consistency */
    if (indent > prev_indent + 1) {
        if (error_msg) {
            snprintf(error_msg, error_msg_size,
                    "Line %d: Invalid indentation level (expected %d tabs, found %d)",
                    line_num, prev_indent + 1, indent);
        }
        return TAML_ERROR_INCONSISTENT_INDENT;
    }
    
    /* Check for orphaned indentation */
    if (indent > prev_indent && had_value) {
        if (error_msg) {
            snprintf(error_msg, error_msg_size,
                    "Line %d: Indented line has no parent", line_num);
        }
        return TAML_ERROR_ORPHANED_LINE;
    }
    
    /* Get content after indentation */
    const char *content = line + indent;
    if (!*content) {
        if (error_msg) {
            snprintf(error_msg, error_msg_size, "Line %d: Line has no key", line_num);
        }
        return TAML_ERROR_EMPTY_KEY;
    }
    
    /* Find first tab in content */
    const char *tab_pos = strchr(content, TAB);
    
    if (tab_pos) {
        /* Key-value pair - validate key and value */
        size_t key_len = tab_pos - content;
        if (key_len == 0) {
            if (error_msg) {
                snprintf(error_msg, error_msg_size, "Line %d: Empty key", line_num);
            }
            return TAML_ERROR_EMPTY_KEY;
        }
        
        /* Check for additional tabs in key */
        char *key = strndup(content, key_len);
        if (has_tab_in_content(key)) {
            if (error_msg) {
                snprintf(error_msg, error_msg_size,
                        "Line %d: Key contains invalid tab character", line_num);
            }
            free(key);
            return TAML_ERROR_INVALID_TAB_IN_KEY;
        }
        free(key);
        
        /* Get value (skip separator tabs) */
        const char *value = tab_pos + 1;
        while (*value == TAB) value++;
        
        /* Check for tabs in value */
        if (has_tab_in_content(value)) {
            if (error_msg) {
                snprintf(error_msg, error_msg_size,
                        "Line %d: Value contains invalid tab character", line_num);
            }
            return TAML_ERROR_INVALID_TAB_IN_VALUE;
        }
    }
    
    return TAML_OK;
}

/* Main validation function */
taml_error_t taml_validate(const char *text, char *error_msg, 
                           size_t error_msg_size, int *error_line) {
    if (!text) return TAML_ERROR_NULL_INPUT;
    
    /* Split text into lines */
    char *text_copy = strdup(text);
    if (!text_copy) return TAML_ERROR_MEMORY;
    
    int line_num = 0;
    int prev_indent = -1;
    bool prev_had_value = false;
    taml_error_t result = TAML_OK;
    
    char *line = strtok(text_copy, "\n");
    while (line) {
        line_num++;
        
        int indent = 0;
        taml_error_t err = validate_line(line, line_num, &indent, prev_indent, 
                                        prev_had_value, error_msg, error_msg_size);
        
        if (err != TAML_OK) {
            result = err;
            if (error_line) *error_line = line_num;
            break;
        }
        
        /* Determine if this line has a value */
        const char *content = line + indent;
        while (*content == TAB) content++;
        
        if (*content && *content != '#') {
            prev_had_value = (strchr(content, TAB) != NULL);
            prev_indent = indent;
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(text_copy);
    return result;
}

/* Validate file */
taml_error_t taml_validate_file(const char *filename, char *error_msg,
                                 size_t error_msg_size, int *error_line) {
    if (!filename) return TAML_ERROR_NULL_INPUT;
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        if (error_msg) {
            snprintf(error_msg, error_msg_size, "Cannot open file: %s", filename);
        }
        return TAML_ERROR_PARSE_FAILED;
    }
    
    /* Get file size */
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    /* Read file */
    char *buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(fp);
        return TAML_ERROR_MEMORY;
    }
    
    size_t read = fread(buffer, 1, size, fp);
    buffer[read] = '\0';
    fclose(fp);
    
    /* Validate */
    taml_error_t result = taml_validate(buffer, error_msg, error_msg_size, error_line);
    free(buffer);
    
    return result;
}
