/**
 * TAML Value and Memory Management Implementation
 */

#define _POSIX_C_SOURCE 200809L

#include "../include/taml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Create object */
taml_value_t* taml_create_object(void) {
    taml_value_t *value = (taml_value_t*)malloc(sizeof(taml_value_t));
    if (!value) return NULL;
    
    value->type = TAML_TYPE_OBJECT;
    value->data.object_val = (taml_object_t*)malloc(sizeof(taml_object_t));
    if (!value->data.object_val) {
        free(value);
        return NULL;
    }
    
    value->data.object_val->pairs = NULL;
    value->data.object_val->count = 0;
    
    return value;
}

/* Create array */
taml_value_t* taml_create_array(void) {
    taml_value_t *value = (taml_value_t*)malloc(sizeof(taml_value_t));
    if (!value) return NULL;
    
    value->type = TAML_TYPE_ARRAY;
    value->data.array_val = (taml_array_t*)malloc(sizeof(taml_array_t));
    if (!value->data.array_val) {
        free(value);
        return NULL;
    }
    
    value->data.array_val->items = NULL;
    value->data.array_val->count = 0;
    
    return value;
}

/* Create string */
taml_value_t* taml_create_string(const char *str) {
    taml_value_t *value = (taml_value_t*)malloc(sizeof(taml_value_t));
    if (!value) return NULL;
    
    value->type = TAML_TYPE_STRING;
    value->data.string_val = str ? strdup(str) : NULL;
    
    return value;
}

/* Create int */
taml_value_t* taml_create_int(long long num) {
    taml_value_t *value = (taml_value_t*)malloc(sizeof(taml_value_t));
    if (!value) return NULL;
    
    value->type = TAML_TYPE_INT;
    value->data.int_val = num;
    
    return value;
}

/* Create float */
taml_value_t* taml_create_float(double num) {
    taml_value_t *value = (taml_value_t*)malloc(sizeof(taml_value_t));
    if (!value) return NULL;
    
    value->type = TAML_TYPE_FLOAT;
    value->data.float_val = num;
    
    return value;
}

/* Create bool */
taml_value_t* taml_create_bool(bool val) {
    taml_value_t *value = (taml_value_t*)malloc(sizeof(taml_value_t));
    if (!value) return NULL;
    
    value->type = TAML_TYPE_BOOL;
    value->data.bool_val = val;
    
    return value;
}

/* Create null */
taml_value_t* taml_create_null(void) {
    taml_value_t *value = (taml_value_t*)malloc(sizeof(taml_value_t));
    if (!value) return NULL;
    
    value->type = TAML_TYPE_NULL;
    
    return value;
}

/* Get value from object */
taml_value_t* taml_object_get(const taml_value_t *obj, const char *key) {
    if (!obj || obj->type != TAML_TYPE_OBJECT || !key) return NULL;
    if (!obj->data.object_val) return NULL;
    
    taml_pair_t *pair = obj->data.object_val->pairs;
    while (pair) {
        if (pair->key && strcmp(pair->key, key) == 0) {
            return pair->value;
        }
        pair = pair->next;
    }
    
    return NULL;
}

/* Set value in object */
taml_error_t taml_object_set(taml_value_t *obj, const char *key, taml_value_t *value) {
    if (!obj || obj->type != TAML_TYPE_OBJECT || !key) return TAML_ERROR_NULL_INPUT;
    if (!obj->data.object_val) return TAML_ERROR_NULL_INPUT;
    
    /* Check if key already exists */
    taml_pair_t *pair = obj->data.object_val->pairs;
    while (pair) {
        if (pair->key && strcmp(pair->key, key) == 0) {
            /* Update existing value */
            if (pair->value) {
                taml_free_value(pair->value);
            }
            pair->value = value;
            return TAML_OK;
        }
        pair = pair->next;
    }
    
    /* Create new pair */
    taml_pair_t *new_pair = (taml_pair_t*)malloc(sizeof(taml_pair_t));
    if (!new_pair) return TAML_ERROR_MEMORY;
    
    new_pair->key = strdup(key);
    if (!new_pair->key) {
        free(new_pair);
        return TAML_ERROR_MEMORY;
    }
    
    new_pair->value = value;
    new_pair->next = obj->data.object_val->pairs;
    obj->data.object_val->pairs = new_pair;
    obj->data.object_val->count++;
    
    return TAML_OK;
}

/* Get array size */
size_t taml_array_size(const taml_value_t *arr) {
    if (!arr || arr->type != TAML_TYPE_ARRAY) return 0;
    if (!arr->data.array_val) return 0;
    
    return arr->data.array_val->count;
}

/* Get value from array */
taml_value_t* taml_array_get(const taml_value_t *arr, size_t index) {
    if (!arr || arr->type != TAML_TYPE_ARRAY) return NULL;
    if (!arr->data.array_val) return NULL;
    
    taml_array_item_t *item = arr->data.array_val->items;
    size_t i = 0;
    while (item && i < index) {
        item = item->next;
        i++;
    }
    
    return item ? item->value : NULL;
}

/* Append to array */
taml_error_t taml_array_append(taml_value_t *arr, taml_value_t *value) {
    if (!arr || arr->type != TAML_TYPE_ARRAY) return TAML_ERROR_NULL_INPUT;
    if (!arr->data.array_val) return TAML_ERROR_NULL_INPUT;
    
    taml_array_item_t *new_item = (taml_array_item_t*)malloc(sizeof(taml_array_item_t));
    if (!new_item) return TAML_ERROR_MEMORY;
    
    new_item->value = value;
    new_item->next = NULL;
    
    /* Append to end */
    if (!arr->data.array_val->items) {
        arr->data.array_val->items = new_item;
    } else {
        taml_array_item_t *item = arr->data.array_val->items;
        while (item->next) {
            item = item->next;
        }
        item->next = new_item;
    }
    
    arr->data.array_val->count++;
    
    return TAML_OK;
}

/* Free value */
void taml_free_value(taml_value_t *value) {
    if (!value) return;
    
    switch (value->type) {
        case TAML_TYPE_STRING:
            if (value->data.string_val) {
                free(value->data.string_val);
            }
            break;
            
        case TAML_TYPE_OBJECT:
            if (value->data.object_val) {
                taml_pair_t *pair = value->data.object_val->pairs;
                while (pair) {
                    taml_pair_t *next = pair->next;
                    if (pair->key) free(pair->key);
                    if (pair->value) taml_free_value(pair->value);
                    free(pair);
                    pair = next;
                }
                free(value->data.object_val);
            }
            break;
            
        case TAML_TYPE_ARRAY:
            if (value->data.array_val) {
                taml_array_item_t *item = value->data.array_val->items;
                while (item) {
                    taml_array_item_t *next = item->next;
                    if (item->value) taml_free_value(item->value);
                    free(item);
                    item = next;
                }
                free(value->data.array_val);
            }
            break;
            
        default:
            break;
    }
    
    free(value);
}

/* Free document */
void taml_free_document(taml_document_t *doc) {
    if (!doc) return;
    
    if (doc->root) {
        taml_free_value(doc->root);
    }
    if (doc->error_message) {
        free(doc->error_message);
    }
    free(doc);
}

/* Free string */
void taml_free_string(char *str) {
    if (str) free(str);
}
